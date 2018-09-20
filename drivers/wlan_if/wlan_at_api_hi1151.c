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



#include <linux/module.h>
#include <linux/delay.h>
#include <linux/string.h>

#include <product_config.h>

#include "wlan_if.h"
#include "wlan_at_api.h"
#include "wlan_at.h"
#include "wlan_utils.h"

#define WLAN_AT_SSID_SUPPORT            2                  /*֧�ֵ�SSID����*/
#define WLAN_AT_KEY_SUPPORT             5                  /*֧�ֵķ�����*/
#define WLAN_AT_MODE_SUPPORT            "1,2,3,4,5"        /*֧�ֵ�ģʽ(a/b/g/n/ac)*/
#define WLAN_AT_BAND_SUPPORT            "0,1,2"            /*֧�ֵĴ���(0-20M/1-40M/2-80M/3-160M)*/
#define WLAN_AT_TSELRF_SUPPORT          "0,1,2,3"          /*֧�ֵ�������������*/
#define WLAN_AT_GROUP_MAX               4                  /*֧�ֵ������������*/
#define RF_2G_0   (0)
#define RF_2G_1   (1)
/*5g��Ƶͨ·*/
#define RF_5G_0   (2)
#define RF_5G_1   (3)
/*MIMO*/
#define RF_2G_MIMO   (4)
#define RF_5G_MIMO   (5)

/*adapter interface*/
#define IF_HIPRIV   "wlan0"

/*WIFI���ʵ�������*/
#define WLAN_AT_POWER_MIN               (0)
#define WLAN_AT_POWER_MAX               (30)
#define WLAN_AT_POW_DEFAULT             (600)

#define RX_PACKET_SIZE                  1000                /*װ��ÿ�η�����*/

#define WLAN_CHANNEL_2G_MIN             1                   /*2.4G�ŵ���Сֵ*/
#define WLAN_CHANNEL_5G_MIN             36                  /*5G�ŵ���Сֵ*/
#define WLAN_CHANNEL_2G_MAX             14                  /*2.4G�ŵ����*/
#define WLAN_CHANNEL_2G_MIDDLE          6
#define WLAN_CHANNEL_5G_MAX             165                 /*5G�ŵ����*/

#define WLAN_CHANNEL_2G_40M_MIN         1
#define WLAN_CHANNEL_2G_40M_MAX         13
#define WLAN_CHANNEL_2G_40M_MID_ABOVE   11
#define WLAN_CHANNEL_2G_40M_MID_BELOW   6

#define WLAN_CHANNEL_5G_W52_START       36
#define WLAN_CHANNEL_5G_W52_END         48
#define WLAN_CHANNEL_5G_W53_START       52
#define WLAN_CHANNEL_5G_W53_END         64
#define WLAN_CHANNEL_5G_W57_START       149
#define WLAN_CHANNEL_5G_W57_END         161

#define WLAN_CHANNEL_5G_INTERVAL        4                     /*5G�ŵ����*/
#define WLAN_CHANNEL_5G_40M_INTERVAL    8                     /*5G 40M�ŵ����*/
#define WLAN_CHANNEL_5G_80M_INTERVAL    16                     /*5G 80M�ŵ����*/
#define WLAN_FREQ_5G_W52_80M_MIN        5180                  /*W52 80M��СƵ��*/
#define WLAN_FREQ_5G_W53_80M_MAX        5260                  /*W53 80M���Ƶ��*/
#define WLAN_FREQ_5G_W56_80M_MIN        5500                  /*W56 80M��СƵ��*/
#define WLAN_FREQ_5G_W56_80M_MAX        5660                  /*W56 80M���Ƶ��*/
#define WLAN_FREQ_5G_W57_80M_MIN        5745                  /*W57 80M��СƵ��*/
#define WLAN_FREQ_5G_W57_80M_MAX        5745                  /*W57 80M���Ƶ��*/
#define WLAN_FREQ_2G_MAX                2484                  /*2.4G���Ƶ��*/
#define WLAN_FREQ_5G_W52_MIN            5180                  /*W52��СƵ��*/
#define WLAN_FREQ_5G_W53_MAX            5320                  /*W53���Ƶ��*/
#define WLAN_FREQ_5G_W52_40M_MIN        5180                  /*W52 40M��СƵ��*/
#define WLAN_FREQ_5G_W53_40M_MAX        5300                  /*W53 40M���Ƶ��*/
#define WLAN_FREQ_5G_W56_MIN            5500                  /*W56��СƵ��*/
#define WLAN_FREQ_5G_W56_MAX            5720                  /*W56���Ƶ��*/
#define WLAN_FREQ_5G_W56_40M_MIN        5500                  /*W56 40M��СƵ��*/
#define WLAN_FREQ_5G_W56_40M_MAX        5700                  /*W56 40M���Ƶ��*/
#define WLAN_FREQ_5G_W57_MIN            5745                  /*W57��СƵ��*/
#define WLAN_FREQ_5G_W57_MAX            5825                  /*W57���Ƶ��*/
#define WLAN_FREQ_5G_W57_40M_MIN        5745                  /*W57��СƵ��*/
#define WLAN_FREQ_5G_W57_40M_MAX        5785                  /*W57���Ƶ��*/

#define WIFI_CMD_MAX_SIZE               256                   /*cmd�ַ���256����*/
#define WIFI_CMD_8_SIZE                 8                     /*cmd�ַ���8����*/
#define HUNDRED                         100

#define WLAN_AT_TYPE_MAX                2                  /*֧�ֻ�ȡ�������Ϣ����*/

#define DYNC_ADJUST_DELAY              (300)                  /* ��̬У׼��ʱ300���� */

/* ��˼wifiоƬ�Ĵ�����ַ��Χ���� */
#define HISI_SOC_REG_ADDR_MIN   (0x20000000)
#define HISI_SOC_REG_ADDR_MAX   (0x2003bfff)

#define WLAN_RFCH_TRANS(_uc_rfch)       (0 == _uc_rfch)? "0001": "0010"

#define PTR_NULL        (0L)
int8*   auc_mode_table_2G[6][3] =
{                           /*AT_WIBAND_20M*/   /*AT_WIBAND_40M*/   /*AT_WIBAND_80M */
    /*AT_WIMODE_CW */       {PTR_NULL,          PTR_NULL,           PTR_NULL},
    /*AT_WIMODE_80211a*/    {PTR_NULL,          PTR_NULL,           PTR_NULL},
    /*AT_WIMODE_80211b*/    {"11b",             PTR_NULL,           PTR_NULL},
    /*AT_WIMODE_80211g */   {"11g",             PTR_NULL,           PTR_NULL},
    /*AT_WIMODE_80211n*/    {"11ng20",          "11ng40plus",       PTR_NULL},
    /*AT_WIMODE_80211ac */  {PTR_NULL,          PTR_NULL,           PTR_NULL},
};
int8*   auc_mode_table_5G[6][3] =
{                           /*AT_WIBAND_20M*/   /*AT_WIBAND_40M*/   /*AT_WIBAND_80M */
    /*AT_WIMODE_CW */       {PTR_NULL,          PTR_NULL,           PTR_NULL},
    /*AT_WIMODE_80211a*/    {"11a",             PTR_NULL,           PTR_NULL},
    /*AT_WIMODE_80211b*/    {PTR_NULL,          PTR_NULL,           PTR_NULL},
    /*AT_WIMODE_80211g */   {PTR_NULL,          PTR_NULL,           PTR_NULL},
    /*AT_WIMODE_80211n*/    {"11na20",          "11na40",           PTR_NULL},
    /*AT_WIMODE_80211ac */  {"11ac20",          "11ac40",           "11ac80"},
};

typedef struct
{
    uint32 brate;
    int8 rate_str[WIFI_CMD_8_SIZE];
}BRATE_ST;

/*ȫ�ֱ���*/
uint8   g_uc_add_user_done = 0;

/*��������*/
enum WAL_ATCMDSRV_IOCTL_CMD
{
    WAL_ATCMDSRV_IOCTL_CMD_NORM_SET         = 0,
    WAL_ATCMDSRV_IOCTL_CMD_RX_PCKG_GET      ,
    WAL_ATCMDSRV_IOCTL_CMD_VAP_DOWN_SET     ,
    WAL_ATCMDSRV_IOCTL_CMD_HW_ADDR_SET      ,
    WAL_ATCMDSRV_IOCTL_CMD_VAP_UP_SET       ,
    WAL_ATCMDSRV_IOCTL_CMD_CHIPCHECK_SET    ,
    WAL_ATCMDSRV_IOCTL_CMD_REGINFO_SET      ,
    WAL_ATCMDSRV_IOCTL_CMD_CALIINFO_SET     ,
    HWIFI_IOCTL_CMD_TEST_BUTT
};

/*wifi����AT���������ڣ������ڳ�ʼ����ʱ��Ὣ�ӿ�ע�����ȫ�ֱ���*/
typedef uint32 *(*hipriv_entry_t)(void *, void*, void *);
static hipriv_entry_t g_at_hipriv_entry = NULL;

/*******************************************************************************
    ͨ�ú�
*******************************************************************************/
#define HIPRIV_BEEN_DONE(ret, cmd, ...) do {    \
        WLAN_TRACE_INFO("ret:%d run:"cmd"\n",(ret),##__VA_ARGS__);    \
        if (ret) return (ret);    \
}while(0)

/*��WiFiоƬ�·���������*/
#define WIFI_CMD_RET(_ret)                     ((int32)((0 == (_ret))?  AT_RETURN_SUCCESS: AT_RETURN_FAILURE))
#define WIFI_TEST_CMD(_cmd_id, _cmd)          WIFI_CMD_RET(g_at_hipriv_entry(IF_HIPRIV, (_cmd_id), (_cmd)))
/* ��ӡwifi vap��Ϣ */
#define WIFI_VAP_INFO(_vap)     \
        WLAN_TRACE_INFO("%4d_%4d_%4d_%4d_%4d_%4d_%4d_%4d_%4d_%4d_%4d_%4X_%4d\n", \
            (_vap).wifiStatus, \
            (_vap).wifiMode, \
            (_vap).wifiBandwith, \
            (_vap).wifiBand, \
            (_vap).wifiFreq.value, \
            (_vap).wifiChannel, \
            (_vap).wifiRate, \
            (_vap).wifiPower, \
            (_vap).wifiTX, \
            (_vap).wifiRX.onoff, \
            (_vap).wifiPckg.good_result, \
            (_vap).wifiGroup, \
            (_vap).wifiFirstTestItem);

#ifndef STATIC
    #define STATIC static
#endif

/*WiFiȫ�ֱ����ṹ�� */
typedef struct tagWlanATGlobal
{
    WLAN_AT_WIENABLE_TYPE   wifiStatus;    /*Ĭ�ϼ��ز���ģʽ*/
    WLAN_AT_WIMODE_TYPE     wifiMode;      /*wifiЭ��ģʽ*/
    WLAN_AT_WIBAND_TYPE     wifiBandwith;  /*wifiЭ����ʽ*/
    WLAN_AT_WIFREQ_TYPE     wifiBand;      /*wifi��ǰƵ��*/
    WLAN_AT_WIFREQ_STRU     wifiFreq;      /*wifiƵ����Ϣ*/
    uint32                  wifiChannel;   /*wifi�ŵ���Ϣ*/
    uint32                  wifiRate;      /*wifi��������*/
    int32                   wifiPower;     /*wifi���书��*/
    WLAN_AT_FEATURE_TYPE    wifiTX;        /*wifi�����״̬*/
    WLAN_AT_WIRX_STRU       wifiRX;        /*wifi���ջ�״̬*/
    WLAN_AT_WIRPCKG_STRU    wifiPckg;      /*wifi�����*/
    uint32                  wifiGroup;     /* wifi����ģʽ:(0,1��2Gͨ��0,1)(2,3��5Gͨ��0,1) */
    uint32                  wifiFirstTestItem;  /* ��¼��ǰ�Ƿ�Ϊwifi 2g(420/617)��5g�ĵ�һ�������� */
}WLAN_AT_GLOBAL_ST;

