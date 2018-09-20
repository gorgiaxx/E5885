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



#ifndef __MBB_CARRIER_ADAPTER_H__
#define __MBB_CARRIER_ADAPTER_H__

#include "product_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define PLMN_SIZE_MAX               (8)
#define CA_CUST_PATH_MAX_LENGTH     (64)
#define CRNV_TEST_STR_LEN           (16)

#define CARRIER_ADAPTER_COMMON_NV_PATH   "/modem_fw/ca_cust_nv/me919bs_common_nv.xml"

typedef enum
{
    CA_NV_SWITCH_MODE_AUTO = 0,     /* �濨ƥ���Զ��л�ģʽ */
    CA_NV_SWITCH_MODE_MANUAL = 1,   /* �濨ƥ���ֶ��л�ģʽ */
} CA_NV_SWITCH_MODE;

/* OEMINFO������index��Ϣ���ֱ���ʾ����������bk������index */
typedef struct
{
    unsigned int index;
    unsigned int index_bk;
} CA_OEM_INFO_STRU;

/* �����������濨ƥ������֧�ֵ�����Ӫ�̵����� */
typedef enum
{
    USA_ATT = 0,                 /* ����AT&T���� */
    MEXICO_ATT = 1,              /* ī����AT&T���� */
    CANADA_ATT = 2,              /* ���ô�AT&T���� */
    NORTH_AMERICA_PTCRB = 3,     /* ����PTCRB���� */
    KOREA_ATT = 4,               /* ����AT&T���� */
    JAPAN_ATT = 5,               /* �ձ�AT&T���� */
    CHINA_UNICOM = 6,             /* �й���ͨ���� */
    GLOBAL_VODAFONE = 7,         /* ȫ���ִ�ᶨ�� */
    RUSSIA_ATT = 8,              /* ����˹AT&T���� */
    KOREA_KT = 9,                /* ����KT���� */
    JAPAN_SOFTBANK = 10,          /* �ձ��������� */
    CARRIER_INDEX_MAX,
} CARRIER_NAME_ENUM;

/* PLMN����Ӫ�̶���NV XML�ļ���Ӧ�б����ݽṹ */
typedef struct
{
    unsigned int carrier_name;
    char cust_nv_path[CA_CUST_PATH_MAX_LENGTH];
} CA_MATCH_XML_STRU;

/* ������Ӫ������ƥ����Ӫ�̶���nv ��·�� */
unsigned int carrier_name_match_cust_nv_path(unsigned int carrier_name, char *cust_nv_path);
/* ��ѯ��ǰ�濨ƥ���л�ģʽ��NV��Ӧ��index */
int query_ca_status_info(unsigned int *mode, unsigned int *index);
/* �����濨ƥ���л�ģʽΪ�Զ��л� */
int set_ca_nv_auto_switch(void);
/* ��Ӫ�̶���NV�ֶ��л� */
int manual_switch_ca_cust_nv(unsigned int index);
/* ��ȡoeminfo������index */
int get_oem_carrier_name(unsigned int *index);
/* ����oeminfo������index */
int set_oem_carrier_name(unsigned int index);
/* ����AT����ȡֵ��Χ */
unsigned int get_crnv_test_string(char *out);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


#endif /* __MBB_CARRIER_ADAPTER_H__ */

