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


/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include "chg_config.h"

#if (MBB_CHG_PLATFORM_BALONG == FEATURE_ON)
#include <bsp_hkadc.h>
#include <bsp_nvim.h>
#include "bsp_sram.h"
#include "power_com.h"
#ifdef CONFIG_COUL
#include <bsp_coul.h>
#endif
#include "hisi_smartstar_coul.h"
#include <product_config.h>
#include <product_nv_id.h>
#elif (MBB_CHG_PLATFORM_QUALCOMM == FEATURE_ON)
#include <linux/huawei_feature.h>
#endif/*MBB_CHG_PLATFORM_QUALCOMM == FEATURE_ON*/
#include "chg_chip_platform.h"
#include "chg_adc_volt_temp_cfg.h"
#if (FEATURE_ON == MBB_CHG_APORT_EXTCHG)
#include "hi_gpio.h"
#endif/*FEATURE_ON == MBB_CHG_APORT_EXTCHG*/

/*----------------------------------------------*
 * �ⲿ����˵��                                 *
 *----------------------------------------------*/
#if (FEATURE_ON == MBB_CHG_BATT_UNIFORM)
extern chg_hardware_data_dtsi g_chg_hardpara_dtsi;
#endif
extern struct i2c_client *g_i2c_client;
extern struct chargeIC_chip *g_chip;
extern int chg_en_flag;
#if (MBB_CHG_EXTCHG == FEATURE_ON)

extern int32_t g_extchg_diable_st;
extern boolean g_exchg_online_flag;
#endif/*MBB_CHG_EXTCHG == FEATURE_ON*/

#if ((MBB_CHG_EXTCHG == FEATURE_ON) || (MBB_CHG_WIRELESS == FEATURE_ON))
#ifndef USB_OTG_ID_PULL_OUT
/*ֱ������������HS_ID*/
#define USB_OTG_ID_PULL_OUT      (0x0003)
#endif/*USB_OTG_ID_PULL_OUT*/
extern void usb_notify_event(unsigned long val, void *v);
#endif/*(MBB_CHG_EXTCHG == FEATURE_ON) || (MBB_CHG_WIRELESS == FEATURE_ON)*/

#if (MBB_CHG_WIRELESS == FEATURE_ON)

extern boolean g_wireless_online_flag;
#endif/*MBB_CHG_WIRELESS == FEATURE_ON*/

/*----------------------------------------------*
 * ȫ�ֱ���                                     *
 *----------------------------------------------*/
/*Ӳ����Ʒ�汾��*/
uint32_t g_hardware_version_id = 0;


/*���ڱ���NV�ж�ȡ�ĵ�ص�ѹУ׼����*/
int32_t g_vbatt_max = 0;
/*���ڱ���NV�ж�ȡ�ĵ�ص�ѹУ׼��С��*/
int32_t g_vbatt_min = 0;
/*���ڱ����Ƿ������У׼��ʼ���ı�־*/
boolean g_is_batt_volt_calib_init = FALSE;

#if (MBB_CHG_PLATFORM_QUALCOMM == FEATURE_ON)
/*��ص�ѹ��ȡͨ��*/
#define VBAT_DETECT             P_MUX6_1_1
/*VBUS��ѹ��ȡͨ��,δ����*/
#define VBUS_DETECT             0xFF
/*����¶ȶ�ȡͨ��*/
#define BATTEMP_DETECT          P_MUX4_1_1
/*���ID��ѹ��ȡͨ��*/
#define BAT_ID_DETECT           LR_MUX2_BAT_ID
/*VPH_PWR��ѹ���*/
#define VPH_PWR_DETECT          LR_MUX4_AMUX_THM1

/* notes: according to < ADC_PARAMETER_ENUM(adc_driver.h)  and CHG_PARAMETER_ENUM(chg_config.h) > */
typedef struct
{
    char *chg_ch_name;
    CHG_PARAMETER_ENUM chg_ch_typ;
    ADC_PARAMETER_ENUM adc_ch_sel;
}chg_adc_adapter;

chg_adc_adapter chg_adc_adapter_tbl[] =
{
    {"VBAT_VOLT", CHG_PARAMETER__BATTERY_VOLT,         ADC_PARAMETER__BATTERY_VOLT     },/* ��ص�ѹ */
    {"VBAT_TEMP", CHG_PARAMETER__BATT_THERM_DEGC,      ADC_PARAMETER__BATT_THERM_DEGC  },/* ���NTC�¶ȶ�Ӧ��ѹ */
    {"VBUS_VOLT", CHG_PARAMETER__VBUS_VOLT,            ADC_PARAMETER__VBUS_VOLT        },/* ���������ڼ��VBUS��ѹ*/
    {"BOT_TEMP",  CHG_PARAMETER__BATT_BOT_THERM_DEGC,  ADC_PARAMETER__BOTT_THERM_DEGC  },/* �弶�¶ȶ�Ӧ��ص�ѹ*/
    {"VBAT_ID",   CHG_PARAMETER__BATTERY_ID_VOLT,      ADC_PARAMETER__BATTERY_ID_VOLT  },/* ���ID��ѹ���ڵ���ͺ�ʶ��*/
    {"VPH_PWR",   CHG_PARAMETER__VPH_PWR_VOLT,         ADC_PARAMETER__VPH_PWR_VOLT     },/* VPH_PWR��ѹ*/
};
/* end of notes: according to < ADC_PARAMETER_ENUM(adc_driver.h) / CHG_PARAMETER_ENUM(chg_config.h) > */
#endif/*#if (MBB_CHG_PLATFORM_QUALCOMM == FEATURE_ON)*/

#if (MBB_CHG_PLATFORM_BALONG == FEATURE_ON)
#define CHG_BATT_THERM_HKADC_ID    CHG_BATT_TEMP_CHAN/*����¶ȶ�ȡͨ��*/
#define CHG_VBUS_VOLT_HKADC_ID     CHG_VBUS_VOLT_CHAN/*VBUS��ѹ��ȡͨ��*/
#define CHG_BATT_VOLT_HKADC_ID     CHG_BATT_VOLT_CHAN/*��ص�ѹ��ȡͨ��*/
#define CHG_BATT_ID_VOLT_HKADC_ID     CHG_BATT_ID_CHAN/*��ص�ѹ��ȡͨ��*/
/*USB�±�*/ 
#if ( FEATURE_ON == MBB_CHG_USB_TEMPPT_ILIMIT )
#define CHG_USB_TEMP_HKADC_ID      CHG_USB_TEMP_CHAN /*USB�¶ȶ�ȡͨ��*/
#endif

/*��ص�ѹADC�ɼ���ѹֵ*/
#endif/*MBB_CHG_PLATFORM_BALONG == FEATURE_ON*/

/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/

#define BAT_VOL_SIZE               4

#define SMEM_FACT_RELEASE_ON       0X19840a0b    /*���߷ŵ�ħ����*/
#define SMEM_FACT_RELEASE_OFF      0X19850b0a    /*���߷ŵ�ħ����*/

#define MICRO_TO_MILLI_V                    (1000)

int32_t bsp_i2c_read(uint8_t reg)
{
    int ret = 0;

    if(NULL == g_i2c_client)
    {
        pr_err("%s failed due to g_i2c_client doesn't exist@~\n", __FUNCTION__);
        return -1;
    }

    ret = i2c_smbus_read_byte_data(g_i2c_client, reg);

    if(ret < 0)
    {
        return -1;
    }
    else
    {
        return ret;
    }
}

int32_t bsp_i2c_write(uint8_t reg, int32_t value)
{
    if(NULL == g_i2c_client)
    {
        pr_err("%s failed due to g_i2c_client doesn't exist@~\n", __FUNCTION__);
        return -1;
    }

    if(0 == i2c_smbus_write_byte_data(g_i2c_client, reg, value))
    {
        return 0;
    }
    else
    {
        return -1;
    }
}
/**********************************************************************
FUNCTION:    CHG_SET_POWER_OFF
DESCRIPTION: The poweroff func of CHG module, all the power-off operation
             except at boot phase MUST be performed via calling this.
INPUT:       The shutdown reason which triggered system poweroff.
             All VALID REASON:
             DRV_SHUTDOWN_BATTERY_ERROR --BATTERY ERROR;
             DRV_SHUTDOWN_TEMPERATURE_PROTECT --EXTREAM HIGH TEMPERATURE.
             DRV_SHUTDOWN_LOW_TEMP_PROTECT --EXTREAM LOW TEMPERATURE
             DRV_SHUTDOWN_CHARGE_REMOVE --CHGR REMOVAL WHILE POWEROFF CHG
             DRV_SHUTDOWN_LOW_BATTERY --LOW BATTERY
OUTPUT:      None.
RETURN:      None.
NOTE:        When this function get called to power system off, it record
             the shutdown reason, then simulate POWER_KEY event to APP to
             perform the real system shutdown process.
             THUS, THIS FUNCTION DOESN'T TAKE AFFECT IF APP DIDN'T STARTUP.
***********************************************************************/
/***************Note:ƽ̨��ش��룬����ƽ̨����Ҫ��ӣ��е�ƽ̨��9x25��Ҫ
              ��ֲ��Ա������Ҫ����ӻ����Ƴ��±ߺ�������***************************/
