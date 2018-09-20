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

#ifndef _CHG_CHARGE_CUST_H
#define _CHG_CHARGE_CUST_H
/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
*----------------------------------------------*/
#include "product_config.h"
#include "product_nv_def.h"

/*----------------------------------------------*
 * �ⲿ����˵��                                 *
*----------------------------------------------*/
/*----------------------------------------------*
 * �ⲿ����ԭ��˵��                             *
*----------------------------------------------*/
/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
*----------------------------------------------*/

/*----------------------------------------------*
 * ģ�鼶����                                   *
*----------------------------------------------*/
/*----------------------------------------------*
 * ��������                                     *
*----------------------------------------------*/
/*----------------------------------------------*
 * �궨��                                       *
*----------------------------------------------*/
/*----------------------------------------------*
* ���������Ͷ���˵��                         *
*----------------------------------------------*/
/*=============���º궨���Ǹ�ͨר�ã���������exl�Զ�����===============*/
#ifndef FEATURE_ON
#define FEATURE_ON  1
#endif/*FEATURE_ON*/

#ifndef FEATURE_OFF
#define FEATURE_OFF 0
#endif/*FEATURE_OFF*/
/*************************BEGIN:��Ʒ�������Ժ궨��***********************/
/*��ŵ繦���ܿ��ƺ�,������ƺ꣬�ⲿ�����������˴����ٶ���*/
//#define    MBB_CHARGE
/*����LINUXƽ̨�򿪴�ƽ̨���Ժ�*/
#ifndef   MBB_CHG_PLATFORM_BALONG
#define   MBB_CHG_PLATFORM_BALONG          FEATURE_OFF
#endif/*HUAWEI_CHG_PLATFORM_BALONG*/

/*��ͨlinuxƽ̨�򿪴�ƽ̨���Ժ�*/
#ifndef   MBB_CHG_PLATFORM_QUALCOMM
#define   MBB_CHG_PLATFORM_QUALCOMM        FEATURE_OFF
#endif/*HUAWEI_CHG_PLATFORM_QUALCOMM*/

/*LCD��Ʒ���Ժ�*/
#ifndef   MBB_CHG_LCD
#define   MBB_CHG_LCD                      FEATURE_OFF
#endif/*MBB_CHG_LCD*/

/*LED��Ʒ���Ժ�*/
#ifndef   MBB_CHG_LED
#define   MBB_CHG_LED                      FEATURE_OFF
#endif/*MBB_CHG_LED*/

#ifndef   MBB_CHG_OLED
#define   MBB_CHG_OLED                     FEATURE_OFF
#endif/*MBB_CHG_OLED*/

/*���ÿ��ؼ�BQ27510���Ժ�*/
#ifndef   MBB_CHG_BQ27510
#define   MBB_CHG_BQ27510                  FEATURE_OFF
#endif/*MBB_CHG_BQ27510*/

/*BQ24196/BQ24192/BQ24296���оƬ���Ժ�*/
#ifndef   MBB_CHG_BQ24196
#define   MBB_CHG_BQ24196                  FEATURE_OFF
#endif/*MBB_CHG_BQ24196*/

/*BQ25892���оƬ���Ժ�*/
#ifndef   MBB_CHG_BQ25892
#define   MBB_CHG_BQ25892                  FEATURE_OFF
#endif/*MBB_CHG_BQ25892*/

/*SMB1351���оƬ���Ժ�*/
#ifndef   MBB_CHG_SMB1351
#define   MBB_CHG_SMB1351                FEATURE_OFF
#endif/*MBB_CHG_SMB1351*/

/*��ѹ������Ժ�*/
#ifndef   MBB_CHG_HVDCP_CHARGE
#define   MBB_CHG_HVDCP_CHARGE             FEATURE_OFF
#endif/*MBB_CHG_HVDCP_CHARGE*/

/*���������Ժ�*/
#ifndef   MBB_CHG_EXTCHG
#define   MBB_CHG_EXTCHG                   FEATURE_OFF
#endif/*MBB_CHG_EXTCHG*/

/*���߳�����Ժ�*/
#ifndef   MBB_CHG_WIRELESS
#define   MBB_CHG_WIRELESS                 FEATURE_OFF
#endif/*MBB_CHG_WIRELESS*/

/*�����������Ժ�*/
#ifndef   MBB_CHG_COMPENSATE
#define   MBB_CHG_COMPENSATE               FEATURE_OFF
#endif/*MBB_CHG_COMPENSATE*/

/*���³�����Ժ�*/
#ifndef   MBB_CHG_WARM_CHARGE
#define   MBB_CHG_WARM_CHARGE              FEATURE_OFF
#endif/*MBB_CHG_WARM_CHARGE*/

/*power supply���Ժ�*/
#ifndef   MBB_CHG_POWER_SUPPLY
#define   MBB_CHG_POWER_SUPPLY             FEATURE_OFF
#endif/*MBB_CHG_POWER_SUPPLY*/

/*���ؼ����Ժ�*/
#ifndef   MBB_CHG_COULOMETER
#define   MBB_CHG_COULOMETER              FEATURE_OFF
#endif/*MBB_CHG_COULOMETER*/

/*��ѹ������Ժ�*/
#ifndef   MBB_CHG_HIGH_VOLT_BATT
#define   MBB_CHG_HIGH_VOLT_BATT          FEATURE_OFF
#endif/*MBB_CHG_HIGH_VOLT_BATT*/

/*�ɲ�ж������Ժ꣬����֧�ֲ���AT��ѯ���ɲ�ж��ش򿪴˺꣬�ǿɲ�ж��عرմ˺�*/
#ifndef   MBB_CHG_BAT_KNOCK_DOWN
#define   MBB_CHG_BAT_KNOCK_DOWN          FEATURE_OFF
#endif/*MBB_CHG_BAT_KNOCK_DOWN*/

/*�������ƴ�USB/�����ȡ�����Ժ�*/
#ifndef   MBB_CHG_CURRENT_SUPPLY_LIMIT
#define   MBB_CHG_CURRENT_SUPPLY_LIMIT     FEATURE_OFF
#endif/*MBB_CHG_CURRENT_SUPPLY_LIMIT*/

/*�����س�������궨�� 20%�궨���Ժ�*/
#ifndef   MBB_CHG_CHARGE_CURRENT_LIMIT
#define   MBB_CHG_CHARGE_CURRENT_LIMIT     FEATURE_OFF
#endif/*MBB_CHG_CURRENT_SUPPLY_LIMIT*/
/*=============���Ϻ궨���Ǹ�ͨר�ã���������exl�Զ�����===============*/

/*=============���������ݽṹ���壬����ʱ���漰�޸�===================*/
#ifndef TRUE
#define TRUE                  1
#endif
#ifndef FALSE
#define FALSE                 0
#endif
#ifndef uint8_t
typedef unsigned char    uint8_t;
#endif

#ifndef int8_t
typedef signed char    int8_t;
#endif

#ifndef boolean
typedef uint8_t    boolean;
#endif

/*�¶ȶ�Ӧ��ѹ�ṹ,��������CHG_TEMP_ADC_TYPE ��hisi�����ж��������ݽṹ
��ͬ������CHG_TEMP2ADC_TYPE����VT��ṹ*/
#if (MBB_CHG_PLATFORM_BALONG == FEATURE_ON)
typedef struct
{
    int   temperature;
    int   voltage;
}CHG_TEMP2ADC_TYPE;
#elif (MBB_CHG_PLATFORM_QUALCOMM == FEATURE_ON)
typedef struct
{
    int   temperature;
    int   voltage;
}CHG_TEMP_ADC_TYPE;
#endif