/* ��¼��ǰ��WiFiģʽ������Ƶ�ʣ����ʵȲ���, 0xFF��������ģʽΪ��Ч */
STATIC WLAN_AT_GLOBAL_ST g_wlan_at_data = {AT_WIENABLE_OFF, AT_WIMODE_80211n, AT_WIBAND_20M
         , AT_WIFREQ_24G, {2412, 0}, 1, 6500, 3175, AT_FEATURE_DISABLE, {AT_FEATURE_DISABLE, {0}, {0}}
         , {0,0}, 0xFF, 0};/*����WiFi��Ĭ�ϲ���*/

/* ����2412����wifi��ǰƵ��,6500����wifi����,3175����wifi���书��,0xFF��������ģʽΪ��Ч */
STATIC WLAN_AT_GLOBAL_ST g_wlan_at_data_old = {AT_WIENABLE_OFF, AT_WIMODE_80211n, AT_WIBAND_20M
         , AT_WIFREQ_24G, {2412, 0}, 1, 6500, 3175, AT_FEATURE_DISABLE, {AT_FEATURE_DISABLE, {0}, {0}}
         , {0,0}, 0xFF, 0};/* ����WiFi���� */

/*******************************************************************************
    �ṩ��������ע��ӿ�
*******************************************************************************/
int reg_at_hipriv_entry(hipriv_entry_t hipriv_entry)
{
    if (NULL == hipriv_entry)
    {
        WLAN_TRACE_ERROR("reg_at_ioctl_entry null point error! ioctl:%p\n", hipriv_entry);
        return -1;
    }
    g_at_hipriv_entry = hipriv_entry;
    return 0;
}
EXPORT_SYMBOL(reg_at_hipriv_entry);
/*****************************************************************************
��������  : int unreg_at_hipriv_entry()
��������  : wifi������AT�ӿ��ÿ�
�������  : hipriv_entry: ����AT���ָ��
�������  : NA
�� �� ֵ  : �����л����,0:�ɹ�
*****************************************************************************/
int unreg_at_hipriv_entry(hipriv_entry_t hipriv_entry)
{
    g_at_hipriv_entry = NULL;
    return (0);
}

#if (MBB_WIFI_RF_PATH_SWITCH == FEATURE_ON)
/*****************************************************************************
��������  : int32 wifi_switch_saw_notify_hisi()
��������  : ֪ͨ��˼wifi����������Ƶ����
�������  : rf_path_index : 0:RF_PATH_NOB40,1:RF_PATH_B40
�������  : NA
�� �� ֵ  : �����л����,0:�ɹ�,-1:ʧ��
*****************************************************************************/
int32 wifi_switch_saw_notify_hisi(int rf_path_index)
{
    int ret = 0;
    int8 wl_cmd[WIFI_CMD_MAX_SIZE] = {0};

    (void)OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), "Hisilicon0 set_2g_path %d", rf_path_index);
    if (NULL == g_at_hipriv_entry)
    {
        WLAN_TRACE_INFO("[wifi_switch_saw_notify_hisi] g_at_hipriv_entry is NULL!\n");
        return (-1);
    }
    ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_NORM_SET, wl_cmd);
    WLAN_TRACE_INFO("wifi_switch_saw_notify_hisi rf_path_index:%d,cmd=%s\n", rf_path_index, wl_cmd);
    return ret;
}
#endif
//////////////////////////////////////////////////////////////////////////
/*(1)^WIENABLE ����WiFiģ��ʹ�� */
//////////////////////////////////////////////////////////////////////////
/*****************************************************************************
 ��������  : int32 WlanATSetWifiEnable(WLAN_AT_WIENABLE_TYPE onoff)
 ��������  : ����wifi �������ģʽ������ģʽ���ر�wifi
 �������  :  0  �ر�
              1  ������ģʽ
              2  �򿪲���ģʽ
 �������  : NA
 �� �� ֵ  : 0 �ɹ�
             1 ʧ��
 ����˵��  :
*****************************************************************************/
STATIC int32 ATSetWifiEnable(WLAN_AT_WIENABLE_TYPE onoff)
{
    int32 ret = AT_RETURN_SUCCESS;
    int8 wl_cmd[WIFI_CMD_MAX_SIZE] = {0};

    if (NULL == g_at_hipriv_entry)
    {
        WLAN_TRACE_INFO("check_wifi_valid failed!\n");
        return AT_RETURN_FAILURE;
    }

    /* wake up vap */
    (void)OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), "Hisilicon0 enable 1", IF_HIPRIV);
    (void)WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_NORM_SET, wl_cmd);
    HIPRIV_BEEN_DONE(ret, "%s", wl_cmd);

    (void)memset(wl_cmd, 0, WIFI_CMD_MAX_SIZE);
    (void)OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), "Hisilicon0 pm_enable 0", IF_HIPRIV);
    (void)WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_NORM_SET, wl_cmd);
    HIPRIV_BEEN_DONE(ret, "%s", wl_cmd);

    ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_CHIPCHECK_SET, "1");
    if (AT_RETURN_SUCCESS != ret)
    {
        WLAN_TRACE_INFO("chip_check failed!\n");
        return ret;
    }

    /* Ĭ�Ϲرն�̬����У׼,0xFF����wifi����ģʽΪ��Ч */
    g_wlan_at_data.wifiGroup = 0xFF;
    g_wlan_at_data.wifiFirstTestItem = 0;
    (void)memcpy(&g_wlan_at_data_old, &g_wlan_at_data, sizeof(g_wlan_at_data_old));
    WIFI_VAP_INFO(g_wlan_at_data_old);
    WIFI_VAP_INFO(g_wlan_at_data);

    switch (onoff)
    {
        case AT_WIENABLE_OFF:
            {
                WLAN_TRACE_INFO("Set wifi to off mode\n");
                ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_VAP_DOWN_SET, "1");
                HIPRIV_BEEN_DONE(ret, "%s", "ifconfig vap0 down");
                g_wlan_at_data.wifiStatus = AT_WIENABLE_OFF;
                g_uc_add_user_done = 0;
            }
            break;
        case AT_WIENABLE_ON:
            {
                WLAN_TRACE_INFO("Set wifi to normal mode\n");
                ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_VAP_UP_SET, "1");
                HIPRIV_BEEN_DONE(ret, "%s", "ifconfig vap0 up");
                g_wlan_at_data.wifiStatus = AT_WIENABLE_ON;
            }
            break;
        case AT_WIENABLE_TEST:
            {
                WLAN_TRACE_INFO("Set wifi to test mode\n");
                ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_VAP_UP_SET, "1");
                HIPRIV_BEEN_DONE(ret, "%s", "ifconfig vap0 up");
                /* ��ӼĴ���������������ر�wifi��Beacon֡ */
                (void)memset(wl_cmd, 0, WIFI_CMD_MAX_SIZE);
                (void)OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), " %s al_tx_51 1", IF_HIPRIV);
                ret += WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_NORM_SET, wl_cmd);
                HIPRIV_BEEN_DONE(ret, "%s", wl_cmd);

                (void)memset(wl_cmd, 0, WIFI_CMD_MAX_SIZE);
                (void)OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), " %s al_tx_51 0", IF_HIPRIV);
                ret += WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_NORM_SET, wl_cmd);
                HIPRIV_BEEN_DONE(ret, "%s", wl_cmd);

                g_wlan_at_data.wifiStatus = AT_WIENABLE_TEST;
            }
            break;
        default:
            ret = AT_RETURN_FAILURE;
            break;
    }

    return ret;
}
/*****************************************************************************
 ��������  : WLAN_AT_WIENABLE_TYPE WlanATGetWifiEnable()
 ��������  : ��ȡ��ǰ��WiFiģ��ʹ��״̬
 �������  : NA
 �������  : NA
 �� �� ֵ  : 0  �ر�
             1  ����ģʽ(����ģʽ)
             2  ����ģʽ(������ģʽ)
 ����˵��  :
*****************************************************************************/
STATIC WLAN_AT_WIENABLE_TYPE ATGetWifiEnable(void)
{
    int32 ret = AT_RETURN_SUCCESS;

    if(NULL == g_at_hipriv_entry)
    {
        WLAN_TRACE_INFO("check_wifi_valid failed!\n");
        return AT_WIENABLE_OFF;
    }

    /* chip check */
    ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_CHIPCHECK_SET, "1");

    if (AT_RETURN_SUCCESS != ret)
    {
        WLAN_TRACE_INFO("chip_check failed!\n");
        return AT_WIENABLE_OFF;
    }
    else
    {
        return g_wlan_at_data.wifiStatus;
    }
}

//////////////////////////////////////////////////////////////////////////
/*(2)^WIMODE ����WiFiģʽ���� Ŀǰ��Ϊ��ģʽ����*/
//////////////////////////////////////////////////////////////////////////
/*****************************************************************************
 ��������  : int32 WlanATSetWifiMode(WLAN_AT_WIMODE_TYPE mode)
 ��������  : ����WiFi AP֧�ֵ���ʽ
 �������  : 0,  CWģʽ
             1,  802.11a��ʽ
             2,  802.11b��ʽ
             3,  802.11g��ʽ
             4,  802.11n��ʽ
             5,  802.11ac��ʽ
 �������  : NA
 �� �� ֵ  : 0 �ɹ�
             1 ʧ��
 ����˵��  :
*****************************************************************************/
STATIC int32 ATSetWifiMode(WLAN_AT_WIMODE_TYPE mode)
{
    int ret = 0;
    int8 wl_cmd[WIFI_CMD_MAX_SIZE] = {0};
    WLAN_TRACE_INFO("%s:enter\n", __FUNCTION__);
    WLAN_TRACE_INFO("WLAN_AT_WIMODE_TYPE:%d\n", mode);

    if (mode > AT_WIMODE_MAX)
    {
        WLAN_TRACE_ERROR("WLAN_AT_WIMODE_TYPE:%d unknow!\n", mode);
        return (AT_RETURN_FAILURE);
    }

    /*˫ͨ��������ģʽ������*/
    if ((AT_WIMODE_80211n == mode)
        || (AT_WIMODE_80211ac == mode))
    {
        /*MIMO ģʽ*/
        if((RF_2G_MIMO == g_wlan_at_data.wifiGroup) || (RF_5G_MIMO == g_wlan_at_data.wifiGroup))
        {
            /*����ģʽ*/
            memset(wl_cmd, 0, WIFI_CMD_MAX_SIZE);
            OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), "%s txch 0011", IF_HIPRIV);
            ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_NORM_SET, wl_cmd);
            HIPRIV_BEEN_DONE(ret, "%s", wl_cmd);

            memset(wl_cmd, 0, WIFI_CMD_MAX_SIZE);
            OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), "%s rxch 0011", IF_HIPRIV);
            ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_NORM_SET, wl_cmd);
            HIPRIV_BEEN_DONE(ret, "%s", wl_cmd);
        }
    }

    g_wlan_at_data.wifiMode = mode;
    return ret;
}

