/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2013-2015. All rights reserved.
 *
 * mobile@huawei.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */



#include <linux/irq.h>
#include <linux/gpio.h>
#include "bsp_icc.h"
#include "huawei_pm.h"
#include "mdrv_version.h"
#if (FEATURE_ON == MBB_USB)
#include "mdrv_usb.h"
#endif
#ifdef BSP_CONFIG_BOARD_TELEMATIC
/* ��Դ����ģ���ʼ��״̬��¼ */
HW_PM_INIT_STATUS g_huawei_pm_init = HUAWEI_PM_INIT_START;
#endif
/* �ϱ����涨ʱ���Ƿ��������жϱ�־ shake_timeout_work_flag �Ķ�дͬ���� */
static DEFINE_SPINLOCK(shake_timeout_work_spin);
/*Global status for huawei PM*/
/*It's only accessed in this file,
other modules can visit it through 
call none static function in this file*/
huawei_pm_type g_huawei_pm_status =
{
    TRUE,
    FALSE,
    FALSE,
    
    RMT_WK_CHN_PIN | RMT_WK_CHN_USB,
    {0},
    {
        REMOTE_WAKEUP_ON, 
        RMT_WK_CHN_PIN | RMT_WK_CHN_USB,
        RMT_WK_SRC_SMS | RMT_WK_SRC_VOICE | RMT_WK_SRC_DATA | RMT_WK_SRC_UR
    }
};

extern unsigned int g_PmAcorePrintOnOff;

dsat_curc_cfg_type g_curc_info = 
{
    DSAT_UR_ENABLE
};
/*Ӳ�������Ķ�ʱ��*/
static WAKEUPIN_T_CTRL_S g_wakeupin_ctx = {0};
static struct delayed_work usb_resume_acore_work;
static struct delayed_work usb_resume_ccore_work;
/* wakeupin�жϴ���ʱ������ */
const char *timer_name = "huawei_pm";

struct wake_lock remote_wakeup_pin_lock; /*���1s�ߵ�ƽ�����з�ֹ˯����*/
struct wake_lock wakeup_in_lock;         /*wakeup_in����˯����*/
struct wake_lock flush_cache_lock;       /*�建���ֹ˯����*/
#if (FEATURE_ON == MBB_FACTORY)
struct wake_lock factory_lock;          /*��Ƭ����˯����*/
#endif

/*���ڱ���Զ�̻��ѻ���ĳ�ʱʱ��*/
unsigned int  remote_wakeup_shake_time = 2000;
unsigned int  shake_timeout_work_flag = 0;
static struct delayed_work  shake_timeout_work;
extern void huawei_pm_rsp_inform_flush_cache_acore(void);
#ifdef BSP_CONFIG_BOARD_TELEMATIC
extern int bsp_usb_is_vbus_connect(void);
extern void bsp_usb_status_change(int status);
extern void wakeup_in_state_report(unsigned char state);
extern void bsp_usb_unlock_wakelock(void);
extern void bsp_usb_lock_wakelock(void);
#endif

/* for s3/s4 cann't report msg new request */
unsigned char remote_wakeup_ctrl_bakeup = REMOTE_WAKEUP_ON;
/*USB s3/s4�ı�ǣ�FALSE��ʾ����S3/S4��TRUE��ʾ�˳�*/
static unsigned char usb_s34_state_flag = FALSE;
/* ��¼���ߴ���ģʽ �·�pstandby֮����� */
static unsigned char g_factory_pstandby_state = 0;
static DEFINE_SPINLOCK(usb_s34_spin);
#ifdef BSP_CONFIG_BOARD_TELEMATIC
#if(FEATURE_ON == MBB_FACTORY)
/*===========================================================================
FUNCTION     :   huawei_pm_factory_unlock
DESCRIPTION  :   ������Ƭ���� .
DEPENDENCIES :   None
RETURN VALUE :   None
SIDE EFFECTS :   None
===========================================================================*/
void huawei_pm_factory_unlock(void)
{
    printk(KERN_ERR"huawei_pm_factory_unlock release factory_lock. \n");
    wake_unlock(&factory_lock);
    return;
}
/*************************************************************
* ������  : huawei_pm_factory_lock
* ��������: ��ֹ��Ƭ˯��
* �������: NA
* �������:
* �� �� ֵ: NA
**************************************************************/

void huawei_pm_factory_lock(void)
{
    printk(KERN_ERR"huawei_pm_factory_lock lock factory_lock. \n");
    wake_lock(&factory_lock);
    return;
}

#endif
#endif
/*************************************************************
* ������  : huawei_pm_pstandby_entry_handle
* ��������: ������ߴ���״̬��֪ͨA C�˹ر�Զ�̻���
* �������: NA
* �������:
* �� �� ֵ: NA
**************************************************************/
void huawei_pm_pstandby_entry_handle(void)
{
    int ret = 0;
    int value = PM_MSG_PSTANDBY_IN;
    unsigned int icc_channel_id = ICC_CHN_IFC << 16 | IFC_RECV_FUNC_M2M_PM;
    
    g_factory_pstandby_state = 1;

    /*����Ϣ��C�ˣ��ر�C��Զ�̻���*/
    ret = bsp_icc_send(ICC_CPU_MODEM, icc_channel_id, (u8*)&value, sizeof(value));
    if (ret != (int)sizeof(value))
    {
        PRINT_ACORE_LOG(PM_MSG_PRINT_ON,"send Ccore set pstandby state msg fail !!!\n");
    }

}
/*************************************************************
* ������  : huawei_pm_pstandby_state_get
* ��������: ��ȡ�Ƿ��ڲ��ߴ���ģʽ
* �������: NA
* �������:
* �� �� ֵ: unsigned char
**************************************************************/
unsigned char huawei_pm_pstandby_state_get(void)
{
    return g_factory_pstandby_state;
}
/*************************************************************
* ��   ��   �� : huawei_pm_usb_s34_state_entry_handle
* ��������  : USB����s3/s4�׶εĴ�����
* �������  : NA
* �������  :
* �� �� ֵ     : NA
***************************************************************/
static void huawei_pm_usb_s34_state_entry_handle(void)
{
    int ret = 0;
    int value = PM_MSG_USB_S34_IN;
    unsigned int icc_channel_id = ICC_CHN_IFC << 16 | IFC_RECV_FUNC_M2M_PM;
    unsigned long flag = 0;

    spin_lock_irqsave(&usb_s34_spin,flag); 

    if(TRUE == usb_s34_state_flag)
    {
        spin_unlock_irqrestore(&usb_s34_spin,flag);
        return;
    }

    /*����Զ�̻��ѿ��أ�����A�˱�Զ�̻���*/
    remote_wakeup_ctrl_bakeup = \
        g_huawei_pm_status.rmt_wk_cfg.remote_wakeup_ctrl;
    g_huawei_pm_status.rmt_wk_cfg.remote_wakeup_ctrl = REMOTE_WAKEUP_OFF;

    /*����Ϣ��C�ˣ��ر�C��Զ�̻���*/
    ret = bsp_icc_send(ICC_CPU_MODEM, icc_channel_id, (u8*)&value, sizeof(value));
    if (ret != (int)sizeof(value))
    {
        PRINT_ACORE_LOG(PM_MSG_PRINT_ON,"send Ccore USB entry s34 state msg fail !!!\n");
    }

    usb_s34_state_flag = TRUE;

    spin_unlock_irqrestore(&usb_s34_spin,flag);
}