/*���ID��Ϣ����*/
typedef enum
{
    CHG_BATT_ID_DEF                     = 0 ,
    CHG_BATT_ID_FEIMAOTUI_2300MAH       = 1 ,
    CHG_BATT_ID_XINGWANGDA_1900MAH      = 2 ,
    CHG_BATT_ID_FEIMAOTUI_1900MAH       = 3 ,
    CHG_BATT_ID_XINGWANGDA_1500MAH      = 4 ,
    CHG_BATT_ID_FEIMAOTUI_1500MAH       = 5 ,
    CHG_BATT_ID_LISHEN_1500MAH          = 6 ,
    CHG_BATT_ID_XINWANGDA_3000MAH       = 7 ,
    CHG_BATT_ID_FEIMAOTUI_4800MAH       = 8 ,
    CHG_BATT_ID_XINGWANGDA_4800MAH      = 9 ,
    CHG_BATT_ID_FEIMAOTUI_3000MAH       = 10 ,
    CHG_BATT_ID_FEIMAOTUI_6400MAH       = 11 ,
    CHG_BATT_ID_XINGWANGDA_6400MAH      = 12 ,
    CHG_BATT_ID_MAX
}CHG_BATT_ID_TYPE;

/*�����ز����ṹ��*/
struct chg_batt_data {
    unsigned int        id_voltage_min;         //��ѹ���IDʶ�����С��ѹ���ݲο���ѹ��ͬ��ѹ��ͬ
    unsigned int        id_voltage_max;         //��ѹ���IDʶ�������ѹ���ݲο���ѹ��ͬ��ѹ��ͬ
    boolean             is_20pct_calib;         //�Ƿ���Ҫ������20%�궨��־
    CHG_BATT_ID_TYPE    batt_id;
    CHG_SHUTOFF_VOLT_PROTECT_NV_TYPE    chg_batt_volt_paras;
};

/*=================================================================
 CHG_CHGR_UNKNOWN: Chgr type has not been check completely from USB module.
 CHG_WALL_CHGR   : Wall standard charger, which D+/D- was short.
 CHG_USB_HOST_PC : USB HOST PC or laptop or pad, etc.
 CHG_NONSTD_CHGR : D+/D- wasn't short and USB enumeration failed.
 CHG_CHGR_INVALID: External Charger invalid or absent.
 ==================================================================*/
typedef enum
{
    /*δ֪����*/
    CHG_CHGR_UNKNOWN        = 0 ,
    /*��׼�����*/
    CHG_WALL_CHGR           = 1 ,
    /*USB*/
    CHG_USB_HOST_PC         = 2 ,
    /*�Ǳ�׼�����*/
    CHG_NONSTD_CHGR         = 3 ,
    /*���߳����*/
    CHG_WIRELESS_CHGR       = 4 ,
    /*��������*/
    CHG_EXGCHG_CHGR         = 5 ,
    /*����*/
    CHG_500MA_WALL_CHGR     = 6 ,
    /*cradle*/
    CHG_USB_OTG_CRADLE      = 7 ,
    /*��ѹ�����*/
    CHG_HVDCP_CHGR          = 8 ,
    /*�����������*/
    CHG_CHGR_INVALID,
}chg_chgr_type_t;

/*��ѹ���0-10��֮�������ṹ��*/
typedef struct
{
    /*���������*/
    chg_chgr_type_t chgr_type;
    /*��������*/
    unsigned int current_limit;
    /*I_USB��������*/
    unsigned int current_limit_usb;
}charger_current_limit_st;

typedef struct
{
    unsigned int pwr_supply_current_limit_in_mA;
    unsigned int chg_current_limit_in_mA;
    unsigned int chg_CV_volt_setting_in_mV;
    unsigned int chg_taper_current_in_mA;
    boolean  chg_is_enabled;
}chg_hw_param_t;

typedef enum
{
    /*CHG_STM_INIT_ST: NOT real state, just for initialize.*/
    CHG_STM_INIT_ST = -1,
    CHG_STM_TRANSIT_ST = 0,
    CHG_STM_FAST_CHARGE_ST,
    CHG_STM_MAINT_ST,
    CHG_STM_INVALID_CHG_TEMP_ST,
    CHG_STM_BATTERY_ONLY,
    CHG_STM_WARMCHG_ST,
    CHG_STM_HVDCP_CHARGE_ST,
    CHG_STM_MAX_ST,
}chg_stm_state_type;

typedef struct
{
    int tem_min;
    int tem_max;
    int revise_tem;
}extchg_batt_temp_revise;

/*=============���������ݽṹ���壬����ʱ���漰�޸�===================*/

/*=============�����ǳ����ز���������ʱ��Ҫ����Ӳ�������޸�=======*/
/*������оƬI2C addr����ͬ���оƬ�в���*/
#define I2C_CHARGER_CHIP_SLAVE_ADDR    (0x6B)
/*SMB1351 ��Ҫ����Ϊ0x1D*/
//#define I2C_CHARGER_CHIP_SLAVE_ADDR    (0x1D)

#define    CHG_ENABLE_GPIO                     (GPIO_SLEEP_0_4)   /* charge enable */
#define    CHG_BATT_LOW_INT                    (GPIO_NULL)        /* batt low int */
#define    CHG_BATT_ID_CHAN                    (3)                /* batt id hkadc channel */
#define    CHG_BATT_TEMP_CHAN                  (0)                /* batt temp hkadc channel */
#define    CHG_BATT_VOLT_CHAN                  (0)                /* batt volt hkadc channel */
#define    CHG_VBUS_VOLT_CHAN                  (11)               /* vbus volt hkadc channel */
#define    CHG_USB_TEMP_CHAN                   (12)                /* usb temp hkadc channel */
#define    EXTCHG_CHG_ENABLE                   (GPIO_SLEEP_0_7)   /* A-port ext-charge enable */
#define    SHORT_PROTECT_EN                    (GPIO_SLEEP_1_0)   /* A-port ext-charge short protect enable */
#define    RE_ILIM_1A_GPIO                     (GPIO_SLEEP_0_2)   /* A-port ext-charge current limit */
#define    RE_ILIM_2A_GPIO                     (GPIO_SLEEP_0_3)   /* A-port ext-charge current limit */
#define    EXTCHG_SHORT_VOLT_CHAN              (1)                /* A-port ext-charge short detect */
#define    OTG_ON_CTRL_GPIO                    (GPIO_SLEEP_1_7)   /* A-port ext-charge release power-key */
#define    EXTCHG_OTG_DET_GPIO                 (GPIO_0_6)         /* A-port ext-charge detect */
#define    GPIO_USB_SELECT                     (GPIO_SLEEP_1_3)   /* A-port M-port select */
#define    USB_GPIO_DM                         (GPIO_5_1)         /* A-port DM detect */
#define    USB_GPIO_DP                         (GPIO_5_2)         /* A-port DP detect */
#define    HW_SYS_GPIO_SHUTDOWN                (GPIO_6_6)         /* hardware system shutdown gpio */

/*�����λ����¶�����*/
#define  BATT_NOT_PRESENCE_TEMP    (-30)
/*�������쳣��ؼ��*/
#define CHG_AUTO_CUR_CTRL_DIE_TEMP_MIN      (-30)

/*��ض�·��ѹ����*/
#define BAT_CHECK_SHORTED_VOLT    (1000)

/*�˳�丳��ص�ѹ����*/
#define BAT_CHECK_JUDGMENT_VOLT    (3000)

/*�͵��ֹ������ص�ѹ����*/
#define BAT_CHECK_POWERON_VOLT    (3300)

/*kernle����ص�ѹ����*/
#define CHG_SHORT_CIRC_BATTERY_THRES    (2700)

#define CHG_SUB_LOW_TEMP_TOP    (10)
#define CHG_TEMP_RESUM    (3)