void chg_set_power_off(DRV_SHUTDOWN_REASON_ENUM real_reason)
{
    /*���ڹرճ�纯��ʹ��I2C����ʱ����msleep�ȷ�ԭ�Ӳ�����Ҫschedule CPU,
     �ڶ�ʱ���ж�����Ҫatomicʹ�ã��ᵼ���ں�BUG*/
    chg_print_level_message(CHG_MSG_ERR, "CHG_PLT:chg_set_power_off:power off by reason = %d \n ",real_reason);
#if (FEATURE_ON == MBB_CHG_PLATFORM_QUALCOMM)
    kernel_power_off();
#elif (FEATURE_ON == MBB_CHG_PLATFORM_BALONG)
    bsp_drv_set_power_off_reason(real_reason);
    bsp_drv_power_off();
#endif/*MBB_CHG_PLATFORM_QUALCOMM == FEATURE_ON*/
    return;
}
/**********************************************************************
FUNCTION:    chg_send_stat_to_app
DESCRIPTION: Charge module sends charge state to application layer.
INPUT:       uint32_t chg_device_id,
             uint32_t chg_event_id
OUTPUT:      None.
RETURN:      None.
NOTE:        None
***********************************************************************/
void chg_send_stat_to_app(uint32_t chg_device_id, uint32_t chg_event_id)
{
#if (MBB_CHG_POWER_SUPPLY == FEATURE_ON)
    /*�ȸ���power supply���ڵ�������Ϣ*/
    chg_update_power_suply_info();

    /*��Ӧ���ϱ�power_supply_changed�¼�*/
    if((DEVICE_ID_TEMP == chg_device_id) || (DEVICE_ID_KEY == chg_device_id)
        || (DEVICE_ID_BATTERY == chg_device_id))
    {
        power_supply_changed(&g_chip->bat);
    }
    else if(DEVICE_ID_WIRELESS == chg_device_id)
    {
        power_supply_changed(&g_chip->wireless);
    }
    else if(DEVICE_ID_EXTCHG == chg_device_id)
    {
        power_supply_changed(&g_chip->extchg);
    }
    else if(DEVICE_ID_USB == chg_device_id)
    {
        power_supply_changed(&g_chip->usb);
    }
    else if(DEVICE_ID_CHARGER == chg_device_id)
    {
        power_supply_changed(&g_chip->ac);
    }
    else
    {
        chg_print_level_message(CHG_MSG_ERR, "CHG_PLT:chg event device id unknow !\n ");
    }

    chg_print_level_message(CHG_MSG_ERR, "CHG_PLT:chg_send_stat_to_app->deviceid %d, eventid %d \n ",
                            chg_device_id,chg_event_id);
#else
    /*********NOTE:����Ƿ�LINUXϵͳ�Ȳ�ʹ��power supply�¼��ϱ�APP��ʽ��ƽ̨��Ҫ��ƽ̨
              �ϱ��¼��Ľӿں����ڴ��޸�,��V7R1ƽ̨��ʹ�����º���BSP_CHGC_SendStaToApp************/
    //BSP_CHGC_SendStaToApp((uint32_t)chg_device_id, (uint32_t)chg_event_id);
    chg_print_level_message(CHG_MSG_ERR, "CHG_PLT:chg_send_stat_to_app->deviceid %d, eventid %d \n ",
                            chg_device_id,chg_event_id);
#endif/*MBB_CHG_POWER_SUPPLY == FEATURE_ON*/
}


#ifdef CHG_STUB
DRV_START_MODE_ENUM chg_get_start_mode(void)
{
    return DRV_START_MODE_NORMAL;
}
#else
DRV_START_MODE_ENUM chg_get_start_mode(void)
{
#if (MBB_CHG_PLATFORM_BALONG == FEATURE_ON)
    DRV_START_MODE mode = DRV_START_MODE_NORMAL;
    DRV_RUN_MODE boot_mode = get_run_mode();
    DRV_CHARGING_MODE pwd_flag = get_pd_charge_flag();
#if (FEATURE_ON == MBB_DLOAD)
    if((RUN_MODE_NORMAL == boot_mode) && (POWER_DOWN_CHARGING_MODE != pwd_flag))
    {
        /* ����ģʽ */
        mode = DRV_START_MODE_NORMAL;
    }
    else if((RUN_MODE_NORMAL == boot_mode) && ((POWER_DOWN_CHARGING_MODE == pwd_flag)))
    {
        /* �ػ����ģʽ */
        mode = DRV_START_MODE_CHARGING;
    }
    else if(RUN_MODE_RECOVERY == boot_mode)
    {
        /* ����ģʽ */
        mode = DRV_START_MODE_UPDATE;
    }
#endif/*FEATURE_ON == MBB_DLOAD*/
    return mode;

#elif(MBB_CHG_PLATFORM_QUALCOMM == FEATURE_ON)
    static_smem_vendor0 *smem_data = NULL;

    /*ͨ����ȡ�����ڴ棬��ȡ��ǰ����״̬���˴���ƽ̨��ֲ��Ա��д,
    ���ݿ���״̬�������ǳ�翪��ģʽ����������ģʽ���쳣����ģʽ*/
    smem_data = (static_smem_vendor0 *)SMEM_HUAWEI_ALLOC(SMEM_ID_VENDOR0 , sizeof(static_smem_vendor0 ));
    if(NULL == smem_data)
    {
        return DRV_START_MODE_NORMAL;
    }
    chg_print_level_message(CHG_MSG_DEBUG, "CHG_PLT:smem_data %x !\n", (unsigned int)(smem_data->smem_huawei_poweroff_chg));

    /*start_mode_flag = */
    switch(smem_data->smem_huawei_poweroff_chg)
    {
        /*case 0:*/
        case POWER_ON_STATUS_FINISH_CHARGE:
        {
            /*break;*/
            return DRV_START_MODE_CHARGING;
        }
        /*case 1:*/
        case POWER_ON_STATUS_FINISH_NORMAL:
        {
            /*break;*/
            return DRV_START_MODE_NORMAL;
        }
        /*case 2:*/
        case POWER_ON_STATUS_BOOTING:
        case POWER_ON_STATUS_CHARGING:
        case POWER_ON_STATUS_CAPACITY_LOW:
        case POWER_ON_STATUS_BATTERY_ERROR:
        default:
        {
            /*break;*/
            return DRV_START_MODE_EXCEPTION;
        }
    }

#endif/*MBB_CHG_PLATFORM_QUALCOMM == FEATURE_ON*/

}
#endif/*CHG_STUB*/

 
boolean chg_is_powdown_charging (void)
{
    DRV_START_MODE_ENUM start_mode = DRV_START_MODE_BUTT;

    /*����ģʽ���岻����ػ����*/
    if(TRUE == chg_is_ftm_mode())
    {
        return FALSE;
    }

    start_mode = chg_get_start_mode();
    if(DRV_START_MODE_CHARGING == start_mode)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

#if (MBB_CHG_WIRELESS == FEATURE_ON)
/*���߳��MMI����ǰ������*/
#define WIRELESS_MMI_IUSB_CURRENT               (900)
/*���߳����������*/
#define WIRELESS_MMI_ICHG_CURRENT               (1024)
/*���߳���ӳ�6S*/
#define WIRELESS_MMI_TEST_DELAY_COUNT           (6000)


void wireless_mmi_test_proc(ulong64_t plug)
{
    int32_t vbus_volt = 0;

    if(0 == plug)
    {
        chg_print_level_message(CHG_MSG_ERR, "CHG_PLT:MMI TEST WIRELESS_CHGR PLUG OUT!\n");
        (void)chg_set_charge_enable(FALSE);
        chg_stm_set_wireless_online_st(FALSE);
        chg_send_stat_to_app((uint32_t)DEVICE_ID_WIRELESS, (uint32_t)CHG_EVENT_NONEED_CARE);
        return;
    }

    vbus_volt = chg_get_volt_from_adc(CHG_PARAMETER__VBUS_VOLT);
    if(vbus_volt < VBUS_JUDGEMENT_THRESHOLD)
    {
        chg_print_level_message(CHG_MSG_INFO, "CHG_PLT:MMI TEST WIRELESS_CHGR PLUG IN!\n");
        chg_set_wireless_chg_enable(TRUE);
        (void)chg_set_dpm_val(CHG_WIRELESS_DPM_VOLT);
        (void)chg_set_cur_level(WIRELESS_MMI_ICHG_CURRENT);
        (void)chg_set_supply_limit(WIRELESS_MMI_IUSB_CURRENT);
        (void)chg_set_charge_enable(TRUE);
        chg_delay_ms(WIRELESS_MMI_TEST_DELAY_COUNT);

        if(TRUE == chg_is_IC_charging())
        {
            chg_stm_set_wireless_online_st(TRUE);
            chg_send_stat_to_app((uint32_t)DEVICE_ID_WIRELESS, (uint32_t)CHG_EVENT_NONEED_CARE);
        }
        else
        {
            chg_stm_set_wireless_online_st(FALSE);
            chg_send_stat_to_app((uint32_t)DEVICE_ID_WIRELESS, (uint32_t)CHG_EVENT_NONEED_CARE);
        }
        (void)chg_set_charge_enable(FALSE);
    }
    else
    {
        chg_print_level_message(CHG_MSG_ERR, "CHG_PLT:MMI TEST VBUS above 700mv!\n");
        (void)chg_set_charge_enable(FALSE);
        chg_stm_set_wireless_online_st(FALSE);
        chg_send_stat_to_app((uint32_t)DEVICE_ID_WIRELESS, (uint32_t)CHG_EVENT_NONEED_CARE);
    }
}
#endif/*MBB_CHG_WIRELESS == FEATURE_ON*/

#if (MBB_CHG_EXTCHG == FEATURE_ON)
#if((FEATURE_ON == MBB_FACTORY) && (FEATURE_ON == MBB_CHG_COULOMETER))
#define MMI_EXTCHG_SHUTDOWN_TIME_MS    (60000)

struct delayed_work mmi_extchg_shutdown_work;
boolean is_extchg_shutdown_init = FALSE;

void mmi_extchg_shutdown(void)
{
    chg_set_extchg_chg_enable(FALSE);
    chg_stm_set_extchg_online_st(FALSE);
    chg_print_level_message(CHG_MSG_ERR, "CHG_MMI:mmi_extchg_shutdown !!!\n");
}
#endif/*(FEATURE_ON == MBB_FACTORY) && (FEATURE_ON == MBB_CHG_COULOMETER)*/

void extchg_mmi_test_proc(void)
{
    boolean extchg_online_st = FALSE;
    /*MMI����ģʽͨ��ʹ�ܶ�������VBUS��ѹ�ж϶�������豸�Ƿ���λ*/
    extchg_online_st = chg_get_extchg_online_status();
    chg_print_level_message(CHG_MSG_INFO, "CHG_PLT:extchg_mmi_test extchg_online_st = %d!!!\n",extchg_online_st);
    chg_stm_set_extchg_online_st(extchg_online_st);
    chg_set_extchg_status(POWER_SUPPLY_EXTCHGSTA_START_CHARGING);
    chg_send_stat_to_app((uint32_t)DEVICE_ID_EXTCHG, (uint32_t)CHG_EVENT_NONEED_CARE);

#if((FEATURE_ON == MBB_FACTORY)\
     && (FEATURE_ON == MBB_CHG_COULOMETER)\
     && (FEATURE_ON == MBB_CHG_EXTCHG ))
    /*MMI��λ���Ӷ�ȡ���ؼƵ����������ڶ�ȡ֮ǰ��Ҫcheck Vbus��ѹ�Ƿ�����*/
    chg_check_vbus_volt();
    /*Vbus��ѹcheck�������ϱ�Ӧ�ÿ��Զ�ȡ���ؼƵ���*/
    chg_send_stat_to_app((uint32_t)DEVICE_ID_EXTCHG, (uint32_t)CHG_EVENT_NONEED_CARE);
    if(FALSE == is_extchg_shutdown_init)
    {
        INIT_DELAYED_WORK(&mmi_extchg_shutdown_work,mmi_extchg_shutdown);
        is_extchg_shutdown_init = TRUE;
    }
    /*Ϊ�˷�ֹ1min�ڶ�β�Σ������²��������cancel��һ�ε�delay work*/
    cancel_delayed_work_sync(&mmi_extchg_shutdown_work);
    schedule_delayed_work(&mmi_extchg_shutdown_work, msecs_to_jiffies(MMI_EXTCHG_SHUTDOWN_TIME_MS));

    chg_print_level_message(CHG_MSG_ERR, "CHG_MMI:stop extchg after one minute!\n");
#endif
    chg_print_level_message(CHG_MSG_ERR, "CHG_MMI:extchg_mmi_test over!\n");
}
#endif/*defined(MBB_CHG_EXTCHG)*/