/*****************************************************************************
 ��������  : int32 WlanATGetWifiMode(WLAN_AT_BUFFER_STRU *strBuf)
 ��������  : ��ȡ��ǰWiFi֧�ֵ���ʽ
             ��ǰģʽ�����ַ�����ʽ����eg: 2
 �������  : NA
 �������  : NA
 �� �� ֵ  : NA
 ����˵��  :
*****************************************************************************/
STATIC int32 ATGetWifiMode(WLAN_AT_BUFFER_STRU *strBuf)
{
    if (NULL == strBuf)
    {
        return (AT_RETURN_FAILURE);
    }

    OSA_SNPRINTF(strBuf->buf, WLAN_AT_BUFFER_SIZE, "%d", g_wlan_at_data.wifiMode);
    return (AT_RETURN_SUCCESS);
}

/*****************************************************************************
 ��������  : int32 WlanATGetWifiModeSupport(WLAN_AT_BUFFER_STRU *strBuf)
 ��������  : ��ȡWiFiоƬ֧�ֵ�����Э��ģʽ
             ֧�ֵ�����ģʽ�����ַ�����ʽ����eg: 2,3,4
 �������  : NA
 �������  : NA
 �� �� ֵ  : 0 �ɹ�
             1 ʧ��
 ����˵��  :
*****************************************************************************/
STATIC int32 ATGetWifiModeSupport(WLAN_AT_BUFFER_STRU *strBuf)
{
    if (NULL == strBuf)
    {
        return (AT_RETURN_FAILURE);
    }
    OSA_SNPRINTF(strBuf->buf, WLAN_AT_BUFFER_SIZE, "%s", WLAN_AT_MODE_SUPPORT);
    return (AT_RETURN_SUCCESS);
}

//////////////////////////////////////////////////////////////////////////
/*(3)^WIBAND ����WiFi������� */
//////////////////////////////////////////////////////////////////////////
/*****************************************************************************
 ��������  : int32 WlanATSetWifiBand(WLAN_AT_WIBAND_TYPE width)
 ��������  : ��������wifi����
 �������  : 0 20M
             1 40M
 �������  : NA
 �� �� ֵ  : 0 �ɹ�
             1 ʧ��
 ����˵��  : ֻ����nģʽ�²ſ�������40M����
*****************************************************************************/
STATIC int32 ATSetWifiBand(WLAN_AT_WIBAND_TYPE band)
{
    int32 ret = AT_RETURN_SUCCESS;
    int8 wl_cmd[WIFI_CMD_MAX_SIZE] = {0};

    WLAN_TRACE_INFO("%s:enter\n", __FUNCTION__);
    WLAN_TRACE_INFO("WLAN_AT_WIBAND_TYPE:%d\n", band);


    switch(band)
    {
        case AT_WIBAND_20M:
        {
            g_wlan_at_data.wifiBandwith = AT_WIBAND_20M;
            break;
        }
        case AT_WIBAND_40M:
        {
            if((AT_WIMODE_80211n == g_wlan_at_data.wifiMode ) || (AT_WIMODE_80211ac == g_wlan_at_data.wifiMode ))
            {
                g_wlan_at_data.wifiBandwith = AT_WIBAND_40M;
            }
            else
            {
                WLAN_TRACE_ERROR("Error wifi mode for bandwidth 40M,must in n or ac mode\n");
                ret = AT_RETURN_FAILURE;
            }
            break;
        }
        case AT_WIBAND_80M:
        {
            if(AT_WIMODE_80211ac == g_wlan_at_data.wifiMode )
            {
                g_wlan_at_data.wifiBandwith = AT_WIBAND_80M;
            }
            else
            {
                WLAN_TRACE_ERROR("Error wifi mode for bandwidth 80M,must in ac mode\n");
                ret = AT_RETURN_FAILURE;
            }
            break;
        }
        default:
            ret = AT_RETURN_FAILURE;
            break;
    }


    /*����ģʽ*/
    memset(wl_cmd, 0, WIFI_CMD_MAX_SIZE);
    if(AT_WIFREQ_24G == g_wlan_at_data.wifiBand)
    {
        OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), " %s mode %s", IF_HIPRIV, auc_mode_table_2G[g_wlan_at_data.wifiMode][g_wlan_at_data.wifiBandwith]);
    }
    else
    {
        OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), " %s mode %s", IF_HIPRIV, auc_mode_table_5G[g_wlan_at_data.wifiMode][g_wlan_at_data.wifiBandwith]);
    }
    ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_NORM_SET, wl_cmd);
    HIPRIV_BEEN_DONE(ret, "%s", wl_cmd);

    return ret;
}

/*****************************************************************************
 ��������  : int32 WlanATGetWifiBand(WLAN_AT_BUFFER_STRU *strBuf)
 ��������  :��ȡ��ǰ��������
            ��ǰ�������ַ�����ʽ����eg: 0
 �������  : NA
 �������  : NA
 �� �� ֵ  : 0 �ɹ�
             1 ʧ��
 ����˵��  :
*****************************************************************************/
STATIC int32 ATGetWifiBand(WLAN_AT_BUFFER_STRU *strBuf)
{
    if (NULL == strBuf)
    {
        return (AT_RETURN_FAILURE);
    }

    OSA_SNPRINTF(strBuf->buf, WLAN_AT_BUFFER_SIZE, "%d", g_wlan_at_data.wifiBandwith);
    return (AT_RETURN_SUCCESS);
}

/*****************************************************************************
 ��������  : int32 WlanATGetWifiBandSupport(WLAN_AT_BUFFER_STRU *strBuf)
 ��������  :��ȡWiFi֧�ֵĴ�������
            ֧�ִ������ַ�����ʽ����eg: 0,1
 �������  : NA
 �������  : NA
 �� �� ֵ  : 0 �ɹ�
             1 ʧ��
 ����˵��  :
*****************************************************************************/
STATIC int32 ATGetWifiBandSupport(WLAN_AT_BUFFER_STRU *strBuf)
{
    if (NULL == strBuf)
    {
        return (AT_RETURN_FAILURE);
    }

    OSA_SNPRINTF(strBuf->buf, WLAN_AT_BUFFER_SIZE, "%s", WLAN_AT_BAND_SUPPORT);
    return (AT_RETURN_SUCCESS);
}