#if (MBB_CHG_COMPENSATE == FEATURE_ON)
#if (MBB_CHG_HIGH_VOLT_BATT == FEATURE_ON)
/*��ز�����ֵ40%����*/
#define TBAT_SUPPLY_VOLT                        (3780)
/*��طŵ���ֵ80%����*/
#define TBAT_DISCHG_VOLT                        (4085)
/*��س���ֹ��ѹ */
#define TBAT_SUPPLY_STOP_VOLT                   (3825)
/*��طŵ��ֹ��ѹ */
#define TBAT_DISCHG_STOP_VOLT                   (4050)
#else
/*��ز�����ֵ40%����*/
#define TBAT_SUPPLY_VOLT                        (3350)
/*��طŵ���ֵ80%����*/
#define TBAT_DISCHG_VOLT                        (3970)
/*��س���ֹ��ѹ */
#define TBAT_SUPPLY_STOP_VOLT                   (3500)
/*��طŵ��ֹ��ѹ */
#define TBAT_DISCHG_STOP_VOLT                   (3865)
#if defined(BSP_CONFIG_BOARD_E5885Ls_93a)
/*��ë�ȵ�ز�����ֵ*/
#define TBAT_SUPPLY_VOLT_FMT                    (3350)
/*��ë�ȵ�طŵ���ֵ*/
#define TBAT_DISCHG_VOLT_FMT                    (3700)
/*��ë�ȵ�س���ֹ��ѹ */
#define TBAT_SUPPLY_STOP_VOLT_FMT               (3400)
/*��ë�ȵ�طŵ��ֹ��ѹ */
#define TBAT_DISCHG_STOP_VOLT_FMT               (3580)
/*�������ز�����ֵ*/
#define TBAT_SUPPLY_VOLT_XWD                    (3350)
/*�������طŵ���ֵ*/
#define TBAT_DISCHG_VOLT_XWD                    (3700)
/*�������س���ֹ��ѹ */
#define TBAT_SUPPLY_STOP_VOLT_XWD               (3400)
/*�������طŵ��ֹ��ѹ */
#define TBAT_DISCHG_STOP_VOLT_XWD               (3580)
#endif
#endif/*HUAWEI_CHG_HIGH_VOLT_BATT*/
#endif/*(MBB_CHG_COMPENSATE == FEATURE_ON)*/

/* ���У׼����С������ѹ**/
#if (MBB_CHG_HIGH_VOLT_BATT == FEATURE_ON)
#define CHG_BATT_CALI_MIN_VOLT  (3450)/*��ѹУ׼����*/
#define CHG_BATT_CALI_MAX_VOLT  (4350)/*��ѹУ׼����*/
#else
#define CHG_BATT_CALI_MIN_VOLT  (3400)/*��ѹУ׼����*/
#define CHG_BATT_CALI_MAX_VOLT  (4200)/*��ѹУ׼����*/
#endif/*MBB_CHG_HIGH_VOLT_BATT == FEATURE_ON*/

/*��ص�ѹADC�ɼ���ѹֵ*/
#define BAT_VOL_MUIT_NUMBER        804 / 200  /*�˴��궨�岻Ҫ������*/
/*VPH_PWR��ѹADC�ɼ���ѹֵ*/
#define VPH_PWR_VOL_MUIT_NUMBER    804 / 200  /*�˴��궨�岻Ҫ������*/

#define CHG_FAST_CHG_TIMER_VALUE                (12 * SECOND_IN_HOUR)
#define CHG_POWEROFF_FAST_CHG_TIMER_VALUE       (12 * SECOND_IN_HOUR)

#define CHG_FAST_USB_TIMER_VALUE                (24 * SECOND_IN_HOUR)
#define CHG_POWEROFF_FAST_USB_TIMER_VALUE       (24 * SECOND_IN_HOUR)

/*��ظ����ٷֱȶ���*/
#if (MBB_CHG_COULOMETER == FEATURE_ON || MBB_CHG_BQ27510 == FEATURE_ON)
#define    BATT_CAPACITY_SHUTOFF     (2)
#define    BATT_CAPACITY_LEVELLOW    (10)
#define    BATT_CAPACITY_LEVEL1      (11)
#define    BATT_CAPACITY_LEVEL2      (20)
#define    BATT_CAPACITY_LEVEL3      (30)
#define    BATT_CAPACITY_LEVEL4      (60)
#define    BATT_CAPACITY_WARMRECHG   (87)
#define    BATT_CAPACITY_RECHG       (95)
#define    BATT_CAPACITY_NINTY_NINE  (99)
#define    BATT_CAPACITY_FULL        (100)
#define    BATT_CAPACITY_INTERVAL    (1)
#define    BATT_CAPACITY_LOW_BATT    (3)
#else
#define    BATT_CAPACITY_SHUTOFF     (0)
#define    BATT_CAPACITY_LEVELLOW    (3)
#define    BATT_CAPACITY_LEVEL1      (10)
#define    BATT_CAPACITY_LEVEL2      (30)
#define    BATT_CAPACITY_LEVEL3      (50)
#define    BATT_CAPACITY_LEVEL4      (80)
#define    BATT_CAPACITY_RECHG       (95)
#define    BATT_CAPACITY_FULL        (100)
#endif/*MBB_CHG_COULOMETER == FEATURE_ON || MBB_CHG_BQ27510 == FEATURE_ON*/

#if (FEATURE_ON == MBB_CHG_COULOMETER)
/*����Ӳ���ṩ�ĵ�Ч�������õ�*/
/*
���㷽��:
ƽ����Ч��ֵΪ10.43021185mohm����������10mohm����ƫ����4.3021185%��
����Ч��ֵΪ������ֵ��104.3021185%��ȡ����Ϊ0.958753  ��
��ֵ��Ϊ����У׼����a��
�������c_offset_a��б�ʣ�����1000000��Ϊ��ֵ
*/
#define TEN_MOHM_RESISTANCE_CORRECT    (980392)
/*�����С��Ҫ���ݵ�������ͳ���������*/
#define R_BATT    (34)
#endif

/*=====================begin NV50016 ����Ĭ��ֵ�궨�� =========================*/
/*SHUTOFF ���¹ػ�ʹ�ܿ���*/
#define SHUTOFF_OVER_TEMP_PROTECT_ENABLE_DEFAULT    (1)
/*SHUTOFF ���¹ػ��¶�����*/
#define SHUTOFF_OVER_TEMP_SHUTOFF_THRESHOLD_DEFAULT    (61)/*power off temp +1 */
/*SHUTOFF ���¹ػ������¶����޴�������*/
#define SHUTOFF_OVER_TEMP_SHUTOFF_CHECK_TIMES_DEFAULT    (1)
/*=====================end NV50016 ����Ĭ��ֵ�궨�� ==========================*/

/*=====================begin NV52005 ����Ĭ��ֵ�궨�� =========================*/
/*SHUTOFF ���¹ػ�ʹ�ܿ���*/
#define SHUTOFF_LOW_TEMP_PROTECT_ENABLE_DEFAULT    (1)
/*SHUTOFF ���¹ػ��¶�����*/
#define SHUTOFF_LOW_TEMP_SHUTOFF_THRESHOLD_DEFAULT    (-20)
/*SHUTOFF ���¹ػ������¶����޴�������*/
#define SHUTOFF_LOW_TEMP_SHUTOFF_CHECK_TIMES_DEFAULT     (1)
/*=====================end  NV52005 ����Ĭ��ֵ�궨�� ==========================*/


