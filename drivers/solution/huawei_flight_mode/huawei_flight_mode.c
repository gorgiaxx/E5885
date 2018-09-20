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
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/wakelock.h>
#include <bsp_nvim.h>
#include <bsp_icc.h>
#include "product_nv_id.h"
#include "product_nv_def.h"
#include "bsp_version.h"
#include "huawei_flight_mode.h"
#include "mbb_flight_mode.h"

#if (FEATURE_ON == MBB_DLOAD)
#include "bsp_sram.h"
#endif /*MBB_DLOAD*/

/*Ӳ�������Ķ�ʱ��*/
static RFSWITCH_T_CTRL_S g_wdis_ctx = {0};
/*�л���Ƶ״̬ʱ��˯����*/
static struct wake_lock rfswitch_pin_lock;

/*����ģʽ״̬ȫ�ֱ���������Ӳ�����ع�ͬ����*/
static unsigned int g_rfswitch_state = 0;
/*�¶ȱ���״̬ȫ�ֱ���*/
static unsigned int g_therm_rf_off = FALSE;

/*����ģʽ�л��������ʹ洢���г��ȡ�Ӳ���������÷���ģʽ����ִ��ʱ��ԼΪ3S��
  Ӳ�����صķ�����ʱ��Ϊ200ms,��ֹ���б�ռ��*/

static queue_trig queue_trigs[TRIG_QUEUE_LENGTH];

static unsigned int queue_trig_head = 0; /*��ͷ*/
static unsigned int queue_trig_tail = 0; /*��β*/

/*���������Զ��в������м�������*/
static spinlock_t trig_lock;



/*****************************************************************************
�� �� ��  : rfswitch_trig_queue_in
��������  : ����ģʽ������ʽ�����
�������  : trig_type ������ʽ
�������  : ��
�� �� ֵ  : ��
*****************************************************************************/
void rfswitch_trig_queue_in(rfswitch_trig_type trig_type)
{
    unsigned long flags = 0;

    spin_lock_irqsave(&trig_lock, flags);
    if (0 == queue_trigs[queue_trig_tail].is_set)
    {
        /*����β��δ�����ñ�������δ����ִ����Ӳ���*/
        queue_trigs[queue_trig_tail].trig_type = trig_type;
        queue_trigs[queue_trig_tail].is_set = 1;
        queue_trig_tail = (queue_trig_tail + 1) % TRIG_QUEUE_LENGTH;
        spin_unlock_irqrestore(&trig_lock, flags);
        return ;
    }
    else
    {
        /* �������� */
        printk(KERN_ERR "\n%s() The Queue Is Full!",__FUNCTION__);
    }
    spin_unlock_irqrestore(&trig_lock, flags);
}

/*****************************************************************************
�� �� ��  : RfSwitch_Trig_Queue_Out
��������  : ����ģʽ������ʽ������
�������  : void
�������  : ��
�� �� ֵ  : ��
*****************************************************************************/
void rfswitch_trig_queue_out(void)
{
    unsigned long flags = 0;

    spin_lock_irqsave(&trig_lock, flags);
    if(queue_trig_head != queue_trig_tail)
    {
        /*�жϵ�ǰ�����Ƿ�Ϊ�գ���Ϊ���򽫶�ͷ��ʶ���ơ�*/
        queue_trigs[queue_trig_head].is_set = 0;
        queue_trig_head = (queue_trig_head + 1) % TRIG_QUEUE_LENGTH;
    }
    else
    {
        /*����Ϊ��*/
        printk(KERN_ERR "\n%s(): The Queue is Empty!", __FUNCTION__);
    }

    spin_unlock_irqrestore(&trig_lock, flags);
}

/*****************************************************************************
�� �� ��  : rfswitch_trig_queue_get
��������  : �ӷ���ģʽ������ʽ�����л�ȡһ��������ʽ
�������  : void
�������  : ��
�� �� ֵ  : ������ʽ
*****************************************************************************/
rfswitch_trig_type rfswitch_trig_queue_get(void)
{
    unsigned long flags = 0;

    /*����*/
    spin_lock_irqsave(&trig_lock, flags);
    if ((0 != queue_trigs[queue_trig_head].is_set))
    {
        /*����*/
        spin_unlock_irqrestore(&trig_lock, flags);
        /*��ǰ��ͷ�Ĵ��������Ƿ���Ч����Ч�򷵻ض�ͷ��������*/
        return queue_trigs[queue_trig_head].trig_type;
    }
    else
    {
        /*����*/
        spin_unlock_irqrestore(&trig_lock, flags);
        return RFSWITCH_NONE_TRIG;
    }
}


