/* sound/soc/balong/snd_module_dai.c
 *
 * ALSA SoC Pcm Layer
 *
 *  Copyright (c) 2009 Huawei Technology Co., Ltd
 *  Author: Huawei Technology Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */


#include <linux/clk.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/pm_runtime.h>
#include <osl_spinlock.h>
#include <osl_sem.h>
#include <sound/soc.h>
#include <sound/pcm_params.h>
#include <bsp_icc.h>
#include "bsp_nvim.h"
#include "drv_comm.h"
#include "bsp_memmap.h"
#include "sio.h"
#include "snd_param.h"
#include "snd_init.h"

#if (FEATURE_ON == MBB_MLOG)
#include <linux/mlog_lib.h>
#endif

#define  en_NV_SIO_VOICE_MASTER  30012
#ifdef BSP_CONFIG_BOARD_TELEMATIC
#define  I2S_RATE_2  2
#endif

/*lint -save -e43 -e959 -e958 */
struct dai_status
{
    int         pflag;
    int         cflag;
    spinlock_t  dai_lock;
};
/*lint -restore*/


static struct dai_status soc_dai_status = {0};

void sio_mode_set(void)
{
    unsigned short sio_mode = 1;
    unsigned int   ret = 0;

    ret = bsp_nvm_read(en_NV_SIO_VOICE_MASTER,&sio_mode,sizeof(unsigned short));
    if(ret)
    {
#if ( FEATURE_ON == MBB_MLOG )
        mlog_print(MLOG_PCM, mlog_lv_factory, "%s : [snd_machine_probe] read nv %d fail,set as master!\n",
                         __func__, en_NV_SIO_VOICE_MASTER);
#endif

        printk(KERN_ERR "[snd_machine_probe] read nv %d fail,set as master!\n",
                        en_NV_SIO_VOICE_MASTER);
    }

    if (1 == sio_mode)
    {
        /* ����SIOΪ��ģʽ */
        sio_master_set(SND_SOC_MASTER);

        /* ʱ�ӷ�Ƶ 245760k/120/256=8k bit0:15=120(pclk=2M) bit16:27=256(sync=8K) */
        //sio_pcm_div(FS_CLK_2M, SYNC_CLK_8K);

        printk(KERN_ERR "[snd_machine_probe]sio set as master!\n");
    }
    else
    {
        /* ����SIOΪ��ģʽ */
        sio_master_set(SND_SOC_SLAVE);
        printk(KERN_ERR "[snd_machine_probe]sio set as slave!\n");
    }
    return;
}

void sio_close(void)
{
    sio_reset();
    //�ر�ʱ�Ӻ�aplay��������Ƶ�ļ����ڴ�绰��ϵͳ�ͻ�crash
#ifndef BSP_CONFIG_BOARD_TELEMATIC
    sio_clock_disable();
#endif

#if ( FEATURE_ON == MBB_MLOG )
    mlog_print(MLOG_PCM, mlog_lv_factory, "%s : sio close\n", __func__);
#endif
    printk(KERN_ERR "Enter this:%s!\n",__func__);
    return;
}

static int snd_soc_dai_startup(struct snd_pcm_substream *substream, struct snd_soc_dai *dai)
{
    int stream = substream->stream;
    unsigned long flag_local = 0;

    spin_lock_irqsave(&soc_dai_status.dai_lock, flag_local);

    if( 0 == soc_dai_status.pflag && 0 == soc_dai_status.cflag )
    {
        sio_mode_set();
        sio_init();
    }

    if( SNDRV_PCM_STREAM_PLAYBACK == stream )
    {
        soc_dai_status.pflag = 1;
    }
    else
    {
        soc_dai_status.cflag = 1;
    }

       spin_unlock_irqrestore(&soc_dai_status.dai_lock, flag_local);

    return 0;
}