/*=====================begin NV50385 ����Ĭ��ֵ�궨��==========================*/
#if (MBB_CHG_WARM_CHARGE == FEATURE_ON)
/*CHG ����±���ʹ�ܿ���*/
#define CHG_TEMP_PROTECT_ENABLE_DEFAULT    (1)
/*CHG �����±���ͣ���¶�����*/
#define CHG_OVER_TEMP_STOP_THRESHOLD_DEFAULT    (55)
/*������³�����¶�����*/
#define CHG_WARM_CHARGE_ENTER_THRESHOLD_DEFAULT    (45)
/*CHG �����±���ͣ���¶�����*/
#define CHG_LOW_TEMP_STOP_THRESHOLD_DEFAULT    (0)
/*CHG �����»ָ��¶�����*/
#define CHG_OVER_TEMP_RESUME_THRESHOLD_DEFAULT    (52)
/*CHG �����»ָ��¶�����*/
#define CHG_LOW_TEMP_RESUME_THRESHOLD_DEFAULT    (3)
/*CHG ���ͣ����ѯ����*/
#define CHG_TEMP_PROTECT_CHECK_TIMES_DEFAULT    (1)
/*CHG ��縴����ѯ����*/
#define CHG_TEMP_RESUME_CHECK_TIMES_DEFAULT    (1)
/*�ɸ��³��ָ������³�����¶�����*/
#define CHG_WARM_CHARGE_EXIT_THRESHOLD_DEFAULT    (42)
#define CHG_TEMP_PROTECT_RESERVED2_DEFAULT    (0)
#else
/*CHG ����±���ʹ�ܿ���*/
#define CHG_TEMP_PROTECT_ENABLE_DEFAULT    (1)
/*CHG �����±���ͣ���¶�����*/
#define CHG_OVER_TEMP_STOP_THRESHOLD_DEFAULT    (45)
/*������³�����¶�����*/
#define CHG_WARM_CHARGE_ENTER_THRESHOLD_DEFAULT    (38)
/*CHG �����±���ͣ���¶�����*/
#define CHG_LOW_TEMP_STOP_THRESHOLD_DEFAULT    (0)
/*CHG �����»ָ��¶�����*/
#define CHG_OVER_TEMP_RESUME_THRESHOLD_DEFAULT    (42)
/*CHG �����»ָ��¶�����*/
#define CHG_LOW_TEMP_RESUME_THRESHOLD_DEFAULT    (3)
/*CHG ���ͣ����ѯ����*/
#define CHG_TEMP_PROTECT_CHECK_TIMES_DEFAULT    (1)
/*CHG ��縴����ѯ����*/
#define CHG_TEMP_RESUME_CHECK_TIMES_DEFAULT    (1)
/*�ɸ��³��ָ������³�����¶�����*/
#define CHG_WARM_CHARGE_EXIT_THRESHOLD_DEFAULT    (0)
#define CHG_TEMP_PROTECT_RESERVED2_DEFAULT    (0)
#endif
/*======================end NV50385 ����Ĭ��ֵ�궨��==========================*/

/*======================begin NV50386 ���ݺ궨��================================*/
/*������ѹ����*/
#define BATT_VOLT_POWER_ON_THR_DEFAULT    (3300)
/*�ػ���ѹ����*/
#define BATT_VOLT_POWER_OFF_THR_DEFAULT    (3300)
/*����ѹ��������(ƽ��ֵ)*/
#define BATT_CHG_OVER_VOLT_PROTECT_THR_DEFAULT    (4220)
/*����ѹ��������(���βɼ�ֵ)*/
#define BATT_CHG_OVER_VOLT_PROTECT_ONE_THR_DEFAULT    (4240)
/*��ʱͣ���ж�����*/
#define BATT_CHG_TEMP_MAINT_THR_DEFAULT    (4100)
/*���³����θ�������*/
#if (FEATURE_ON == MBB_CHG_BATT_EXPAND_PROTECT)
#define BATT_HIGH_TEMP_RECHARGE_THR_DEFAULT    (4000)
#else
#define BATT_HIGH_TEMP_RECHARGE_THR_DEFAULT    (4050)
#endif
/*�͵���������*/
#define BATT_VOLT_LEVELLOW_MAX_DEFAULT    (3550)
/*0���ѹ��������*/
#define BATT_VOLT_LEVEL0_MAX_DEFAULT    (3550)
/*1���ѹ��������*/
#define BATT_VOLT_LEVEL1_MAX_DEFAULT    (3610)
/*2���ѹ��������*/
#define BATT_VOLT_LEVEL2_MAX_DEFAULT    (3670)
/*3���ѹ�������� */
#define BATT_VOLT_LEVEL3_MAX_DEFAULT    (3770)
/*�жϲ��������Ƿ��������*/
#define BATT_INSERT_CHARGE_THR_DEFAULT    (4150)
/*���³��ĸ�������*/
#define BATT_NORMAL_TEMP_RECHARGE_THR_DEFAULT    (4100)
/*=======================end NV50386 ���ݺ궨��================================*/

/*������ͱ�����������*/
/*����˵��*/
/*
    ����Ӧ�ó�����ֻҪ�������λ�ҵ���¶ȳ���45��ʱ�Ͳ��õ�ص�ѹ����4.1V��
    1���������λand����¶�>=45��and��ص�ѹ>=4.1V ��������ͬʱ���㣬��USB����Suspendģʽ��
    2����ص�ѹ<4.05V����Ҫ���ͻ����䣩���ߵ���¶�<42�㣬����������һ��������USB�˳�Suspendģʽ��
    3�����¸�������ͳһ��4.05V�޸���4.0V��Ϊ�˱�����ָ�ȡ�����оƬ��������100mA����ָ������⣩
*/
#define CHG_BATTERY_PROTECT_TEMP            (45)  /*��ر����¶�����*/
#define CHG_BATTERY_PROTECT_RESUME_TEMP     (42)  /*�˳���ر����¶�����*/
#define CHG_BATTERY_PROTECT_VOLTAGE         (4100)  /*�¶ȳ�����ر�������ʱ����ص�ص�ѹ����*/
#define CHG_BATTERY_PROTECT_RESUME_VOLTAGE  (4050)  /*�˳���ر�����ѹ����*/
#define CHG_BATTERY_PROTECT_CV_VOLTAGE      (4100)  /*��ر�����ֹ��ѹ����*/

/*USB�±�����ֵ,Ӳ��������ɺ��ṩ*/ 
#if ( FEATURE_ON == MBB_CHG_USB_TEMPPT_ILIMIT )
#define CHG_USB_TEMP_LIMIT      (120)     /*USB�±����ƴ�������*/
#define CHG_USB_TEMP_RESUME     (100)    /*USB�¶Ȼ��ƻָ�����*/
#endif