/*****************************************************************************
�� �� ��  : rfswitch_therm_state_set
��������  : ����ģʽ�¶ȱ���״̬����
�������  : state:��Ҫ���õ�״̬
�������  : ��
�� �� ֵ  : ��
*****************************************************************************/
static void rfswitch_therm_state_set(unsigned int state)
{
    g_therm_rf_off = state;
}

/*****************************************************************************
�� �� ��  : rfswitch_therm_state_get
��������  : ����ģʽ�¶ȱ���״̬����
�������  : ��
�������  : ��
�� �� ֵ  : ��ǰ�¶ȱ���״̬
*****************************************************************************/
unsigned int rfswitch_therm_state_get(void)
{
    return g_therm_rf_off;
}
/*****************************************************************************
�� �� ��  : rfswitch_sw_set
��������  : ����ģʽ�����������
�������  : swstate�������״̬
�������  : ��
�� �� ֵ  : ��
*****************************************************************************/
void rfswitch_sw_set(unsigned int swstate)
{
    int ret = 0;
    FLIGHT_MODE_STRU  swswitchPara;     
    swswitchPara.huawei_flight_mode = swstate;

    ret = bsp_nvm_write(NV_ID_FLIGHT_MODE,
                       (unsigned char *)&(swswitchPara),
                        sizeof(swswitchPara));
    if(0 != ret)
    {
        printk(KERN_ERR "write sw state to nv failed\n");
    }
    return;
}
/*****************************************************************************
�� �� ��  : rfswitch_sw_get
��������  : ����ģʽ������ز�ѯ
�������  : ��
�������  : ��
�� �� ֵ  : 0:������ش򿪣��ر���Ƶ
            1:������عرգ�����Ƶ
*****************************************************************************/
unsigned int rfswitch_sw_get(void)
{
    unsigned int ret = 0;
    FLIGHT_MODE_STRU  swswitchPara;

    (void)memset(&swswitchPara, 0x1, sizeof(swswitchPara));
    ret = bsp_nvm_read(NV_ID_FLIGHT_MODE,
                       (unsigned char *)&(swswitchPara),
                       sizeof(swswitchPara));
    if(0 != ret)
    {
        printk(KERN_ERR "read sw state from nv failed\n");
    }
    return swswitchPara.huawei_flight_mode;
}