/*PT��λʹ�ܳ��ǰ������*/
#define PT_ENABLECHG_IUSB_CURRENT          (500)
/*PT��λʹ�ܳ��������*/
#define PT_ENABLECHG_ICHG_CURRENT          (576)
/*PT��λʹ�ܳ���ѹ��ѹ*/
#define PT_ENABLECHG_CV_VOLTAGE            (4200)
/*PT��λʹ�ܳ���ֹ����*/
#define PT_ENABLECHG_TERMINATE_CURRENT     (128)

boolean chg_pt_mmi_test_proc(void)
{
    if(FALSE == chg_set_supply_limit(PT_ENABLECHG_IUSB_CURRENT))
    {
        chg_print_level_message(CHG_MSG_ERR, "CHG_PLT:chg_pt_mmi set supply limit fail\n");
        return FALSE;
    }
    if(FALSE == chg_set_cur_level(PT_ENABLECHG_ICHG_CURRENT))
    {
        chg_print_level_message(CHG_MSG_ERR, "CHG_PLT:chg_pt_mmi set current level fail\n");
        return FALSE;
    }
    if(FALSE == chg_set_vreg_level(PT_ENABLECHG_CV_VOLTAGE))
    {
        chg_print_level_message(CHG_MSG_ERR, "CHG_PLT:chg_pt_mmi set vreg level fail\n");
        return FALSE;
    }
    if(FALSE == chg_set_term_current(PT_ENABLECHG_TERMINATE_CURRENT))
    {
        chg_print_level_message(CHG_MSG_ERR, "CHG_PLT:chg_pt_mmi set term level fail\n");
        return FALSE;
    }
    if(FALSE == chg_set_charge_enable(TRUE))
    {
        chg_print_level_message(CHG_MSG_ERR, "CHG_PLT:chg_pt_mmi set charge enable fail\n");
        return FALSE;
    }
    return TRUE;
}

static inline char* get_chgr_name_by_id(int32_t chg_type)
{
    static char* chg_chgr_type_name_tab[CHG_CHGR_INVALID + 1] =
    {
        "Unknown",                      //���������δ֪
        "Wall Charger",                 //��׼�����
        "USB HOST PC",                  //USB
        "Non-Standard Chgr",            //�����������
        "wireless chgr"                 //���߳����
        "extchg chgr"                   //��������
        "Invalid",                      //�����������
    };

    return ((chg_type >= CHG_CHGR_UNKNOWN && chg_type <= CHG_CHGR_INVALID) \
                         ? chg_chgr_type_name_tab[chg_type] : "null");
}