/*----------------------------------------------*
 * ȫ�ֱ���                                     *
*----------------------------------------------*/
/*����¶ȵ�ѹ��(V/T��)*/
#if (MBB_CHG_PLATFORM_BALONG == FEATURE_ON)
static const  CHG_TEMP2ADC_TYPE  g_adc_batt_therm_map[] =
#elif (MBB_CHG_PLATFORM_QUALCOMM == FEATURE_ON)
static const  CHG_TEMP_ADC_TYPE  g_adc_batt_therm_map[] =
#endif
{
    /*�ñ���Ӳ���ṩ*/
    {-30,      1654},   /*vol to temp*/
    {-29,      1647},   /*vol to temp*/
    {-28,      1639},   /*vol to temp*/
    {-27,      1632},   /*vol to temp*/
    {-26,      1624},   /*vol to temp*/
    {-25,      1615},   /*vol to temp*/
    {-24,      1607},   /*vol to temp*/
    {-23,      1598},   /*vol to temp*/
    {-22,      1589},   /*vol to temp*/
    {-21,      1580},   /*vol to temp*/
    {-20,      1570},   /*vol to temp*/
    {-19,      1560},   /*vol to temp*/
    {-18,      1550},   /*vol to temp*/
    {-17,      1539},   /*vol to temp*/
    {-16,      1528},   /*vol to temp*/
    {-15,      1517},   /*vol to temp*/
    {-14,      1506},   /*vol to temp*/
    {-13,      1494},   /*vol to temp*/
    {-12,      1482},   /*vol to temp*/
    {-11,      1470},   /*vol to temp*/
    {-10,      1457},    /*vol to temp*/
    {-9,       1444},    /*vol to temp*/
    {-8,       1431},    /*vol to temp*/
    {-7,       1418},    /*vol to temp*/
    {-6,       1404},    /*vol to temp*/
    {-5,       1390},    /*vol to temp*/
    {-4,       1376},    /*vol to temp*/
    {-3,       1361},    /*vol to temp*/
    {-2,       1346},    /*vol to temp*/
    {-1,       1332},    /*vol to temp*/
    {0,        1316},    /*vol to temp*/
    {1,        1301},    /*vol to temp*/
    {2,        1286},    /*vol to temp*/
    {3,        1270},    /*vol to temp*/
    {4,        1254},    /*vol to temp*/
    {5,        1238},    /*vol to temp*/
    {6,        1222},    /*vol to temp*/
    {7,        1205},    /*vol to temp*/
    {8,        1189},    /*vol to temp*/
    {9,        1172},    /*vol to temp*/
    {10,       1155},    /*vol to temp*/
    {11,       1139},    /*vol to temp*/
    {12,       1122},    /*vol to temp*/
    {13,       1105},    /*vol to temp*/
    {14,       1088},    /*vol to temp*/
    {15,       1070},    /*vol to temp*/
    {16,       1053},    /*vol to temp*/
    {17,       1036},    /*vol to temp*/
    {18,       1019},    /*vol to temp*/
    {19,       1002},    /*vol to temp*/
    {20,       985},    /*vol to temp*/
    {21,       968},    /*vol to temp*/
    {22,       951},    /*vol to temp*/
    {23,       934},    /*vol to temp*/
    {24,       917},    /*vol to temp*/
    {25,       900},    /*vol to temp*/
    {26,       883},    /*vol to temp*/
    {27,       867},    /*vol to temp*/
    {28,       850},    /*vol to temp*/
    {29,       817},    /*vol to temp*/
    {30,       801},    /*vol to temp*/
    {31,       785},    /*vol to temp*/
    {32,       769},    /*vol to temp*/
    {33,       753},    /*vol to temp*/
    {34,       738},    /*vol to temp*/
    {35,       723},    /*vol to temp*/
    {36,       707},    /*vol to temp*/
    {37,       693},    /*vol to temp*/
    {38,       663},    /*vol to temp*/
    {39,       649},    /*vol to temp*/
    {40,       635},    /*vol to temp*/
    {41,       621},    /*vol to temp*/
    {42,       607},    /*vol to temp*/
    {43,       593},    /*vol to temp*/
    {44,       580},    /*vol to temp*/
    {45,       567},    /*vol to temp*/
    {46,       554},    /*vol to temp*/
    {47,       541},    /*vol to temp*/
    {48,       529},    /*vol to temp*/
    {49,       517},    /*vol to temp*/
    {50,       505},    /*vol to temp*/
    {51,       493},    /*vol to temp*/
    {52,       481},    /*vol to temp*/
    {53,       470},    /*vol to temp*/
    {54,       459},    /*vol to temp*/
    {55,       454},    /*vol to temp*/
    {56,       448},    /*vol to temp*/
    {57,       438},    /*vol to temp*/
    {58,       427},    /*vol to temp*/
    {59,       417},    /*vol to temp*/
    {60,       407},     /*vol to temp*/
    {61,       397},     /*vol to temp*/
    {62,       388},     /*vol to temp*/
    {63,       379},     /*vol to temp*/
    {64,       370},     /*vol to temp*/
    {65,       361},     /*vol to temp*/
    {66,       353},     /*vol to temp*/
    {67,       344},     /*vol to temp*/
    {68,       336},     /*vol to temp*/
    {69,       328},     /*vol to temp*/
    {70,       320},     /*vol to temp*/
    {71,       312},     /*vol to temp*/
    {72,       305},     /*vol to temp*/
    {73,       298},     /*vol to temp*/
    {74,       291},     /*vol to temp*/
    {75,       284},     /*vol to temp*/
    {76,       277},     /*vol to temp*/
    {77,       270},     /*vol to temp*/
    {78,       264},     /*vol to temp*/
    {79,       257},     /*vol to temp*/
    {80,       251},     /*vol to temp*/
    {81,       245},     /*vol to temp*/
    {82,       240},     /*vol to temp*/
    {83,       234},     /*vol to temp*/
    {84,       228},     /*vol to temp*/
    {85,       223},     /*vol to temp*/
};

/*USB�±���USBV-T,Ӳ���ṩ*/ 
#if ( FEATURE_ON == MBB_CHG_USB_TEMPPT_ILIMIT )
#if (MBB_CHG_PLATFORM_BALONG == FEATURE_ON)
static const  CHG_TEMP2ADC_TYPE  g_adc_usb_therm_map[] =
#elif (MBB_CHG_PLATFORM_QUALCOMM == FEATURE_ON)
static const  CHG_TEMP_ADC_TYPE  g_adc_usb_therm_map[] =
#endif
{
    {-10,      1665},   /*vol to temp*/
    {0,        1575},   /*vol to temp*/
    {5,        1516},   /*vol to temp*/
    {10,       1448},    /*vol to temp*/
    {15,       1370},    /*vol to temp*/
    {20,       1285},    /*vol to temp*/
    {25,       1192},    /*vol to temp*/
    {30,       1095},    /*vol to temp*/
    {35,       996},    /*vol to temp*/
    {40,       897},    /*vol to temp*/
    {45,       801},    /*vol to temp*/
    {50,       710},    /*vol to temp*/
    {55,       624},    /*vol to temp*/
    {60,       546},    /*vol to temp*/
    {65,       476},    /*vol to temp*/
    {66,       463},    /*vol to temp*/
    {67,       450},    /*vol to temp*/
    {68,       437},    /*vol to temp*/
    {69,       425},    /*vol to temp*/
    {70,       413},    /*vol to temp*/
    {71,       401},    /*vol to temp*/
    {72,       390},    /*vol to temp*/
    {73,       379},    /*vol to temp*/
    {74,       368},    /*vol to temp*/
    {75,       358},    /*vol to temp*/
    {76,       347},    /*vol to temp*/
    {77,       337},    /*vol to temp*/
    {78,       328},    /*vol to temp*/
    {79,       318},    /*vol to temp*/
    {80,       309},     /*vol to temp*/
    {81,       300},     /*vol to temp*/
    {82,       292},     /*vol to temp*/
    {83,       283},     /*vol to temp*/
    {84,       275},     /*vol to temp*/
    {85,       267},     /*vol to temp*/
    {86,       259},     /*vol to temp*/
    {87,       252},     /*vol to temp*/
    {88,       244},     /*vol to temp*/
    {89,       237},     /*vol to temp*/
    {90,       231},     /*vol to temp*/
    {91,       224},     /*vol to temp*/
    {92,       217},     /*vol to temp*/
    {93,       211},     /*vol to temp*/
    {94,       205},     /*vol to temp*/
    {95,       199},     /*vol to temp*/
    {96,       193},     /*vol to temp*/
    {97,       188},     /*vol to temp*/
    {98,       182},     /*vol to temp*/
    {99,       177},     /*vol to temp*/
    {100,      172},     /*vol to temp*/
    {101,      167},     /*vol to temp*/
    {102,      162},     /*vol to temp*/
    {103,      158},     /*vol to temp*/
    {104,      153},     /*vol to temp*/
    {105,      149},     /*vol to temp*/
    {106,      145},     /*vol to temp*/
    {107,      141},     /*vol to temp*/
    {108,      137},     /*vol to temp*/
    {109,      133},     /*vol to temp*/
    {110,      129},     /*vol to temp*/
    {111,      125},     /*vol to temp*/
    {112,      122},     /*vol to temp*/
    {113,      118},     /*vol to temp*/
    {114,      115},     /*vol to temp*/
    {115,      112},     /*vol to temp*/
    {116,      109},     /*vol to temp*/
    {117,      106},     /*vol to temp*/
    {118,      103},     /*vol to temp*/
    {119,      100},     /*vol to temp*/
    {120,      98},     /*vol to temp*/
    {121,      95},     /*vol to temp*/
    {122,      92},     /*vol to temp*/
    {123,      90},     /*vol to temp*/
    {124,      87},     /*vol to temp*/
    {125,      85},     /*vol to temp*/
};
#endif

