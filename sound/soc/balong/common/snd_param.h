 /*
 * Copyright (C) Huawei Technologies Co., Ltd. 2012-2015. All rights reserved.
 * foss@huawei.com
 *
 * If distributed as part of the Linux kernel, the following license terms
 * apply:
 *
 * * This program is free software; you can redistribute it and/or modify
 * * it under the terms of the GNU General Public License version 2 and 
 * * only version 2 as published by the Free Software Foundation.
 * *
 * * This program is distributed in the hope that it will be useful,
 * * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * * GNU General Public License for more details.
 * *
 * * You should have received a copy of the GNU General Public License
 * * along with this program; if not, write to the Free Software
 * * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
 *
 * Otherwise, the following license terms apply:
 *
 * * Redistribution and use in source and binary forms, with or without
 * * modification, are permitted provided that the following conditions
 * * are met:
 * * 1) Redistributions of source code must retain the above copyright
 * *    notice, this list of conditions and the following disclaimer.
 * * 2) Redistributions in binary form must reproduce the above copyright
 * *    notice, this list of conditions and the following disclaimer in the
 * *    documentation and/or other materials provided with the distribution.
 * * 3) Neither the name of Huawei nor the names of its contributors may 
 * *    be used to endorse or promote products derived from this software 
 * *    without specific prior written permission.
 * 
 * * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
 
 
 
#ifndef __SND_PARAM_H
#define __SND_PARAM_H

/*ʱ�ӷ�Ƶ��һЩ����*/
#define FS_CLK_8M     (8192000)
#ifdef BSP_CONFIG_BOARD_TELEMATIC
#define FS_CLK_4M     (4096000)
#endif
#define FS_CLK_2M     (2048000)
#define SYNC_CLK_8K   (8000)
#define SYNC_CLK_16K  (16000)

#ifdef BSP_CONFIG_BOARD_TELEMATIC
/* ����ʱ�ӷ�Ƶ�ļĴ�����ַ */
#define SIO_CLK_DIV_ADDR (0x90000100)
/* ��ʱ��Ϊ49.152MHz */
#define SIO_MAIN_CLK     (49152000)
/* ��ȡ��NV30005��ֵ��Ҫ����8λ����ʱ�����õ�ֵ */
#define NV_SIO_CFG_BIT_CODECFREQ (8)

#define MODULE_RATES (SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_11025 \
            | SNDRV_PCM_RATE_16000 | SNDRV_PCM_RATE_22050 \
            | SNDRV_PCM_RATE_32000 | SNDRV_PCM_RATE_44100 \
            | SNDRV_PCM_RATE_48000)

#define MODULE_FORMATS (SNDRV_PCM_FMTBIT_S8 | SNDRV_PCM_FORMAT_S8 \
            | SNDRV_PCM_FMTBIT_U8     | SNDRV_PCM_FORMAT_U8 \
            | SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FORMAT_S16_LE \
            | SNDRV_PCM_FMTBIT_S24_LE | SNDRV_PCM_FORMAT_S24_LE \
            | SNDRV_PCM_FMTBIT_S32_LE | SNDRV_PCM_FORMAT_S32_LE)
#endif

enum
{
    SND_SAMPLE_RATE_8K = 0,
    SND_SAMPLE_RATE_WIDE_8K,
    SND_SAMPLE_RATE_16K
};
void snd_sample_rate_set(unsigned int rate);
unsigned int snd_sample_rate_get(void);

enum
{
    SND_SOC_MASTER = 0,
    SND_SOC_SLAVE
};
void snd_master_slave_set(unsigned int mode);
unsigned int snd_master_slave_get(void);

enum
{
	SND_PCM_MODE = 0,
	SND_I2S_MODE
};
void snd_data_mode_set(unsigned int mode);
unsigned int snd_data_mode_get(void);

enum
{
	SND_XRUN_SILENCE = 0,
	SND_XRUN_STOP
};
void snd_xrun_mode_set(unsigned int mode);
unsigned int snd_xrun_mode_get(void);

#ifdef BSP_CONFIG_BOARD_TELEMATIC
//������ȡͨ��״̬�Ľӿں����������ں˵���
extern unsigned int AT_IsCallOn(unsigned char ucIndex);
extern unsigned int g_bclk_rate;
#endif

#endif
