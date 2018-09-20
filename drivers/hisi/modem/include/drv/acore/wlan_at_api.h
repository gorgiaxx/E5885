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
 
 
#ifndef ATWLANAPI_H
#define ATWLANAPI_H

#ifdef __cplusplus
    #if __cplusplus
    extern "C" {
    #endif
#endif

#ifndef _UINT32_DEFINED
typedef  unsigned long int  uint32;      /* Unsigned 32 bit value */
#define _UINT32_DEFINED
#endif

#ifndef _UINT16_DEFINED
typedef  unsigned short     uint16;      /* Unsigned 16 bit value */
#define _UINT16_DEFINED
#endif

#ifndef _UINT8_DEFINED
typedef  unsigned char      uint8;       /* Unsigned 8  bit value */
#define _UINT8_DEFINED
#endif

#ifndef _INT32_DEFINED
typedef  signed long int    int32;       /* Signed 32 bit value */
#define _INT32_DEFINED
#endif

#ifndef _INT16_DEFINED
typedef  signed short       int16;       /* Signed 16 bit value */
#define _INT16_DEFINED
#endif

#ifndef _INT8_DEFINED
typedef  char        int8;        /* Signed 8  bit value */
#define _INT8_DEFINED
#endif

/* ��������ֵ�б� */
typedef enum
{
    AT_RETURN_FAILURE   =  -1,  /*����ʧ��*/
    AT_RETURN_SUCCESS   =   0   /*�ɹ�*/
}WLAN_AT_RETURN_TYPE;

/* ���Կ��ص�ö�� */
typedef enum
{
    AT_FEATURE_DISABLE = 0,
    AT_FEATURE_ENABLE  = 1
}WLAN_AT_FEATURE_TYPE;

/* Buffer����ṹ */
#define WLAN_AT_BUFFER_SIZE 512
typedef struct _WLAN_AT_BUFFER
{
    int32 reserve;                /* �����ֶ� */
    int8 buf[WLAN_AT_BUFFER_SIZE];/* Buffer���棬����ַ������������'\0' */
}WLAN_AT_BUFFER_STRU;

/*���Ա��Ϣ*/
typedef struct _GROUPMEMBER_STRU
{
    int32 group;                             /*groupֵ��ʾ�����indexֵ,ȡֵ��Χ 0~255*/
    uint8 content[WLAN_AT_BUFFER_SIZE * 2];  /*��Ӧ�����Ա������*/
}WLAN_AT_GROUPMEMBER_STRU;

/*===========================================================================
 (1)^WIENABLE ����WiFiģ��ʹ��
===========================================================================*/
typedef enum
{
    AT_WIENABLE_OFF  = 0,   /*�ر� */
    AT_WIENABLE_ON   = 1,   /*������ģʽ ����ģʽ*/
    AT_WIENABLE_TEST = 2    /*�򿪲���ģʽ����WT��MT����ʱʹ�ø�ģʽ���в��ԣ�������-ϵͳ����ʱ��Ĭ������*/
}WLAN_AT_WIENABLE_TYPE;
int32 WlanATSetWifiEnable(WLAN_AT_WIENABLE_TYPE onoff);
WLAN_AT_WIENABLE_TYPE WlanATGetWifiEnable(void);

/*===========================================================================
 (2)^WIMODE ����WiFiģʽ���� Ŀǰ��Ϊ��ģʽ����
===========================================================================*/
typedef enum
{
    AT_WIMODE_CW      = 0,  /*CWģʽ      */
    AT_WIMODE_80211a  = 1,  /*802.11a��ʽ */
    AT_WIMODE_80211b  = 2,  /*802.11b��ʽ */
    AT_WIMODE_80211g  = 3,  /*802.11g��ʽ */
    AT_WIMODE_80211n  = 4,  /*802.11n��ʽ */
    AT_WIMODE_80211ac = 5,  /*802.11ac��ʽ*/
    AT_WIMODE_MAX
}WLAN_AT_WIMODE_TYPE;
int32 WlanATSetWifiMode(WLAN_AT_WIMODE_TYPE mode);
/* ��ǰģʽ�����ַ�����ʽ����eg: 2 */
int32 WlanATGetWifiMode(WLAN_AT_BUFFER_STRU *strBuf);
/* ֧�ֵ�����ģʽ�����ַ�����ʽ����eg: 2,3,4 */
int32 WlanATGetWifiModeSupport(WLAN_AT_BUFFER_STRU *strBuf);

/*===========================================================================
 (3)^WIBAND ���á���ȡWiFi�������
===========================================================================*/
typedef enum
{
    AT_WIBAND_20M   = 0,  /*���� 20MHz*/
    AT_WIBAND_40M   = 1,  /*���� 40MHz*/
    AT_WIBAND_80M   = 2,  /*���� 80MHz*/
    AT_WIBAND_160M  = 3,  /*����160MHz*/
}WLAN_AT_WIBAND_TYPE;

int32 WlanATSetWifiBand(WLAN_AT_WIBAND_TYPE width);
/* ��ǰ�������ַ�����ʽ����eg: 0 */
int32 WlanATGetWifiBand(WLAN_AT_BUFFER_STRU *strBuf);
/* ֧�ִ������ַ�����ʽ����eg: 0,1 */
int32 WlanATGetWifiBandSupport(WLAN_AT_BUFFER_STRU *strBuf);

/*===========================================================================
 (4)^WIFREQ ����WiFiƵ��
===========================================================================*/
typedef enum
{
    AT_WIFREQ_24G   = 0,  /*Ƶ�� 2.4G*/
    AT_WIFREQ_50G   = 1,  /*Ƶ�� 5G*/
}WLAN_AT_WIFREQ_TYPE;
typedef struct _WIFREQ_STRU
{
    uint16 value;   /*WiFiƵ�㣬��λΪMHz��ȡֵ��ΧΪ 0��65535*/
    int16 offset;  /*����Ƶƫ��Ϣ����λΪKHz��ȡֵ��ΧΪ-32768��32767��Ĭ�ϴ�0*/
}WLAN_AT_WIFREQ_STRU;
int32 WlanATSetWifiFreq(WLAN_AT_WIFREQ_STRU *pFreq);
int32 WlanATGetWifiFreq(WLAN_AT_WIFREQ_STRU *pFreq);

/*===========================================================================
 (5)^WIDATARATE ���úͲ�ѯ��ǰWiFi���ʼ�����
  WiFi���ʣ���λΪ0.01Mb/s��ȡֵ��ΧΪ0��65535
===========================================================================*/
int32 WlanATSetWifiDataRate(uint32 rate);
uint32 WlanATGetWifiDataRate(void);

/*===========================================================================
 (6)^WIPOW ������WiFi���书��
   WiFi���书�ʣ���λΪ0.01dBm��ȡֵ��ΧΪ -32768��32767
===========================================================================*/
int32 WlanATSetWifiPOW(int32 power_dBm_percent);
int32 WlanATGetWifiPOW(void);

/*===========================================================================
 (7)^WITX ����WiFi�����״̬
===========================================================================*/
int32 WlanATSetWifiTX(WLAN_AT_FEATURE_TYPE onoff);
WLAN_AT_FEATURE_TYPE WlanATGetWifiTX(void);

/*===========================================================================
 (8)^WIRX ����WiFi���ջ�����
===========================================================================*/
#define MAC_ADDRESS_LEN          20       /* MAC��ַ��Ӧ���� */
typedef struct _WIRX_STRU
{
    WLAN_AT_FEATURE_TYPE onoff;     /*0-�ر� 1-��*/
    uint8 src_mac[MAC_ADDRESS_LEN]; /*��ѡ��������������ʱ����Ҫ���˵�Դ��MAC��ַ��MAC��ַ��Ҫ��ð��*/
    uint8 dst_mac[MAC_ADDRESS_LEN]; /*��ѡ��������������ʱ����Ҫ���˵�Ŀ��MAC��ַ��MAC��ַ��Ҫ��ð��*/
}WLAN_AT_WIRX_STRU;
int32 WlanATSetWifiRX(WLAN_AT_WIRX_STRU *params);
int32 WlanATGetWifiRX(WLAN_AT_WIRX_STRU *params);

/*===========================================================================
 (9)^WIRPCKG ��ѯWiFi���ջ�����룬�ϱ����յ��İ�������
===========================================================================*/
typedef struct _WIRPCKG_STRU
{
    uint16 good_result; /*������յ��ĺð�����ȡֵ��ΧΪ0~65535*/
    uint16 bad_result;  /*������յ��Ļ�������ȡֵ��ΧΪ0~65535*/
}WLAN_AT_WIRPCKG_STRU;
int32 WlanATSetWifiRPCKG(int32 flag);
int32 WlanATGetWifiRPCKG(WLAN_AT_WIRPCKG_STRU *params);

/*===========================================================================
 (10)^WIINFO ��ѯWiFi�������Ϣ
===========================================================================*/
#define MAX_PWR_SIZE                  (8)
#define MAX_CHANNEL24G_SIZE           (32)                    /* ����洢24���ŵ���֮�䶺�ŵ�ASCII��ֵ */
#define MAX_CHANNEL5G_SIZE            (96)
#define SIZE_OF_INFOGROUP(group) (sizeof(group) / sizeof(WLAN_AT_WIINFO_MEMBER_STRU))

#ifndef WLAN_TRACE_INFO
    #define WLAN_TRACE_INFO(fmt, ...)     printk("AT <INFO> [%s:%d]: "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#endif
#ifndef WLAN_TRACE_ERROR 
    #define WLAN_TRACE_ERROR(fmt, ...)    printk("AT <ERRO> [%s:%d]: "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#endif

typedef struct _WLAN_AT_WIINFO_MEMBER
{
    const char *name;
    const char *value;
}WLAN_AT_WIINFO_MEMBER_STRU;
typedef struct _WLAN_AT_WIINFO_GROUP
{
    WLAN_AT_WIINFO_MEMBER_STRU *member;
    uint32 size;
}WLAN_AT_WIINFO_GROUP_STRU;
typedef enum
{
    AT_WIINFO_CHANNEL  = 0,    /*֧�ֵ��ŵ���*/
    AT_WIINFO_POWER    = 1,    /*֧�ֵ�Ŀ�깦��*/
    AT_WIINFO_FREQ     = 2,    /*֧�ֵ�Ƶ��*/
}WLAN_AT_WIINFO_TYPE_ENUM;
typedef struct _WIINFO_STRU
{
    WLAN_AT_WIINFO_TYPE_ENUM type; /*��ѯ��Ϣ���ͣ�ȡֵ��ΧΪ 0��255*/
    WLAN_AT_GROUPMEMBER_STRU member;/*�������ݰ�����ʱ�����ַ����������\0�������ȡ�����ַ�������*/
}WLAN_AT_WIINFO_STRU;

int32 WlanATGetWifiInfo(WLAN_AT_WIINFO_STRU *params);

/*===========================================================================
 (11)^WIPLATFORM ��ѯWiFi����ƽ̨��Ӧ����Ϣ
===========================================================================*/
typedef enum
{
    AT_WIPLATFORM_BROADCOM  = 0,    /* ��ͨоƬ */
    AT_WIPLATFORM_ATHEROS   = 1,    /* atherosоƬ */
    AT_WIPLATFORM_QUALCOMM  = 2,    /* ��ͨоƬ */
    AT_WIPLATFORM_TI        = 3,    /* TIоƬ */
    AT_WIPLATFORM_REALTEK   = 4,    /* realtekоƬ */
    AT_WIPLATFORM_HISI      = 5,    /* ��˼оƬ */
}WLAN_AT_WIPLATFORM_TYPE;
WLAN_AT_WIPLATFORM_TYPE WlanATGetWifiPlatform(void);

/*===========================================================================
 (12)^TSELRF ��ѯ���õ����WiFi��Ƶͨ·
===========================================================================*/
int32 WlanATGetTSELRF(void); /* ��ȡ���� */
int32 WlanATSetTSELRF(uint32 group);/*�������ߣ��Ƕ�ͨ·��0 */
/* ֧�ֵ������������У����ַ�����ʽ����eg: 0,1,2,3 */
int32 WlanATGetTSELRFSupport(WLAN_AT_BUFFER_STRU *strBuf);

/*===========================================================================
 (13)^WiPARANGE���á���ȡWiFi PA���������
===========================================================================*/
typedef enum
{
    AT_WiPARANGE_LOW    = 'l',  /*������ģʽ���൱��NO PA*/
    AT_WiPARANGE_HIGH   = 'h',  /*������ģʽ���൱�ڲ���ƽ̨�ϵ�PAģʽ*/
    AT_WiPARANGE_BUTT   = 0xff /*���ֵ*/
}WLAN_AT_WiPARANGE_TYPE;
int32 WlanATSetWifiParange(WLAN_AT_WiPARANGE_TYPE pa_type);
WLAN_AT_WiPARANGE_TYPE WlanATGetWifiParange(void);

/* ֧�ֵ�paģʽ���У����ַ�����ʽ����eg: l,h */
int32 WlanATGetWifiParangeSupport(WLAN_AT_BUFFER_STRU *strBuf);

/*===========================================================================
 (14)^WICALTEMP���á���ȡWiFi���¶Ȳ���ֵ
===========================================================================*/
typedef struct _AT_WICALTEMP_STRU
{
    int32 index;    /* ����ֵ��ȡֵ��ΧΪ 0��255 */
    int32 value;    /* �¶�У׼ֵ��ȡֵ -32768��32767 */
}WLAN_AT_WICALTEMP_STRU;
int32 WlanATGetWifiCalTemp(WLAN_AT_WICALTEMP_STRU *params);
int32 WlanATSetWifiCalTemp(WLAN_AT_WICALTEMP_STRU *params);

/*===========================================================================
 (15)^WICALDATA���á���ȡָ�����͵�WiFi��������
===========================================================================*/
typedef enum
{
    AT_WICALDATA_REL_CAL_POW   = 0,  /* ��Թ���У׼ֵ����λ��dB�� */
    AT_WICALDATA_ABS_CAL_POW   = 1,  /* ���Թ���У׼ֵ����λ��dBm��*/
    AT_WICALDATA_REL_IDX_POW   = 2,  /* ��Թ�������ֵ����Indexֵ��*/
    AT_WICALDATA_ABS_IDX_POW   = 3,  /* ���Թ�������ֵ����Indexֵ��*/
    AT_WICALDATA_ABS_CAL_FREQ  = 4,  /* Ƶ�ʲ���ģ��ֵ����λ��KHz��*/
    AT_WICALDATA_ABS_IDX_FREQ  = 5,  /* Ƶ�ʲ�������ֵ����Indexֵ��*/
    AT_WICALDATA_ABS_CAL_TEMP  = 6,  /* �¶�У׼ģ��ֵ����λ��0.1���϶ȣ�*/
    AT_WICALDATA_ABS_IDX_TEMP  = 7,  /* �¶�У׼����ֵ����Indexֵ��*/
}WLAN_AT_WICALDATA_TYPE;
typedef struct _AT_WICALDATA_STRU
{
    WLAN_AT_WICALDATA_TYPE type;     /* У׼���� */
    uint32 group;                    /* �������� */
    WLAN_AT_WIMODE_TYPE mode;        /* ����ģʽ */
    WLAN_AT_WIFREQ_TYPE band;        /* Ƶ����Ϣ */
    WLAN_AT_WIBAND_TYPE bandwidth;  /* ������Ϣ */
    uint32 freq;                     /* Ƶ����Ϣ */
    char  data[128];                /* ��Ϣ��ӦЯ�����ݣ�������󳤶�128�ֽ� */
}WLAN_AT_WICALDATA_STRU;
int32 WlanATSetWifiCalData(WLAN_AT_WICALDATA_STRU *params);
int32 WlanATGetWifiCalData(WLAN_AT_WICALDATA_STRU *params);

/*===========================================================================
 (16)^WICAL���á���ȡУ׼������״̬���Ƿ�֧�ֲ���
===========================================================================*/
int32 WlanATSetWifiCal(WLAN_AT_FEATURE_TYPE onoff);
WLAN_AT_FEATURE_TYPE WlanATGetWifiCal(void);
WLAN_AT_FEATURE_TYPE WlanATGetWifiCalSupport(void);

/*===========================================================================
 (17)^WICALFREQ ���á���ѯƵ�ʲ���ֵ
===========================================================================*/
typedef enum
{
    AT_WICALFREQ_INDEX   = 0,  /* Ƶ�ʶ�Ӧ���� */
    AT_WICALFREQ_FREQ    = 1,  /* ����Ƶ��ֵ */
}WLAN_AT_WICALFREQ_TYPE;
typedef struct _AT_WICALFREQ_STRU
{
    WLAN_AT_WICALFREQ_TYPE type;     /* Ƶ��ֵ���� */
    int32 value;                     /* ��Ӧֵ��-32768~32767 */
}WLAN_AT_WICALFREQ_STRU;
int32 WlanATSetWifiCalFreq(WLAN_AT_WICALFREQ_STRU *params);
int32 WlanATGetWifiCalFreq(WLAN_AT_WICALFREQ_STRU *params);

/*===========================================================================
 (18)^WICALPOW ���á���ѯ���ʲ���ֵ
===========================================================================*/
typedef enum
{
    AT_WICALPOW_INDEX   = 0,  /* ���ʶ�Ӧ���� */
    AT_WICALPOW_POWER   = 1,  /* ���幦��ֵ */
}WLAN_AT_WICALPOW_TYPE;
typedef struct _AT_WICALPOW_STRU
{
    WLAN_AT_WICALPOW_TYPE type;     /* Ƶ��ֵ���� */
    int32 value;                     /* ��Ӧֵ��-32768~32767 */
}WLAN_AT_WICALPOW_STRU;
int32 WlanATSetWifiCalPOW(WLAN_AT_WICALPOW_STRU *params);
int32 WlanATGetWifiCalPOW(WLAN_AT_WICALPOW_STRU *params);

extern int wlan_run_shell(const char *pshell);
extern void wifi_power_off_4356(void);

/*===========================================================================
 (19)^WIPAVARS2G ���á���ѯ���ʲ���ֵ
===========================================================================*/
#define     WIFI_2G_RF_GROUPS_PARA_NUMBER    (3)     /* ÿ���������������� */
#define     WIFI_2G_RF_GROUP_PARA_LEN        (8)     /* ����ÿ�������Ļ�������СΪ�˸��ֽ� */

typedef struct _AT_PAVARS2G_STRU
{    
    int32 ANT_Index;      /*����ANT0,1,2...*/
    char  data[WIFI_2G_RF_GROUPS_PARA_NUMBER][WIFI_2G_RF_GROUP_PARA_LEN];    /*16������Ƶ����*/
}WLAN_AT_PAVARS2G_STRU;

int32 WlanATGetWifi2GPavars(WLAN_AT_BUFFER_STRU *params);
int32 WlanATSetWifi2GPavars(WLAN_AT_PAVARS2G_STRU *params);
/*===========================================================================
 (20)^WIPAVARS5G ���á���ѯ���ʲ���ֵ
===========================================================================*/
#define     WIFI_5G_RF_GROUPS_PARA_NUMBER    (12)    /* ÿ��������ʮ�������� */
#define     WIFI_5G_RF_GROUP_PARA_LEN        (8)     /* ����ÿ�������Ļ�������СΪ�˸��ֽ� */

typedef struct _AT_PAVARS5G_STRU
{    
    int32 ANT_Index;      /*����ANT0,1,2...*/
    char  data[WIFI_5G_RF_GROUPS_PARA_NUMBER][WIFI_5G_RF_GROUP_PARA_LEN];    /*16������Ƶ����*/
}WLAN_AT_PAVARS5G_STRU;

int32 WlanATGetWifi5GPavars(WLAN_AT_BUFFER_STRU *params);
int32 WlanATSetWifi5GPavars(WLAN_AT_PAVARS5G_STRU *params);
/*===========================================================================
 (21)^WIFIDEBUG ��дWIFI�Ĵ����������Ϣ
===========================================================================*/
typedef enum
{
    AT_WIDEBUG_REG_READ         = 1,  /* �������������Ĵ��������� */
    AT_WIDEBUG_REG_WRITE        = 2,  /* �����Ĵ���д���� */
    AT_WIDEBUG_REG_FILE         = 3,  /* �����Ĵ����ļ�д���� */
    AT_WIDEBUG_TEM_COM_SWTICH   = 4,  /* �²����� */
    AT_WIDEBUG_CALI_INTO_READ   = 5,  /* ��ӡ�ϵ�У׼�Ͷ�̬У׼ʱ����ز��� */
}WLAN_AT_DEBUG_TYPE;

/*****************************************************************************
 ��������  : int32 AT_SetWifiDebug(WLAN_AT_DEBUG_TYPE debug_type, uint32 value1, uint32 *value2)
 ��������  :^WIFIDEBUG ��дWIFI�Ĵ����������Ϣ
 �������  :
    <debug_type> ��ѯ����1~5���ɸ���������չ����
        1 �������������Ĵ���������
        2 �����Ĵ���д����
        3 ��ȡ���мĴ���ֵ��д���ļ�(д���ļ�Ϊ/online/hal_all_reg_data*.txt)
        4 �²�����
        5 ��ӡ�ϵ�У׼�Ͷ�̬У׼ʱ����ز��� 
    <value1> �������ã���type=1��2��ʾ�Ĵ�����ַ��type=3Ϊ�գ�����Ϊ�Զ������ã�
    <value2> ����/������ã���type=1��2��ʾ�Ĵ���ֵ��type=3Ϊ�գ�����Ϊ�Զ������ã�
 �������  : NA
 �� �� ֵ  : WLAN_AT_RETURN_TYPE
 ����˵��  : NA
*****************************************************************************/
int32 WlanATSetWifiDebug(WLAN_AT_DEBUG_TYPE debug_type, uint32 vaule1, uint32 *valule2);

#define HAL_DEVICE_CALI_2G_PATH_NUM    (2)          /* 2G֧��2��saw */
#define WITP_CHANNEL_SUPPORT_NUMS      (2)          /* оƬ֧�����2������ */
#define EQUIPMENT_TEST_5G_SUB_BAND_NUM (4)          /* װ������5G 4����ͬband */
typedef struct
{
    /* RC/R/C */
    uint8    uc_rc_code_40M ;
    uint8    uc_rc_code_20M ;
    uint8    uc_r_code      ;
    uint8    uc_c_code      ;

    /* 2G */
    /* RXDC */
    uint16   us_ana_rxdc[HAL_DEVICE_CALI_2G_PATH_NUM][WITP_CHANNEL_SUPPORT_NUMS];
    uint16   us_dig_rxdc_i[HAL_DEVICE_CALI_2G_PATH_NUM][WITP_CHANNEL_SUPPORT_NUMS];
    uint16   us_dig_rxdc_q[HAL_DEVICE_CALI_2G_PATH_NUM][WITP_CHANNEL_SUPPORT_NUMS];

    /* TXPOWER */
    uint8    uc_csw_band1[HAL_DEVICE_CALI_2G_PATH_NUM][WITP_CHANNEL_SUPPORT_NUMS];
    uint8    uc_csw_band2[HAL_DEVICE_CALI_2G_PATH_NUM][WITP_CHANNEL_SUPPORT_NUMS];
    uint8    uc_csw_band3[HAL_DEVICE_CALI_2G_PATH_NUM][WITP_CHANNEL_SUPPORT_NUMS];
    uint8    uc_csw_band4[HAL_DEVICE_CALI_2G_PATH_NUM][WITP_CHANNEL_SUPPORT_NUMS];
    uint8    uc_csw_band5[HAL_DEVICE_CALI_2G_PATH_NUM][WITP_CHANNEL_SUPPORT_NUMS];

    uint8    uc_upc_band1_init[HAL_DEVICE_CALI_2G_PATH_NUM][WITP_CHANNEL_SUPPORT_NUMS];
    uint8    uc_upc_band2_init[HAL_DEVICE_CALI_2G_PATH_NUM][WITP_CHANNEL_SUPPORT_NUMS];
    uint8    uc_upc_band3_init[HAL_DEVICE_CALI_2G_PATH_NUM][WITP_CHANNEL_SUPPORT_NUMS];
    uint8    uc_upc_band4_init[HAL_DEVICE_CALI_2G_PATH_NUM][WITP_CHANNEL_SUPPORT_NUMS];
    uint8    uc_upc_band5_init[HAL_DEVICE_CALI_2G_PATH_NUM][WITP_CHANNEL_SUPPORT_NUMS];

    uint8    uc_upc_band1[HAL_DEVICE_CALI_2G_PATH_NUM][WITP_CHANNEL_SUPPORT_NUMS];
    uint8    uc_upc_band2[HAL_DEVICE_CALI_2G_PATH_NUM][WITP_CHANNEL_SUPPORT_NUMS];
    uint8    uc_upc_band3[HAL_DEVICE_CALI_2G_PATH_NUM][WITP_CHANNEL_SUPPORT_NUMS];
    uint8    uc_upc_band4[HAL_DEVICE_CALI_2G_PATH_NUM][WITP_CHANNEL_SUPPORT_NUMS];
    uint8    uc_upc_band5[HAL_DEVICE_CALI_2G_PATH_NUM][WITP_CHANNEL_SUPPORT_NUMS];


    /* TXDC */
    int8     c_txdc_i1[HAL_DEVICE_CALI_2G_PATH_NUM][WITP_CHANNEL_SUPPORT_NUMS];
    int8     c_txdc_i2[HAL_DEVICE_CALI_2G_PATH_NUM][WITP_CHANNEL_SUPPORT_NUMS];
    int8     c_txdc_q1[HAL_DEVICE_CALI_2G_PATH_NUM][WITP_CHANNEL_SUPPORT_NUMS];
    int8     c_txdc_q2[HAL_DEVICE_CALI_2G_PATH_NUM][WITP_CHANNEL_SUPPORT_NUMS];

    /* TXIQ */
    int8     c_txiq_p[HAL_DEVICE_CALI_2G_PATH_NUM][WITP_CHANNEL_SUPPORT_NUMS];
    int8     c_txiq_e[HAL_DEVICE_CALI_2G_PATH_NUM][WITP_CHANNEL_SUPPORT_NUMS];

    /*******************RXIQ*****************************/
    int8     c_rxiq_p[HAL_DEVICE_CALI_2G_PATH_NUM][WITP_CHANNEL_SUPPORT_NUMS];
    int8     c_rxiq_e[HAL_DEVICE_CALI_2G_PATH_NUM][WITP_CHANNEL_SUPPORT_NUMS];


    /* 5G */
    /* RXDC */
    uint16   us_ana_rxdc_40M[WITP_CHANNEL_SUPPORT_NUMS][EQUIPMENT_TEST_5G_SUB_BAND_NUM];
    uint16   us_dig_rxdc_40M_i[WITP_CHANNEL_SUPPORT_NUMS][EQUIPMENT_TEST_5G_SUB_BAND_NUM];
    uint16   us_dig_rxdc_40M_q[WITP_CHANNEL_SUPPORT_NUMS][EQUIPMENT_TEST_5G_SUB_BAND_NUM];

    uint16   us_ana_rxdc_20M[WITP_CHANNEL_SUPPORT_NUMS][EQUIPMENT_TEST_5G_SUB_BAND_NUM];
    uint16   us_dig_rxdc_20M_i[WITP_CHANNEL_SUPPORT_NUMS][EQUIPMENT_TEST_5G_SUB_BAND_NUM];
    uint16   us_dig_rxdc_20M_q[WITP_CHANNEL_SUPPORT_NUMS][EQUIPMENT_TEST_5G_SUB_BAND_NUM];

    /* TXPOWER */
    uint8    uc_csw_5g[WITP_CHANNEL_SUPPORT_NUMS][EQUIPMENT_TEST_5G_SUB_BAND_NUM];
    uint8    uc_upc_5g_init[WITP_CHANNEL_SUPPORT_NUMS][EQUIPMENT_TEST_5G_SUB_BAND_NUM];
    uint8    uc_upc_5g[WITP_CHANNEL_SUPPORT_NUMS][EQUIPMENT_TEST_5G_SUB_BAND_NUM];

    /* TXDC */
    int8     c_txdc_40M_i1[WITP_CHANNEL_SUPPORT_NUMS];
    int8     c_txdc_40M_i2[WITP_CHANNEL_SUPPORT_NUMS];
    int8     c_txdc_40M_q1[WITP_CHANNEL_SUPPORT_NUMS];
    int8     c_txdc_40M_q2[WITP_CHANNEL_SUPPORT_NUMS];

    int8     c_txdc_20M_i1[WITP_CHANNEL_SUPPORT_NUMS];
    int8     c_txdc_20M_i2[WITP_CHANNEL_SUPPORT_NUMS];
    int8     c_txdc_20M_q1[WITP_CHANNEL_SUPPORT_NUMS];
    int8     c_txdc_20M_q2[WITP_CHANNEL_SUPPORT_NUMS];

    /* TXIQ */
    int8     c_txiq_40M_p[WITP_CHANNEL_SUPPORT_NUMS][EQUIPMENT_TEST_5G_SUB_BAND_NUM];
    int8     c_txiq_40M_e[WITP_CHANNEL_SUPPORT_NUMS][EQUIPMENT_TEST_5G_SUB_BAND_NUM];

    int8     c_txiq_20M_p[WITP_CHANNEL_SUPPORT_NUMS][EQUIPMENT_TEST_5G_SUB_BAND_NUM];
    int8     c_txiq_20M_e[WITP_CHANNEL_SUPPORT_NUMS][EQUIPMENT_TEST_5G_SUB_BAND_NUM];


    /* RXIQ */
    int8     c_rxiq_p_5g[WITP_CHANNEL_SUPPORT_NUMS][EQUIPMENT_TEST_5G_SUB_BAND_NUM];
    int8     c_rxiq_e_5g[WITP_CHANNEL_SUPPORT_NUMS][EQUIPMENT_TEST_5G_SUB_BAND_NUM];

}HI1151_EQUIP_CALI_INFO_STRU;

//////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
    #if __cplusplus
    }
    #endif
#endif    
#endif //end of #ifndef ATWLANAPI_H