//////////////////////////////////////////////////////////////////////////
/*(4)^WIFREQ ����WiFiƵ�� */
//////////////////////////////////////////////////////////////////////////
/*****************************************************************************
 ��������  : int32 WlanATSetWifiFreq(WLAN_AT_WIFREQ_STRU *pFreq)
 ��������  : ����WiFiƵ��
 �������  : NA
 �������  : NA
 �� �� ֵ  : 0 �ɹ�
             1 ʧ��
 ����˵��  :
*****************************************************************************/
STATIC int32 ATSetWifiFreq(WLAN_AT_WIFREQ_STRU *pFreq)
{
    /*2.4GƵ�㼯��*/
    const uint16   ausChannels[] = {2412,2417,2422,2427,2432,2437,2442,2447,2452,2457,2462,2467,2472};/*2.4G*/

/*BCM4354оƬ Ӧװ����һ��Ҫ�󣬲��õ�ǰ�����µĵ�һ��20MƵ���ʾ��ǰ�ŵ���Ƶ��*/
    /*5G 20MƵ�㼯��*/
    const uint16 aulChannel036[] = {5180,5200,5220,5240,5260,5280,5300,5320};/*w52��w53*/
    const uint16 aulChannel100[] = {5500,5520,5540,5560,5580,5600,5620,5640,5660,5680,5700,5720};/*w56*/
    const uint16 aulChannel149[] = {5745,5765,5785,5805,5825};/*w57*/

    /*2.4G 40MƵ�㼯��,��Ӧװ���ŵ�3��11*/
                                      /* 1+,   2+,   3+,   4+,   5+,   6+,   7+,   8+,   9+ */
    const uint16   ausChannels_40M[] = {2422, 2427, 2432, 2437, 2442, 2447, 2452, 2457, 2462};/*2.4G 40M*/
    /*5G 40MƵ�㼯��*/
    const uint16 aulChannel036_40M[] = {5180, 5220, 5260, 5300};/*5G 40M*/
    const uint16 aulChannel100_40M[] = {5500, 5540, 5580, 5620, 5660,5700};/*5G 40M*/
    const uint16 aulChannel149_40M[] = {5745, 5785};/*5G 40M*/
    /*5G 80MƵ�㼯��*/
    const uint16 aulChannel036_80M[] = {5180, 5260};/*5G 80M*/
    const uint16 aulChannel100_80M[] = {5500, 5580,5660};/*5G 80M*/
    const uint16 aulChannel149_80M[] = {5745};/*5G 80M*/

    char ul_for_40M = '\0';//��ʼ��Ϊ0
    uint16 ulWifiFreq = 0;
    uint16 i = 0;
    int32 ret = AT_RETURN_SUCCESS;
    int8 wl_cmd[WIFI_CMD_MAX_SIZE] = {0};

    if (NULL == pFreq)
    {
        return (AT_RETURN_FAILURE);
    }

    /*20M����*/
    if (AT_WIBAND_20M == g_wlan_at_data.wifiBandwith)
    {
        if (pFreq->value <= WLAN_FREQ_2G_MAX)
        {
            for (i = 0; i < (sizeof(ausChannels) / sizeof(uint16)); i++)
            {
                if (pFreq->value == ausChannels[i])
                {
                    ulWifiFreq = (i + 1);
                    break;
                }
            }
        }
        /*WIFI 5G �ŵ��㷨���£�Ƶ�����������ŵ��ţ������Ӧ��Ӧ��Ϊ

        case 36:
            iWIFIchannel = 5180;
            break;
        case 40:
            iWIFIchannel = 5200;
            break;
        case 44:
            iWIFIchannel = 5220;
            break;
        case 48:
            iWIFIchannel = 5240;
            break;
        case 52:
            iWIFIchannel = 5260;
            break;
        case 56:
            iWIFIchannel = 5280;
            break;
        case 60:
            iWIFIchannel = 5300;
            break;
        case 64:
            iWIFIchannel = 5320;
            break;
        */
        else if ((pFreq->value >= WLAN_FREQ_5G_W52_MIN) && (pFreq->value <= WLAN_FREQ_5G_W53_MAX))
        {
            for (i = 0; i < (sizeof(aulChannel036) / sizeof(uint16)); i++)
            {
                if (pFreq->value == aulChannel036[i])
                {
                    ulWifiFreq = (i * WLAN_CHANNEL_5G_INTERVAL + WLAN_CHANNEL_5G_MIN);
                    break;
                }
            }
        }
        /*WIFI 5G �ŵ��㷨���£�Ƶ�����������ŵ��ţ������Ӧ��Ӧ��Ϊ

        case 100:
            iWIFIchannel = 5500;
            break;
        case 104:
            iWIFIchannel = 5520;
            break;
        case 108:
            iWIFIchannel = 5540;
            break;
        case 112:
            iWIFIchannel = 5560;
            break;
        case 116:
            iWIFIchannel = 5580;
            break;
        case 120:
            iWIFIchannel = 5600;
            break;
        case 124:
            iWIFIchannel = 5620;
            break;
        case 128:
            iWIFIchannel = 5640;
            break;
        case 132:
            iWIFIchannel = 5660;
            break;
        case 136:
            iWIFIchannel = 5680;
            break;
        case 140:
            iWIFIchannel = 5700;
            break;
        */
        else if ((pFreq->value >= WLAN_FREQ_5G_W56_MIN) && (pFreq->value <= WLAN_FREQ_5G_W56_MAX))
        {
            for (i = 0; i < (sizeof(aulChannel100) / sizeof(uint16)); i++)
            {
                if (pFreq->value == aulChannel100[i])
                {
                    ulWifiFreq = (i * WLAN_CHANNEL_5G_INTERVAL + HUNDRED);
                    break;
                }
            }

        }
        /*WIFI 5G �ŵ��㷨���£�Ƶ�����������ŵ��ţ������Ӧ��Ӧ��Ϊ
        case 149:
            iWIFIchannel = 5745;
            break;
        case 153:
            iWIFIchannel = 5765;
            break;
        case 157:
            iWIFIchannel = 5785;
            break;
        case 161:
            iWIFIchannel = 5805;
            break;
        case 165:
            iWIFIchannel = 5825;
            break;
        */
        else if ((pFreq->value >= WLAN_FREQ_5G_W57_MIN) && (pFreq->value <= WLAN_FREQ_5G_W57_MAX))
        {
            for (i = 0; i < (sizeof(aulChannel149) / sizeof(uint16)); i++)
            {
                if (pFreq->value == aulChannel149[i])
                {
                    ulWifiFreq = (i * WLAN_CHANNEL_5G_INTERVAL + WLAN_CHANNEL_5G_W57_START);
                    break;
                }
            }
        }
        else
        {
            WLAN_TRACE_INFO("Error 20M wifiFreq parameters\n");
            return AT_RETURN_FAILURE;
        }

        WLAN_TRACE_INFO("Target Channel = %d\n", ulWifiFreq);

        if (!(((WLAN_CHANNEL_2G_MIN <= ulWifiFreq) && (WLAN_CHANNEL_2G_MAX >= ulWifiFreq))
            || ((WLAN_CHANNEL_5G_MIN <= ulWifiFreq) && (WLAN_CHANNEL_5G_MAX >= ulWifiFreq))))
        {
            WLAN_TRACE_INFO("Target Channel ERROR,ulWifiFreq = %u!\n", ulWifiFreq);
            return AT_RETURN_FAILURE;
        }

    }
    else if(AT_WIBAND_40M == g_wlan_at_data.wifiBandwith)
    {
        if (pFreq->value <= WLAN_FREQ_2G_MAX)
        {
            for (i = 0; i < (sizeof(ausChannels_40M) / sizeof(uint16)); i++)
            {
                if (pFreq->value == ausChannels_40M[i])
                {
                    ulWifiFreq = (i + 1);
                    break;
                }
            }
        }
        else if ((pFreq->value >= WLAN_FREQ_5G_W52_40M_MIN) && (pFreq->value <= WLAN_FREQ_5G_W53_40M_MAX))
        {
            for (i = 0; i < (sizeof(aulChannel036_40M) / sizeof(uint16)); i++)
            {
                if (pFreq->value == aulChannel036_40M[i])
                {
                    ulWifiFreq = (i * WLAN_CHANNEL_5G_40M_INTERVAL + WLAN_CHANNEL_5G_MIN);
                    break;
                }
            }
        }
        else if ((pFreq->value >= WLAN_FREQ_5G_W56_40M_MIN) && (pFreq->value <= WLAN_FREQ_5G_W56_40M_MAX))
        {
            for (i = 0; i < (sizeof(aulChannel100_40M) / sizeof(uint16)); i++)
            {
                if (pFreq->value == aulChannel100_40M[i])
                {
                    ulWifiFreq = (i * WLAN_CHANNEL_5G_40M_INTERVAL + HUNDRED);
                    break;
                }
            }

        }
        else if ((pFreq->value >= WLAN_FREQ_5G_W57_40M_MIN) && (pFreq->value <= WLAN_FREQ_5G_W57_40M_MAX))
        {
            for (i = 0; i < (sizeof(aulChannel149_40M) / sizeof(uint16)); i++)
            {
                if (pFreq->value == aulChannel149_40M[i])
                {
                    ulWifiFreq = (i * WLAN_CHANNEL_5G_40M_INTERVAL + WLAN_CHANNEL_5G_W57_START);
                    break;
                }
            }
        }
        else
        {
            WLAN_TRACE_INFO("Error 40M wifiFreq parameters\n");
            return AT_RETURN_FAILURE;
        }

        WLAN_TRACE_INFO("Target Channel = %d\n", ulWifiFreq);
        if (!(((WLAN_CHANNEL_2G_MIN <= ulWifiFreq) && (WLAN_CHANNEL_2G_MAX >= ulWifiFreq))
            || ((WLAN_CHANNEL_5G_MIN <= ulWifiFreq) && (WLAN_CHANNEL_5G_MAX >= ulWifiFreq))))
        {
            WLAN_TRACE_INFO("Target Channel ERROR!\n");
            return AT_RETURN_FAILURE;
        }
    }
    else if(AT_WIBAND_80M == g_wlan_at_data.wifiBandwith)
    {
        if ((pFreq->value >= WLAN_FREQ_5G_W52_80M_MIN) && (pFreq->value <= WLAN_FREQ_5G_W53_80M_MAX))
        {
            for (i = 0; i < (sizeof(aulChannel036_80M) / sizeof(uint16)); i++)
            {
                if (pFreq->value == aulChannel036_80M[i])
                {
                    ulWifiFreq = (i * WLAN_CHANNEL_5G_80M_INTERVAL + WLAN_CHANNEL_5G_MIN);
                    break;
                }
            }
        }
        else if ((pFreq->value >= WLAN_FREQ_5G_W56_80M_MIN) && (pFreq->value <= WLAN_FREQ_5G_W56_80M_MAX))
        {
            for (i = 0; i < (sizeof(aulChannel100_80M) / sizeof(uint16)); i++)
            {
                if (pFreq->value == aulChannel100_80M[i])
                {
                    ulWifiFreq = (i * WLAN_CHANNEL_5G_80M_INTERVAL + HUNDRED);
                    break;
                }
            }
        }
        else if ((pFreq->value >= WLAN_FREQ_5G_W57_80M_MIN) && (pFreq->value <= WLAN_FREQ_5G_W57_80M_MAX))
        {
            for (i = 0; i < (sizeof(aulChannel149_80M) / sizeof(uint16)); i++)
            {
                if (pFreq->value == aulChannel149_80M[i])
                {
                    ulWifiFreq = (i * WLAN_CHANNEL_5G_80M_INTERVAL + WLAN_CHANNEL_5G_W57_START);
                    break;
                }
            }
        }
        else
        {
            WLAN_TRACE_ERROR("Error 80M wifiFreq parameters %u!\n",pFreq->value);
            return AT_RETURN_FAILURE;
        }
        WLAN_TRACE_INFO("Target Channel = %d\n", ulWifiFreq);
        if (!(WLAN_CHANNEL_5G_MIN <= ulWifiFreq) && (WLAN_CHANNEL_5G_MAX >= ulWifiFreq))
        {
            WLAN_TRACE_INFO("Target Channel ERROR,ulWifiFreq = %u!\n", ulWifiFreq);
            return AT_RETURN_FAILURE;
        }

    }

    /*�����ŵ�*/
    memset(wl_cmd, 0, WIFI_CMD_MAX_SIZE);
    OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), " %s freq %d", IF_HIPRIV, ulWifiFreq);
    ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_NORM_SET, wl_cmd);
    HIPRIV_BEEN_DONE(ret, "%s", wl_cmd);

    /* ����ȫ�ֱ�����Ա���ѯ */
    g_wlan_at_data.wifiFreq.value = pFreq->value;
    g_wlan_at_data.wifiFreq.offset = pFreq->offset;
    g_wlan_at_data.wifiChannel = ulWifiFreq;

    return ret;
}

/*****************************************************************************
 ��������  : int32 WlanATGetWifiFreq(WLAN_AT_WIFREQ_STRU *pFreq)
 ��������  : ��ȡWiFiƵ��
 �������  : NA
 �������  : NA
 �� �� ֵ  : 0 �ɹ�
             1 ʧ��
 ����˵��  :
*****************************************************************************/
STATIC int32 ATGetWifiFreq(WLAN_AT_WIFREQ_STRU *pFreq)
{
    if (NULL == pFreq)
    {
        return (AT_RETURN_FAILURE);
    }

    memcpy(pFreq, &(g_wlan_at_data.wifiFreq), sizeof(WLAN_AT_WIFREQ_STRU));
    return (AT_RETURN_SUCCESS);
}

//////////////////////////////////////////////////////////////////////////
/*(5)^WIDATARATE ���úͲ�ѯ��ǰWiFi���ʼ�����
  WiFi���ʣ���λΪ0.01Mb/s��ȡֵ��ΧΪ0��65535 */
