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
 



#ifndef __MBB_MULTI_CARRIER_H__
#define __MBB_MULTI_CARRIER_H__

#if (MBB_MULTI_CARRIER == FEATURE_ON)

#ifdef __cplusplus
extern "C"
{
#endif

#include "product_nv_id.h"
#include "product_nv_def.h"

typedef enum
{
    MTC_DEFAULT_SET,
    MTC_DEFAULT_RUN,
} MTC_DFT_OPTION;

typedef enum
{
    MTC_QUERY_DEFAULT,
    MTC_QUERY_RUNNING,
    MTC_QUERY_SUPPORT,
} MTC_QUERY_TYPE;

typedef enum
{
    MTC_SWITCH_CLOSE,
    MTC_SWITCH_OPEN,
} MTC_AUTOSEL_OPTION;

enum
{
    MTC_CUST_NV_IS_DEFAULT = 0, /* �ͻ�����NVδ���޸� */
    MTC_CUST_NV_IS_SET     = 1  /* �ͻ�����NV�ѱ��޸� */
};

typedef enum
{
    MTC_DEST_CARRIER_SET      = 0, /* �û��趨��sim����������Ӫ�� */
    MTC_DEST_CARRIER_DEFAULT  = 1, /* Ĭ����Ӫ�� */
    MTC_DEST_CARRIER_GENERAL  = 2  /* ͨ����Ӫ�� */
} mtc_dest_carrier_types;

typedef enum
{
    MTC_FEATURE_VER,
    MTC_PACKAGE_VER,
    MTC_OEM_VER,
    MTC_IMAGE_VER,
    MTC_VERSION_MAX,
} mtc_version_type;

typedef enum
{
    MTC_SIM_SWITCH,     /* SIM���Զ��л� */
    MTC_MANUAL_SWITCH,  /* �ֶ��л� */
    MTC_DEAAULT_SWITCH, /* ���ò�����Ĭ����Ӫ�� */
    MTC_OP_VALID,
} mtc_op_type;

/* �л���Ӫ�̵Ľ�� */
typedef enum
{
    MTC_SWITCH_CARRIER_FAIL,    /* �л�ʧ�� */
    MTC_SWITCH_CARRIER_SAME,    /* ��ǰ��Ӫ�̼�ΪĿ����Ӫ�̣�����Ҫ�л� */
    MTC_SWITCH_CARRIER_SUCCESS, /* �л��ɹ� */
} mtc_switch_carrier_results;

/* �汾�Ų�ѯ�ķ���ֵ */
typedef enum
{
    RES_VER_OK    = 0,
    RES_VER_ERROR = 1,
} res_ver;

#define START_MT_CARRIER_PLMN_LIST_LABEL   "<plmn_list>"
#define END_MT_CARRIER_PLMN_LIST_LABEL   "</plmn_list>"
#define START_MT_CARRIER_PLMN_LABEL   "<carrier_plmn="
#define END_MT_CARRIER_PLMN_LABEL  "</carrier_plmn>"
#define START_MT_SIM_PLMN_LABEL     "<sim_plmn>"
#define END_MT_SIM_PLMN_LABEL    "</sim_plmn>"
#define MTC_CUST_NV_PCVENDOR_XML_PATH             "/mnvm2:0/mtc/pc_vendor.bin"
#define MTC_CUST_NV_CUST_XML_PATH                 "/mnvm2:0/mtc/cust.xml"
#define MTC_CUST_NV_USERLIST_XML_PATH             "/mnvm2:0/mtc/usr_list.bin"
#define MTC_CUST_NV_DEFAULT_PATH                  "/online/mtc/pc_vendor.bin"
#define MTC_PLMN_LIST_FILE                        "/online/mtc/plmn_map.bin"

#define MTC_SOFT_VER_MAX 28
#define MTC_PLMN_MAX  8
#define MTC_GEN_PLMN "00000"

typedef struct
{
    unsigned char cur_running_plmn[MTC_PLMN_MAX];
    unsigned char default_plmn[MTC_PLMN_MAX];
} NV_DRV_PLMN_CFG;

/* ��TAF_PLMN_ID_STRU����һ�� */
typedef struct
{
    unsigned int    Mcc;
    unsigned int    Mnc;
} MTC_PLMN_ID_STRU;

typedef struct
{
    MTC_PLMN_ID_STRU    stHplmn;
    unsigned char       ucHplmnMncLen;
    unsigned char       aucReserved[3];  /* ��TAF_HPLMN_WITH_MNC_LEN_STRU����һ�� */
} MTC_HPLMN_WITH_MNC_LEN_STRU;

extern mtc_op_type mt_carrier_op ;
extern NV_DRV_PLMN_CFG mt_carrier_cur_plmninfo_from_nv;

bool balong_basic_exec_mtcarrier_query(char *dest_buff_ptr, u32 *length, MTC_QUERY_TYPE query);
mtc_switch_carrier_results balong_basic_exec_mtcarrier_cmd(char *plmn);

bool balong_basic_exec_mtcdft_cmd(char *plmn, MTC_DFT_OPTION dft_option);
res_ver huawei_mtc_get_version_entry(char *version, unsigned char size, mtc_version_type ver_type);
u32 bsp_nvm_revert_default_mtc_cust_nv(void);
bool huawei_mtc_manual_switch_entry(char *plmn);

#ifdef __cplusplus
}
#endif
#endif

#endif