/*�¶Ȳ������������ڳ���λ�¶Ȳ�����*/
static const extchg_batt_temp_revise g_batt_tem_revise_map_only_chg[] =
{
    {-30,  1,   0},
    {  1,  50,  3},
    { 50,  85,  2},
};

/*�¶Ȳ����������������λ�����ʱ�¶Ȳ�����*/
static const extchg_batt_temp_revise g_batt_tem_revise_map_only_extchg_high_crt[] =
{
    {-30,  36, -1},
    { 36,  41, -4},
    { 41,  46, -7},
    { 46,  51, -9},
    { 51,  56, -10},
    { 56,  61, -11},
    { 61,  66, -13},
    { 66,  71, -14},
    { 71,  75, -16},
    { 75,  80, -18},
    { 80,  82, -20},
    { 82,  83, -21},
    { 83,  85, -22},
};

/*�¶Ȳ����������������λС����ʱ�¶Ȳ�����*/
static const extchg_batt_temp_revise g_batt_tem_revise_map_only_extchg_low_crt[] =
{
    {-30,   1, -2},
    {  1,  36,  1},
    { 36,  41, -1},
    { 41,  46, -2},
    { 46,  51, -3},
    { 51,  56, -4},
    { 56,  61, -5},
    { 61,  62, -4},
    { 62,  66, -3},
    { 66,  71, -6},
    { 71,  85, -9},
};

/*�¶Ȳ��������������ͬʱ��λ�¶Ȳ�����*/
static const extchg_batt_temp_revise g_batt_tem_revise_map_chg_extchg[] =
{
    {-30,  41,  2},
    { 41,  56,  1},
    { 56,  61, -2},
    { 61,  62, -3},
    { 62,  85, -4},
};

/*�¶Ȳ����������������λ�¶Ȳ�����*/
static const extchg_batt_temp_revise g_batt_tem_revise_map_only_batt[] =
{
    {-30,  54,  0},
    { 54,  85, -1},
};

/*����Ϊ��ͬ����صĲ���*/
/*��ͨ���*/
static struct chg_batt_data chg_batt_data_default = {
        .is_20pct_calib   = FALSE,
        .batt_id                = CHG_BATT_ID_DEF,
        .chg_batt_volt_paras    = {3450,3450,4220,4240,4100,4000,3550,3550,3610,3670,3770,4150,4100},/*batt data*/
};

#if (MBB_CHG_PLATFORM_BALONG == FEATURE_ON)
/*
static struct chg_batt_data chg_batt_data_feimaotui_1500mah = {
        .id_voltage_min = 887,  //batt id volt
        .id_voltage_max = 1137, //batt id volt
        .is_20pct_calib   = FALSE,
        .batt_id                = CHG_BATT_ID_FEIMAOTUI_1500MAH,
        .chg_batt_volt_paras    = {3450,3450,4370,4390,4150,4030,3620,3620,3708,3779,4030,4250,4200},//batt data


};
*/
/*��ë��2300mah��ѹ���*/
/*
static struct chg_batt_data chg_batt_data_feimaotui_2300mah = {
        .id_voltage_min = 852, //batt id volt
        .id_voltage_max = 1032,//batt id volt
        .is_20pct_calib   = FALSE,
        .batt_id                    = CHG_BATT_ID_FEIMAOTUI_2300MAH,
        .chg_batt_volt_paras        = {3450,3450,4370,4390,4250,4050,3600,3600,3690,3803,3948,4280,4200},//batt data
};
*/

/*��ë��3000mah��ѹ���*/
static struct chg_batt_data chg_batt_data_feimaotui_3000mah = {
        .id_voltage_min = 1110, //batt id volt
        .id_voltage_max = 1300,//batt id volt
        .is_20pct_calib   = FALSE,
        .batt_id                    = CHG_BATT_ID_FEIMAOTUI_3000MAH,
        .chg_batt_volt_paras        = {3450,3450,4370,4390,4250,4000,3630,3630,3720,3800,4030,4280,4200},/*batt data*/
};

/*������3000mah��ѹ���*/
static struct chg_batt_data chg_batt_data_xinwangda_3000mah = {
        .id_voltage_min = 840, //batt id volt
        .id_voltage_max = 1030,//batt id volt
        .is_20pct_calib   = FALSE,
        .batt_id                    = CHG_BATT_ID_XINWANGDA_3000MAH,
        .chg_batt_volt_paras        = {3450,3450,4370,4390,4250,4000,3630,3630,3720,3800,4030,4280,4200},/*batt data*/
};
/*��ë��4800mah��ѹ���*/
static struct chg_batt_data chg_batt_data_feimaotui_4800mah = {
        .id_voltage_min = 220, //batt id volt
        .id_voltage_max = 420,//batt id volt
        .is_20pct_calib   = FALSE,
        .batt_id                    = CHG_BATT_ID_FEIMAOTUI_4800MAH,
        .chg_batt_volt_paras        = {3300,3300,4370,4390,4250,4000,3604,3450,3716,3824,4022,4280,4200},/*batt data*/
};

/*������4800mah��ѹ���*/
static struct chg_batt_data chg_batt_data_xinwangda_4800mah = {
        .id_voltage_min = 1530, //batt id volt
        .id_voltage_max = 1730,//batt id volt
        .is_20pct_calib   = FALSE,
        .batt_id                    = CHG_BATT_ID_XINGWANGDA_4800MAH,
        .chg_batt_volt_paras        = {3300,3300,4370,4390,4250,4000,3604,3450,3716,3824,4022,4280,4200},/*batt data*/
};

/*��ë��6400mah���*/
static struct chg_batt_data chg_batt_data_feimaotui_6400mah = {
        .id_voltage_min = 1100, //batt id volt
        .id_voltage_max = 1300,//batt id volt
        .is_20pct_calib   = FALSE,
        .batt_id                    = CHG_BATT_ID_FEIMAOTUI_6400MAH,
        .chg_batt_volt_paras        = {3300,3300,4370,4390,4250,4000,3630,3630,3720,3800,4030,4280,4200},/*batt data*/
};

/*������6400mah���*/
static struct chg_batt_data chg_batt_data_xinwangda_6400mah = {
        .id_voltage_min = 620, //batt id volt
        .id_voltage_max = 830,//batt id volt
        .is_20pct_calib   = FALSE,
        .batt_id                    = CHG_BATT_ID_XINGWANGDA_6400MAH,
        .chg_batt_volt_paras        = {3300,3300,4370,4390,4250,4000,3630,3630,3720,3800,4030,4280,4200},/*batt data*/
};

/*��Ʒ֧�ֵĵ�ع��list*/
static struct chg_batt_data *chg_batt_data_array[] = {
    &chg_batt_data_default,
    &chg_batt_data_xinwangda_6400mah,
    &chg_batt_data_feimaotui_6400mah,
};

#elif (MBB_CHG_PLATFORM_QUALCOMM == FEATURE_ON)
/*��ë��2300mah��ѹ���*/
static struct chg_batt_data chg_batt_data_feimaotui_2300mah = {
        .id_voltage_min = 852, //batt id volt
        .id_voltage_max = 1032,//batt id volt
        .is_20pct_calib   = FALSE,
        .batt_id                    = CHG_BATT_ID_FEIMAOTUI_2300MAH,
        .chg_batt_volt_paras        = {3450,3450,4370,4390,4150,4000,3600,3600,3690,3803,3948,4280,4200},/*batt data*/
};