//////////////////////////////////////////////////////////////////////////
/*****************************************************************************
 ��������  : int32 WlanATSetWifiDataRate(uint32 rate)
 ��������  : ����WiFi��������
 �������  : NA
 �������  : NA
 �� �� ֵ  : 0 �ɹ�
             1 ʧ��
 ����˵��  :
*****************************************************************************/
STATIC int32 ATSetWifiDataRate(uint32 rate)
{
    int32 ret = 0;
    uint32  ulNRate = 0;
    uint32  ulacSS = 1; /* 11ac spatial stream,1--SISO, 2--MIMO*/
    int8 wl_cmd[WIFI_CMD_MAX_SIZE] = {0};

    int8 *wifi_bw[] = {"20", "40", "80"};
    /*BGģʽ��ֱ�Ӵ�rate��������˲���Ҫ����ת��*/
    const BRATE_ST wifi_brates_table[] = {{100, "1"}, {200, "2"}, {550, "5.5"}, {1100, "11"}};

    /* WIFI n ��ac ģʽ AT^WIDATARATE���õ�����ֵ��WL��������ֵ�Ķ�Ӧ�� */
    const uint32 wifi_20m_nrates_table[] = {650, 1300, 1950, 2600, 3900, 5200, 5850, 6500, \
                                                                1300, 2600, 3900, 5200, 7800, 10400, 11700, 13000};
    const uint32 wifi_40m_nrates_table[] =  {1350, 2700, 4050, 5400, 8100, 10800, 12150, 13500, \
                                                                 2700, 5400, 8100, 10800, 16200, 21600, 24300, 27000};
    const uint32 wifi_20m_acrates_table[] = {650, 1300, 1950, 2600, 3900, 5200, 5850, 6500, 7800,\
                                                                 1300, 2600, 3900, 5200, 7800, 10400, 11700, 13000,15600};
    const uint32 wifi_40m_acrates_table[] = {650, 1300, 1950, 2600, 3900, 5200, 5850, 6500, 16200, 18000,\
                                                                 1300, 2600, 3900, 5200, 7800, 10400, 11700, 13000,32400,36000};
    const uint32 wifi_80m_acrates_table[] = {650, 1300, 1950, 2600, 3900, 5200, 5850, 6500, 35100, 39000,\
                                                                 1300, 2600, 3900, 5200, 7800, 10400, 11700, 13000,70200,78000};

    //#define WIFI_BRATES_TABLE_SIZE (sizeof(wifi_brates_table) / sizeof(BRATE_ST))
    #define WIFI_20M_NRATES_TABLE_SIZE (sizeof(wifi_20m_nrates_table) / sizeof(uint32))
    #define WIFI_40M_NRATES_TABLE_SIZE (sizeof(wifi_40m_nrates_table) / sizeof(uint32))

   #define WIFI_20M_ACRATES_TABLE_SIZE (sizeof(wifi_20m_acrates_table) / sizeof(uint32))
   #define WIFI_40M_ACRATES_TABLE_SIZE (sizeof(wifi_40m_acrates_table) / sizeof(uint32))
   #define WIFI_80M_ACRATES_TABLE_SIZE (sizeof(wifi_80m_acrates_table) / sizeof(uint32))

   WLAN_TRACE_INFO("WifiRate = %u\n", rate / HUNDRED);

    /*���ô���*/
    memset(wl_cmd, 0, WIFI_CMD_MAX_SIZE);
    OSA_SNPRINTF(wl_cmd,  sizeof(wl_cmd), " %s bw %s", IF_HIPRIV, wifi_bw[g_wlan_at_data.wifiBandwith]);
    ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_NORM_SET, wl_cmd);
    HIPRIV_BEEN_DONE(ret, "%s", wl_cmd);

    /*��������*/
    switch (g_wlan_at_data.wifiMode)
    {
        case AT_WIMODE_CW:
            WLAN_TRACE_INFO("AT_WIMODE_CW\n");
            return (AT_RETURN_FAILURE);
        case AT_WIMODE_80211b:
            for (ulNRate = 0; ulNRate < WIFI_20M_NRATES_TABLE_SIZE; ulNRate++)
            {
                if (wifi_brates_table[ulNRate].brate == rate)
                    {
                    WLAN_TRACE_INFO("20M NRate Index = %u\n", ulNRate);
                    break;
                }
            }

            if (WIFI_20M_NRATES_TABLE_SIZE == ulNRate)
            {
                WLAN_TRACE_ERROR("20M NRate Error!\n");
                return (AT_RETURN_FAILURE);
            }

            memset(wl_cmd, 0, WIFI_CMD_MAX_SIZE);
            OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), " %s rate %s", IF_HIPRIV, wifi_brates_table[ulNRate].rate_str);
            ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_NORM_SET, wl_cmd);
            HIPRIV_BEEN_DONE(ret, "%s", wl_cmd);
            break;
        case AT_WIMODE_80211a:
        case AT_WIMODE_80211g:
            /*�����ڲ���11ag������ֵ����У��*/
            memset(wl_cmd, 0, WIFI_CMD_MAX_SIZE);
            OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), " %s rate %d", IF_HIPRIV, rate / HUNDRED);
            ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_NORM_SET, wl_cmd);
            HIPRIV_BEEN_DONE(ret, "%s", wl_cmd);
            break;
        case AT_WIMODE_80211n:
            if (AT_WIBAND_20M == g_wlan_at_data.wifiBandwith)
            {
                /* WIFI 20M nģʽWL���������ֵΪ0~15����16�� */
                for (ulNRate = 0; ulNRate < WIFI_20M_NRATES_TABLE_SIZE; ulNRate++)
                {
                    if (wifi_20m_nrates_table[ulNRate] == rate)
                    {
                        if(((RF_2G_MIMO == g_wlan_at_data.wifiGroup) || (RF_5G_MIMO == g_wlan_at_data.wifiGroup))
                            && (ulNRate < (WIFI_20M_NRATES_TABLE_SIZE/2)))
                        {
                            //1300, 2600, 3900, 5200
                        }
                        else
                        {
                            WLAN_TRACE_INFO("20M NRate Index = %u\n", ulNRate);
                            break;
                        }
                    }
                }

                if (WIFI_20M_NRATES_TABLE_SIZE == ulNRate)
                {
                    WLAN_TRACE_ERROR("20M NRate Error!\n");
                    return (AT_RETURN_FAILURE);
                }
            }
            else if (AT_WIBAND_40M == g_wlan_at_data.wifiBandwith)
            {
                for (ulNRate = 0; ulNRate < WIFI_40M_NRATES_TABLE_SIZE; ulNRate++)
                {
                    if (wifi_40m_nrates_table[ulNRate] == rate)
                    {
                        if(((RF_2G_MIMO == g_wlan_at_data.wifiGroup) || (RF_5G_MIMO == g_wlan_at_data.wifiGroup))
                            && (ulNRate < (WIFI_20M_NRATES_TABLE_SIZE/2)))
                        {
                            //2700, 5400, 8100, 10800
                        }
                        else
                        {
                            WLAN_TRACE_INFO("40M NRate Index = %u\n", ulNRate);
                            break;
                        }
                    }
                }

                if (WIFI_40M_NRATES_TABLE_SIZE == ulNRate)
                {
                    WLAN_TRACE_ERROR("40M NRate Error!\n");
                    return (AT_RETURN_FAILURE);
                }
            }
            memset(wl_cmd, 0, WIFI_CMD_MAX_SIZE);
            OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), " %s mcs %d", IF_HIPRIV, ulNRate);
            ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_NORM_SET, wl_cmd);
            HIPRIV_BEEN_DONE(ret, "%s", wl_cmd);
            break;
        case AT_WIMODE_80211ac:
            if (AT_WIBAND_20M == g_wlan_at_data.wifiBandwith)
            {
                for (ulNRate = 0; ulNRate < WIFI_20M_ACRATES_TABLE_SIZE; ulNRate++)
                {
                    if (wifi_20m_acrates_table[ulNRate] == rate)
                    {
                        if(ulNRate < (WIFI_20M_ACRATES_TABLE_SIZE/2))
                        {
                            if(g_wlan_at_data.wifiGroup < WLAN_AT_GROUP_MAX)
                            {
                                ulacSS = 1;
                                WLAN_TRACE_INFO("20M ACRate Index = %u\n", ulNRate);
                                break;
                            }
                        }
                        else
                        {
                            ulacSS = 2;
                            ulNRate = ulNRate - WIFI_20M_ACRATES_TABLE_SIZE/2;
                            WLAN_TRACE_INFO("20M ACRate Index = %u\n", ulNRate);
                            break;
                        }
                    }
                }
                if (WIFI_20M_ACRATES_TABLE_SIZE == ulNRate)
                {
                    WLAN_TRACE_ERROR("20M ACRate Error!\n");
                    return (AT_RETURN_FAILURE);
                }
            }
            else if (AT_WIBAND_40M == g_wlan_at_data.wifiBandwith)
            {
                for (ulNRate = 0; ulNRate < WIFI_40M_ACRATES_TABLE_SIZE; ulNRate++)
                {
                    if (wifi_40m_acrates_table[ulNRate] == rate)
                    {
                        if(ulNRate < (WIFI_40M_ACRATES_TABLE_SIZE/2))
                        {
                            if(g_wlan_at_data.wifiGroup < WLAN_AT_GROUP_MAX)
                            {
                                ulacSS = 1;
                                WLAN_TRACE_INFO("40M ACRate Index = %u\n", ulNRate);
                                break;
                            }
                        }
                        else
                        {
                            ulacSS = 2;
                            ulNRate = ulNRate - WIFI_40M_ACRATES_TABLE_SIZE/2;
                            WLAN_TRACE_INFO("40M ACRate Index = %u\n", ulNRate);
                            break;
                        }
                    }
                }
                if (WIFI_40M_ACRATES_TABLE_SIZE == ulNRate)
                {
                    WLAN_TRACE_ERROR("40M ACRate Error!\n");
                    return (AT_RETURN_FAILURE);
                }
            }
            else if(AT_WIBAND_80M == g_wlan_at_data.wifiBandwith)
            {
                for (ulNRate = 0; ulNRate < WIFI_80M_ACRATES_TABLE_SIZE; ulNRate++)
                {
                    if (wifi_80m_acrates_table[ulNRate] == rate)
                    {
                        if(ulNRate < (WIFI_80M_ACRATES_TABLE_SIZE/2))
                        {
                            if(g_wlan_at_data.wifiGroup < WLAN_AT_GROUP_MAX)
                            {
                                ulacSS = 1;
                                WLAN_TRACE_INFO("80M ACRate Index = %u\n", ulNRate);
                                break;
                            }
                        }
                        else
                        {
                            ulacSS = 2;
                            ulNRate = ulNRate - WIFI_80M_ACRATES_TABLE_SIZE/2;
                            WLAN_TRACE_INFO("80M ACRate Index = %u\n", ulNRate);
                            break;
                        }
                    }
                }
                if (WIFI_80M_ACRATES_TABLE_SIZE == ulNRate)
                {
                    WLAN_TRACE_ERROR("80M ACRate Error! rate=%d\n", rate);
                    return (AT_RETURN_FAILURE);
                }
            }
            memset(wl_cmd, 0, WIFI_CMD_MAX_SIZE);
            OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), " %s mcsac %d", IF_HIPRIV, ulNRate);
            ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_NORM_SET, wl_cmd);
            HIPRIV_BEEN_DONE(ret, "%s", wl_cmd);

            //if(ulacSS > 1)
            //{
                memset(wl_cmd, 0, WIFI_CMD_MAX_SIZE);
                OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), " %s nss %d", IF_HIPRIV, ulacSS);
                ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_NORM_SET, wl_cmd);
                HIPRIV_BEEN_DONE(ret, "%s", wl_cmd);
            //}

            break;
        default:
            return (AT_RETURN_FAILURE);
    }

    /*����ȫ�ֱ�����Ա���ѯ*/
    g_wlan_at_data.wifiRate = rate;

    return ret;
}
/*****************************************************************************
 ��������  : uint32 WlanATGetWifiDataRate()
 ��������  : ��ѯ��ǰWiFi��������
 �������  : NA
 �������  : NA
 �� �� ֵ  : wifi����
 ����˵��  :
*****************************************************************************/
STATIC uint32 ATGetWifiDataRate(void)
{
    return g_wlan_at_data.wifiRate;
}

//////////////////////////////////////////////////////////////////////////
/*(6)^WIPOW ������WiFi���书��
   WiFi���书�ʣ���λΪ0.01dBm��ȡֵ��ΧΪ -32768��32767 */
//////////////////////////////////////////////////////////////////////////
/*****************************************************************************
 ��������  : int32 WlanATSetWifiPOW(int32 power_dBm_percent)
 ��������  : ����WiFi���书��
 �������  : NA
 �������  : NA
 �� �� ֵ  : 0 �ɹ�
             1 ʧ��
 ����˵��  :
*****************************************************************************/
STATIC int32 ATSetWifiPOW(int32 power_dBm_percent)
{
    int ret = AT_RETURN_SUCCESS;
    int32 lWifiPower = power_dBm_percent / HUNDRED;
    int8 wl_cmd[WIFI_CMD_MAX_SIZE] = {0};

    if ( WLAN_AT_POW_DEFAULT == lWifiPower )
    {
        return (AT_RETURN_SUCCESS);
    }

    if ((lWifiPower >= WLAN_AT_POWER_MIN) && (lWifiPower <= WLAN_AT_POWER_MAX))
    {
        memset(wl_cmd, 0, WIFI_CMD_MAX_SIZE);
        OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), " %s txpower %d", IF_HIPRIV, lWifiPower);
        ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_NORM_SET, wl_cmd);
        HIPRIV_BEEN_DONE(ret, "%s", wl_cmd);
    }
    else
    {
        WLAN_TRACE_INFO("Invalid argument for WifiPOW\n");
        ret = AT_RETURN_FAILURE;
    }

    if ( AT_RETURN_SUCCESS ==  ret)
    {
         /*����ȫ�ֱ�����Ա���ѯ*/
        g_wlan_at_data.wifiPower = power_dBm_percent;
    }

    return ret;
}