/*****************************************************************************
 �� �� ��  : get_chgr_type_from_usb
 ��������  : Get real charger type from USB module.
             This function called the platform interfaces to obtain the real
             charger type from USB module.
 �������  : plug:���Դ����γ�
 �������  : chg_type:���������
 �� �� ֵ  : Charger type of state machine needed.
 CALL-WHOM : Platform interfaces.
 WHO-CALLED: chg_check_and_update_hw_param_per_chgr_type
             chg_transit_state_period_func
 NOTICE    : 1. Platform chgr_type enum may be different from chg_stm design,
             we need convert or remap them before return.
             2. Balong V3R2/V7R1 platform didn't support CHG_USB_HOST_PC,
             CHG_NONSTD_CHGR and CHG_USB_HOST_PC would all be treated as
             CHG_NONSTD_CHGR.
             3. Export Required.
*****************************************************************************/
chg_chgr_type_t get_chgr_type_from_usb(ulong64_t plug,int32_t chg_type)
{

    chg_stm_state_type cur_stat = chg_stm_get_cur_state();
    CHG_MODE_ENUM cur_chg_mode = chg_get_cur_chg_mode();
    chg_chgr_type_t cur_chgr_type = chg_stm_get_chgr_type();
printk("get_chgr_type_from_usb cur_chgr_type is %d\r\n", cur_chgr_type);
    chg_print_level_message(CHG_MSG_INFO, "CHG_PLT:get_chgr_type_from_usb cur_stat=%d,cur_chg_mode=%d,cur_chgr_type=%d\n",
            cur_stat,cur_chg_mode,cur_chgr_type);
    chg_print_level_message(CHG_MSG_INFO, "CHG_PLT:get_chgr_type_from_usb plug=%d,chg_type=%d\n",
                            plug,chg_type);

    mlog_print(MLOG_CHG, mlog_lv_info, "charger type %s(%d) %s detected.\n",
               get_chgr_name_by_id(chg_type), chg_type,
               plug ? "insertion" : "removal");

    if(TRUE == chg_is_ftm_mode())
    {
#if (MBB_CHG_WIRELESS == FEATURE_ON)
        if(CHG_WIRELESS_CHGR == chg_type)
        {
            wireless_mmi_test_proc(plug);
            return CHG_CHGR_INVALID;
        }
#endif /*MBB_CHG_WIRELESS == FEATURE_ON*/

#if (MBB_CHG_EXTCHG == FEATURE_ON)
        if(CHG_EXGCHG_CHGR == chg_type)
        {
            /*USB ID�������MMI���*/
            if(1 == plug)
            {
                extchg_mmi_test_proc();
                return CHG_CHGR_INVALID;
            }
            /*USB ID�γ�����USB�����ӿ�D+ D-ֱ������������HS_ID*/
            else
            {
#if((FEATURE_ON == MBB_FACTORY) && (FEATURE_ON == MBB_CHG_COULOMETER))
                /*���ؼ�MMI�����緽��֧�����²���������豸������ڰγ��������豸����Ҫ�رն���������*/
                mmi_extchg_shutdown();
#endif
                return CHG_CHGR_INVALID;
            }
        }
#endif/*MBB_CHG_EXTCHG == FEATURE_ON*/
        return CHG_CHGR_INVALID;
    }
    /*�����е����Ĳ����������У���˲���Ӧ����¼�*/
    if(CHG_SUPPLY_MODE == cur_chg_mode)
    {
        chg_print_level_message(CHG_MSG_ERR, "CHG_PLT: cur_chg_mode = CHG_SUPPLY_MODE!\n ");
        return CHG_USB_HOST_PC;
    }

    /*���������в������������Ҳ���ܽ���״̬�л�����ֹ��ʱ��δ���屻ʹ�õ��µ�������*/
    if(TRUE == is_in_update_mode())
    {
        return CHG_USB_HOST_PC;
    }
    /*���³��������*/
    chg_stm_set_chgr_type(chg_type);
    /*�����紦��*/
#if (MBB_CHG_EXTCHG == FEATURE_ON)
    if(CHG_EXGCHG_CHGR == chg_type)
    {
        chg_extchg_config_data_init();
        if (plug)
        {
            chg_extchg_insert_proc();
        }
        else
        {
            chg_extchg_remove_proc();
        }
        return CHG_EXGCHG_CHGR;
    }
#endif/*MBB_CHG_EXTCHG == FEATURE_ON*/
    /*���ڳ�紦��*/
    if (plug)
    {
#if (MBB_CHG_WIRELESS == FEATURE_ON)
        if(CHG_WIRELESS_CHGR == chg_type)
        {
            chg_stm_set_wireless_online_st(TRUE);
        }
#endif/*MBB_CHG_WIRELESS == FEATURE_ON*/
        if(CHG_CHGR_UNKNOWN == chg_type)
        {
            chg_start_chgr_type_checking();
        }
        chg_charger_insert_proc(chg_stm_get_chgr_type());
        //chg_send_msg_to_main(CHG_CHARGER_IN_EVENT);
    }
    else
    {
#if (MBB_CHG_WIRELESS == FEATURE_ON)
        if(CHG_WIRELESS_CHGR == chg_type)
        {
            chg_stm_set_wireless_online_st(FALSE);
        }
#endif/*MBB_CHG_WIRELESS == FEATURE_ON*/
        chg_charger_remove_proc(chg_stm_get_chgr_type());
        //chg_send_msg_to_main(CHG_CHARGER_OUT_EVENT);
    }

    return (chg_chgr_type_t)chg_type;
}


void chg_lcd_display(CHG_BATT_DISPLAY_TYPE disp_type)
{
    switch(disp_type)
    {
        case CHG_DISP_OK:
            //LCD�����ӿڣ�����ֲ��Ա����ʵ�������װ��
            break;
        case CHG_DISP_FAIL:
            //LCD�����ӿڣ�����ֲ��Ա����ʵ�������װ��
            break;
        case CHG_DISP_BATTERY_LOWER:
            //LCD�����ӿڣ�����ֲ��Ա����ʵ�������װ��
            break;
        case CHG_DISP_BATTERY_BAD:
            //LCD�����ӿڣ�����ֲ��Ա����ʵ�������װ��
            break;
        case CHG_DISP_OVER_HEATED:
            //LCD�����ӿڣ�����ֲ��Ա����ʵ�������װ��
            break;
        case CHG_DISP_TEMP_LOW:
            //LCD�����ӿڣ�����ֲ��Ա����ʵ�������װ��
            break;
        default:
            chg_print_level_message( CHG_MSG_ERR,"CHG_PLT:chg_lcd_display->disp_type invalid!!!\r\n ");
            break;
    }
    return;
}


void chg_led_display(CHG_BATT_DISPLAY_TYPE disp_type)
{
    switch(disp_type)
    {
        case CHG_DISP_OK:
        case CHG_DISP_FAIL:
            //logָʾ���ݲ�֧��(���Ʋ�ƷĿǰ���ǿɲ�ж���);
            chg_print_level_message( CHG_MSG_ERR,"CHG_PLT:chg_led_display->led product not support!!!\r\n ");
            break;
        case CHG_DISP_BATTERY_LOWER:
            /*�������ƽӿڣ���Ƴ���������Ϩ��*/
            break;
        case CHG_DISP_BATTERY_BAD:
        case CHG_DISP_OVER_HEATED: /*ָ���¹ػ�����ͣ��*/
        case CHG_DISP_TEMP_LOW: /*ָ���¹ػ�����ͣ��*/
            /*�����̵�ȫ���ӿ�*/
            break;
        default:
            chg_print_level_message( CHG_MSG_ERR,"CHG_PLT:chg_led_display->disp_type invalid!!!\r\n ");
            break;
    }
    return;
}


void chg_display_interface(CHG_BATT_DISPLAY_TYPE disp_type)
{
#if (MBB_CHG_LCD == FEATURE_ON)
    chg_lcd_display(disp_type);
#elif (MBB_CHG_LED == FEATURE_ON)
    chg_led_display(disp_type);
#endif/*MBB_CHG_LED == FEATURE_ON*/

    return;
}
#if (FEATURE_ON == MBB_CHG_BATT_UNIFORM)
/******************************************************************************
  Function      chg_adc_volt_temp_map_init
  Description   ��ع�һ��Ʒ����Ӳ���汾�Ž�VT���Ƶ�ȫ�ֱ�����
  Input         ��
  Output        N/A
  Return        ��
  Others        N/A
******************************************************************************/
void chg_adc_volt_temp_map_init(void)
{
    unsigned int number = ARRAY_SIZE(g_chg_adc_vt_all);
    int i = 0;
    for (i = 0; i < number; i++)
    {
        if (g_chg_adc_vt_all[i].product_type == g_hardware_version_id)
        {   /* ���ҵ�ƥ��Ĳ�Ʒ���򽫹�һ��Ʒ��VT��ֵ��ȫ�ֱ��� */
            memcpy(g_adc_batt_therm_map,g_chg_adc_vt_all[i].g_adc_batt_therm_map,sizeof(g_adc_batt_therm_map));
#if ( FEATURE_ON == MBB_CHG_USB_TEMPPT_ILIMIT )
            memcpy(g_adc_usb_therm_map,g_chg_adc_vt_all[i].g_adc_usb_therm_map,sizeof(g_adc_usb_therm_map));
#endif
            return;
        }
    }
    return;
}
#endif
/******************************************************************************
  Function      batt_volt_calib_atoi
  Description   ���ַ��������ַ�'0'���ַ�'9'��ɵ����ݶ�ת��Ϊ����
  Input         *name: ��Ҫ����ת�����ַ�����ĵ�ַ
  Output        N/A
  Return        val  : ��ת�������ɵ����α�������
  Others        N/A
******************************************************************************/
static int batt_volt_calib_atoi(char *name)
{
    int val = 0;

    for (;; name++)
    {
        switch (*name)
        {
            case '0' ... '9':
            val = ATOI_CONVERT_NUM * val + (*name - '0');
            break;
            default:
            return val;
        }
    }
}


/***************Note:ƽ̨��ش��룬����ƽ̨����Ҫ��ӣ��е�ƽ̨��9x25��Ҫ
              ��ֲ��Ա������Ҫ����ӻ����Ƴ��±ߺ�������***************************/