/*************************************************************
* ��   ��   �� : huawei_pm_usb_s34_state_exit_handle
* ��������  : USB�˳�s3/s4�׶εĴ�����
* �������  : NA
* �������  :
* �� �� ֵ     : NA
***************************************************************/
static void huawei_pm_usb_s34_state_exit_handle(void)
{
    int ret = 0;
    int value = PM_MSG_USB_S34_OUT;
    unsigned int icc_channel_id = ICC_CHN_IFC << 16 | IFC_RECV_FUNC_M2M_PM;
    unsigned long flag = 0;

    spin_lock_irqsave(&usb_s34_spin,flag); 

    if(FALSE == usb_s34_state_flag)
    {
        spin_unlock_irqrestore(&usb_s34_spin,flag);
        return;
    }

    /*�ָ�Զ�̻��ѿ��ص�ԭʼ״ֵ̬*/
    g_huawei_pm_status.rmt_wk_cfg.remote_wakeup_ctrl = remote_wakeup_ctrl_bakeup;
    /*����Ϣ��C�ˣ��ָ�C��Զ�̻��ѵ�ԭʼ״ֵ̬*/
    ret = bsp_icc_send(ICC_CPU_MODEM, icc_channel_id, (u8*)&value, sizeof(value));
    if (ret != (int)sizeof(value))
    {
        PRINT_ACORE_LOG(PM_MSG_PRINT_ON,"send Ccore USB entry s34 state msg fail !!!\n");
    }

    usb_s34_state_flag = FALSE;

    spin_unlock_irqrestore(&usb_s34_spin,flag);

}

/*************************************************************
* ��   ��   �� : huawei_pm_usb_s34_state_change_handle
* ��������  : ��Դ����ģ���USB�ṩ�Ĵ�����
* �������  : enType --- TRUE   ��ʾUSB  ����S3/S4
                                             FALSE  ��ʾUSB  �˳�S3/S4
* �������  :
* ��  ��  ֵ   :  NA
***************************************************************/
void huawei_pm_usb_s34_state_change_handle(unsigned char enType)
{
    nv_huawei_s3s4_remote_wakeup_enable  wakeup_enable = {0};
    if (NV_OK != NV_ReadEx(MODEM_ID_0, NV_HUAWEI_S3S4_REMOTE_WAKEUP_ENABLE, 
                                    &wakeup_enable, sizeof(wakeup_enable)))
    {
        PRINT_ACORE_LOG(PM_MSG_PRINT_ON," NV_HUAWEI_S3S4_REMOTE_WAKEUP_ENABLE read fail!!! \n"); 
        /*���NV��ʧ�ܣ�Ĭ��Ϊ1*/
        wakeup_enable.remote_wakeup_enable = 1;
    }
    /*���NVֵ��Ϊ0����ʾ�ͻ�ϣ����WIN7ϵͳ�ϣ�
    ��������S3S4��Զ�̻��Ѽ�����Ч��
    �����ϱ��ɱ����汾Ĭ��Ϊ1���û�����PCVENDOR����*/	
    if (0 != wakeup_enable.remote_wakeup_enable)
    {
        return;
    }
    if(enType)
    {
        huawei_pm_usb_s34_state_entry_handle();
    }
    else
    {
        huawei_pm_usb_s34_state_exit_handle();
    }
}

/*===========================================================================
FUNCTION 
    huawei_pm_reset_pin_status
DESCRIPTION
    Reset the WAKEUP_OUT PIN status
DEPENDENCIES
    None
RETURN VALUE
    None.
SIDE EFFECTS
    None
===========================================================================*/
static void huawei_pm_reset_pin_status(void)
{
    gpio_direction_output(WAKEUP_OUT, GPIO_LOW_VALUE); 
    g_huawei_pm_status.pin_timer.is_start = FALSE;
}