/*������1900mah��ѹ���*/
static struct chg_batt_data chg_batt_data_xingwangda_1900mah = {
        .id_voltage_min = 697,//batt id volt
        .id_voltage_max = 878,//batt id volt
        .is_20pct_calib   = FALSE,
        .batt_id                    = CHG_BATT_ID_XINGWANGDA_1900MAH,
        .chg_batt_volt_paras        = {3450,3450,4370,4390,4150,4000,3570,3570,3638,3721,3870,4280,4200},/*batt data*/
};
/*��ë��1900mah��ѹ���*/
static struct chg_batt_data chg_batt_data_feimaotui_1900mah = {
        .id_voltage_min = 187,//batt id volt
        .id_voltage_max = 367,//batt id volt
        .is_20pct_calib   = FALSE,
        .batt_id                    = CHG_BATT_ID_FEIMAOTUI_1900MAH,
        .chg_batt_volt_paras        = {3450,3450,4370,4390,4150,4000,3563,3563,3620,3708,3839,4280,4200},/*batt data*/
};

/*��Ʒ֧�ֵĵ�ع��list*/
static struct chg_batt_data *chg_batt_data_array[] = {
    &chg_batt_data_default,
    &chg_batt_data_feimaotui_2300mah,
    &chg_batt_data_xingwangda_1900mah,
    &chg_batt_data_feimaotui_1900mah,
};
#endif/*MBB_CHG_PLATFORM_QUALCOMM == FEATURE_ON*/

/*�����ǳ��оƬ���ò���������Ӳ���ṩ�Ĳ�������*/
#if (MBB_CHG_HIGH_VOLT_BATT == FEATURE_ON)
/*����0--10�ȳ�����*/
static const charger_current_limit_st charger_current_limit_paras[CHG_CHGR_INVALID] =
{
    {
        CHG_WALL_CHGR,       /*���*/
        896,                 /*mA,Charge Current limit*/
        2000,                /*mA,USB Current limit*/
    },
    {
        CHG_USB_HOST_PC,    /*USB2.0/USB3.0*/
        576,                /*mA,Charge Current limit*/
        500,                /*mA,USB Current limit*/
    },
    {
        CHG_WIRELESS_CHGR,    /*�Ǳ�*/
        576,                /*mA,Charge Current limit*/
        500,                /*mA,USB Current limit*/
    },
    {
        CHG_HVDCP_CHGR,     /*��ѹ�����*/
        576,
        2000,                /*mA,USB Current limit*/
    },
};
#endif/*MBB_CHG_HIGH_VOLT_BATT == FEATURE_ON*/

