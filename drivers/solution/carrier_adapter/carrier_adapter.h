/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2013-2016. All rights reserved.
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



#ifndef _CARRIER_ADAPTER_H_
#define _CARRIER_ADAPTER_H_

#include "mbb_carrier_adapter.h"
#include "product_nv_def.h"
#include "product_nv_id.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MCC_STR_SIZE                (4)
#define RESERVE_SIZE                (3)
#define MCC_SUPPORT_COUNT_MAX       (16)
#define PLMN_SUPPORT_MAX            (16)

/* �����������Ӫ�̶���NV·�� */
#define USA_ATT_CUST_NV_PATH      "/modem_fw/ca_cust_nv/usa_att_cust_nv.xml"
#define MEX_ATT_CUST_NV_PATH      "/modem_fw/ca_cust_nv/mex_att_cust_nv.xml"
#define CA_ATT_CUST_NV_PATH       "/modem_fw/ca_cust_nv/can_att_cust_nv.xml"
#define NA_PTRB_CUST_NV_PATH      "/modem_fw/ca_cust_nv/na_ptcrb_cust_nv.xml"
#define KR_ATT_CUST_NV_PATH       "/modem_fw/ca_cust_nv/kr_att_cust_nv.xml"
#define JP_ATT_CUST_NV_PATH       "/modem_fw/ca_cust_nv/jp_att_cust_nv.xml"
#define CHN_UNICOM_CUST_NV_PATH   "/modem_fw/ca_cust_nv/chn_unicom_cust_nv.xml"
#define GBL_VODAFONE_CUST_NV_PATH "/modem_fw/ca_cust_nv/gbl_vodafone_cust_nv.xml"
#define RUS_ATT_CUST_NV_PATH      "/modem_fw/ca_cust_nv/rus_att_cust_nv.xml"
#define KR_KT_CUST_NV_PATH        "/modem_fw/ca_cust_nv/kr_kt_cust_nv.xml"
#define JP_SOFTBANK_CUST_NV_PATH  "/modem_fw/ca_cust_nv/jp_softbank_cust_nv.xml"

/* ��TAF_PLMN_ID_STRU����һ�� */
typedef struct
{
    unsigned int    Mcc;
    unsigned int    Mnc;
} TAF_PLMN_ID_STRU;

typedef struct
{
    TAF_PLMN_ID_STRU    stHplmn;
    unsigned char       ucHplmnMncLen;
    unsigned char       aucReserved[RESERVE_SIZE];
} CA_HPLMN_WITH_MNC_LEN_STRU;

/* ��Ӫ����Ϣ�ṹ�壬������Ӫ�����Ƽ���ʹ�õ�PLMN */
typedef struct
{
    unsigned int carrier_name;                      /* ��Ӫ������ */
    int count;                                      /* ֧�ֵ�PLMN���� */
    char plmn[PLMN_SUPPORT_MAX][PLMN_SIZE_MAX];     /* PLMN�б� */
} CA_PLMN_INFO_STRU;

/* �濨ƥ�书�ܿ��� */
enum
{
    CA_SWITCH_OFF = 0,   /* �濨ƥ��ر� */
    CA_SWITCH_ON = 1,    /* �濨ƥ��� */
};

/* �濨ƥ��NV�л�����״̬�����ڷ�ֹ�������� */
enum
{
    CRNVSWT_STATUS_IDLE = 0,    /* ����״̬ */
    CRNVSWT_STATUS_BUSY = 1,        /* �����л�״̬ */
};

/* MCC ��Ĭ����Ӫ�����ƶ�Ӧ�б����ݽṹ */
typedef struct
{
    char mcc_str[MCC_STR_SIZE];
    unsigned int def_carrier_name;
} MCC_MATCH_CARRIER_INFO_STRU;

/******************* for debug log output ********************/
#ifdef MBB_BUILD_DEBUG

#define LOG_ERR     "[ERR]: "
#define LOG_WARN    "[WARN]: "
#define LOG_INFO    "[INFO]: "

#define carrier_adapter_log(lvl, fmt, ...) \
    printk("<CARRIER_ADAPTER>" lvl fmt "\n", ##__VA_ARGS__)

#else
#define carrier_adapter_log(lvl, fmt, ...)
#endif

/* �ֲ�ʹ�ú궨�壬����ħ������ */
#define CA_MODULE_NAME          "carrier_adapter"
#define CA_TSK_NAME             "carrier_adapter_task"

#define NV_SWITCH_WAIT_TIME     ((long)msecs_to_jiffies(5000))

#define FIRST_BIT_MASK          (0x0000000f)
#define SECOND_BIT_MASK         (0x00000f00)
#define THIRD_BIT_MASK          (0x000f0000)

#define FIRST_BIT_SHIFT         (0)
#define SECOND_BIT_SHIFT        (8)
#define THIRD_BIT_SHIFT         (16)

/* �ڴ��е�NV������CRC */
extern unsigned int nv_crc_make_ddr(void);
/* ����ddr�е��ڴ浽img���� */
extern unsigned int nv_img_flush_all(void);
/* ����ddr�е��ڴ浽sys���� */
extern unsigned int bsp_nvm_flushSys(void);
/* ����ddr�е��ڴ浽backlte���� */
extern unsigned int bsp_nvm_backup(unsigned int crc_flag);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


#endif /* _CARRIER_ADAPTER_H_ */