/*****************************************************************************
 ��������  : int32 WlanATGetWifiPOW()
 ��������  : ��ȡWiFi��ǰ���书��
 �������  : NA
 �������  : NA
 �� �� ֵ  : NA
 ����˵��  :
*****************************************************************************/
STATIC int32 ATGetWifiPOW(void)
{
    return g_wlan_at_data.wifiPower;
}

/*****************************************************************************
 ��������  : int trigger_dync_adjust()
 ��������  : ������̬����У׼
 �������  : NA
 �������  : NA
 �� �� ֵ  : 0 �ɹ�
             1 ʧ��
 ����˵��  :
 *****************************************************************************/
STATIC int trigger_dync_adjust(void)
{
    int ret = 0;
    int8 wl_cmd[WIFI_CMD_MAX_SIZE] = {0};

    (void)OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), "Hisilicon0 dync_txpower 1");
    ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_NORM_SET, wl_cmd);
    HIPRIV_BEEN_DONE(ret, "%s", wl_cmd);

    msleep(DYNC_ADJUST_DELAY);
    (void)memset(wl_cmd, 0, WIFI_CMD_MAX_SIZE);
    (void)OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), "Hisilicon0 dync_txpower 0");
    ret += WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_NORM_SET, wl_cmd);
    HIPRIV_BEEN_DONE(ret, "%s", wl_cmd);

    return ret;
}

//////////////////////////////////////////////////////////////////////////
/*(7)^WITX ������WiFi��������� */
//////////////////////////////////////////////////////////////////////////
/*****************************************************************************
 ��������  : int32 WlanATSetWifiTX(WLAN_AT_FEATURE_TYPE onoff)
 ��������  : �򿪻�ر�wifi�����
 �������  : 0 �ر�
             1 ��
 �������  : NA
 �� �� ֵ  : 0 �ɹ�
             1 ʧ��
 ����˵��  :
*****************************************************************************/
STATIC int32 ATSetWifiTX(WLAN_AT_FEATURE_TYPE onoff)
{
    int ret = 0;
    int i = 0;
    int8 wl_cmd[WIFI_CMD_MAX_SIZE] = {0};

    if (onoff > AT_FEATURE_ENABLE)
    {
        WLAN_TRACE_ERROR("ATSetWifiTX:%d unknow!\n", ATSetWifiTX);
        return (AT_RETURN_FAILURE);
    }

    /* ����ȫ�ֱ�����ѱ���ѯ */
    g_wlan_at_data.wifiTX = onoff;

    if (AT_WIMODE_CW == g_wlan_at_data.wifiMode)
    {
        if (AT_FEATURE_DISABLE == onoff)
        {
            OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), " %s cfg_cw_signal %d", IF_HIPRIV, onoff);
            ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_NORM_SET, wl_cmd);
            HIPRIV_BEEN_DONE(ret, "%s", wl_cmd);
        }
        else
        {
            OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), " %s cfg_cw_signal %d", IF_HIPRIV, (g_wlan_at_data.wifiGroup % 2 + 1));
            ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_NORM_SET, wl_cmd);
            HIPRIV_BEEN_DONE(ret, "%s", wl_cmd);
        }

        return ret;
    }

    if (AT_FEATURE_ENABLE == onoff)
    {
        OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), " %s add_user 07:06:05:04:03:02 1", IF_HIPRIV);
        ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_NORM_SET, wl_cmd);
        HIPRIV_BEEN_DONE(ret, "%s", wl_cmd);
    }

    OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), " %s al_tx_51 %d", IF_HIPRIV, onoff);
    ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_NORM_SET, wl_cmd);
    HIPRIV_BEEN_DONE(ret, "%s", wl_cmd);

    if (AT_FEATURE_ENABLE == onoff)
    {
        WIFI_VAP_INFO(g_wlan_at_data_old);
        WIFI_VAP_INFO(g_wlan_at_data);

        /* ʹ�ܶ�̬У׼ */
        /* 1)��һ���л�����Ƶͨ��0ʱ��������2G 617��420 saw��5G(ANT1\ANT2)�ĵ�һ�������� */
        if ((g_wlan_at_data.wifiGroup != g_wlan_at_data_old.wifiGroup)
            && ((RF_2G_0 == g_wlan_at_data.wifiGroup) || (RF_5G_0 == g_wlan_at_data.wifiGroup) || (RF_5G_1 == g_wlan_at_data.wifiGroup)))
        {
            WLAN_TRACE_INFO("ATSetWifiTX::This is the first item!\n");
            g_wlan_at_data.wifiFirstTestItem = 1;
            ret = trigger_dync_adjust();
            if (0 != ret)
            {
                return ret;
            }
        }

        /* 2)����ǵ�һ�������������������ȫ��ͬ����Ϊװ�����ԣ�
                    ��Ҫ���¶�̬У׼��������ò�ͬ������������л� */
        else if (1 == g_wlan_at_data.wifiFirstTestItem)
        {
            ret = memcmp(&g_wlan_at_data_old, &g_wlan_at_data, sizeof(g_wlan_at_data));
            if(0 == ret)
            {
                WLAN_TRACE_INFO("ATSetWifiTX::This is the first item retry again!\n");
                ret = trigger_dync_adjust();
                if (0 != ret)
                {
                    return ret;
                }
            }
            else
            {
                WLAN_TRACE_INFO("ATSetWifiTX::This is to clean wifiFirstTestItem flag!\n");
                g_wlan_at_data.wifiFirstTestItem = 0;
            }
        }
        else
        {
            WLAN_TRACE_INFO("ATSetWifiTX::This is the no use branch!\n");
        }

        (void)memcpy(&g_wlan_at_data_old, &g_wlan_at_data, sizeof(g_wlan_at_data_old));
    }

    WLAN_TRACE_INFO("ATSetWifiTX::g_wlan_at_data.wifiFirstTestItem=%d!\n",
        g_wlan_at_data.wifiFirstTestItem);

    return (AT_RETURN_SUCCESS);
}
/*****************************************************************************
 ��������  : WLAN_AT_FEATURE_TYPE WlanATGetWifiTX()
 ��������  : ��ѯ��ǰWiFi�����״̬��Ϣ
 �������  : NA
 �������  : NA
 �� �� ֵ  : 0 �رշ����
             1 �򿪷����
 ����˵��  :
*****************************************************************************/
STATIC WLAN_AT_FEATURE_TYPE ATGetWifiTX(void)
{
    return g_wlan_at_data.wifiTX;
}

/*****************************************************************************
 ��������  : static uint8 charToData()
 ��������  : �ַ�ת����
 �������  : NA
 �������  : NA
 �� �� ֵ  :
 ����˵��  :
*****************************************************************************/
static uint8 charToData(const char ch)
{
    switch(ch)
    {
    case '0':
        return 0;
    case '1':
        return 1;
    case '2':
        return 2;
    case '3':
        return 3;
    case '4':
        return 4;
    case '5':
        return 5;
    case '6':
        return 6;
    case '7':
        return 7;
    case '8':
        return 8;
    case '9':
        return 9;
    case 'a':
    case 'A':
        return 10;
    case 'b':
    case 'B':
        return 11;
    case 'c':
    case 'C':
        return 12;
    case 'd':
    case 'D':
        return 13;
    case 'e':
    case 'E':
        return 14;
    case 'f':
    case 'F':
        return 15;
    }
    return 0;
}

/***************************************************************************
 ��������  : wl_ether_atoe()
 ��������  : ��ѯ��ǰWiFi�����״̬��Ϣ
 �������  : const char *a  ,struct ether_addr *n
 �������  : NA
 �� �� ֵ  : NA
 ����˵��  :

****************************************************************************/
static void wl_ether_atoe(const char *a, uint8* addr)
{
    char *c = NULL;
    int i = 0;
    int j = 0;
    if (NULL == a || NULL == addr || '\0' == *a)
    {
        WLAN_TRACE_ERROR("params error\n");
        return;
    }
    for (i = 0; i < MAC_ADDRESS_LEN; i++)
    {
        addr[i] = charToData(*a++) * 16;    //ʮ������תʮ����
        addr[i] += charToData(*a++);
        if ('\0' == *a)
        {
            //WLAN_TRACE_INFO("The End of MAC address!!\n");
            break;
        }
        a++; //����ð��
    }
    return ;
}


//////////////////////////////////////////////////////////////////////////
/*(8)^WIRX ����WiFi���ջ����� */
//////////////////////////////////////////////////////////////////////////
STATIC int32 ATSetWifiRX(WLAN_AT_WIRX_STRU *params)
{
    int ret = AT_RETURN_SUCCESS;
    int8 wl_cmd[WIFI_CMD_MAX_SIZE] = {0};
    uint8 addr[MAC_ADDRESS_LEN] = {0};

    if ((NULL == params) || (params->onoff > AT_FEATURE_ENABLE))
    {
        return (AT_RETURN_FAILURE);
    }

    /*����mac��ַ���û�*/
    if (0 != strncmp(params->src_mac,"",MAC_ADDRESS_LEN))
    {
        WLAN_TRACE_INFO("params->src_mac is %s\n",params->src_mac);
        wl_ether_atoe(params->src_mac, &addr);

        ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_VAP_DOWN_SET, "1");
        HIPRIV_BEEN_DONE(ret, "%s", "ifconfig vap0 down");
        ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_HW_ADDR_SET, addr);
        HIPRIV_BEEN_DONE(ret, "%s", "ifconfig vap0 hw ether");
        ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_VAP_UP_SET, "1");
        HIPRIV_BEEN_DONE(ret, "%s", "ifconfig vap0 up");

        if(0 == g_uc_add_user_done)
        {
            OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), " %s add_user 07:06:05:04:03:02 1", IF_HIPRIV);
            ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_NORM_SET, wl_cmd);
            HIPRIV_BEEN_DONE(ret, "%s", wl_cmd);
            g_uc_add_user_done++;
        }
    }
    else
    {
        WLAN_TRACE_ERROR("src mac is NULL\n");
    }

    /*����*/
    memset(wl_cmd, 0, WIFI_CMD_MAX_SIZE);
    OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), " %s al_rx_51 %d", IF_HIPRIV, params->onoff);
    ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_NORM_SET, wl_cmd);
    HIPRIV_BEEN_DONE(ret, "%s", wl_cmd);

    memcpy(&g_wlan_at_data.wifiRX, params, sizeof(WLAN_AT_WIRX_STRU));

    return (ret);
}

/*****************************************************************************
 ��������  : int32 WlanATGetWifiRX(WLAN_AT_WIRX_STRU *params)
 ��������  : ��ȡwifi���ջ���״̬
 �������  : NA
 �������  : NA
 �� �� ֵ  : NA
 ����˵��  :
*****************************************************************************/
STATIC int32 ATGetWifiRX(WLAN_AT_WIRX_STRU *params)
{
    if (NULL == params)
    {
        return (AT_RETURN_FAILURE);
    }

    memcpy(params, &g_wlan_at_data.wifiRX, sizeof(WLAN_AT_WIRX_STRU));

    return (AT_RETURN_SUCCESS);
}