/*****************************************************************************
�� �� ��  : rfswitch_hw_get
��������  : ��ȡ����ģʽ����GPIO״̬a�˵���
�������  : ��
�������  : ��
�� �� ֵ  : 0:Ӳ�����ش򿪷���ģʽ���ر���Ƶ
            1:Ӳ�����عرշ���ģʽ������Ƶ
*****************************************************************************/
unsigned int rfswitch_hw_get(void)
{
    int value = 1;
    value = gpio_get_value(W_DISABLE_PIN);
    if ( 0 == value )
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/*****************************************************************************
�� �� ��  : rfswitch_State_Set
��������  : ���÷���ģʽ״̬
�������  : rf_state ��Ƶģʽ
�������  : ��
�� �� ֵ  : ��
*****************************************************************************/
void rfswitch_state_set(unsigned int rf_state)
{
    if(RF_ON != rf_state && RF_OFF != rf_state)
    {
        printk(KERN_ERR "<rfswitch_state_set>para is error:rf_state=%d\r\n",rf_state);
        rf_state = RF_ON;
    }
    g_rfswitch_state = rf_state;
}

/*****************************************************************************
�� �� ��  : rfswitch_state_get
��������  : ��ѯ����ģʽ״̬
�������  : ��
�������  : ��
�� �� ֵ  : g_rfswitch_state
*****************************************************************************/
unsigned int rfswitch_state_get(void)
{
    return g_rfswitch_state;
}


/*****************************************************************************
�� �� ��  : rfswitch_Change_Mode_To_State
��������  : ��������RF��mode���RFSWITCH��state
�������  : current_mode ���õ�ģʽ
�������  : ��
�� �� ֵ  : 0:����������ģʽ���ر���Ƶ
            1:����˳�����ģʽ������Ƶ
*****************************************************************************/
static int rfswitch_change_mode_to_state(unsigned int current_mode)
{
    unsigned int temp_state = RF_ON;
    if (1 ==current_mode)
    {
        temp_state = RF_ON;
    }
    else
    {
        temp_state = RF_OFF;
    }
    return temp_state;
}

/*****************************************************************************
�� �� ��  : RfSwitch_Swswtich_op_end
��������  : ��������ģʽ����������
�������  : trig_type:������ʽ��op_mode:�������ģʽ
�������  : ��
�� �� ֵ  : ��
*****************************************************************************/
void rfswitch_switch_end_operation(rfswitch_trig_type trig_type, unsigned int sw_op_mode)
{
    unsigned int sw_state = RF_ON; 
    unsigned int hw_state = RF_ON; 
    unsigned int rf_state = RF_ON; 
    
    /*��ȡ��ʵ��Ӳ������״̬����GPIO�ܽŻ�ȡ*/
    hw_state = rfswitch_hw_get();

    if(RFSWITCH_INIT_TRIG == trig_type)
    {
        /*��ȡ�������״̬����nv��ȡ*/
        sw_state = rfswitch_sw_get();
        rf_state = (sw_state & hw_state) & RF_ON;
    }  
    else if(RFSWITCH_SW_TRIG == trig_type)
    {
        /*��ȡ�������״̬������ξ���*/
        sw_state = rfswitch_change_mode_to_state(sw_op_mode);
        rf_state = (sw_state & hw_state) & RF_ON;
        
        /*�����������ģʽ����NV*/
        rfswitch_sw_set(sw_state); 
        
        /*�����ϱ�*/
        rfswitch_state_report(sw_state,hw_state);
#if  (FEATURE_ON == MBB_MODULE_THERMAL_PROTECT)
        /*֪ͨ�¶ȱ���ģ�飬��ǰ����ģʽ��״̬*/
        rfswitch_state_info_therm(rf_state);
#endif
    }
    else if(RFSWITCH_HW_TRIG == trig_type)
    {
        /*��ȡ�������״̬����nv��ȡ*/
        sw_state = rfswitch_sw_get();
        rf_state = (sw_state & hw_state) & RF_ON;
        
        /*�����ϱ�*/
        rfswitch_state_report(sw_state,hw_state); 
    }

     /*���÷���ģʽ���*/
    rfswitch_state_set(rf_state);
    rfswitch_trig_queue_out();
    return;
}
/*****************************************************************************
�� �� ��  : rfswitch_Hwswitch_handle
��������  : Ӳ������ģʽ������
�������  :current_mode ��ǰģʽ
�������  : ��
�� �� ֵ  : ��
*****************************************************************************/
static irqreturn_t rfswitch_hwswitch_handle(int irq, void *dev_id)
{
    unsigned long flags;

    spin_lock_irqsave(&g_wdis_ctx.lock, flags);

    /*�Ȼ�ȡһ��gpio��ֵ*/
    g_wdis_ctx.hw_temp = rfswitch_hw_get();
    
    /*��ʱ��,�������δ����ʱ��ʱ������ɾ��*/
    (void)bsp_softtimer_delete(&g_wdis_ctx.irq_timer);
    
    /*��timerǰ�ӳ�ϵͳ ���ߣ���ֹģ������timerʧЧ��ʱ��Ҫ����timer��ʱʱ��*/
    wake_lock_timeout(&rfswitch_pin_lock, (long)msecs_to_jiffies(WDIS_N_TIMER_LENGTH+2000)); /*lint !e526 !e628 !e516*/
    bsp_softtimer_add(&g_wdis_ctx.irq_timer);

    spin_unlock_irqrestore(&g_wdis_ctx.lock, flags);
    return IRQ_HANDLED;
}

/*****************************************************************************
�� �� ��  : rfswitch_wdis_timer_handler
��������  :����ģʽӲ�����ض�ʱ���ص�������
           ��������Ϣ��rfswtich_task�л���Ƶ
�������  : ��
�������  : ��
�� �� ֵ  : ��
*****************************************************************************/
static void rfswitch_wdis_timer_handler(unsigned int param)
{
    unsigned int sw_state = RF_ON;
    unsigned int hw_state = RF_ON;
    unsigned int rf_state = RF_ON;

    sw_state = rfswitch_sw_get();
    hw_state = rfswitch_hw_get();
    
    printk(KERN_ERR "rfswitch hwstate %d, swstate is %d\r\n",hw_state,sw_state);
    
    /*��ʱ��ȡ��ֵ���ϴε�ֵ��һ���򲻴�������*/
    if (hw_state != g_wdis_ctx.hw_temp)
    {
        printk(KERN_ERR "two read gpio value is different.\n");
        return;
    }

    /*�����Ҫ�л���״̬��Ŀǰ״̬һ�£��򲻴���*/
    if (hw_state == g_wdis_ctx.hw_state)
    {
        printk(KERN_ERR "the state to set is the same as now.\n");
        return;
    }

    g_wdis_ctx.hw_state = hw_state;
    /*����RF״̬*/
    rf_state = (sw_state & hw_state) & RF_ON;
    /*Ӳ�������������*/
    rfswitch_trig_queue_in(RFSWITCH_HW_TRIG);
    /*�л�״̬*/
    rfswitch_change(rf_state);
}
/*****************************************************************************
�� �� ��  : rfswitch_timer_init
��������  : ����ģʽ��ʱ����ʼ��
�������  : ��
�������  : ��
�� �� ֵ  : ��
*****************************************************************************/
static int rfswitch_timer_init(void)
{
    int ret = -1;
    struct softtimer_list *irq_timer = &g_wdis_ctx.irq_timer;
    
    /*irq_timer����Ӳ�����صĴ���*/
    irq_timer->func = rfswitch_wdis_timer_handler;
    irq_timer->para = (u32)&g_wdis_ctx;
    irq_timer->timeout = WDIS_N_TIMER_LENGTH;
    irq_timer->wake_type = SOFTTIMER_WAKE;
    ret = bsp_softtimer_create(irq_timer);

    if(0 != ret)
    {
        printk(KERN_ERR "rfswitch_timer_init create fail\n");
        return -1;
    }
    return 0;
}

/*****************************************************************************
�� �� ��  : rfswitch_gpio_init
��������  : ����ģʽӲ�����س�ʼ��
�������  : ��
�������  : ��
�� �� ֵ  : ��
*****************************************************************************/
static int rfswitch_gpio_init(void)
{
    int value = 0; /*ע�Ṳ���ж�ʱdev_id����Ϊ��*/
    
    if(0 != gpio_request(W_DISABLE_PIN, WDIS_DRIVER_NAME))
    {
        printk(KERN_ERR "gpio_request W_DISABLE_PIN  fail \n");
        return -1;
    }
    
    (void)gpio_direction_input(W_DISABLE_PIN);

    if(0 != request_irq(gpio_to_irq(W_DISABLE_PIN), rfswitch_hwswitch_handle, \
        IRQF_NO_SUSPEND | IRQF_SHARED | IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, \
        (char *)WDIS_DRIVER_NAME, &value))
    {
        printk(KERN_ERR "rfswitch_gpio_init request_irq  fail \n");
        gpio_free(W_DISABLE_PIN);
        return -1;
    }
    return 0;
}

/*****************************************************************************
 �� �� ��  : rfswitch_icc_init_cb
 ��������  : C�˳�ʼ������A�˷�icc��Ϣ�Ļص�����
 �������  : NULL
 �������  : ��
 �� �� ֵ  : 0:�ɹ�  -1:ʧ��
*****************************************************************************/
static int rfswitch_icc_cb(void)
{
    int read_len = 0;
    unsigned int rf_init = (unsigned int)RFSWITCH_RF_NONE;
    unsigned int channel_id = ICC_CHN_IFC << 16 | IFC_RECV_FUNC_RF_INIT;
    read_len = bsp_icc_read(channel_id, (unsigned char*)&rf_init, sizeof(rf_init));
    if ( sizeof(rf_init) != read_len )
    {
        printk("rfswitch_icc_cb bsp_icc_read fail\r\n");
        return -1;
    }
    
    /*�¶ȱ���Ҫ������Ƶ������ֻ��Ҫ�޸ı�Ǽ���*/
    if(RFSWITCH_RF_OFF == (rfswitch_ccore_op_type)rf_init)
    {
        rfswitch_therm_state_set(TRUE);
        return 0;
    }
    else if(RFSWITCH_RF_ON == (rfswitch_ccore_op_type)rf_init)
    {
        rfswitch_therm_state_set(FALSE);
        return 0;
    }
    else if(RFSWITCH_RF_INIT == (rfswitch_ccore_op_type)rf_init)
    {
        /*���Ӳ������ģʽ״̬*/
        g_wdis_ctx.hw_state = rfswitch_hw_get();
        /*����������ģʽ״̬*/
        g_wdis_ctx.sw_state = rfswitch_sw_get();

        /*����ϵ��ʼ��������������ģʽ����ֱ���л�״̬*/  
        if ((RF_OFF == g_wdis_ctx.sw_state) || (RF_OFF == g_wdis_ctx.hw_state))
        {  
            /*�ϵ��ʼ��������ʽ���*/
            rfswitch_trig_queue_in(RFSWITCH_INIT_TRIG);
            /*����ϵ��ʼ�����������ģʽ����ֱ���л�״̬*/    
            rfswitch_change(RF_OFF);  
            return 0;
        }
        rfswitch_state_set(RF_ON);
        return 0;
    }  
    else
    {
        printk("RfSwitch_icc_cb rf_init is rfswitch_RF_NONE\r\n");
        return -1;
    }

}

/*****************************************************************************
�� �� ��  : rfswitch_Init
��������  : ����ģʽ��ʼ������
�������  : ��
�������  : ��
�� �� ֵ  : ��
*****************************************************************************/
static int __init  rfswitch_init(void)
{
    int ret = -1;
    SOLUTION_PRODUCT_TYPE module_type = PRODUCT_TYPE_INVALID;
    unsigned int channel_id = ICC_CHN_IFC << 16 | IFC_RECV_FUNC_RF_INIT;
    
#if (FEATURE_ON == MBB_DLOAD)
    huawei_smem_info *smem_data = NULL;
    smem_data = (huawei_smem_info *)SRAM_DLOAD_ADDR;
    if (NULL == smem_data)
    {
        return ret;
    }
    if(SMEM_DLOAD_FLAG_NUM == smem_data->smem_dload_flag)
    {
        return ret;
    }
#endif /*MBB_DLOAD*/

    /* ��CEģ��ų�ʼ����ģ�� */
    module_type = bsp_get_solution_type();
    if (PRODUCT_TYPE_CE != module_type)
    {
        printk(KERN_ERR "rfswitch module will not init!");
        return ret;
    }

    /*trig type ���г�ʼ��*/
    (void)memset(queue_trigs, 0x0, sizeof(queue_trigs));
    
    if (rfswitch_timer_init())
    {
        printk(KERN_ERR "%s(): timer init fail. \n",__FUNCTION__);
        return ret;
    }

    (void)memset(queue_trigs, 0x0, sizeof(queue_trigs));
    
    /*��ʼ��ϵͳ ��*/
    wake_lock_init(&rfswitch_pin_lock, WAKE_LOCK_SUSPEND, WDIS_DRIVER_NAME);
    spin_lock_init(&trig_lock);
    
    /*�趨����ģʽ�ܽ��ж�״̬*/ 
    ret = rfswitch_gpio_init();
    if(0 != ret)
    {
        printk(KERN_ERR "rfswitch_gpio_init fail\n");
    }
    
    /*ע��icc�¼���ΪC�˳�ʼ��׼��*/
    ret = bsp_icc_event_register(channel_id, (read_cb_func)rfswitch_icc_cb, NULL, NULL, NULL);
    if(0 != ret)
    {
        printk(KERN_ERR "rfswitch_Init bsp_icc_event_register fail\n");
    }
    return 0 ;
}
module_init(rfswitch_init);