void chg_batt_volt_calib_init(void)
{
#if (MBB_CHG_PLATFORM_BALONG == FEATURE_ON)
    int32_t ret = 0;
    VBAT_CALIBRATION_TYPE vbatt_calib_value;

    memset((void *)&vbatt_calib_value, 0, sizeof(VBAT_CALIBRATION_TYPE));

    ret = bsp_nvm_read(NV_BATT_VOLT_CALI_I, &vbatt_calib_value, sizeof(VBAT_CALIBRATION_TYPE));
    if(0 == ret)
    {
        g_vbatt_max = vbatt_calib_value.max_value;
        g_vbatt_min = vbatt_calib_value.min_value;

        chg_print_level_message( CHG_MSG_ERR,"read calibrate value successs! max=%d,min=%d\n",
        g_vbatt_max,g_vbatt_min,0);
    }
    else
    {
        chg_print_level_message(CHG_MSG_ERR,"read chg nv failed when read check value\n");
        return;
    }
    if(g_vbatt_max <= g_vbatt_min)
    {
        chg_print_level_message( CHG_MSG_ERR,"CHG_PLT:chg_batt_volt_calib_init->max min value error!!!\r\n ");
        return;
    }
    /*��У׼��ʼ����־λ*/
    g_is_batt_volt_calib_init = TRUE;

#elif (MBB_CHG_PLATFORM_QUALCOMM == FEATURE_ON)

    mm_segment_t old_fs = {0};
    struct file *fp_v_low = NULL;
    struct file *fp_v_high = NULL ;
    int bat_low = 0;
    int bat_high = 0;
    loff_t pos_v_low = 0;
    loff_t pos_v_high = 0;
    char bat_vol_low[BAT_VOL_SIZE + 1] = {0};
    char bat_vol_high[BAT_VOL_SIZE + 1] = {0};

    fp_v_low = filp_open("/data/voltage_low", O_RDONLY, 0);
    if (IS_ERR(fp_v_low))
    {
        fp_v_low = NULL;
        chg_print_level_message( CHG_MSG_INFO,"CHG_PLT: Open Bat vol Low file failed!\n");
        return;
    }
    pos_v_low = fp_v_low->f_pos;

    old_fs = get_fs();
    set_fs(KERNEL_DS);

    vfs_read(fp_v_low, bat_vol_low, BAT_VOL_SIZE, &pos_v_low);

    set_fs(old_fs);

    filp_close(fp_v_low, NULL);
    fp_v_high = filp_open("/data/voltage_high", O_RDONLY, 0);
    if (IS_ERR(fp_v_high))
    {
        fp_v_high = NULL;
        chg_print_level_message( CHG_MSG_ERR,"CHG_PLT: Open Bat vol High file failed!\n");
        return;
    }
    pos_v_high = fp_v_high->f_pos;

    old_fs = get_fs();
    set_fs(KERNEL_DS);

    vfs_read(fp_v_high, bat_vol_high, BAT_VOL_SIZE, &pos_v_high);

    set_fs(old_fs);

    filp_close(fp_v_high, NULL);

    bat_low = batt_volt_calib_atoi(bat_vol_low);
    bat_high = batt_volt_calib_atoi(bat_vol_high);

    chg_print_level_message( CHG_MSG_ERR,"CHG_PLT: bat_low = %d!\n",bat_low);
    chg_print_level_message( CHG_MSG_ERR,"CHG_PLT: bat_high = %d!\n",bat_high);

    g_vbatt_min = bat_low;
    g_vbatt_max = bat_high;

    if(g_vbatt_max <= g_vbatt_min)
    {
        chg_print_level_message( CHG_MSG_ERR,"CHG_PLT:chg_batt_volt_calib_init->max min value error!!!\r\n ");
        return;
    }

    /*��У׼��ʼ����־λ*/
    g_is_batt_volt_calib_init = TRUE;

#endif/*MBB_CHG_PLATFORM_QUALCOMM == FEATURE_ON*/
}


int32_t chg_calc_batt_volt_calib_value(int32_t orig_val)
{
    int32_t rtn_vbat_val = 0;
    if(FALSE == g_is_batt_volt_calib_init)
    {
        chg_print_level_message( CHG_MSG_INFO,"CHG_PLT:calib is not init!!!\r\n ");
        return orig_val;
    }
    /*�����ص�ѹУ׼ֵ*/
#if (FEATURE_ON == MBB_CHG_BATT_UNIFORM)
    rtn_vbat_val = g_chg_hardpara_dtsi.batt_cali_volt_min + (orig_val - g_vbatt_min)
                * (g_chg_hardpara_dtsi.batt_cali_volt_max - g_chg_hardpara_dtsi.batt_cali_volt_min)
                / (g_vbatt_max - g_vbatt_min);
#else
    rtn_vbat_val = CHG_BATT_CALI_MIN_VOLT + (orig_val - g_vbatt_min)
                * (CHG_BATT_CALI_MAX_VOLT - CHG_BATT_CALI_MIN_VOLT)
                / (g_vbatt_max - g_vbatt_min);
#endif
    return rtn_vbat_val;
}

void chg_debug_adc(void)
{
    int tmp_ret           = -1;
    uint16_t rtn_vbat_val = 0;
    unsigned int i        = 0;

    for (i = 0; i < 14; i++)
    {
        tmp_ret = bsp_hkadc_convert(i, &rtn_vbat_val);
        /*��˼������������¶Ⱦ��ȣ���ADC��ѹ�ĵ�λ��mv�ĳ���0.1mv�����ADC����ֵ�����10��*/
        rtn_vbat_val = rtn_vbat_val / HKADC_PRECISION;
        if (tmp_ret)
        {
            chg_print_level_message( CHG_MSG_ERR,"%s: bsp_hkadc_convert failed with %d.\n", 
                __func__, tmp_ret);
        }
        else
        {
            chg_print_level_message( CHG_MSG_ERR,"hkadc%d = %d\n", i, rtn_vbat_val);
        }
    }
}