/*===========================================================================
FUNCTION 
    huawei_pm_request_wakeup_pin_cb
DESCRIPTION
    Callback for WAKEUP_OUT PIN wakeup timer
DEPENDENCIES
  None
RETURN VALUE
  None.
SIDE EFFECTS
  None
===========================================================================*/
static void huawei_pm_request_wakeup_pin_cb(struct work_struct *w)
{
    huawei_pm_reset_pin_status();
    /*1s�ߵ�ƽԶ�̻��ѻ����ź����������Ͷ�޳�Ʊ����֤��Ӱ������ϵͳ˯��*/
    wake_unlock(&remote_wakeup_pin_lock);
}
/*****************************************************************************
�� �� ��  : wakeupin_timer_handler
��������  : wakeupin��ʱ���ص�������
�������  : ��
�������  : ��
�� �� ֵ  : ��
*****************************************************************************/
static void wakeupin_timer_handler(void *arg)
{
    unsigned int wakeup_level = 0;

    wakeup_level = gpio_get_value(WAKEUP_IN);

    /*��ʱ��ȡ��ֵ���ϴε�ֵ��һ���򲻴�������*/
    if (wakeup_level != g_wakeupin_ctx.wakeup_temp)
    {
        PRINT_ACORE_LOG(g_PmAcorePrintOnOff,"two read gpio value is different.\n");
        return;
    }

    /*�����Ҫ�л���״̬��Ŀǰ״̬һ�£��򲻴���*/
    if (wakeup_level != g_wakeupin_ctx.wakeup_level)
    {
        PRINT_ACORE_LOG(g_PmAcorePrintOnOff,"the state to set is different\n");
#ifdef BSP_CONFIG_BOARD_TELEMATIC
        /*������ߴ�����ֱ�Ӷ����ϱ�*/
        if (0 == g_factory_pstandby_state)
        {
            /* �ϱ�wakeupin event�¼� */
            wakeup_in_state_report(wakeup_level);
        }
        
#endif
    }

    g_wakeupin_ctx.wakeup_level = wakeup_level;
    if (0 == wakeup_level)
    {
        printk("low level irq happen !\n");
        wake_unlock(&wakeup_in_lock);
#ifdef BSP_CONFIG_BOARD_TELEMATIC
        bsp_usb_status_change(0);
#endif
    }
    else
    {
        printk("high level irq happen !\n");
        /*��λ��ͨ��wakeup_in����ģ�飬˵����λ�� atpV2 ���ڴ���normal̬������pm�Ļ�������*/
        huawei_pm_remote_shake_complete();
    }

}

/*****************************************************************************
�� �� ��  : wakeupin_timer_init
��������  : wakeupin��ʱ����ʼ��
�������  : ��
�������  : ��
�� �� ֵ  : ��
*****************************************************************************/
static int wakeupin_timer_init(void)
{
    int ret = -1;
    struct softtimer_list *irq_timer = &g_wakeupin_ctx.irq_timer;
    
    /*irq_timer����Ӳ��wakeupin�жϵĴ���*/
    irq_timer->func = wakeupin_timer_handler;
    irq_timer->name = timer_name;
    irq_timer->timeout = WAKEUPIN_N_TIMER_LENGTH;
    irq_timer->wake_type = SOFTTIMER_WAKE;
    ret = bsp_softtimer_create(irq_timer);

    if (0 != ret)
    {
        printk(KERN_ERR "wakeupin_timer_init create fail\n");
        return -1;
    }
    return 0;
}
/*===========================================================================
FUNCTION 
    huawei_pm_sleep_in_event_isr
DESCRIPTION
    The interrupt handler for WAKEUP_IN interrupt.
DEPENDENCIES
    None
RETURN VALUE
    None
SIDE EFFECTS
    None
===========================================================================*/
static irqreturn_t huawei_pm_sleep_in_event_isr(void)
{
    unsigned int uclevel = 0;
    unsigned long flags = 0;

    pm_status_s *pm_st = (pm_status_s *)(SRAM_MODULE_PM_ADDR);
    spin_lock_irqsave(&g_wakeupin_ctx.lock, flags);
    uclevel = gpio_get_value(WAKEUP_IN);
    g_wakeupin_ctx.wakeup_temp = uclevel;
    /*��ʱ��,�������δ����ʱ��ʱ������ɾ��*/
    (void)bsp_softtimer_delete(&g_wakeupin_ctx.irq_timer);

    if ( uclevel )
    {
        /*wakeup_in����,ͶƱ���������*/
        wake_lock(&wakeup_in_lock);
#ifdef BSP_CONFIG_BOARD_TELEMATIC
        bsp_usb_lock_wakelock();
#endif
            
        pm_st->pin_status = PIN_RESUME_STATUS;
    }
    else
    {
        pm_st->pin_status = PIN_SUSPEND_STATUS;
        /*wakeup_in���ͺ�,ͶƱ�������*/
    }
    bsp_softtimer_add(&g_wakeupin_ctx.irq_timer);
    spin_unlock_irqrestore(&g_wakeupin_ctx.lock, flags);
    return IRQ_HANDLED;
        
}

/*===========================================================================
FUNCTION 
    huawei_pm_udpate_nv_configuration
DESCRIPTION
    Update NV Setting through dsatutil_put_nv_item.
DEPENDENCIES
    This API just allowed been used by DS task.
RETURN VALUE
    TRUE:   success.
    FALSE:  fail
SIDE EFFECTS
    None
===========================================================================*/
static boolean huawei_pm_udpate_nv_configuration
(
    remote_wakeup_info_type config
)
{
    /* status var for reading from NV */
    remote_wakeup_info_type local_nv_item;
    
    local_nv_item.remote_wakeup_ctrl 
                                = config.remote_wakeup_ctrl;
    local_nv_item.remote_wakeup_channel 
                                = config.remote_wakeup_channel;
    local_nv_item.remote_wakeup_source 
                                = config.remote_wakeup_source;
    
    if (NV_OK != NV_WriteEx(MODEM_ID_0,
        en_NV_Item_WAKEUP_CFG_FLAG, &local_nv_item, sizeof(local_nv_item)))
    {
        return FALSE;
    }

    return TRUE;
}

/*===========================================================================
FUNCTION 
    huawei_pm_disable_wakeup_pin
DESCRIPTION
    Set disable pull WAKEUP_OUT PIN flag.
DEPENDENCIES
    None
RETURN VALUE
    None.
SIDE EFFECTS
    None
===========================================================================*/
void huawei_pm_disable_wakeup_pin(boolean is_disable)
{
    g_huawei_pm_status.is_pm_pin_disable = is_disable;
}
 
/*===========================================================================
FUNCTION 
    huawei_pm_request_wakeup_pin
DESCRIPTION
    Wakeup the Host through WAKEUP_OUT PIN
DEPENDENCIES
    None
RETURN VALUE
    None.
SIDE EFFECTS
    None
===========================================================================*/
void huawei_pm_request_wakeup_pin(void)
{
    if (TRUE == g_huawei_pm_status.is_pm_pin_disable)
    {
        return;
    }

    /*If the timer starting,We do nothing*/
    if (FALSE == g_huawei_pm_status.pin_timer.is_start)
    {
        /* Ͷ����Ʊ����֤�����ܹ��������1��PIN�ߵ�ƽ�źţ�
          *���򵥰�������PIN���޷����ָ����� */
        wake_lock(&remote_wakeup_pin_lock);
        g_huawei_pm_status.pin_timer.is_start = TRUE;
        gpio_direction_output(WAKEUP_OUT, GPIO_HIGH_VALUE);
        /*Start timer*/
        schedule_delayed_work(&g_huawei_pm_status.pin_timer.work, \
            msecs_to_jiffies(WAKEUP_OUT_TIMEOUT));
    }
}