static const chg_hw_param_t chg_std_1A_chgr_hw_paras_def[CHG_STM_MAX_ST] =
{
    /*0:-Transition State*/
    {
        1200,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4208,  /*mV, CV Voltage setting.*/
         128,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
    /*1:-Fast Charge State*/
    {
        1200,  /*mA, Power Supply front-end Current limit.*/
        1024,  /*mA, Charge Current limit.*/
        4208,  /*mV, CV Voltage setting.*/
         128,  /*mA, Taper(Terminate) current.*/
        TRUE   /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
    /*2:-Maintenance State*/
    {
         900,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4208,  /*mV, CV Voltage setting.*/
         128,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
    /*3:-Invalid Charge Temperature State*/
    {
         900,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4208,  /*mV, CV Voltage setting.*/
         128,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },

    /*4:-Battery Only State*/
    {
        900,  /*mA, Power Supply front-end Current limit.*/
        576,  /*mA, Charge Current limit.*/
       4208,  /*mV, CV Voltage setting.*/
        128,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },

#if (MBB_CHG_WARM_CHARGE == FEATURE_ON)
    /*5:-Warm charge State*/
    {
         900,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4100,  /*mV, CV Voltage setting.*/
         128,  /*mA, Taper(Terminate) current.*/
        TRUE   /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
#endif /* MBB_CHG_WARM_CHARGE == FEATURE_ON */
    /*6:-hvdcp charge State*/
    {
            0,  /*mA, Power Supply front-end Current limit.*/
            0,  /*mA, Charge Current limit.*/
            0,  /*mV, CV Voltage setting.*/
            0,  /*mA, Taper(Terminate) current.*/
        FALSE   /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
};
static const chg_hw_param_t chg_std_2A_chgr_hw_paras_def[CHG_STM_MAX_ST] =
{
    /*0:-Transition State*/
    {
         900,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4208,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
    /*1:-Fast Charge State*/
    {
        2000,  /*mA, Power Supply front-end Current limit.*/
        2048,  /*mA, Charge Current limit.*/
        4208,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
        TRUE   /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
    /*2:-Maintenance State*/
    {
        2000,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4208,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
    /*3:-Invalid Charge Temperature State*/
    {
        1500,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4208,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },

    /*4:-Battery Only State*/
    {
        900,  /*mA, Power Supply front-end Current limit.*/
        576,  /*mA, Charge Current limit.*/
       4208,  /*mV, CV Voltage setting.*/
        256,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },

#if (MBB_CHG_WARM_CHARGE == FEATURE_ON)
    /*5:-Warm charge State*/
    {
        2000,  /*mA, Power Supply front-end Current limit.*/
         896,  /*mA, Charge Current limit.*/
        4100,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
        TRUE   /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
#endif /* MBB_CHG_WARM_CHARGE == FEATURE_ON */
    /*6:-hvdcp charge State*/
    {
            0,  /*mA, Power Supply front-end Current limit.*/
            0,  /*mA, Charge Current limit.*/
            0,  /*mV, CV Voltage setting.*/
            0,  /*mA, Taper(Terminate) current.*/
        FALSE   /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
};

static const chg_hw_param_t chg_std_2A_chgr_hw_paras_4800mAh_batt[CHG_STM_MAX_ST] =
{
    /*0:-Transition State*/
    {
        2000,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4352,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
    /*1:-Fast Charge State*/
    {
        2000,  /*mA, Power Supply front-end Current limit.*/
        2048,  /*mA, Charge Current limit.*/
        4352,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
        TRUE   /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
    /*2:-Maintenance State*/
    {
        2000,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4352,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
    /*3:-Invalid Charge Temperature State*/
    {
         900,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4352,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },

    /*4:-Battery Only State*/
    {
        900,  /*mA, Power Supply front-end Current limit.*/
        576,  /*mA, Charge Current limit.*/
       4352,  /*mV, CV Voltage setting.*/
        256,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },

#if (MBB_CHG_WARM_CHARGE == FEATURE_ON)
    /*5:-Warm charge State*/
    {
         900,  /*mA, Power Supply front-end Current limit.*/
        896,  /*mA, Charge Current limit.*/
        4100,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
        TRUE   /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
#endif /* MBB_CHG_WARM_CHARGE == FEATURE_ON */
    /*6:-hvdcp charge State*/
    {
            0,  /*mA, Power Supply front-end Current limit.*/
            0,  /*mA, Charge Current limit.*/
            0,  /*mV, CV Voltage setting.*/
            0,  /*mA, Taper(Terminate) current.*/
        FALSE   /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
};

static const chg_hw_param_t chg_usb_chgr_hw_paras_def[CHG_STM_MAX_ST] =
{
    /*0:-Transition State*/
    {
         500,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4208,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
    /*1:-Fast Charge State*/
    {
         500,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4208,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
         TRUE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
    /*2:-Maintenance State*/
    {
         500,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4208,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
    /*3:-Invalid Charge Temperature State*/
    {
         500,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4208,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },

    /*4:-Battery Only State*/
    {
         500,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4208,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },

#if (MBB_CHG_WARM_CHARGE == FEATURE_ON)
    /*5:-Warm charge State*/
    {
         500,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4100,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
        TRUE   /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
#endif /* MBB_CHG_WARM_CHARGE == FEATURE_ON */
    /*6:-hvdcp charge State*/
    {
            0,  /*mA, Power Supply front-end Current limit.*/
            0,  /*mA, Charge Current limit.*/
            0,  /*mV, CV Voltage setting.*/
            0,  /*mA, Taper(Terminate) current.*/
        FALSE   /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
};

static const chg_hw_param_t chg_usb_chgr_hw_paras_4800mAh_batt[CHG_STM_MAX_ST] =
{
    /*0:-Transition State*/
    {
         500,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4352,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
    /*1:-Fast Charge State*/
    {
         500,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4352,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
         TRUE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
    /*2:-Maintenance State*/
    {
         500,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4352,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
    /*3:-Invalid Charge Temperature State*/
    {
         500,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4352,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },

    /*4:-Battery Only State*/
    {
         500,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4352,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },

#if (MBB_CHG_WARM_CHARGE == FEATURE_ON)
    /*5:-Warm charge State*/
    {
         500,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4150,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
        TRUE   /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
#endif /* MBB_CHG_WARM_CHARGE == FEATURE_ON */
    /*6:-hvdcp charge State*/
    {
            0,  /*mA, Power Supply front-end Current limit.*/
            0,  /*mA, Charge Current limit.*/
            0,  /*mV, CV Voltage setting.*/
            0,  /*mA, Taper(Terminate) current.*/
        FALSE   /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
};

/*usb3.0��������*/
static const chg_hw_param_t chg_usb3_chgr_hw_paras_def[CHG_STM_MAX_ST] =
{
    /*0:-Transition State*/
    {
         900,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4352,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
    /*1:-Fast Charge State*/
    {
         900,  /*mA, Power Supply front-end Current limit.*/
        1024,  /*mA, Charge Current limit.*/
        4352,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
         TRUE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
    /*2:-Maintenance State*/
    {
         900,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4352,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
    /*3:-Invalid Charge Temperature State*/
    {
         900,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4352,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },

    /*4:-Battery Only State*/
    {
         900,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4352,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },

#if (MBB_CHG_WARM_CHARGE == FEATURE_ON)
    /*5:-Warm charge State*/
    {
         900,  /*mA, Power Supply front-end Current limit.*/
         896,  /*mA, Charge Current limit.*/
        4100,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
        TRUE   /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
#endif /* MBB_CHG_WARM_CHARGE == FEATURE_ON */
    /*6:-hvdcp charge State*/
    {
            0,  /*mA, Power Supply front-end Current limit.*/
            0,  /*mA, Charge Current limit.*/
            0,  /*mV, CV Voltage setting.*/
            0,  /*mA, Taper(Terminate) current.*/
        FALSE   /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
};

static const chg_hw_param_t chg_usb3_chgr_hw_paras_4800mAh_batt[CHG_STM_MAX_ST] =
{
    /*0:-Transition State*/
    {
         900,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4352,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
    /*1:-Fast Charge State*/
    {
         900,  /*mA, Power Supply front-end Current limit.*/
        1024,  /*mA, Charge Current limit.*/
        4352,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
         TRUE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
    /*2:-Maintenance State*/
    {
         900,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4352,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
    /*3:-Invalid Charge Temperature State*/
    {
         900,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4352,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },

    /*4:-Battery Only State*/
    {
         900,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4352,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },

#if (MBB_CHG_WARM_CHARGE == FEATURE_ON)
    /*5:-Warm charge State*/
    {
         900,  /*mA, Power Supply front-end Current limit.*/
        1024,  /*mA, Charge Current limit.*/
        4150,  /*mV, CV Voltage setting.*/
         256,  /*mA, Taper(Terminate) current.*/
        TRUE   /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
#endif /* MBB_CHG_WARM_CHARGE == FEATURE_ON */
    /*6:-hvdcp charge State*/
    {
            0,  /*mA, Power Supply front-end Current limit.*/
            0,  /*mA, Charge Current limit.*/
            0,  /*mV, CV Voltage setting.*/
            0,  /*mA, Taper(Terminate) current.*/
        FALSE   /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
};

static const chg_hw_param_t chg_weak_chgr_hw_paras_def[CHG_STM_MAX_ST] =
{
    /*0:-Transition State*/
    {
         500,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4208,  /*mV, CV Voltage setting.*/
         128,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
    /*1:-Fast Charge State*/
    {
         500,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4208,  /*mV, CV Voltage setting.*/
         128,  /*mA, Taper(Terminate) current.*/
         TRUE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
    /*2:-Maintenance State*/
    {
         500,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4208,  /*mV, CV Voltage setting.*/
         128,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
    /*3:-Invalid Charge Temperature State*/
    {
         500,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4208,  /*mV, CV Voltage setting.*/
         128,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },

    /*4:-Battery Only State*/
    {
         500,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4208,  /*mV, CV Voltage setting.*/
         128,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },

#if (MBB_CHG_WARM_CHARGE == FEATURE_ON)
    /*5:-Warm charge State*/
    {
         500,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4100,  /*mV, CV Voltage setting.*/
         128,  /*mA, Taper(Terminate) current.*/
        TRUE   /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
#endif /* MBB_CHG_WARM_CHARGE == FEATURE_ON */
    /*6:-hvdcp charge State*/
    {
            0,  /*mA, Power Supply front-end Current limit.*/
            0,  /*mA, Charge Current limit.*/
            0,  /*mV, CV Voltage setting.*/
            0,  /*mA, Taper(Terminate) current.*/
        FALSE   /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
};

#if (MBB_CHG_WIRELESS == FEATURE_ON)
static const chg_hw_param_t chg_wireless_chgr_hw_paras_def[CHG_STM_MAX_ST] =
{
    /*0:-Transition State*/
    {
         500,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4208,  /*mV, CV Voltage setting.*/
         128,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
    /*1:-Fast Charge State*/
    {
         900,  /*mA, Power Supply front-end Current limit.*/
        1024,  /*mA, Charge Current limit.*/
        4208,  /*mV, CV Voltage setting.*/
         128,  /*mA, Taper(Terminate) current.*/
        TRUE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
    /*2:-Maintenance State*/
    {
         500,  /*mA, Power Supply front-end Current limit.*/
        1024,  /*mA, Charge Current limit.*/
        4208,  /*mV, CV Voltage setting.*/
         128,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
    /*3:-Invalid Charge Temperature State*/
    {
         500,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4208,  /*mV, CV Voltage setting.*/
         128,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },

    /*4:-Battery Only State*/
    {
         500,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4208,  /*mV, CV Voltage setting.*/
         128,  /*mA, Taper(Terminate) current.*/
        FALSE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },

#if (MBB_CHG_WARM_CHARGE == FEATURE_ON)
    /*5:-Warm charge State*/
    {
         500,  /*mA, Power Supply front-end Current limit.*/
         576,  /*mA, Charge Current limit.*/
        4100,  /*mV, CV Voltage setting.*/
         128,  /*mA, Taper(Terminate) current.*/
        TRUE   /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
#endif /* MBB_CHG_WARM_CHARGE == FEATURE_ON */
    /*6:-hvdcp charge State*/
    {
            0,  /*mA, Power Supply front-end Current limit.*/
            0,  /*mA, Charge Current limit.*/
            0,  /*mV, CV Voltage setting.*/
            0,  /*mA, Taper(Terminate) current.*/
        FALSE   /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
    },
};

/*�ػ����߳�����������*/
#if (MBB_CHG_WARM_CHARGE == FEATURE_ON)
static const chg_hw_param_t chg_wireless_poweroff_warmchg_paras =
{
     900,  /*mA, Power Supply front-end Current limit.*/
    1024,  /*mA, Charge Current limit.*/
    4100,  /*mV, CV Voltage setting.*/
     128,  /*mA, Taper(Terminate) current.*/
    TRUE  /*If charge enabled: FALSE:-Disable, TRUE:-Enable.*/
};
#endif /* MBB_CHG_WARM_CHARGE == FEATURE_ON */

#endif /* MBB_CHG_WIRELESS == FEATURE_ON */
/*=============�����ǳ����ز���������ʱ��Ҫ����Ӳ�������޸�=======*/

#endif/*#define _CHG_CHARGE_CUST_H*/