int32_t chg_get_volt_from_adc(CHG_PARAMETER_ENUM param_type)
{
#if (MBB_CHG_PLATFORM_BALONG == FEATURE_ON)

    int ret = 0;
    int32_t rtn_vbat_val = 0;
    int tmp_ret = -1;
    u16 tmp_rtn_vbat_val = 0;

    if (CHG_PARAMETER__BATTERY_VOLT == param_type)
    {
        /*��ص�ѹʹ�ÿ��ؼ�*/
#if (MBB_CHG_COULOMETER == FEATURE_ON)
        rtn_vbat_val = smartstar_battery_voltage_uv_nocali();
        rtn_vbat_val = rtn_vbat_val / 1000;/*��ѹuVת��mV*/
        chg_print_level_message( CHG_MSG_DEBUG,"CHG:VBAT = %d\n", rtn_vbat_val,0,0);

#elif (MBB_CHG_BQ27510 == FEATURE_ON)
        rtn_vbat_val = hisi_battery_voltage();
#else
#if (FEATURE_ON == MBB_CHG_BATT_UNIFORM)
        tmp_ret = bsp_hkadc_convert(g_chg_hardpara_dtsi.batt_volt_hkadc_chan, &tmp_rtn_vbat_val);
#else
        tmp_ret = bsp_hkadc_convert(CHG_BATT_VOLT_HKADC_ID, &tmp_rtn_vbat_val);
#endif
        /*��˼������������¶Ⱦ��ȣ���ADC��ѹ�ĵ�λ��mv�ĳ���0.1mv�����ADC����ֵ�����10��*/
        tmp_rtn_vbat_val = tmp_rtn_vbat_val / HKADC_PRECISION;
        
        rtn_vbat_val = (int32_t)(tmp_rtn_vbat_val * BAT_VOL_MUIT_NUMBER);

        if (tmp_ret < 0)
        {
            chg_print_level_message( CHG_MSG_ERR,"fail to convert, return value %d\n",
                                            tmp_ret,0,0);
        }
        else
        {
            chg_print_level_message( CHG_MSG_DEBUG,"CHG:VBAT = %d\n", rtn_vbat_val,0,0);
        }
#endif/*MBB_CHG_COULOMETER == FEATURE_ON*/
    }
    else if (CHG_PARAMETER__BATT_THERM_DEGC == param_type)
    {
#if (MBB_CHG_COULOMETER == FEATURE_ON)
#if (FEATURE_ON == MBB_CHG_BATT_UNIFORM)
        rtn_vbat_val = bsp_hkadc_get_volt(g_chg_hardpara_dtsi.batt_temp_hkadc_chan);
#else
        rtn_vbat_val = bsp_hkadc_get_volt(CHG_BATT_THERM_HKADC_ID);
#endif
        if (rtn_vbat_val < 0)
        {
            chg_print_level_message( CHG_MSG_ERR,
                "bsp_hkadc_get_volt: get adc value failed!!!\r\n", 0,0,0);
            return rtn_vbat_val;
        }
#else
#if (FEATURE_ON == MBB_CHG_BATT_UNIFORM)
        ret = bsp_hkadc_convert(g_chg_hardpara_dtsi.batt_temp_hkadc_chan, &tmp_rtn_vbat_val);
#else
        ret = bsp_hkadc_convert(CHG_BATT_THERM_HKADC_ID, &tmp_rtn_vbat_val);
#endif
        tmp_rtn_vbat_val = tmp_rtn_vbat_val / HKADC_PRECISION;
        if(ret)
        {
            chg_print_level_message( CHG_MSG_ERR,
                "chg_boot_get_volt_from_adc: get adc value failed!!!\r\n", 0,0,0);
            return ret;
        }
        rtn_vbat_val = (int32_t)tmp_rtn_vbat_val;
#endif
        chg_print_level_message( CHG_MSG_DEBUG,"CHG:BAT_TEMP = %d\n", rtn_vbat_val,0,0);

    }
    else if (CHG_PARAMETER__VBUS_VOLT == param_type)
    {
#if (FEATURE_ON == MBB_CHG_BATT_UNIFORM)
        ret = bsp_hkadc_convert(g_chg_hardpara_dtsi.vbus_volt_hkadc_chan, &tmp_rtn_vbat_val);
#else
        ret = bsp_hkadc_convert(CHG_VBUS_VOLT_HKADC_ID, &tmp_rtn_vbat_val);
#endif
        tmp_rtn_vbat_val = tmp_rtn_vbat_val / HKADC_PRECISION;
        if(ret)
        {
            chg_print_level_message( CHG_MSG_ERR,"chg_boot_get_volt_from_adc: get adc value failed!!!\r\n ",0,0,0 );
            return ret;
        }
        rtn_vbat_val = (int32_t)tmp_rtn_vbat_val;
        chg_print_level_message( CHG_MSG_DEBUG,"CHG:VBUS = %d\n", rtn_vbat_val,0,0);

    }
    else if (CHG_PARAMETER__BATTERY_ID_VOLT == param_type)
    {
#if (FEATURE_ON == MBB_CHG_BATT_UNIFORM)
        ret = bsp_hkadc_convert(g_chg_hardpara_dtsi.batt_id_hkadc_chan, &tmp_rtn_vbat_val);
#else
        ret = bsp_hkadc_convert(CHG_BATT_ID_VOLT_HKADC_ID, &tmp_rtn_vbat_val);
#endif
        tmp_rtn_vbat_val = tmp_rtn_vbat_val / HKADC_PRECISION;
        if(ret)
        {
            chg_print_level_message( CHG_MSG_ERR,"chg_boot_get_volt_from_adc: get adc value failed!!!\r\n ",0,0,0 );
            return ret;
        }
        rtn_vbat_val = (int32_t)tmp_rtn_vbat_val;
        chg_print_level_message( CHG_MSG_DEBUG,"CHG:VBUS = %d\n", rtn_vbat_val,0,0);

    }
    /*USB�±�*/ 
#if ( FEATURE_ON == MBB_CHG_USB_TEMPPT_ILIMIT )
    else if (CHG_PARAMETER__USB_PORT_TEMP_DEGC == param_type)
    {
#if (FEATURE_ON == MBB_CHG_BATT_UNIFORM)
        ret = bsp_hkadc_convert(g_chg_hardpara_dtsi.usb_temp_hkadc_chan, &tmp_rtn_vbat_val);
#else
        ret = bsp_hkadc_convert(CHG_USB_TEMP_CHAN, &tmp_rtn_vbat_val);
#endif
        tmp_rtn_vbat_val = tmp_rtn_vbat_val / HKADC_PRECISION;
        if(ret)
        {
            chg_print_level_message( CHG_MSG_ERR,"chg_boot_get_usb_volt_from_adc: get adc value failed!!!\r\n ",0,0,0 );
            return ret;
        }
        rtn_vbat_val = (int32_t)tmp_rtn_vbat_val;
        chg_print_level_message( CHG_MSG_DEBUG,"CHG:USB_TEMP_VOLT = %d\n", rtn_vbat_val,0,0);
    } 
#endif
    else
    {
        chg_print_level_message( CHG_MSG_INFO,"VADC bad param_type %d\n", param_type,0,0);
        return 0;
    }

    return rtn_vbat_val;

#elif (MBB_CHG_PLATFORM_QUALCOMM == FEATURE_ON)

    int32_t rtn_vadc_val = 0;
    int32_t cnt = 0;
    int32_t param_type_match_flag = 0;

    if (param_type >= CHG_PARAMETER__INVALID)
    {
        chg_print_level_message( CHG_MSG_INFO,"CHG_PLT:chg_get_volt_from_adc: invalid param_type %d\n", param_type);
        return 0;
    }

    for(cnt = 0; cnt < ARRAY_SIZE(chg_adc_adapter_tbl); cnt++)
    {
        if (param_type == chg_adc_adapter_tbl[cnt].chg_ch_typ)
        {
            param_type_match_flag = 1;

            rtn_vadc_val = get_volt_from_adc_channel(chg_adc_adapter_tbl[cnt].adc_ch_sel);

            if (CHG_PARAMETER__BATTERY_VOLT == param_type)
            {
                rtn_vadc_val = rtn_vadc_val * BAT_VOL_MUIT_NUMBER;
            }
            if (CHG_PARAMETER__VBUS_VOLT == param_type)
            {
                rtn_vadc_val = rtn_vadc_val * VPH_PWR_VOL_MUIT_NUMBER;
            }

            chg_print_level_message( CHG_MSG_INFO,"CHG_PLT:chg_get_volt_from_adc: %s = %d\n", chg_adc_adapter_tbl[cnt].chg_ch_name, rtn_vadc_val);
            break;
        }
    }

    if (0 == param_type_match_flag)
    {
        chg_print_level_message( CHG_MSG_ERR,"CHG_PLT:chg_get_volt_from_adc:Can't find param_type %d in chg_boot_adc_adapter_tbl[]!\n", param_type);
        return 0;
    }
    return rtn_vadc_val;

#endif/*MBB_CHG_PLATFORM_QUALCOMM == FEATURE_ON*/
}


int32_t chg_get_batt_volt_value(void)
{
    int32_t vbatt_from_adc = 0;
    int32_t rtn_vbatt_value = 0;
    if (FALSE == g_is_batt_volt_calib_init)
    {
        chg_batt_volt_calib_init();
    }

    vbatt_from_adc = chg_get_volt_from_adc(CHG_PARAMETER__BATTERY_VOLT);
    rtn_vbatt_value = chg_calc_batt_volt_calib_value(vbatt_from_adc);

    return rtn_vbatt_value;
}


int32_t chg_get_vph_pwr_volt_value(void)
{
    int32_t vbatt_from_adc = 0;
    if (FALSE == g_is_batt_volt_calib_init)
    {
        chg_batt_volt_calib_init();
    }

    vbatt_from_adc = chg_get_volt_from_adc(CHG_PARAMETER__VPH_PWR_VOLT);
    chg_print_level_message( CHG_MSG_DEBUG,"CHG_PLT: vph_pwr_volt=%d\r\n",vbatt_from_adc);
    return vbatt_from_adc;
}



#if (FEATURE_ON == MBB_CHG_PLATFORM_BALONG)
int32_t chg_volt_to_temp (int32_t volt_value, CHG_TEMP2ADC_TYPE *AdcTable, uint32_t table_size)
#elif (FEATURE_ON == MBB_CHG_PLATFORM_QUALCOMM)
int32_t chg_volt_to_temp (int32_t volt_value, CHG_TEMP_ADC_TYPE *AdcTable, uint32_t table_size)
#endif
{
    boolean   desending_flag = TRUE;
    uint32_t  idx = 0;

    if(NULL == AdcTable)
    {
        chg_print_level_message( CHG_MSG_ERR,"CHG_PLT: invalid input para!!!\r\n ");
        /*ֱ�ӷ��ص�ѹ*/
        return volt_value;
    }

    /*�жϱ��ǰ���ѹֵ����or����*/
    if(1 < table_size)
    {
        if(AdcTable[0].voltage < AdcTable[1].voltage)
        {
            desending_flag = FALSE;
        }
    }

    /*��������ڱ���е�λ��*/
    while(idx < table_size)
    {
        if((TRUE == desending_flag) && (AdcTable[idx].voltage < volt_value))
        {
            break;
        }
        else if((FALSE == desending_flag) && (AdcTable[idx].voltage > volt_value))
        {
            break;
        }
        else
        {
            idx++;
        }
    }

    /*����ֵ�жϣ�����ֵ��ʽ���*/
    if(0 == idx)
    {
        return AdcTable[0].temperature;
    }
    else if(table_size == idx)
    {
        return AdcTable[table_size - 1].temperature;
    }
    else
    {
        /*��ֹб�ʼ���Ƿ�*/
        if(AdcTable[idx - 1].voltage == AdcTable[idx].voltage)
        {
            chg_print_level_message( CHG_MSG_ERR,"CHG_PLT: slop invalid!!!\r\n ");
            return AdcTable[idx - 1].temperature;
        }

        return (((AdcTable[idx - 1].temperature - AdcTable[idx].temperature )
            * (volt_value - AdcTable[idx - 1].voltage)) / (AdcTable[idx - 1].voltage - AdcTable[idx].voltage)
            + AdcTable[idx - 1].temperature);
    }
}