/*****************************************************************************
* �� �� ��  : huawei_pm_remote_wakeup_time_update
* ��������  : ����˯�߻��ѵ����ֳ�ʱʱ��
* �������  : timeValue -- ����ʱ��
* �������  : NA
* �� �� ֵ  : NA
*****************************************************************************/
void huawei_pm_remote_wakeup_time_update(unsigned int timeValue)
{
    remote_wakeup_shake_time = timeValue;
}

/*****************************************************************************
* �� �� ��  : huawei_pm_remote_wakeup_time_get
* ��������  : ��ȡ˯�߻��ѵ����ֳ�ʱʱ��ֵ
* �������  : NA
* �������  : NA
* �� �� ֵ  : remote_wakeup_shake_time -- ��ʱʱ��
*****************************************************************************/
unsigned int huawei_pm_remote_wakeup_time_get(void)
{
    return remote_wakeup_shake_time;
}

/*****************************************************************************
* �� �� ��  : huawei_pm_remote_wakeup_timerout_cb
* ��������  : ���ֳ�ʱ����
* �������  : NA
* �������  : NA
* �� �� ֵ  : NA
*****************************************************************************/
void huawei_pm_remote_wakeup_timerout_cb(struct work_struct *w)
{
    unsigned long flags = 0;

    if(NULL == w)
    {
        return;
    }

    /* ��ֹshake_timeout_work_flag��־λ��д���� */
    spin_lock_irqsave(&shake_timeout_work_spin, flags);
    /*�����־*/
    shake_timeout_work_flag = FALSE;
    spin_unlock_irqrestore(&shake_timeout_work_spin, flags);

    /*֪ͨA/C�ˣ���ʼ���ͻ�������*/
    huawei_pm_remote_shake_complete();

    /*��ʱ500ms����֤�����ܹ����ͳ�ȥ����Ͷ�޳�Ʊ������˯��*/
    msleep(500);
    spin_lock_irqsave(&shake_timeout_work_spin, flags);
    /* ������������������ź������˶�ʱ�����򲻷��� */
    if (FALSE == shake_timeout_work_flag)
    {
        /*���ݷ�����ɺ�Ͷ�޳�Ʊ����˯��*/
        wake_unlock(&flush_cache_lock);
    }
    spin_unlock_irqrestore(&shake_timeout_work_spin, flags);
}

/*****************************************************************************
* �� �� ��  : huawei_pm_remote_wakeup_timer
* ��������  : �����ֳ�ʱ��ʱ��
* �������  : NA
* �������  : NA
* �� �� ֵ  : NA
*****************************************************************************/
void huawei_pm_remote_wakeup_timer(void)
{
    unsigned long flags = 0;

    /* ��ֹshake_timeout_work_flag��־λ��д���� */
    spin_lock_irqsave(&shake_timeout_work_spin, flags);
    /*����ʱ�������������ظ���ʱ��*/
    if(TRUE == shake_timeout_work_flag)
    {
        spin_unlock_irqrestore(&shake_timeout_work_spin, flags);
        return;
    }
    spin_unlock_irqrestore(&shake_timeout_work_spin, flags);

    /*ʹ�ð�����ͶƱ��Ͷ����Ʊ����֤�ڶ�ʱ��ʱ����ڵ��岻˯��*/
    wake_lock(&flush_cache_lock);

    /*Start timer*/
    schedule_delayed_work(&shake_timeout_work, \
        msecs_to_jiffies(remote_wakeup_shake_time));

    spin_lock_irqsave(&shake_timeout_work_spin, flags);
    shake_timeout_work_flag = TRUE;
    spin_unlock_irqrestore(&shake_timeout_work_spin, flags);
}

/*****************************************************************************
* �� �� ��  : huawei_pm_remote_wakeup_timer
* ��������  : ������ɵĴ�����
* �������  : NA
* �������  : NA
* �� �� ֵ  : NA
*****************************************************************************/
void huawei_pm_remote_shake_complete(void)
{
    /*����USB��ͨ��״̬Ϊ��������ֵ������wakeup_in���ѵ���ʱ�����޷�ͨ��USB�ϱ�*/
    huawei_pm_update_wakeup_channel_status();
    /*֪ͨA/C�ˣ���ʼ���ͻ�������*/
    schedule_delayed_work(&usb_resume_acore_work, 0);
    schedule_delayed_work(&usb_resume_ccore_work, 0);
}

/*===========================================================================
FUNCTION 
    huawei_pm_get_usb_suspended_status
DESCRIPTION
    Get USB Suspend Status.
DEPENDENCIES
    None
RETURN VALUE
    if USB has enter SUSPEND then return TRUE,
    else return FALSE;
SIDE EFFECTS
    None
===========================================================================*/
boolean huawei_pm_get_usb_suspended_status(void)
{
    PRINT_ACORE_LOG(g_PmAcorePrintOnOff," is_usb_suspend is %d!\n",\
                                 g_huawei_pm_status.is_usb_suspend );

    return g_huawei_pm_status.is_usb_suspend;
}