static void snd_soc_dai_shutdown(struct snd_pcm_substream *substream, struct snd_soc_dai *dai)
{
    int stream = substream->stream;

    if( SNDRV_PCM_STREAM_PLAYBACK == stream )
    {
        soc_dai_status.pflag = 0;
    }
    else
    {
        soc_dai_status.cflag = 0;
    }
    if (0 == soc_dai_status.pflag && 0 == soc_dai_status.cflag)
    {
        sio_close();
    }

    return;
}
static int snd_soc_hw_params(struct snd_pcm_substream *substream,
        struct snd_pcm_hw_params *hw_params, struct snd_soc_dai *dai)
{
    int rate =  params_rate(hw_params);
#ifdef BSP_CONFIG_BOARD_TELEMATIC
    unsigned int channel_mode_ret = 0;
#endif
    printk(KERN_ERR "rate: %d\n", rate);

    /* ʱ�ӷ�Ƶ 245760k/120/256=8k bit0:15=120(pclk=2M) bit16:27=256(sync=8K) */
#ifdef BSP_CONFIG_BOARD_TELEMATIC
    /* ʱ������ΪNV 30005�����õ�ֵ */
    channel_mode_ret = snd_data_mode_get();
    if (SND_PCM_MODE == channel_mode_ret)
    {
        sio_pcm_div(g_bclk_rate, rate);
    }
    else
    {
       /* I2Sģʽ�²�����Ƶ�ļ������������������ţ�������Ҫ��2 */
       sio_pcm_div(g_bclk_rate, rate * I2S_RATE_2);
    }
#else
    //ʱ�ӷ�Ƶ����Ϊ4M������绰�����ʷ����˱仯
    sio_pcm_div(FS_CLK_2M, rate);
#endif

    return 0;
}

static int snd_soc_dai_startup_dummy(struct snd_pcm_substream *substream, struct snd_soc_dai *dai)
{
    return 0;
}

static void snd_soc_dai_shutdown_dummy(struct snd_pcm_substream *substream, struct snd_soc_dai *dai)
{
    return;
}

static const struct snd_soc_dai_ops snd_pcm_dai_ops = {
    .startup    = snd_soc_dai_startup,
    .shutdown   = snd_soc_dai_shutdown,
    .hw_params  = snd_soc_hw_params,
};
static const struct snd_soc_dai_ops snd_hifi_pcm_dai_ops = {
    .startup    = snd_soc_dai_startup_dummy,
    .shutdown   = snd_soc_dai_shutdown_dummy,
};

static struct snd_soc_dai_driver snd_pcm_dai[] = {
    [0]={
        .name    = "dma_pcm_dai",
        .ops = &snd_pcm_dai_ops,
        .playback = {
            .channels_min    = 1,
            .channels_max    = 2,
            .rates        = MODULE_RATES,
            .formats      = MODULE_FORMATS,
        },
        .capture = {
            .channels_min    = 1,
            .channels_max    = 2,
            .rates        = MODULE_RATES,
            .formats      = MODULE_FORMATS,
        }
    },
    [1]={
        .name    = "hifi_pcm_dai",
        .ops = &snd_hifi_pcm_dai_ops,
        .playback = {
            .channels_min    = 1,
            .channels_max    = 2,
            .rates        = MODULE_RATES,
            .formats      = MODULE_FORMATS,
        },
        .capture = {
            .channels_min    = 1,
            .channels_max    = 2,
            .rates        = MODULE_RATES,
            .formats      = MODULE_FORMATS,
        }
    },
};

static const struct snd_soc_component_driver snd_pcm_component[] = {
    [0]={
        .name = "dma_pcm_dai",
    },
    [1]={
        .name = "hifi_pcm_dai",
    },
};


int snd_pcm_dev_probe(struct platform_device *pdev)
{
    int ret = 0;

    spin_lock_init(&soc_dai_status.dai_lock);

    ret = snd_soc_register_component(&pdev->dev, &snd_pcm_component, &snd_pcm_dai, 2);
    if (ret < 0) {
        printk(KERN_ERR "failed to get register DAI: %d\n", ret);
        return ret;
    }

    printk(KERN_ERR "snd pcm probe ok!\n");

    return ret;
}

int snd_pcm_dev_remove(struct platform_device *pdev)
{
    snd_soc_unregister_component(&pdev->dev);
    printk(KERN_ERR "snd pcm dev remove ok!\n");
    return 0;
}


/* Module information */
MODULE_AUTHOR("Huawei Technology Co., Ltd");
MODULE_DESCRIPTION("Balong ASoC PCM Driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:balong-pcm");