//////////////////////////////////////////////////////////////////////////
/*(9)^WIRPCKG ��ѯWiFi���ջ�����룬�ϱ����յ��İ�������*/
//////////////////////////////////////////////////////////////////////////
/*****************************************************************************
 ��������  : int32 WlanATSetWifiRPCKG(int32 flag)
 ��������  : ���Wifi����ͳ�ư�Ϊ��
 �������  : 0 ���wifiͳ�ư�
             ��0 ��Ч����
 �������  : NA
 �� �� ֵ  : 0 �ɹ�
             1 ʧ��
 ����˵��  :
*****************************************************************************/
STATIC int32 ATSetWifiRPCKG(int32 flag)
{
    int ret = AT_RETURN_SUCCESS;
    int32   good_result = 0;

    if (0 != flag)
    {
        WLAN_TRACE_INFO("Exit on flag = %d\n", flag);
        return (AT_RETURN_FAILURE);
    }
    ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_RX_PCKG_GET, &good_result);

    return (ret);
}


/*****************************************************************************
 ��������  : int32 WlanATSetWifiRPCKG(int32 flag)
 ��������  : ��ѯWiFi���ջ�����룬�ϱ����յ��İ�������
 �������  : WLAN_AT_WIRPCKG_STRU *params
 �������  : uint16 good_result; //������յ��ĺð�����ȡֵ��ΧΪ0~65535
             uint16 bad_result;  //������յ��Ļ�������ȡֵ��ΧΪ0~65535
 �� �� ֵ  : NA
 ����˵��  :
*****************************************************************************/
STATIC int32 ATGetWifiRPCKG(WLAN_AT_WIRPCKG_STRU *params)
{
    int32 ret = AT_RETURN_SUCCESS;
    uint8 auc_result[2] = {0};

    if (NULL == params)
    {
        WLAN_TRACE_ERROR("%s:POINTER_NULL!\n", __FUNCTION__);
        ret = AT_RETURN_FAILURE;
        return ret;
    }

    /* �жϽ��ջ��Ƿ�� */
    if(AT_FEATURE_DISABLE == g_wlan_at_data.wifiRX.onoff)
    {
        WLAN_TRACE_ERROR("%s:Not Rx Mode.\n", __FUNCTION__);
        ret = AT_RETURN_FAILURE;
        return ret;
    }

    ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_RX_PCKG_GET, auc_result);
    params->good_result = (auc_result[0]<<8) + auc_result[1];
    WLAN_TRACE_INFO("auc_result=%X_%X\n", auc_result[0], auc_result[1]);

    params->bad_result = 0;     /* ����ע������ */
    WLAN_TRACE_INFO("Exit [good = %d, bad = %d]\n", params->good_result, params->bad_result);

    return (ret);
}

//////////////////////////////////////////////////////////////////////////
/*(10)^WIINFO ��ѯWiFi�������Ϣ*/
//////////////////////////////////////////////////////////////////////////
#define SIZE_OF_INFOGROUP(group) (sizeof(group) / sizeof(WLAN_AT_WIINFO_MEMBER_STRU))
/*****************************************************************************
 ��������  : uint32 ATGetWifiInfo(WLAN_AT_WIINFO_STRU *params)
 ��������  : ��ѯWiFi�������Ϣ(�ڲ��ӿ�)
 �������  : NA
 �������  : NA
 �� �� ֵ  : NA
 ����˵��  :
*****************************************************************************/
STATIC int32 ATGetWifiInfo(WLAN_AT_WIINFO_STRU *params)
{
    static char sChannels24G[] = "1,2,3,4,5,6,7,8,9,10,11,12,13";
    static char sChannels24G_n4[] = "3,4,5,6,7,8,9,10,11";
    static char sChannels5G[] = "36,40,44,48,52,56,60,64,100,104,108,112,116,120,124,128,132,136,140,149,153,157,161,165";
    static char sChannels5G_ac4[] = "36,44,52,60,100,108,116,124,132,140,149,157";
    static char sChannels5G_ac8[] = "36,52,100,116,132,149";

    /*�ŵ���Ϣ*/
    static WLAN_AT_WIINFO_MEMBER_STRU sChannelGroup0[] =
    {
        {"b", sChannels24G},
        {"g", sChannels24G},
        {"n", sChannels24G},
        {"n4", sChannels24G_n4}
    };
    static WLAN_AT_WIINFO_MEMBER_STRU sChannelGroup1[] =
    {
        {"b", sChannels24G},
        {"g", sChannels24G},
        {"n", sChannels24G},
        {"n4", sChannels24G_n4}
    };
    static WLAN_AT_WIINFO_MEMBER_STRU sChannelGroup2[] =
    {
        {"a", sChannels5G},
        {"n", sChannels5G},
        {"n4", sChannels5G_ac4},
        {"ac", sChannels5G},
        {"ac4", sChannels5G_ac4},
        {"ac8", sChannels5G_ac8}
    };
    static WLAN_AT_WIINFO_MEMBER_STRU sChannelGroup3[] =
    {
        {"a", sChannels5G},
        {"n", sChannels5G},
        {"n4", sChannels5G_ac4},
        {"ac", sChannels5G},
        {"ac4", sChannels5G_ac4},
        {"ac8", sChannels5G_ac8}
    };
    /*������Ϣ*/
    static WLAN_AT_WIINFO_MEMBER_STRU sPowerGroup0[] =
    {
        {"b", "120"},
        {"g", "120"},
        {"n", "120"},
        {"n4", "120"}
    };
    static WLAN_AT_WIINFO_MEMBER_STRU sPowerGroup1[] =
    {
        {"b", "120"},
        {"g", "120"},
        {"n", "120"},
        {"n4", "120"}
    };
    static  WLAN_AT_WIINFO_MEMBER_STRU sPowerGroup2[] =
    {
        {"a",   "100"},
        {"n",   "100"},
        {"n4",  "100"},
        {"ac",  "100"},
        {"ac4", "100"},
        {"ac8", "100"}
    };
    static WLAN_AT_WIINFO_MEMBER_STRU sPowerGroup3[] =
    {
        {"a",   "100"},
        {"n",   "100"},
        {"n4",  "100"},
        {"ac",  "100"},
        {"ac4", "100"},
        {"ac8", "100"}
    };
    /*Ƶ����Ϣ:0��ʾ2.4G,1��ʾ5G*/
    static WLAN_AT_WIINFO_MEMBER_STRU sFreqGroup0[] =
    {
        {"b", "0"},
        {"g", "0"},
        {"n", "0"},
        {"n4", "0"}
    };
    static WLAN_AT_WIINFO_MEMBER_STRU sFreqGroup1[] =
    {
        {"b", "0"},
        {"g", "0"},
        {"n", "0"},
        {"n4", "0"}
    };
    static WLAN_AT_WIINFO_MEMBER_STRU sFreqGroup2[] =
    {
        {"a",   "1"},
        {"n",   "1"},
        {"n4",  "1"},
        {"ac",  "1"},
        {"ac4", "1"},
        {"ac8", "1"}
    };
    static WLAN_AT_WIINFO_MEMBER_STRU sFreqGroup3[] =
    {
        {"a",   "1"},
        {"n",   "1"},
        {"n4",  "1"},
        {"ac",  "1"},
        {"ac4", "1"},
        {"ac8", "1"}
    };

    static WLAN_AT_WIINFO_GROUP_STRU sInfoGroup0[] =
    {
        {sChannelGroup0, SIZE_OF_INFOGROUP(sChannelGroup0)},
        {sPowerGroup0, SIZE_OF_INFOGROUP(sPowerGroup0)},
        {sFreqGroup0, SIZE_OF_INFOGROUP(sFreqGroup0)}
    };

    static WLAN_AT_WIINFO_GROUP_STRU sInfoGroup1[] =
    {
        {sChannelGroup1, SIZE_OF_INFOGROUP(sChannelGroup1)},
        {sPowerGroup1, SIZE_OF_INFOGROUP(sPowerGroup1)},
        {sFreqGroup1, SIZE_OF_INFOGROUP(sFreqGroup1)}

    };

    static WLAN_AT_WIINFO_GROUP_STRU sInfoGroup2[] =
    {
        {sChannelGroup2, SIZE_OF_INFOGROUP(sChannelGroup2)},
        {sPowerGroup2, SIZE_OF_INFOGROUP(sPowerGroup2)},
        {sFreqGroup2, SIZE_OF_INFOGROUP(sFreqGroup2)}

    };

    static WLAN_AT_WIINFO_GROUP_STRU sInfoGroup3[] =
    {
        {sChannelGroup3, SIZE_OF_INFOGROUP(sChannelGroup3)},
        {sPowerGroup3, SIZE_OF_INFOGROUP(sPowerGroup3)},
        {sFreqGroup3, SIZE_OF_INFOGROUP(sFreqGroup3)}
    };

    static WLAN_AT_WIINFO_GROUP_STRU *sTotalInfoGroups[] =
    {
        sInfoGroup0,
        sInfoGroup1,
        sInfoGroup2,
        sInfoGroup3
    };

    char *strBuf = NULL;
    int32 idx = 0, iLen = 0, igroup = 0,itype = 0, iTmp = 0;
    WLAN_AT_WIINFO_GROUP_STRU *pstuInfoGrup = NULL;
    WLAN_AT_WIINFO_GROUP_STRU *pstuInfoType = NULL;

    ASSERT_NULL_POINTER(params, AT_RETURN_FAILURE);
    PLAT_WLAN_INFO("Enter ATGetWifiInfo [group=%d,type=%u]\n", params->member.group, params->type);

    igroup = (int32)params->member.group;
    if (WLAN_AT_GROUP_MAX < igroup)
    {
        return (AT_RETURN_FAILURE);
    }

    itype = (int32)params->type;
    if (WLAN_AT_TYPE_MAX < itype)
    {
        return (AT_RETURN_FAILURE);
    }

    strBuf = (char *)params->member.content;
    iLen = (int32)(sizeof(params->member.content) - 1);

    pstuInfoGrup = sTotalInfoGroups[igroup];

    pstuInfoType = &pstuInfoGrup[itype];
    for(idx = 0; idx < pstuInfoType->size; idx++)
    {
        if (NULL == pstuInfoType->member[idx].name
          || NULL == pstuInfoType->member[idx].value)
        {
            continue;
        }

        OSA_SNPRINTF(strBuf, iLen, "%s,%s"
                    , pstuInfoType->member[idx].name
                    , pstuInfoType->member[idx].value);

        iTmp = (int32)(strlen(strBuf) + 1);
        iLen -= iTmp;
        strBuf += iTmp;
        if (iLen <= 0)
        {
            return (AT_RETURN_FAILURE);
        }
    }

    *strBuf = '\0';
    return (AT_RETURN_SUCCESS);
}

//////////////////////////////////////////////////////////////////////////
/*(11)^WIPLATFORM ��ѯWiFi����ƽ̨��Ӧ����Ϣ */
//////////////////////////////////////////////////////////////////////////
/*****************************************************************************
 ��������  : WLAN_AT_WIPLATFORM_TYPE WlanATGetWifiPlatform()
 ��������  : ��ѯWiFi����ƽ̨��Ӧ����Ϣ
 �������  : NA
 �������  : NA
 �� �� ֵ  : NA
 ����˵��  :
*****************************************************************************/
STATIC WLAN_AT_WIPLATFORM_TYPE ATGetWifiPlatform(void)
{
    return (AT_WIPLATFORM_HISI);
}