/*===========================================================================
* �� �� ��  : huawei_pm_get_pin_suspended_status
* ��������  : ͨ��PIN���ַ�ʽ��ȡ��λ����˯��״̬
* �������  : NA
* �������  : NA
* �� �� ֵ  : 1-��λ��˯�ߣ�0-��λ����˯��
===========================================================================*/
unsigned int huawei_pm_get_pin_suspended_status(void)
{
    BSP_U32 pin_status = 0;
    pm_status_s *pm_st = (pm_status_s *)(SRAM_MODULE_PM_ADDR);

    pin_status = pm_st->pin_status;

    if( PIN_SUSPEND_STATUS ==  pin_status )
    {
        /*lint -e506*/
        PRINT_ACORE_LOG(g_PmAcorePrintOnOff, "uart suspend!\n");
        /*lint +e506*/
        return 1;
    }
    if( PIN_RESUME_STATUS ==  pin_status )
    {
        /*lint -e506*/
        PRINT_ACORE_LOG(g_PmAcorePrintOnOff, "uart resumed!\n");
        /*lint +e506*/
        return 0;
    }

    PRINT_ACORE_LOG(PM_MSG_PRINT_ON,"GET MEMORY_PIN_STATUS_ADDR ERROR!\n");

    return 1;
}
/*===========================================================================
FUNCTION 
    huawei_pm_set_pin_status
DESCRIPTION
    ����wakeupin PIN״̬
DEPENDENCIES
    
RETURN VALUE
    NO
SIDE EFFECTS
    None
===========================================================================*/
void huawei_pm_set_pin_status(int pin_status )
{
    pm_status_s *pm_st = (pm_status_s *)(SRAM_MODULE_PM_ADDR);
    if (FALSE == pin_status)
    {
         pm_st->pin_status = PIN_SUSPEND_STATUS; 
         PRINT_ACORE_LOG(g_PmAcorePrintOnOff, "pin suspend!\n");
    }
    else
    {
        pm_st->pin_status = PIN_RESUME_STATUS; 
        PRINT_ACORE_LOG(g_PmAcorePrintOnOff, "pin resumed!\n");
    }
}
/*===========================================================================
FUNCTION 
    huawei_pm_set_usb_enum_status
DESCRIPTION
    Set USB has been enumerated.
DEPENDENCIES
    None
RETURN VALUE
    None
SIDE EFFECTS
    None
 ===========================================================================*/
void huawei_pm_set_usb_enum_status(void)
{
    pm_status_s *pm_st = (pm_status_s *)(SRAM_MODULE_PM_ADDR);
    pm_st->usb_status = USB_RESUME_STATUS;

    g_huawei_pm_status.is_usb_enum = TRUE;
    g_huawei_pm_status.is_usb_suspend = FALSE;
    //todo: BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_USB);
}

/*===========================================================================
FUNCTION 
    huawei_pm_handle_usb_resume_complete
DESCRIPTION
    Do something when USB resume complete
DEPENDENCIES
    None
RETURN VALUE
    None.
SIDE EFFECTS
    None
===========================================================================*/
void huawei_pm_handle_usb_resume_complete(void)
{
    unsigned char pin_channel = FALSE;
    pm_status_s *pm_st = (pm_status_s *)(SRAM_MODULE_PM_ADDR);

    if (TRUE == g_huawei_pm_status.is_usb_suspend)
    {
        printk(KERN_ERR "%s:usb resumed !!!\n",__FUNCTION__);
        g_huawei_pm_status.is_usb_suspend = FALSE;        
        /*Update wakeup channel mask*/
        g_huawei_pm_status.rmt_wake_host_mask 
                = g_huawei_pm_status.rmt_wk_cfg.remote_wakeup_channel;

        pm_st->usb_status = USB_RESUME_STATUS;

        /*USB resumeʱ���ж������Ƿ�����PINΪԶ�̻���ͨ����
         ��PIN�ǣ��򲻶Ի������ݽ����ϱ�������֮Ҫ�ϱ�*/
        pin_channel = huawei_pm_get_wakeup_channel_status(RMT_WK_CHN_PIN);
        if(TRUE == pin_channel)
        {
            return;
        }
        
        huawei_pm_remote_shake_complete();
    }
    return;

}

static void huawei_pm_usb_resume_acore_cb(struct work_struct *w)
{
    /*Indicate Flush all stored response*/
    huawei_pm_rsp_inform_flush_cache_acore();
}
static void huawei_pm_usb_resume_ccore_cb(struct work_struct *w)
{
    int ret = 0;
    int value = PM_MSG_FLUSH_CACHE;
    unsigned int icc_channel_id = ICC_CHN_IFC << 16 | IFC_RECV_FUNC_M2M_PM;

    ret = bsp_icc_send(ICC_CPU_MODEM, icc_channel_id, (u8*)&value, sizeof(value));
    if (ret != (int)sizeof(value))
    {
        PRINT_ACORE_LOG(PM_MSG_PRINT_ON,"send Ccore flush cache msg fail !!!\n");
    }
}

/*===========================================================================
FUNCTION 
    huawei_pm_handle_usb_suspend
DESCRIPTION
    Do something when USB suspend.
DEPENDENCIES
    None.
RETURN VALUE
    None.
SIDE EFFECTS
    None
===========================================================================*/
void huawei_pm_handle_usb_suspend(void)
{
    pm_status_s *pm_st = (pm_status_s *)(SRAM_MODULE_PM_ADDR);

    if (FALSE == g_huawei_pm_status.is_usb_suspend)
    {
        printk(KERN_ERR "%s:usb suspend !!!\n",__FUNCTION__);
        g_huawei_pm_status.is_usb_suspend = TRUE;
        
        /*Disable USB wakeup channel*/
        g_huawei_pm_status.rmt_wake_host_mask &= ~RMT_WK_CHN_USB;
        pm_st->usb_status = USB_SUSPEND_STATUS;
    }
    return;
}

/*===========================================================================
FUNCTION 
    huawei_pm_get_wakeup_channel_status
DESCRIPTION
    Get wakeup channels status.
DEPENDENCIES
    None
RETURN VALUE
    TRUE:   permit
    FALSE:  forbid
SIDE EFFECTS
    None
===========================================================================*/
boolean huawei_pm_get_wakeup_channel_status(rmt_wk_chl_type channel)
{
    if (channel == (g_huawei_pm_status.rmt_wk_cfg.remote_wakeup_channel & channel))
    {
        return TRUE;
    }
    
    return FALSE;    
}
int huawei_pm_get_usb_channel_status(void)
{
    if (RMT_WK_CHN_USB == (g_huawei_pm_status.rmt_wake_host_mask & RMT_WK_CHN_USB))
    {
        return TRUE;
    }
    
    return FALSE;    
}