int32_t chg_get_temp_value(CHG_PARAMETER_ENUM param_type)
{
    int32_t voltage_from_adc = 0;
    int32_t rtn_temp_value = 0;

    voltage_from_adc = chg_get_volt_from_adc(param_type);

    chg_print_level_message(CHG_MSG_DEBUG,"CHG_PLT:voltage_from_adc =%d \r\n ",voltage_from_adc);

    switch(param_type)
    {
        case CHG_PARAMETER__BATT_THERM_DEGC:
        {
#if ((MBB_CHG_BQ27510 == FEATURE_ON) && (MBB_CHG_PLATFORM_BALONG == FEATURE_ON))
            rtn_temp_value = hisi_battery_temperature();
#else
            rtn_temp_value = chg_volt_to_temp(voltage_from_adc, g_adc_batt_therm_map,
                                        sizeof(g_adc_batt_therm_map) / sizeof(g_adc_batt_therm_map[0]));
#endif/*(MBB_CHG_BQ27510 == FEATURE_ON) && (MBB_CHG_PLATFORM_BALONG == FEATURE_ON)*/
            chg_print_level_message( CHG_MSG_DEBUG,"CHG_PLT:rtn_temp_value =%d \r\n ",rtn_temp_value);

            break;
        }
        /*�弶�¶ȣ���Ӧ�ĵ�ѹ�¶ȶ��ձ�ʹ�õ�صĵ�ѹ�¶ȶ��ձ���ͬ��Ʒ������Ҫ����
         ��Ӱ弶�¶ȵĵ�ѹ�¶ȶ��ձ�*/
        case CHG_PARAMETER__BATT_BOT_THERM_DEGC:
        {
            rtn_temp_value = chg_volt_to_temp(voltage_from_adc, g_adc_batt_therm_map,
                                        sizeof(g_adc_batt_therm_map) / sizeof(g_adc_batt_therm_map[0]));
            break;
        }
        /*USB�±�*/ 
#if ( FEATURE_ON == MBB_CHG_USB_TEMPPT_ILIMIT )
        case CHG_PARAMETER__USB_PORT_TEMP_DEGC:
        {
            rtn_temp_value = chg_volt_to_temp(voltage_from_adc, g_adc_usb_therm_map,
                                        sizeof(g_adc_usb_therm_map) / sizeof(g_adc_usb_therm_map[0]));
            chg_print_level_message( CHG_MSG_DEBUG,"CHG_PLT:USB_TEMP:rtn_temp_value =%d.\n",rtn_temp_value);
            break;
        } 
#endif
        default:
        {
            /*�ȷ��ص�ѹֵ*/
            rtn_temp_value = voltage_from_adc;
            break;
        }
    }

    return rtn_temp_value;
}

#if (MBB_CHG_EXTCHG == FEATURE_ON)
/*���º�����ͬƽ̨�ڲ�ʵ�ֲ�ͬ����ֲ��Ա��ע��*/

int extchg_gpio_init(void)
{
#if (MBB_CHG_PLATFORM_BALONG == FEATURE_ON)
    /*Balongƽ̨������GPIO����*/
#elif (MBB_CHG_PLATFORM_QUALCOMM == FEATURE_ON)
    if (!gpio_is_valid(EXTCHG_CONTROL_GPIO_EN1))         /* */
    {
        chg_print_level_message( CHG_MSG_DEBUG,"CHG_PLT: set EXTCHG_EN1 GPIO59 failed!!\r\n ");
        return -1;
    }
    if (!gpio_is_valid(EXTCHG_CONTROL_GPIO_EN2))         /* */
    {
        chg_print_level_message( CHG_MSG_DEBUG,"CHG_PLT: set EXTCHG_EN2 GPIO60 failed!!\r\n ");
        return -1;
    }
    if (gpio_request(EXTCHG_CONTROL_GPIO_EN1, "EXTCHG_EN1"))         /* */
    {
        chg_print_level_message( CHG_MSG_DEBUG,"CHG_PLT: request EXTCHG_EN1 GPIO59 failed!!\r\n ");
        return -1;
    }
    if (gpio_request(EXTCHG_CONTROL_GPIO_EN2, "EXTCHG_EN2"))         /* */
    {
        chg_print_level_message( CHG_MSG_DEBUG,"CHG_PLT: request EXTCHG_EN2 GPIO60 failed!!\r\n ");
        return -1;
    }
#endif/*MBB_CHG_PLATFORM_QUALCOMM == FEATURE_ON*/

#if (FEATURE_ON == MBB_CHG_APORT_EXTCHG)
    /*requesting the gpio of ext-charge*/
    if (!gpio_is_valid(EXTCHG_CHG_ENABLE))
    {
        chg_print_level_message( CHG_MSG_ERR, \
            "CHG_PLT: EXTCHG_CHG_ENABLE is not valid!!\r\n ");
        return -1;
    }
    if (!gpio_is_valid(RE_ILIM_1A_GPIO))
    {
        chg_print_level_message( CHG_MSG_ERR, \
            "CHG_PLT: RE_ILIM_1A_GPIO is not valid!!\r\n ");
        return -1;
    }
    if (!gpio_is_valid(RE_ILIM_2A_GPIO))
    {
        chg_print_level_message( CHG_MSG_ERR, \
            "CHG_PLT: RE_ILIM_2A_GPIO is not valid!!\r\n ");
        return -1;
    }
    if (!gpio_is_valid(OTG_ON_CTRL_GPIO))
    {
        chg_print_level_message( CHG_MSG_ERR, \
            "CHG_PLT: OTG_ON_CTRL_GPIO is not valid!!\r\n ");
        return -1;
    }
    if (!gpio_is_valid(EXTCHG_OTG_DET_GPIO))
    {
        chg_print_level_message( CHG_MSG_ERR, \
            "CHG_PLT: EXTCHG_OTG_DET_GPIO is not valid!!\r\n ");
        return -1;
    }
    if (!gpio_is_valid(USB_GPIO_DM))
    {
        chg_print_level_message( CHG_MSG_ERR, \
            "CHG_PLT: USB_GPIO_DM is not valid!!\r\n ");
        return -1;
    }
    if (!gpio_is_valid(USB_GPIO_DP))
    {
        chg_print_level_message( CHG_MSG_ERR, \
            "CHG_PLT: USB_GPIO_DP is not valid!!\r\n ");
        return -1;
    }

    if (gpio_request(EXTCHG_OTG_DET_GPIO, "EXTCHG_OTG_DET_GPIO"))
    {
        chg_print_level_message( CHG_MSG_ERR, \
            "CHG_PLT: request EXTCHG_OTG_DET_GPIO failed!!\r\n ");
        return -1;
    }

    if (gpio_request(USB_GPIO_DM, "USB_GPIO_DM"))
    {
        chg_print_level_message( CHG_MSG_ERR, \
            "CHG_PLT: request USB_GPIO_DM failed!!\r\n ");
        gpio_free(EXTCHG_OTG_DET_GPIO);
        return -1;
    }

    if (gpio_request(USB_GPIO_DP, "USB_GPIO_DP"))
    {
        chg_print_level_message( CHG_MSG_ERR, \
            "CHG_PLT: request USB_GPIO_DP failed!!\r\n ");
        gpio_free(EXTCHG_OTG_DET_GPIO);
        gpio_free(USB_GPIO_DM);
        return -1;
    }

    if (gpio_request(EXTCHG_CHG_ENABLE, "EXTCHG_CHG_ENABLE"))
    {
        chg_print_level_message( CHG_MSG_ERR, \
            "CHG_PLT: request EXTCHG_CHG_ENABLE failed!!\r\n ");
        gpio_free(EXTCHG_OTG_DET_GPIO);
        gpio_free(USB_GPIO_DM);
        gpio_free(USB_GPIO_DP);
        return -1;
    }

    if (gpio_request(OTG_ON_CTRL_GPIO, "OTG_ON_CTRL_GPIO"))
    {
        chg_print_level_message( CHG_MSG_ERR, \
            "CHG_PLT: request OTG_ON_CTRL_GPIO failed!!\r\n ");
        gpio_free(EXTCHG_OTG_DET_GPIO);
        gpio_free(USB_GPIO_DM);
        gpio_free(USB_GPIO_DP);
        gpio_free(EXTCHG_CHG_ENABLE);
        return -1;
    }

    if (gpio_request(RE_ILIM_1A_GPIO, "RE_ILIM_1A_GPIO"))
    {
        chg_print_level_message( CHG_MSG_ERR, \
            "CHG_PLT: request RE_ILIM_1A_GPIO failed!!\r\n ");
        gpio_free(EXTCHG_OTG_DET_GPIO);
        gpio_free(USB_GPIO_DM);
        gpio_free(USB_GPIO_DP);
        gpio_free(EXTCHG_CHG_ENABLE);
        gpio_free(OTG_ON_CTRL_GPIO);
        return -1;
    }

    if (gpio_request(RE_ILIM_2A_GPIO, "RE_ILIM_2A_GPIO"))
    {
        chg_print_level_message( CHG_MSG_ERR, \
            "CHG_PLT: request RE_ILIM_2A_GPIO failed!!\r\n ");
        gpio_free(EXTCHG_OTG_DET_GPIO);
        gpio_free(USB_GPIO_DM);
        gpio_free(USB_GPIO_DP);
        gpio_free(EXTCHG_CHG_ENABLE);
        gpio_free(OTG_ON_CTRL_GPIO);
        gpio_free(RE_ILIM_1A_GPIO);
        return -1;
    }

    if (gpio_request(SHORT_PROTECT_EN, "SHORT_PROTECT_EN"))
    {
        chg_print_level_message( CHG_MSG_ERR, \
            "CHG_PLT: request SHORT_PROTECT_EN failed!!\r\n ");
        gpio_free(EXTCHG_OTG_DET_GPIO);
        gpio_free(USB_GPIO_DM);
        gpio_free(USB_GPIO_DP);
        gpio_free(EXTCHG_CHG_ENABLE);
        gpio_free(OTG_ON_CTRL_GPIO);
        gpio_free(RE_ILIM_1A_GPIO);
        gpio_free(RE_ILIM_2A_GPIO);
        return -1;
    }
#endif

    return 0;
}