//////////////////////////////////////////////////////////////////////////
/*(12)^TSELRF ��ѯ���õ����WiFi��Ƶͨ·*/
//////////////////////////////////////////////////////////////////////////
/*****************************************************************************
 ��������  : int32 WlanATSetTSELRF(uint32 group)
 ��������  : �������ߣ��Ƕ�ͨ·��0
 �������  : NA
 �������  : NA
 �� �� ֵ  : NA
 ����˵��  :
*****************************************************************************/
STATIC int32 ATSetTSELRF(uint32 group)
{
    int ret = AT_RETURN_SUCCESS;
    int8 wl_cmd[WIFI_CMD_MAX_SIZE] = {0};
    int8* auc_rfch[3]= {"0000", "0001", "0010"};

    if(RF_5G_MIMO < group)
    {
        WLAN_TRACE_ERROR("WLAN_AT_GROUP_TYPE:%d unknow!\n", group);
        return AT_RETURN_FAILURE;
    }
    g_wlan_at_data.wifiGroup = group;
    WLAN_TRACE_INFO("[%s]:Enter,group = %u\n", __FUNCTION__, group);

    if ( AT_WIMODE_CW == g_wlan_at_data.wifiMode )
    {
        return (AT_RETURN_SUCCESS);
    }

    if(group < WLAN_AT_GROUP_MAX)
    {
        /*����ģʽ*/
        memset(wl_cmd, 0, WIFI_CMD_MAX_SIZE);
        OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), " %s txch %s", IF_HIPRIV, auc_rfch[(group % 2 + 1)]);
        ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_NORM_SET, wl_cmd);
        HIPRIV_BEEN_DONE(ret, "%s", wl_cmd);
        /*����ģʽ*/
        memset(wl_cmd, 0, WIFI_CMD_MAX_SIZE);
        OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), " %s rxch %s", IF_HIPRIV, auc_rfch[(group % 2 + 1)]);
        ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_NORM_SET, wl_cmd);
        HIPRIV_BEEN_DONE(ret, "%s", wl_cmd);

    }
    else
    {
        ;/*goup=4ʱ��ATSetWifiMode����������ģʽ��
           ����ֵ�� ������ʼ�Ѿ�У�飬���ﲻ�ٴ���*/
    }

    if (g_wlan_at_data.wifiGroup == RF_2G_0 || g_wlan_at_data.wifiGroup == RF_2G_1
     || g_wlan_at_data.wifiGroup == RF_2G_MIMO)
    {
        g_wlan_at_data.wifiBand = AT_WIFREQ_24G;
    }
    else if (g_wlan_at_data.wifiGroup == RF_5G_0 || g_wlan_at_data.wifiGroup == RF_5G_1
     || g_wlan_at_data.wifiGroup == RF_5G_MIMO)
    {
        g_wlan_at_data.wifiBand = AT_WIFREQ_50G;
    }

    return (ret);
}

/*****************************************************************************
 ��������  : int32 WlanATGetTSELRF(void)
 ��������  : ��ȡ����
 �������  : NA
 �������  : NA
 �� �� ֵ  : ����
 ����˵��  :
*****************************************************************************/
int32 ATGetTSELRF(void) /* ��ȡ���� */
{
    return g_wlan_at_data.wifiGroup;
}
/*****************************************************************************
 ��������  : int32 WlanATGetTSELRFSupport(WLAN_AT_BUFFER_STRU *strBuf)
 ��������  : ֧�ֵ������������У����ַ�����ʽ����eg: 0,1,2,3
 �������  : NA
 �������  : NA
 �� �� ֵ  : NA
 ����˵��  :
*****************************************************************************/
STATIC int32 ATGetTSELRFSupport(WLAN_AT_BUFFER_STRU *strBuf)
{
    if (NULL == strBuf)
    {
        return (AT_RETURN_FAILURE);
    }
     OSA_SNPRINTF(strBuf->buf, sizeof(strBuf->buf), "%s", WLAN_AT_TSELRF_SUPPORT);
    return (AT_RETURN_SUCCESS);
}

//////////////////////////////////////////////////////////////////////////

/*****************************************************************************
 ��������  : int32 ATWifiDebug(WLAN_AT_DEBUG_TYPE type, uint32 value1, uint32 *value2)
 ��������  :^WIFIDEBUG ��дWIFI�Ĵ����������Ϣ
 �������  :
     <type> ��ѯ����1~5���ɸ���������չ����
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
STATIC int32 ATWifiDebug(WLAN_AT_DEBUG_TYPE type, uint32 value1, uint32 *value2)
{

    int8 cmd[WIFI_CMD_MAX_SIZE] = {0};
    int32 ret = AT_RETURN_SUCCESS;

    if (NULL == g_at_hipriv_entry)
    {
        WLAN_TRACE_INFO("check_wifi_valid failed!\n");
        return AT_RETURN_FAILURE;
    }

    switch (type)
    {
        case AT_WIDEBUG_REG_READ: /* sample: AT^WIFIDEBUG=1,0x20038c00,2 */
            {
                if (NULL == value2)
                {
                    WLAN_TRACE_INFO("input param error!\n");
                    return AT_RETURN_FAILURE;
                }

                /* check reg address is valid or not */
                if ((HISI_SOC_REG_ADDR_MIN > value1)
                    || (HISI_SOC_REG_ADDR_MAX < value1))
                {
                    WLAN_TRACE_INFO("input soc register address invalid, must between \"0x%08X~0x%08X\"!\n",
                        HISI_SOC_REG_ADDR_MIN, HISI_SOC_REG_ADDR_MAX);
                    return AT_RETURN_FAILURE;
                }

                (void)OSA_SNPRINTF(cmd, sizeof(cmd), "soc 0x%x 0x%x", value1, value1);
                ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_REGINFO_SET, cmd);
                *value2 = *(uint32 *)cmd;
            }
            break;
        case AT_WIDEBUG_REG_WRITE: /* sample: AT^WIFIDEBUG=2,0x20038c00,0x10 */
            {
                if (NULL == value2)
                {
                    WLAN_TRACE_INFO("input param error!\n");
                    return AT_RETURN_FAILURE;
                }

                /* check reg address is valid or not */
                if ((HISI_SOC_REG_ADDR_MIN > value1)
                    || (HISI_SOC_REG_ADDR_MAX < value1))
                {
                    WLAN_TRACE_INFO("input soc register address invalid, must between \"0x%08X~0x%08X\"!\n",
                        HISI_SOC_REG_ADDR_MIN, HISI_SOC_REG_ADDR_MAX);
                    return AT_RETURN_FAILURE;
                }

                (void)OSA_SNPRINTF(cmd, sizeof(cmd), "Hisilicon0 regwrite soc 0x%x 0x%x", value1, *value2);
                ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_NORM_SET, cmd);
            }
            break;
        case AT_WIDEBUG_REG_FILE: /* sample: AT^WIFIDEBUG=3 */
            {
                (void)OSA_SNPRINTF(cmd, sizeof(cmd), "Hisilicon0 get_all_regs");
                ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_NORM_SET, cmd);
            }
            break;
        case AT_WIDEBUG_TEM_COM_SWTICH: /* sample: AT^WIFIDEBUG=4,1 */
            {
                /* check temperature compesation switch is valid or not */
                if ((0 != value1) && (1 != value1))
                {
                    WLAN_TRACE_INFO("input temperature compesation switch value \"%d\" invalid!\n", value1);
                    return AT_RETURN_FAILURE;
                }

                (void)OSA_SNPRINTF(cmd, sizeof(cmd), "Hisilicon0 dync_txpower %d", value1);
                ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_NORM_SET, cmd);
            }
            break;
        case AT_WIDEBUG_CALI_INTO_READ: /* sample: AT^WIFIDEBUG=5 */
            {
                WLAN_TRACE_INFO("AT^WIFIDEBUG=5 input point addr=%p\n", value2);
                ret = WIFI_TEST_CMD(WAL_ATCMDSRV_IOCTL_CMD_CALIINFO_SET, value2);
                /* ��ָ������װ�����Խ���ʱ����У׼ά����Ϣ��װ��Ҫ���ָ��һ������OK�����ⵥ�����fail */
                return AT_RETURN_SUCCESS;
            }
            break;
        default:
            {
                WLAN_TRACE_INFO("type %d not support\n", type);
                ret = AT_RETURN_FAILURE;
            }
            break;
    }

    return ret;
}

//////////////////////////////////////////////////////////////////////////

WLAN_CHIP_OPS hi1151_ops =
{
    .WlanATSetWifiEnable = ATSetWifiEnable,
    .WlanATGetWifiEnable = ATGetWifiEnable,

    .WlanATSetWifiMode   = ATSetWifiMode,
    .WlanATGetWifiMode   = ATGetWifiMode,
    .WlanATGetWifiBandSupport = ATGetWifiModeSupport,

    .WlanATSetWifiBand = ATSetWifiBand,
    .WlanATGetWifiBand = ATGetWifiBand,
    .WlanATGetWifiBandSupport = ATGetWifiBandSupport,

    .WlanATSetWifiFreq = ATSetWifiFreq,
    .WlanATGetWifiFreq = ATGetWifiFreq,

    .WlanATSetWifiDataRate = ATSetWifiDataRate,
    .WlanATGetWifiDataRate = ATGetWifiDataRate,

    .WlanATSetWifiPOW = ATSetWifiPOW,
    .WlanATGetWifiPOW = ATGetWifiPOW,

    .WlanATSetWifiTX = ATSetWifiTX,
    .WlanATGetWifiTX = ATGetWifiTX,

    .WlanATSetWifiRX = ATSetWifiRX,
    .WlanATGetWifiRX = ATGetWifiRX,

    .WlanATSetWifiRPCKG = ATSetWifiRPCKG,
    .WlanATGetWifiRPCKG = ATGetWifiRPCKG,
    .WlanATGetWifiPlatform = ATGetWifiPlatform,
    .WlanATGetWifiInfo = ATGetWifiInfo,

    .WlanATSetTSELRF = ATSetTSELRF,
    .WlanATGetTSELRF = NULL,
    .WlanATGetTSELRFSupport = ATGetTSELRFSupport,

    .WlanATSetWifiParange = NULL,
    .WlanATGetWifiParange = NULL,

    .WlanATGetWifiParangeSupport = NULL,

    .WlanATGetWifiCalTemp = NULL,
    .WlanATSetWifiCalTemp = NULL,
    .WlanATSetWifiCalData = NULL,
    .WlanATGetWifiCalData = NULL,
    .WlanATSetWifiCal = NULL,
    .WlanATGetWifiCal = NULL,
    .WlanATGetWifiCalSupport = NULL,
    .WlanATSetWifiCalFreq = NULL,
    .WlanATGetWifiCalFreq = NULL,
    .WlanATSetWifiCalPOW = NULL,
    .WlanATGetWifiCalPOW = NULL,
    .WlanATSetWifi2GPavars = NULL,
    .WlanATGetWifi2GPavars = NULL,
    .WlanATSetWifi5GPavars = NULL,
    .WlanATGetWifi5GPavars = NULL,

    .WlanATSetWifiDebug = ATWifiDebug,
};

/*===========================================================================
                       ���������б�
===========================================================================*/
EXPORT_SYMBOL(unreg_at_hipriv_entry);