/*===========================================================================
FUNCTION 
    huawei_pm_update_wakeup_channel_status
DESCRIPTION
    Update wakeup channels status.
DEPENDENCIES
    None
RETURN VALUE
    None
SIDE EFFECTS
    None
===========================================================================*/
void huawei_pm_update_wakeup_channel_status(void)
{
    /*Update wakeup channel mask*/
    g_huawei_pm_status.rmt_wake_host_mask 
                = g_huawei_pm_status.rmt_wk_cfg.remote_wakeup_channel;  
}

/*===========================================================================
FUNCTION
    huawei_pm_sleep_status_output
DESCRIPTION
    SLEEP_STATUS pin output
DEPENDENCIES
    None
RETURN VALUE
    None
SIDE EFFECTS
    None
===========================================================================*/
void huawei_pm_sleep_status_output(int  level)
{
    gpio_direction_output(SLEEP_STATUS, level);
}
/******************************************************************************
Function:       huawei_pm_remote_wakuep_update
Description:   update remotewakeup info
Input:           None
Output:         None
Return:         None
Others:         None
******************************************************************************/
void huawei_pm_remote_wakuep_update(remote_wakeup_info_type src_config)
{
    g_huawei_pm_status.rmt_wk_cfg = src_config;

    g_huawei_pm_status.rmt_wake_host_mask 
        = src_config.remote_wakeup_channel;
}
/******************************************************************************
Function:       huawei_pm_save_curc_info
Description:   save curc info
Input:           None
Output:         None
Return:         None
Others:         None
******************************************************************************/
void huawei_pm_save_curc_info(dsat_curc_cfg_type *src_curc)
{
    if( NULL == src_curc )
    {
        return;
    }
    g_curc_info.ctrl_status = src_curc->ctrl_status;

    return;
}
/******************************************************************************
Function:       huawei_pm_save_curc_info
Description:   save curc info
Input:           None
Output:         None
Return:         None
Others:         None
******************************************************************************/
void huawei_pm_get_curc_info(dsat_curc_cfg_type *dst_curc)
{
    if( NULL == dst_curc )
    {
        return;
    }
    dst_curc->ctrl_status = g_curc_info.ctrl_status;
}

/******************************************************************************
Function:       huawei_pm_get_curc_status
Description:   
Input:           huawei_pm_rsp_id_type send_id
Output:         None
Return:          dsat_curc_status
Others:          None
******************************************************************************/
dsat_curc_status huawei_pm_get_curc_status(void)
{
    return g_curc_info.ctrl_status;
}
/******************************************************************************
Function:       huawei_pm_wakeup_remote_host
Description:   
Input:           rmt_wk_src_type rmt_wk_src
Output:         None
Return:          boolean wether the src can wakeup host
Others:          None
******************************************************************************/
boolean huawei_pm_wakeup_remote_host(rmt_wk_src_type rmt_wk_src)
{
    /*Check wakeup source validity*/    
    if ((REMOTE_WAKEUP_OFF 
            == g_huawei_pm_status.rmt_wk_cfg.remote_wakeup_ctrl) 
            || (rmt_wk_src 
            != (rmt_wk_src 
            & g_huawei_pm_status.rmt_wk_cfg.remote_wakeup_source)) 
            || ( 0 == g_huawei_pm_status.rmt_wk_cfg.remote_wakeup_channel ))
    {
        return FALSE;
    }

    return TRUE;
}

/*****************************************************************************
 �� �� ��  : bsp_pm_read_cb
 ��������  : A�˺˼�ص�����
 �������  :
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :
*****************************************************************************/
static s32 bsp_pm_read_cb( void )
{
    int read_len = 0;
    int value;
    u32 channel_id = ICC_CHN_IFC << 16 | IFC_RECV_FUNC_M2M_PM;

    read_len = bsp_icc_read(channel_id, (u8*)&value, sizeof(value));
    if(read_len != (int)sizeof(value))
    {
        PRINT_ACORE_LOG(PM_MSG_PRINT_ON,"read len(%x) != expected len(%x) !\n",read_len,sizeof(value));

        return -1;
    }
    
    PRINT_ACORE_LOG(g_PmAcorePrintOnOff, "bsp_pm_read_cb: value = %d \n", value);

    switch(value)
    {
        case PM_MSG_RMT_CHANNEL_SET:
            huawei_pm_update_wakeup_channel_status();
            break;
        case PM_MSG_WAKEUP_PIN_SET:
            huawei_pm_request_wakeup_pin();
            break;
        case PM_MSG_WAKEUP_TIMER_SET:
            huawei_pm_remote_wakeup_timer();
            break;
        default:
            PRINT_ACORE_LOG(PM_MSG_PRINT_ON," invalid para !!! \n");
            break;
    }

    return 0;
}
/******************************************************************************
Function:       huawei_pm_bsp_ifc_init
Description:   ��ʼ��˫��ͨ�Žӿ�
Input:           None
Output:         None
Return:         uint32
Others:         None
******************************************************************************/
int huawei_pm_bsp_ifc_init(void)
{

    int ret = 0;
    BSP_U32 icc_channel_id = ICC_CHN_IFC << 16 | IFC_RECV_FUNC_M2M_PM;
    
    ret |= bsp_icc_event_register(icc_channel_id, \
        (read_cb_func)bsp_pm_read_cb, NULL, NULL, NULL);

    return ret;
}