void extchg_gpio_control(uint32_t gpio,int32_t level)
{
   gpio_direction_output(gpio,level);
}

#endif/*MBB_CHG_EXTCHG*/


#if (MBB_CHG_WIRELESS == FEATURE_ON)
/*���º�����ͬƽ̨�ڲ�ʵ�ֲ�ͬ����ֲ��Ա��ע��*/

int wireless_gpio_init(void)
{
    if (!gpio_is_valid(WIRELESS_CONTROL_GPIO_EN1))         /* */
    {
        chg_print_level_message( CHG_MSG_DEBUG,"CHG_PLT: set WIRELESS_EN1 GPIO59 failed!!\r\n ");
        return -1;
    }
    if (!gpio_is_valid(WIRELESS_CONTROL_GPIO_EN2))         /* */
    {
        chg_print_level_message( CHG_MSG_DEBUG,"CHG_PLT: set WIRELESS_EN2 GPIO60 failed!!\r\n ");
        return -1;
    }
    if (gpio_request(WIRELESS_CONTROL_GPIO_EN1, "WIRELESS_EN1"))         /* */
    {
        chg_print_level_message( CHG_MSG_DEBUG,"CHG_PLT: request WIRELESS_EN1 GPIO59 failed!!\r\n ");
        return -1;
    }
    if (gpio_request(WIRELESS_CONTROL_GPIO_EN2, "WIRELESS_EN2"))         /* */
    {
        chg_print_level_message( CHG_MSG_DEBUG,"CHG_PLT: request WIRELESS_EN2 GPIO60 failed!!\r\n ");
        return -1;
    }
    return 0;
}

void wireless_gpio_control(uint32_t gpio,int32_t level)
{
   gpio_direction_output(gpio,level);
}

#endif/*MBB_CHG_WIRELESS*/


#ifdef CHG_STUB
boolean is_in_update_mode(void)
{
    return FALSE;
}
#elif (FEATURE_ON == MBB_CHG_PLATFORM_QUALCOMM)
boolean is_in_update_mode(void)
{
    static_smem_vendor0 *smem_data = NULL;
    smem_data = (static_smem_vendor0 *)SMEM_HUAWEI_ALLOC(SMEM_ID_VENDOR0, sizeof(static_smem_vendor0));
    if(NULL == smem_data)
    {
        return FALSE;
    }
    if(SMEM_DLOAD_FLAG_NUM == smem_data->smem_dload_flag)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
#elif (FEATURE_ON == MBB_CHG_PLATFORM_BALONG)
boolean is_in_update_mode(void)
{
    huawei_smem_info *smem_data = NULL;
    smem_data = (huawei_smem_info *)SRAM_DLOAD_ADDR;


    if(SMEM_DLOAD_FLAG_NUM == smem_data->smem_dload_flag)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
#endif/*CHG_STUB*/

/**********************************************************************
�� �� ��  : int32_t chg_config_para_read(int32_t num)
��������  : ���������ò���
�������  : none
�������  : �ޡ�
�� �� ֵ  : �ޡ�
ע������  : �ɸ���ƽ̨��ͬ�޸�ʵ�ַ���������NV�ȡ�
***********************************************************************/
int32_t chg_config_para_read(uint16_t nvID, void *pItem, uint32_t ulLength)
{
#if (MBB_CHG_PLATFORM_BALONG == FEATURE_ON)
    if(bsp_nvm_read(nvID,pItem,ulLength))
    {
        chg_print_level_message(CHG_MSG_INFO, "CHG_STM:READ NV ERROR!!!\n");
        return CHG_ERROR;
    }
    else
    {
        chg_print_level_message(CHG_MSG_INFO, "CHG_STM:READ NV OK!!!\n");
        return CHG_OK;
    }
#elif (MBB_CHG_PLATFORM_QUALCOMM == FEATURE_ON)
    /*��ͨƽ̨��֧��*/
    return CHG_ERROR;
#endif/*MBB_CHG_PLATFORM_QUALCOMM == FEATURE_ON*/
}

#if (MBB_CHG_COMPENSATE == FEATURE_ON)

#ifdef CHG_STUB
void chg_set_fact_release_mode(boolean on)
{
    return;
}
#else

#ifdef  CONFIG_MBB_KERNEL_NOTIFY
extern int huawei_notify_kernel_A2M(unsigned int notify_id);
#endif

void chg_set_fact_release_mode(boolean on)
{
#if (MBB_CHG_PLATFORM_BALONG == FEATURE_ON)
    /*Balongƽ̨��ʹ�ù����ڴ�ķ�ʽ*/
#elif (MBB_CHG_PLATFORM_QUALCOMM == FEATURE_ON)

    dynamic_smem_vendor1 *smem_data = NULL;

#ifdef  CONFIG_MBB_KERNEL_NOTIFY
    int ret = -1 ;
#endif/*CONFIG_MBB_KERNEL_NOTIFY*/
    smem_data = (dynamic_smem_vendor1 *)SMEM_HUAWEI_ALLOC(SMEM_ID_VENDOR1 ,
                                        sizeof(dynamic_smem_vendor1 ));
    if(NULL == smem_data)
    {
        chg_print_level_message( CHG_MSG_ERR,"CHG_PLT: smem alloc fail!!!\r\n ");
        return;
    }

    if (TRUE == on)
    {
        smem_data->smem_fact_chg_flag = SMEM_FACT_RELEASE_ON;
    }
    else
    {
        smem_data->smem_fact_chg_flag = SMEM_FACT_RELEASE_OFF;
    }

#ifdef  CONFIG_MBB_KERNEL_NOTIFY
    ret = huawei_notify_kernel_A2M(NOTIFY_ID_0);
    if ( ret < 0 )
    {
        printk(KERN_EMERG "%s: huawei_notify_kernel_A2M(0) fail \n",__func__);
    }
#endif/*CONFIG_MBB_KERNEL_NOTIFY*/

#endif/*MBB_CHG_PLATFORM_QUALCOMM == FEATURE_ON*/

    return;
}
#endif/*CHG_STUB*/
#endif/*MBB_CHG_COMPENSATE == FEATURE_ON*/


#ifdef CHG_STUB
void chg_get_hw_version_id(void)
{
}
#else
void chg_get_hw_version_id(void)
{
#if (MBB_CHG_PLATFORM_BALONG == FEATURE_ON)
    g_hardware_version_id = mbb_version_get_board_type();
    chg_print_level_message(CHG_MSG_ERR, 
        "CHG_PLT:get g_hardware_version_id = 0x%x.\n",g_hardware_version_id);
#elif (MBB_CHG_PLATFORM_QUALCOMM == FEATURE_ON)
        dynamic_smem_vendor1 *smem_data = NULL;
        smem_data = (dynamic_smem_vendor1 *)SMEM_HUAWEI_ALLOC(SMEM_ID_VENDOR1 ,
                                             sizeof(dynamic_smem_vendor1 ));
        if(NULL == smem_data)
        {
                chg_print_level_message(CHG_MSG_ERR, "chg_get_hw_version_id smem_data is NULL\n");
                return;
        }
        g_hardware_version_id = (uint32_t)smem_data->hwid.hard_main_version_id;
        chg_print_level_message(CHG_MSG_ERR, "CHG_PLT:get g_hardware_version_id == %d\n",g_hardware_version_id);
#endif/*MBB_CHG_PLATFORM_QUALCOMM == FEATURE_ON*/
}

#endif/*CHG_STUB*/