/*===========================================================================
FUNCTION 
    huawei_pm_remote_wakeupcfg_init
DESCRIPTION
    Init Remote Wakuep Configuration. If NV read failure then use the default
    setting.
DEPENDENCIES
    None
RETURN VALUE
    None
SIDE EFFECTS
    None
===========================================================================*/
void huawei_pm_remote_wakeupcfg_init(void)
{
    /* status var for reading from NV */
    /* NV interface data */
    remote_wakeup_info_type local_config = {0};
      
    /*If NV read failure then use the default*/
    if (NV_OK == NV_ReadEx(MODEM_ID_0, en_NV_Item_WAKEUP_CFG_FLAG, 
                                    &local_config, sizeof(local_config)))
    {
        /*Check the range of remote wakeup configuration*/
        if ((REMOTE_WAKEUP_ON != local_config.remote_wakeup_ctrl)
            && (REMOTE_WAKEUP_OFF != local_config.remote_wakeup_ctrl))
        {
            /*The NV values out of range,Use global value update it*/
            (void)huawei_pm_udpate_nv_configuration
                    (g_huawei_pm_status.rmt_wk_cfg);

        }
        else if(RMT_WK_CHN_MAX < local_config.remote_wakeup_channel)
        {
            /*The NV values out of range,Use global value update it*/
            (void)huawei_pm_udpate_nv_configuration
                    (g_huawei_pm_status.rmt_wk_cfg);        
        }
        else if(RMT_WK_SRC_MAX < local_config.remote_wakeup_source)
        {
            /*The NV values out of range,Use global value update it*/
            (void)huawei_pm_udpate_nv_configuration
                    (g_huawei_pm_status.rmt_wk_cfg);        
        }        
        else
        {
            /*Use NV values*/
            g_huawei_pm_status.rmt_wk_cfg = local_config;
            /*Update channel mask*/
            g_huawei_pm_status.rmt_wake_host_mask 
                = local_config.remote_wakeup_channel;
        }
    }/*end if (NV_DONE_S == nv_status)*/
    else
    {
        PRINT_ACORE_LOG(g_PmAcorePrintOnOff, "read wakeupcfg nv fail !!! \n");
    }
}
/******************************************************************************
Function:       huawei_pm_wakeupout_gpio_init
Description:   Initialize the wakeup_out gpio
Input:           None
Output:         None
Return:         int, 0 = success, -1 = failed
Others:         None
******************************************************************************/
int huawei_pm_sleepstatus_gpio_init(void)
{
    int ret = 0;
    ret = gpio_request(SLEEP_STATUS, LABEL_SLEEP_STATUS);
    if(ret)
    {
        PRINT_ACORE_LOG(g_PmAcorePrintOnOff, "request SLEEP_STATUS fail  \n");
        goto fail1;
    }
    gpio_direction_output(SLEEP_STATUS, GPIO_HIGH_VALUE); 

    return 0;
fail1:
    return -1;
}

/******************************************************************************
Function:       huawei_pm_wakeupout_gpio_init
Description:   Initialize the wakeup_out gpio
Input:           None
Output:         None
Return:         int, 0 = success, -1 = failed
Others:         None
******************************************************************************/
int huawei_pm_wakeupout_gpio_init(void)
{
    int ret = 0;

    /*���øùܽ�ΪGPIO������־��GPIO�Ѿ���ʹ��*/
    ret = gpio_request(WAKEUP_OUT, LABEL_WAKEUP_OUT);

    if(ret)
    {
        PRINT_ACORE_LOG(g_PmAcorePrintOnOff, "request WAKEUP_OUT fail  \n");
        goto fail1;
    }
    
    /*����GPIOΪ������͵�ƽ*/
    gpio_direction_output(WAKEUP_OUT, GPIO_LOW_VALUE); 
    return 0;

fail1:
    return -1;
}

/******************************************************************************
Function:       huawei_pm_wakeupin_gpio_init
Description:   Initialize the wakeup_in gpio
Input:           None
Output:         None
Return:         int, 0 = success, -1 = failed
Others:         None
******************************************************************************/
int huawei_pm_wakeupin_gpio_init(void)
{
    int ret = 0;
    int level = 0;
    unsigned int int_num;
    int value = 0; 
    pm_status_s *pm_st = (pm_status_s *)(SRAM_MODULE_PM_ADDR);

    ret = gpio_request(WAKEUP_IN, LABEL_WAKEUP_IN);
    if(ret)
    {
        PRINT_ACORE_LOG(g_PmAcorePrintOnOff, "request WAKEUP_IN fail  \n");
        goto fail1;
    }
    /*����GPIOΪ����*/
    gpio_direction_input(WAKEUP_IN);

    /*����GPIOΪ�жϷ�ʽ*/
    level = gpio_get_value(WAKEUP_IN);
    g_wakeupin_ctx.wakeup_level = level;
    /*����GPIO�����жϴ�����ʽ*/
    if(level)
    {
        /*�ߵ�ƽ������������*/
        wake_lock(&wakeup_in_lock);
        pm_st->pin_status = PIN_RESUME_STATUS;
    }
    else
    {
        pm_st->pin_status = PIN_SUSPEND_STATUS;
        /*�͵�ƽ����������*/
        wake_unlock(&wakeup_in_lock);
    }

    int_num = gpio_to_irq(WAKEUP_IN);
    /*�����жϺŶ�Ӧ�Ļص�����*/
    ret = request_irq(int_num, \
            (irq_handler_t) huawei_pm_sleep_in_event_isr, 
            IRQF_NO_SUSPEND | IRQF_SHARED | IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, 
            LABEL_WAKEUP_IN, &value);
    if (ret) 
    {
        PRINT_ACORE_LOG(g_PmAcorePrintOnOff,"request WAKEUP_IN irq fail  \n");
        goto fail2;
    }
#ifdef BSP_CONFIG_BOARD_TELEMATIC
    /* ����wakeupin GPIO���û��ռ�ʹ�� */
    ret = gpio_export(WAKEUP_IN, 0);
    if (0 != ret)
    {
        PRINT_ACORE_LOG(g_PmAcorePrintOnOff,"export WAKEUP_IN to userspace fail  \n");
    }
#endif
    return 0;

fail2:
    gpio_free(WAKEUP_IN);
fail1:
    return -1;
}

#ifdef BSP_CONFIG_BOARD_TELEMATIC
/*****************************************************************************
 �� �� ��  : huawei_pm_status_get
 ��������  : ��ѯ��Դ����ģ���Ƿ��ʼ�����
 �������  : none
 �������  : none
 �� �� ֵ  : HW_PM_INIT_STATUS
*****************************************************************************/
HW_PM_INIT_STATUS huawei_pm_status_get(void)
{
    return g_huawei_pm_init;
}
#endif

/******************************************************************************
Function:       huawei_pm_init
Description:   Initialize huawei PM
Input:           None
Output:         None
Return:          None
Others:         None
******************************************************************************/
int huawei_pm_init(void)
{
    int error = 0;
#ifdef BSP_CONFIG_BOARD_TELEMATIC
    int is_vbus_connect = 1;
#endif
    int is_wakeup_timer_init = 0;
    int is_bsp_ifc_init = 0;
    pm_status_s *pm_st = (pm_status_s *)(SRAM_MODULE_PM_ADDR);
    nv_huawei_sleepcfg_type Huawei_sleep_cfg;
    
    MDRV_VER_SOLUTION_TYPE module_type = MDRV_VER_TYPE_M2M;
    module_type = mdrv_ver_get_solution_type();
#ifdef BSP_CONFIG_BOARD_TELEMATIC
/* �ж�VBUS�����Ƿ��������VBUS����wakeupin�ܽŵ��ж��Լ���ʼ���������dwc3_conn_tbox_balong.c���棬
USBģ����ϵ��ʼ��������wakepupin�ܽŵ��жϣ���������ԡ����VBUS�رգ����ڴ˴���ʼ����USB��wakeupin���� */
    is_vbus_connect = bsp_usb_is_vbus_connect();
#endif
   
    PRINT_ACORE_LOG(g_PmAcorePrintOnOff,"begin module_pm init... \n");

    /*��ʼ��ʱ�Թ����ڴ�ֵ���г�ֵ����*/
#ifdef BSP_CONFIG_BOARD_TELEMATIC
    if (0 == is_vbus_connect)
#endif
    {
         pm_st->pin_status = PIN_SUSPEND_STATUS;
    }

    /* ��USBδö�ٻ��߹����ڴ治Ϊsuspend/resume״̬ʱ���������ڴ����Ϊ0 */
    if ((!g_huawei_pm_status.is_usb_enum)
        || (( USB_SUSPEND_STATUS != pm_st->usb_status ) && ( USB_RESUME_STATUS != pm_st->usb_status )))
    {
        pm_st->usb_status = 0;
    }

    (void)memset(&Huawei_sleep_cfg, 0, sizeof(nv_huawei_sleepcfg_type));

    INIT_DELAYED_WORK(&usb_resume_acore_work, 
                        huawei_pm_usb_resume_acore_cb);
    INIT_DELAYED_WORK(&usb_resume_ccore_work, 
                        huawei_pm_usb_resume_ccore_cb);
    
    INIT_DELAYED_WORK(&g_huawei_pm_status.pin_timer.work, 
                        huawei_pm_request_wakeup_pin_cb);
    
    INIT_DELAYED_WORK(&shake_timeout_work, 
                        huawei_pm_remote_wakeup_timerout_cb);

    /* ��ֹ���Զ�̻����ź�1s�ߵ�ƽ������ϵͳ˯�� */
    wake_lock_init(&remote_wakeup_pin_lock, WAKE_LOCK_SUSPEND, "pm_remotepin");
#ifdef BSP_CONFIG_BOARD_TELEMATIC
    if (0 == is_vbus_connect)
#endif
    {
        /* ������Ӧwakeup_in����˯�ߵ���ϵͳ*/
        wake_lock_init(&wakeup_in_lock, WAKE_LOCK_SUSPEND, "pm_wakeupin");
    }

    /* ��ֹ����pm buffer�������ݹ�����ϵͳ˯��*/
    wake_lock_init(&flush_cache_lock, WAKE_LOCK_SUSPEND, "pm_flush_cache");

#if(FEATURE_ON == MBB_FACTORY)
    wake_lock_init(&factory_lock, WAKE_LOCK_SUSPEND, "pm_factory_lock");
    /*��Ƭ�汾����˯�ߣ�Ĭ�ϳ���*/
    wake_lock(&factory_lock);
#endif
    /* ͨ��NV50467��ȡԶ�̻������ֳ�ʱ��ʱ�� */
    if (NV_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_SLEEP_CFG_FLAG, 
                           &Huawei_sleep_cfg, sizeof(Huawei_sleep_cfg)))
    {
        PRINT_ACORE_LOG(PM_MSG_PRINT_ON, "read sleepcfg NV fail !\n");
    }
    else
    {
        remote_wakeup_shake_time = \
            Huawei_sleep_cfg.huawei_sleep_cfg[0].sleepcfg_value;
    }

    /*config the WAKEUP_OUT GPIO*/
    error = huawei_pm_wakeupout_gpio_init();
    if ( error )
    {
        PRINT_ACORE_LOG(g_PmAcorePrintOnOff,"wakeup_out init fail  \n");
        goto fail1;
    }

    /* CE û��Sleep_status/WAKEUP_IN�ܽ� */
    if(MDRV_VER_TYPE_CE == module_type)
    {
        PRINT_ACORE_LOG(PM_MSG_PRINT_ON,"CE no Sleep_status and WAKEUP_IN pin !! \n");
    }
    else
    {
#ifdef BSP_CONFIG_BOARD_TELEMATIC
        if (0 == is_vbus_connect)
#endif
        {
            is_wakeup_timer_init = wakeupin_timer_init();
            if (0 != is_wakeup_timer_init)
            {
                printk(KERN_ERR "%s(): timer init fail. \n",__FUNCTION__);
                goto fail1;
            }
        }
        /*config the Sleep_status GPIO*/
        error = huawei_pm_sleepstatus_gpio_init();
        if(error)
        {
            PRINT_ACORE_LOG(g_PmAcorePrintOnOff,"sleep_status init fail  \n");
            goto fail1;
        }
#ifdef BSP_CONFIG_BOARD_TELEMATIC
        if (0 == is_vbus_connect)
#endif
        {
            /*config the WAKEUP_IN GPIO*/
            error = huawei_pm_wakeupin_gpio_init();
            if ( error )
            {
                PRINT_ACORE_LOG(g_PmAcorePrintOnOff,"wakeup_in init fail  \n");
                goto fail1;
            }
        }
    }
    
    huawei_pm_remote_wakeupcfg_init();

    is_bsp_ifc_init = huawei_pm_bsp_ifc_init();
    if ( BSP_ERROR == is_bsp_ifc_init)
    {
        goto fail1;
    }

#ifdef BSP_CONFIG_BOARD_TELEMATIC
    g_huawei_pm_init = HUAWEI_PM_INIT_OK;
#endif
    return 0;
fail1:
#ifdef BSP_CONFIG_BOARD_TELEMATIC
    g_huawei_pm_init = HUAWEI_PM_INIT_FAIL;
#endif
    return -1;
}

module_init(huawei_pm_init);


