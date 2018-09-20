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



#include "carrier_adapter.h"
#include "drv_comm.h"
#include "bsp_version.h"
#include <linux/semaphore.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/wakelock.h>
#include <bsp_nvim.h>
#include <linux/mtd/flash_huawei_dload.h>
#include <bsp_icc.h>
#if ((FEATURE_ON == MBB_DLOAD) || (FEATURE_ON == MBB_IPL))
#include <bsp_sram.h>
#endif /* (FEATURE_ON == MBB_DLOAD) || (FEATURE_ON == MBB_IPL) */
#if (FEATURE_ON == MBB_DRV_SYSREBOOT_REPORT)
#include "mbb_drv_reboot_report.h"
#endif

#define UNINIT_INDEX    (VOS_UINT32)0xFFFFFFFF
/*****************************************************************************
  *
  * index����֮��Ӧ�Ķ���NV ·���� �������Ӫ�̺����
  * ͬ���޸�ͷ�ļ��е�ö�٣��ͱ������Ա�е���Ϣ
  *
  ****************************************************************************/
const CA_MATCH_XML_STRU g_ca_match_xml_list[CARRIER_INDEX_MAX] =
{
    /* ����AT&T ����NV ��Ϣ */
    {USA_ATT,                USA_ATT_CUST_NV_PATH},
    /* ī����AT&T ����NV ��Ϣ */
    {MEXICO_ATT,             MEX_ATT_CUST_NV_PATH},
    /* ���ô�AT&T ����NV ��Ϣ */
    {CANADA_ATT,             CA_ATT_CUST_NV_PATH},
    /* ����PTCRB ��֤����NV ��Ϣ */
    {NORTH_AMERICA_PTCRB,    NA_PTRB_CUST_NV_PATH},
    /* ����AT&T ����NV ��Ϣ */
    {KOREA_ATT,              KR_ATT_CUST_NV_PATH},
    /* �ձ�AT&T ����NV ��Ϣ */
    {JAPAN_ATT,              JP_ATT_CUST_NV_PATH},
    /* �й���ͨ����NV ��Ϣ */
    {CHINA_UNICOM,           CHN_UNICOM_CUST_NV_PATH},
    /* ȫ���ִ�� ����NV ��Ϣ */
    {GLOBAL_VODAFONE,        GBL_VODAFONE_CUST_NV_PATH},
    /* ����˹AT&T ����NV ��Ϣ */
    {RUSSIA_ATT,             RUS_ATT_CUST_NV_PATH},
    /* �ձ���������NV ��Ϣ */
    {JAPAN_SOFTBANK,         JP_SOFTBANK_CUST_NV_PATH},
    /* ����KT ����NV ��Ϣ */
    {KOREA_KT,               KR_KT_CUST_NV_PATH},
};

/* PLMN��index��Ӧ��ϵ�� */
const CA_PLMN_INFO_STRU g_ca_support_plmn_list[CARRIER_INDEX_MAX] =
{
    {
        USA_ATT,
        9,
        {"310170", "310150", "310380", "310410", "310280", "310030", "310560", "310950", "311180", }
    },
    {
        MEXICO_ATT,
        9,
        {"310170", "310150", "310380", "310410", "310280", "310030", "310560", "310950", "311180", }
    },
    {
        CANADA_ATT,
        9,
        {"310170", "310150", "310380", "310410", "310280", "310030", "310560", "310950", "311180", }
    },
    {
        NORTH_AMERICA_PTCRB,
        1,
        {"00101", }
    },
    {
        KOREA_ATT,
        9,
        {"310170", "310150", "310380", "310410", "310280", "310030", "310560", "310950", "311180", }
    },
    {
        JAPAN_ATT,
        9,
        {"310170", "310150", "310380", "310410", "310280", "310030", "310560", "310950", "311180", }
    },
    {
        CHINA_UNICOM,
        2,
        {"46001", "46009", }
    },
    {
        GLOBAL_VODAFONE,
        1,
        {"20404", }
    },
    {
        RUSSIA_ATT,
        9,
        {"310170", "310150", "310380", "310410", "310280", "310030", "310560", "310950", "311180", }
    },
    {
        KOREA_KT,
        6,
        {"45008", }
    },
    {
        JAPAN_SOFTBANK,
        6,
        {"44020", }
    },
};

/* �������һ�������ڴ����MCC ��Ӧ��Ĭ����Ӫ������ */
const MCC_MATCH_CARRIER_INFO_STRU g_mcc_default_carrier_name[MCC_SUPPORT_COUNT_MAX] =
{
    /* MCC Ϊ460 ��Ĭ��Ϊ��ͨ�� */
    {"460", CHINA_UNICOM},
    /* MCC Ϊ450��Ĭ��ΪKT�� */
    {"450", KOREA_KT},
    /* MCC Ϊ420��Ĭ��Ϊsoftbank�� */
    {"440", JAPAN_SOFTBANK},
    /* MCC Ϊ310��Ĭ��ΪAT&T�� (�����Ӳ�Ʒ)*/
    {"310", USA_ATT},
    /* MCC Ϊ310��Ĭ��ΪAT&T�� (��̫�Ӳ�Ʒ)*/
    {"310", KOREA_ATT},
    /* MCC Ϊ310��Ĭ��ΪAT&T�� (ŷ���Ӳ�Ʒ)*/
    {"310", RUSSIA_ATT},
    /* MCC Ϊ311��Ĭ��ΪAT&T�� */
    {"311", USA_ATT},
    /* MCCΪ204��Ĭ��Ϊ�ִ�Ῠ */
    {"204", GLOBAL_VODAFONE},
};

/* ���ڴ洢C�˷��͹�����PLMN��Ϣ */
static CA_HPLMN_WITH_MNC_LEN_STRU g_plmn_info_stru;
static struct semaphore carrier_adapter_sem;
static struct task_struct *carrier_adapter_tsk = NULL;
static struct wake_lock carrier_adapter_lock;
/* �������while(1)��������CUnit�еĲ��������޷����� */
static int TASK_KEEP_RUNNING = TRUE;
/* NV�л��ź��� */
static struct semaphore nv_switch_task_sem;

/*****************************************************************************
�� �� ��  : set_default_data
��������  : ��ѯ�濨ƥ����Ӫ��IDʱ����IDΪ-1�����忨���߲��벻֧�ֵ�SIM����������ֵ����ΪĬ����Ӫ��ID
�������  : index:�濨ƥ���ѯ����Ӫ��ID������IDΪ-1ʱ���øú���
�������  : ��
�� �� ֵ  : ��
*****************************************************************************/
void set_default_data(VOS_UINT32* index)
{
    u32 board_id = 0;
    const BSP_VERSION_INFO_S *board_info = NULL;
    if (NULL == index)
    {
        carrier_adapter_log(LOG_ERR, "index is NULL!!\r\n");
        return;
    }
    /* �ӹ����ڴ��л�ȡ����boardid */
    board_info = bsp_get_version_info();
    board_id = board_info->board_id;
    switch (board_id)
    {
        case HW_VER_PRODUCT_ME919Bs_127bN_A:
        case HW_VER_PRODUCT_ME919Bs_127bN_B:
        case HW_VER_PRODUCT_ME919Bs_127bN_C:
        case HW_VER_PRODUCT_ME919Bs_127bN_D:
            *index = GLOBAL_VODAFONE;
            break;
        case HW_VER_PRODUCT_ME919Bs_567bN_A:
        case HW_VER_PRODUCT_ME919Bs_567bN_B:
        case HW_VER_PRODUCT_ME919Bs_567bN_C:
        case HW_VER_PRODUCT_ME919Bs_567bN_D:
            *index = USA_ATT;
            break;
        case HW_VER_PRODUCT_ME919Bs_821bN_A:
        case HW_VER_PRODUCT_ME919Bs_821bN_B:
        case HW_VER_PRODUCT_ME919Bs_821bN_C:
        case HW_VER_PRODUCT_ME919Bs_821bN_D:
            *index = KOREA_ATT;
            break;
        case HW_VER_PRODUCT_ME919Bs_823a_A:
        case HW_VER_PRODUCT_ME919Bs_823a_B:
        case HW_VER_PRODUCT_ME919Bs_823a_C:
        case HW_VER_PRODUCT_ME919Bs_823a_D:
            *index = CHINA_UNICOM;
            break;
        default:
            carrier_adapter_log(LOG_ERR, "Get default index fail!!");
    }
}

/*****************************************************************************
 �� �� ��  : get_oem_carrier_name
 ��������  : ��ȡoeminfo�����洢��index��Ϣ
 �������  : ��
 �������  : index: ��ȡ����oeminfo�д洢��index
 �� �� ֵ  : BSP_OK: ����ɹ�
                       BSP_ERROR: ����ʧ��
*****************************************************************************/
int get_oem_carrier_name(unsigned int *index)
{
    CA_OEM_INFO_STRU oeminfo;
    bool bret = FALSE;
#if (FEATURE_ON == MBB_IPL)
    smem_huawei_ipl_fota_type *smem_data = NULL;
#endif

    /* ����ж� */
    if (NULL == index)
    {
        carrier_adapter_log(LOG_ERR, "NULL pointer!");
        return BSP_ERROR;
    }

    /* ��ʼ�� */
    (void)memset(&oeminfo, 0, sizeof(oeminfo));
    /* ��ȡoeminfo �����洢�ĵ�ǰnv ��index��Ϣ */

    /* ��oeminfo������ȡ���� */
    bret = flash_get_share_region_info(RGN_CARRIER_ADAPTER_NV_FLAG, &oeminfo, \
                                       sizeof(CA_OEM_INFO_STRU));

    if (!bret)
    {
        carrier_adapter_log(LOG_ERR, "failed to get oeminfo region");
        return BSP_ERROR;
    }

#if (FEATURE_ON == MBB_IPL)
    /* ��ȡsmem �����ڴ� */
    smem_data = (smem_huawei_ipl_fota_type *)SRAM_MBB_IPL_FOTA_FLAG_ADDR;

    if (NULL == smem_data)
    {
        carrier_adapter_log(LOG_ERR, "get share memory failed");
        return BSP_ERROR;
    }

    /* ��ȡ��ǰ����������Ӧ��oeminfo �е�PLMN ��Ϣ */
    if (HUAWEI_IPL_IMAGE_FIRST == smem_data->smem_huawei_ipl_magic_run_parition)
    {
        *index = oeminfo.index;
    }
    else
    {
        *index = oeminfo.index_bk;
    }

#else
    *index = oeminfo.index;
#endif

    return BSP_OK;
}

/*****************************************************************************
 �� �� ��  : set_oem_carrier_name
 ��������  : ����oeminfo�����洢��index��Ϣ
 �������  : index: Ҫ���õ�indexֵ
 �������  : ��
 �� �� ֵ  : BSP_OK: ����ɹ�
                       BSP_ERROR: ����ʧ��
*****************************************************************************/
int set_oem_carrier_name(unsigned int index)
{
    CA_OEM_INFO_STRU oeminfo;
    bool bret = FALSE;
#if (FEATURE_ON == MBB_IPL)
    smem_huawei_ipl_fota_type *smem_data = NULL;
#endif

    /* ����ж� */
    if (CARRIER_INDEX_MAX <= index)
    {
        carrier_adapter_log(LOG_ERR, "invalid index to set!");
        return BSP_ERROR;
    }

    /* ��ʼ�� */
    (void)memset(&oeminfo, 0, sizeof(oeminfo));
    /* ��ȡoeminfo �����洢�ĵ�ǰnv ��index��Ϣ */

    carrier_adapter_log(LOG_INFO, "start to set index %d to oeminfo", index);

    /* ��oeminfo������ȡ��ǰ���� */
    bret = flash_get_share_region_info(RGN_CARRIER_ADAPTER_NV_FLAG, &oeminfo, \
                                       sizeof(CA_OEM_INFO_STRU));

    if (!bret)
    {
        carrier_adapter_log(LOG_ERR, "failed to get oeminfo region");
        return BSP_ERROR;
    }

#if (FEATURE_ON == MBB_IPL)
    /* ��ȡsmem �����ڴ� */
    smem_data = (smem_huawei_ipl_fota_type *)SRAM_MBB_IPL_FOTA_FLAG_ADDR;

    if (NULL == smem_data)
    {
        carrier_adapter_log(LOG_ERR, "get share memory failed");
        return BSP_ERROR;
    }

    /* ˢ�µ�ǰ����������Ӧ��oeminfo �е�index��Ϣ */
    if (HUAWEI_IPL_IMAGE_FIRST == smem_data->smem_huawei_ipl_magic_run_parition)
    {
        oeminfo.index = index;
    }
    else
    {
        oeminfo.index_bk = index;
    }

#else
    oeminfo.index = index;
#endif

    /* ����oeminfo �������� */
    bret = flash_update_share_region_info(RGN_CARRIER_ADAPTER_NV_FLAG, &oeminfo, sizeof(CA_OEM_INFO_STRU));

    if (!bret)
    {
        carrier_adapter_log(LOG_ERR, "failed to set oeminfo region");
        return BSP_ERROR;
    }

    return BSP_OK;
}

/*****************************************************************************
 �� �� ��  : ca_decode_plmn_info
 ��������  : ����plmn ��Ϣ�ṹ�壬������plmn (�ַ�����ʽ)
 �������  : stHplmn: ԭʼPLMN ��Ϣ�ṹ��
 �������  : sim_plmn: plmn �ַ���
 �� �� ֵ  :  BSP_OK: �����ɹ�
                        BSP_ERROR: ����ʧ��
*****************************************************************************/
static int ca_decode_plmn_info(CA_HPLMN_WITH_MNC_LEN_STRU *stHplmn, char *sim_plmn)
{
    char plmn_temp[PLMN_SIZE_MAX] = {0};
    char basic_zero = '0';

    if ((NULL == stHplmn) || (NULL == sim_plmn))
    {
        carrier_adapter_log(LOG_ERR, "NULL pointer!");
        return BSP_ERROR;
    }

    /* ��ȡMCC�ĵ�1����ֵ */
    plmn_temp[0] = ((FIRST_BIT_MASK & stHplmn->stHplmn.Mcc) >> FIRST_BIT_SHIFT) \
                   + basic_zero;
    /* ��ȡMCC�ĵ�2����ֵ */
    plmn_temp[1] = ((SECOND_BIT_MASK & stHplmn->stHplmn.Mcc) >> SECOND_BIT_SHIFT) \
                   + basic_zero;
    /* ��ȡMCC�ĵ�3����ֵ */
    plmn_temp[2] = ((THIRD_BIT_MASK & stHplmn->stHplmn.Mcc) >> THIRD_BIT_SHIFT) \
                   + basic_zero;

    /*2λmnc*/
    if (2 == stHplmn->ucHplmnMncLen)
    {
        /* ��ȡMNC�ĵ�1����ֵ */
        plmn_temp[3] = ((FIRST_BIT_MASK & stHplmn->stHplmn.Mnc) >> FIRST_BIT_SHIFT) \
                       + basic_zero;
        /* ��ȡMNC�ĵ�2����ֵ */
        plmn_temp[4] = ((SECOND_BIT_MASK & stHplmn->stHplmn.Mnc) >> SECOND_BIT_SHIFT) \
                       + basic_zero;
    }
    /*3λmnc*/
    else if (3 == stHplmn->ucHplmnMncLen)
    {
        /* ��ȡMNC�ĵ�1����ֵ */
        plmn_temp[3] = ((FIRST_BIT_MASK & stHplmn->stHplmn.Mnc) >> FIRST_BIT_SHIFT) \
                       + basic_zero;
        /* ��ȡMNC�ĵ�2����ֵ */
        plmn_temp[4] = ((SECOND_BIT_MASK & stHplmn->stHplmn.Mnc) >> SECOND_BIT_SHIFT) \
                       + basic_zero;
        /* ��ȡMNC�ĵ�3����ֵ */
        plmn_temp[5] = ((THIRD_BIT_MASK & stHplmn->stHplmn.Mnc) >> THIRD_BIT_SHIFT) \
                       + basic_zero;
    }
    else
    {
        carrier_adapter_log(LOG_ERR, "HPLMN MNC LEN INVAILID");
        return BSP_ERROR;
    }

    (void)memset(sim_plmn, 0, PLMN_SIZE_MAX);

    (void)strncpy(sim_plmn, plmn_temp, strlen(plmn_temp));
    return BSP_OK;
}

/*****************************************************************************
 �� �� ��  : carrier_adapter_icc_cb
 ��������  : SIM����ʼ������A�˷�icc��Ϣ�Ļص�����
 �������  : ��
 �������  : ��
 �� �� ֵ  : BSP_OK: ����ɹ�
                       BSP_ERROR: ����ʧ��
*****************************************************************************/
static signed int carrier_adapter_icc_cb(void)
{
    int read_len = 0;
    int ret = BSP_ERROR;
    NV_CARRIER_ADAPTER_CONFIG_STRU ca_mode;

    /* �濨ƥ�����ϢID */
    unsigned int channel_id = ICC_CHN_IFC << 16 | IFC_RECV_FUNC_SIM_CA;

    /* ��ʼ�� */
    (void)memset(&ca_mode, 0, sizeof(ca_mode));

    /* ��ȡC �˷��͵�PLMN ��Ϣ */
    read_len = bsp_icc_read(channel_id, (unsigned char *)&g_plmn_info_stru, \
                            sizeof(g_plmn_info_stru));

    if (sizeof(g_plmn_info_stru) != read_len)
    {
        carrier_adapter_log(LOG_ERR, "bsp_icc_read len is %d.", read_len);
        return BSP_ERROR;
    }

    /* ���濨ƥ��ģʽNV 50635 */
    ret = bsp_nvm_read(NV_CARRIER_ADAPTER_CONFIG, (u8 *)&ca_mode, sizeof(ca_mode));

    if (BSP_OK != ret)
    {
        carrier_adapter_log(LOG_ERR, "nv read err");
        return BSP_ERROR;
    }

    /* ��ǰΪ�Զ�ģʽ������NV�л����� */
    if (CA_NV_SWITCH_MODE_AUTO == ca_mode.switch_mode)
    {
        up(&carrier_adapter_sem);
    }
    else
    {
        carrier_adapter_log(LOG_INFO, "switch mode is manual, still sleeping");
    }

    carrier_adapter_log(LOG_INFO, "carrier_adapter_icc_cb proccess finish");

    return BSP_OK;
}

/*****************************************************************************
 �� �� ��  : flush_nv_data_to_partition
 ��������  : ˢ���ڴ��е�NV���ݵ�������
 �������  : ��
 �������  : ��
 �� �� ֵ  : BSP_OK: �ɹ�
             BSP_ERROR: ʧ��
*****************************************************************************/
static int flush_nv_data_to_partition(void)
{
    u32 ret = NV_ERROR;

    /* ˢ��img�������� */
    ret = nv_img_flush_all();

    if (NV_OK != ret)
    {
        carrier_adapter_log(LOG_ERR, "write back to img partition failed");
        return BSP_ERROR;
    }

    /* ˢ��backlte�������� */
    ret = bsp_nvm_backup(NV_FLAG_NO_CRC);

    if (NV_OK != ret)
    {
        carrier_adapter_log(LOG_ERR, "write back to backlte partition failed");
        return BSP_ERROR;
    }

    /* ˢ��sys�������� */
    ret = bsp_nvm_flushSys();

    if (NV_OK != ret)
    {
        carrier_adapter_log(LOG_ERR, "write back to sys partition failed");
        return BSP_ERROR;
    }

    return BSP_OK;
}

/*****************************************************************************
 �� �� ��  : carrier_name_match_cust_nv_path
 ��������  : ������Ӫ������ƥ����Ӫ�̶���nv ��·��
 �������  : carrier_name: ��Ӫ������
 �������  : cust_nv_path: ��Ӫ�̶���NV ·��
 �� �� ֵ  : NV_OK:  ִ�гɹ�
             NV_ERROR: ִ��ʧ��
*****************************************************************************/
unsigned int carrier_name_match_cust_nv_path(unsigned int carrier_name, char *cust_nv_path)
{
    u32 ret = NV_ERROR;
    u32 len = 0;
    u32 i = 0;

    if (NULL == cust_nv_path)
    {
        return ret;
    }

    /* ���б���ƥ����Ӫ�̶���NV */
    for (i = 0; i < CARRIER_INDEX_MAX; i++)
    {
        if (carrier_name == g_ca_match_xml_list[i].carrier_name)
        {
            len = strlen(g_ca_match_xml_list[i].cust_nv_path);

            if ((0 < len) && (CA_CUST_PATH_MAX_LENGTH > len))
            {
                (void)snprintf(cust_nv_path, (CA_CUST_PATH_MAX_LENGTH - 1), "%s", \
                               g_ca_match_xml_list[i].cust_nv_path);
                ret = NV_OK;
            }
            else
            {
                carrier_adapter_log(LOG_ERR, "cann't find cust_nv_path for this plmn\n");
                ret = NV_ERROR;
            }

            break;
        }
    }

    return ret;
}

/*****************************************************************************
 �� �� ��  : carrier_adapter_update_nv
 ��������  : �л�nv ������
 �������  : carrier_name: ��Ӫ������
 �������  : ��
 �� �� ֵ  : BSP_OK:�ɹ�  BSP_ERROR:ʧ��
*****************************************************************************/
static int carrier_adapter_update_nv(unsigned int carrier_name)
{
    int ret = BSP_ERROR;
    char cust_nv_path[CA_CUST_PATH_MAX_LENGTH] = {0};

    /* �жϵ�ǰ�����Ƿ������������룬�����ȡ�ź���ʧ������������ERROR */
    if (0 != down_trylock(&nv_switch_task_sem))
    {
        carrier_adapter_log(LOG_ERR, "carrier_adapter NV Switch process is busying");
        return BSP_ERROR;
    }

    do
    {
        /* ƥ��plmn ��Ӧ��׼��NV �ļ�·�� */
        ret = carrier_name_match_cust_nv_path(carrier_name, cust_nv_path);

        if (NV_OK != ret)
        {
            carrier_adapter_log(LOG_ERR, "can not find cust nv path");
            ret = BSP_ERROR;
            break;
        }

        /* ����xml �ļ� */
        ret = bsp_nvm_ca_xml_decode(cust_nv_path);

        if (BSP_OK != ret)
        {
            carrier_adapter_log(LOG_WARN, "decode xml error ret = %d", ret);
            ret = BSP_ERROR;
            break;
        }

        /* ddr �е�NV ������CRC У��*/
        ret = nv_crc_make_ddr();

        if (NV_OK != ret)
        {
            carrier_adapter_log(LOG_WARN, "make nv crc error");
            ret = BSP_ERROR;
            break;
        }

        /*����������д���������*/
        ret = flush_nv_data_to_partition();

        if (BSP_OK != ret)
        {
            carrier_adapter_log(LOG_ERR, "flush nv data to flash failed");
            ret = BSP_ERROR;
            break;
        }

        /* ��������ִ�гɹ���NV�л���ɣ�����BSP_OK */
        ret = BSP_OK;
    } while (0);

    /* NV�л����̽������ͷ��ź��� */
    up(&nv_switch_task_sem);

    carrier_adapter_log(LOG_INFO, "nv switch action finished, ret = %d", ret);

    return ret;
}

/*****************************************************************************
 �� �� ��  : is_carrier_name_valid
 ��������  : ����Ƿ�����Ч����Ӫ��
 �������  : carrier_name: ��Ӫ������
 �������  : ��
 �� �� ֵ  : TRUE:��Ч  FALSE:��Ч
*****************************************************************************/
static bool is_carrier_name_valid(unsigned int carrier_name)
{
    bool ret = FALSE;
    u32 board_id = 0;
    const BSP_VERSION_INFO_S *board_info = NULL;

    board_info = bsp_get_version_info();

    if (NULL == board_info)
    {
        carrier_adapter_log(LOG_ERR, "get board info failed");
        return FALSE;
    }

    board_id = board_info->board_id;

    switch (board_id)
    {
        /* ME909Bs-566N��ME919Bs-567bN֧�ֱ���AT&T�ͱ���PTCRB */
        case HW_VER_PRODUCT_ME909Bs_566N_A:
        case HW_VER_PRODUCT_ME909Bs_566N_B:
        case HW_VER_PRODUCT_ME909Bs_566N_C:
        case HW_VER_PRODUCT_ME909Bs_566N_D:
        case HW_VER_PRODUCT_ME919Bs_567bN_A:
        case HW_VER_PRODUCT_ME919Bs_567bN_B:
        case HW_VER_PRODUCT_ME919Bs_567bN_C:
        case HW_VER_PRODUCT_ME919Bs_567bN_D:
        {
            if ((USA_ATT == carrier_name)
                || (CANADA_ATT == carrier_name)
                || (MEXICO_ATT == carrier_name)
                || (NORTH_AMERICA_PTCRB == carrier_name))
            {
                ret = TRUE;
            }
            else
            {
                ret = FALSE;
            }

            break;
        }

        /* ME919Bs-127bN֧��ȫ���ִ��Ͷ���˹AT&T */
        case HW_VER_PRODUCT_ME919Bs_127bN_A:
        case HW_VER_PRODUCT_ME919Bs_127bN_B:
        case HW_VER_PRODUCT_ME919Bs_127bN_C:
        case HW_VER_PRODUCT_ME919Bs_127bN_D:
        {
            if ((GLOBAL_VODAFONE == carrier_name)
                || (RUSSIA_ATT == carrier_name))
            {
                ret = TRUE;
            }
            else
            {
                ret = FALSE;
            }

            break;
        }

        /* ME919Bs-821bN֧�ֺ���AT&T���ձ�AT&T���й���ͨ */
        case HW_VER_PRODUCT_ME919Bs_821bN_A:
        case HW_VER_PRODUCT_ME919Bs_821bN_B:
        case HW_VER_PRODUCT_ME919Bs_821bN_C:
        case HW_VER_PRODUCT_ME919Bs_821bN_D:
        {
            if ((KOREA_ATT == carrier_name)
                || (JAPAN_ATT == carrier_name)
                || (CHINA_UNICOM == carrier_name))
            {
                ret = TRUE;
            }
            else
            {
                ret = FALSE;
            }

            break;
        }

        /* ME919Bs-823a֧�֣�KT��SOFTBANK��UNICOM */
        case HW_VER_PRODUCT_ME919Bs_823a_A:
        case HW_VER_PRODUCT_ME919Bs_823a_B:
        case HW_VER_PRODUCT_ME919Bs_823a_C:
        case HW_VER_PRODUCT_ME919Bs_823a_D:
        {
            if ((KOREA_KT == carrier_name)
                || (JAPAN_SOFTBANK == carrier_name)
                || (CHINA_UNICOM == carrier_name))
            {
                ret = TRUE;
            }
            else
            {
                ret = FALSE;
            }

            break;
        }

        default:
        {
            break;
        }
    }

    return ret;
}

/*****************************************************************************
 �� �� ��  : plmn_get_carrier_name
 ��������  : ��ȡplmn ��������Ӫ��
 �������  : plmn: plmn�ַ�����Ϣ
 �������  : out: ��Ӫ������
 �� �� ֵ  : BSP_OK:�ɹ�  BSP_ERROR:ʧ��
*****************************************************************************/
static int plmn_get_carrier_name(unsigned int *out, const char *plmn)
{
    int ret = BSP_ERROR;
    bool bret = FALSE;
    int i = 0;
    int j = 0;
    char sim_mcc_str[MCC_STR_SIZE] = {0};

    if (NULL == out || NULL == plmn)
    {
        return ret;
    }

    /* ���ƥ��PLMN ��Ӧ����Ӫ������ */
    for (i = 0; i < CARRIER_INDEX_MAX; i++)
    {
        for (j = 0; j < g_ca_support_plmn_list[i].count; j++)
        {
            ret = strcmp(g_ca_support_plmn_list[i].plmn[j], plmn);

            if (0 == ret)
            {
                /* ƥ��ɹ���֧�֣����ؽ����ƥ��ɹ��Ҳ�֧�ּ������� */
                bret = is_carrier_name_valid(g_ca_support_plmn_list[i].carrier_name);

                if (bret)
                {
                    /* PLMNƥ����Ӫ�̳ɹ����������л�������Ӫ�� */
                    *out = g_ca_support_plmn_list[i].carrier_name;
                    return BSP_OK;
                }
            }
        }
    }

    /* ����� plmn ƥ��ʧ���˲Ż��ߵ�����, ʹ��MCCƥ��Ĭ����Ӫ�� */
    carrier_adapter_log(LOG_WARN, "SIM Hplmn doesn't match, try to match MCC");
    /* ��plmn ����ȡmcc */
    (void)strncpy(sim_mcc_str, plmn, (MCC_STR_SIZE - 1));

    /* ���ƥ��MCC ��Ӧ����Ӫ������ */
    for (i = 0; i < MCC_SUPPORT_COUNT_MAX; i++)
    {
        ret = strcmp(g_mcc_default_carrier_name[i].mcc_str, sim_mcc_str);

        if (0 == ret)
        {
            /* ƥ��ɹ���֧�֣����ؽ����ƥ��ɹ��Ҳ�֧�ּ������� */
            bret = is_carrier_name_valid(g_mcc_default_carrier_name[i].def_carrier_name);

            if (bret)
            {
                /* MCCƥ��Ĭ����Ӫ�̳ɹ����������л�������Ӫ�� */
                *out = g_mcc_default_carrier_name[i].def_carrier_name;
                return BSP_OK;
            }
        }
    }

    return BSP_ERROR;
}

/*****************************************************************************
 �� �� ��  : carrier_adapter_task
 ��������  : �濨ƥ�䴦��������ں���
 �������  : NULL
 �������  : ��
 �� �� ֵ  : BSP_OK:�ɹ�  BSP_ERROR:ʧ��
*****************************************************************************/
static int carrier_adapter_task(void *args)
{
    int ret = BSP_ERROR;
    char sim_plmn[PLMN_SIZE_MAX] = {0};
    unsigned int oem_carrier_name = CARRIER_INDEX_MAX;
    unsigned int sim_carrier_name = CARRIER_INDEX_MAX;

    /* Ҫ�����������ȼ��Ƚϸ�,�����˾�ִ��*/
    set_user_nice(current, 10);

    do
    {
        /* һֱ�ȴ�ֱ������ź���*/
        if (0 != down_interruptible(&carrier_adapter_sem))
        {
            carrier_adapter_log(LOG_INFO, "waiting for sema timeout");
            continue;
        }

        /* �л�ǰ�ӳ�ϵͳ ����5s ����ֹģ�������л�ʧЧ */
        wake_lock_timeout(&carrier_adapter_lock, NV_SWITCH_WAIT_TIME);

        /* ����PLMN ��ϢΪ�ַ��� */
        ret = ca_decode_plmn_info(&g_plmn_info_stru, sim_plmn);

        if (BSP_OK != ret)
        {
            carrier_adapter_log(LOG_ERR, "decode plmn info failed!");
            continue;
        }
        else
        {
            carrier_adapter_log(LOG_INFO, "SIM PLMN is %s", sim_plmn);
        }

        /* ����SIM ����PLMN ��ȡ��Ӫ������ */
        ret = plmn_get_carrier_name(&sim_carrier_name, sim_plmn);

        if (BSP_OK != ret)
        {
            carrier_adapter_log(LOG_WARN, "SIM carrier name is unknown");
            continue;
        }

        /* ��ȡoeminfo �����洢�ĵ�ǰnv ��index��Ϣ */
        ret = get_oem_carrier_name(&oem_carrier_name);

        if (BSP_OK != ret)
        {
            carrier_adapter_log(LOG_ERR, "failed to get oeminfo index");
            continue;
        }

        carrier_adapter_log(LOG_INFO, "oeminfo carrier index is %d", oem_carrier_name);

        /* oeminfo ����Ӫ�� ��sim ������Ӫ�̲�ͬ, ���߻�ȡ
               * oeminfo ������Ӫ�����Ƴ�����������nv�л����� */
        if (sim_carrier_name == oem_carrier_name)
        {
            carrier_adapter_log(LOG_INFO, "don't need switch carrier NV");
            continue;
        }
        else
        {
            carrier_adapter_log(LOG_INFO, "SIM Carrier changed, start to switch carrier NV");
        }

        /* �л�NV */
        ret = carrier_adapter_update_nv(sim_carrier_name);

        if (BSP_OK != ret)
        {
            carrier_adapter_log(LOG_ERR, "switch carrier nv failed, ret= %d", ret);
            continue;
        }
        else
        {
            carrier_adapter_log(LOG_INFO, "NV switch finish");
        }

        /* ����oeminfo������Ϣ */
        ret = set_oem_carrier_name(sim_carrier_name);

        if (BSP_OK != ret)
        {
            carrier_adapter_log(LOG_ERR, "update oeminfo failed");
        }
        else
        {
            carrier_adapter_log(LOG_INFO, "update oeminfo successfully");

#if (FEATURE_ON == MBB_DRV_SYSREBOOT_REPORT)
            /* �����ϱ� */
            request_sys_reboot_report(SYS_REBOOT_REASON_CRNVSWT);
            carrier_adapter_log(LOG_INFO, "kernel do report finish");
#endif
        }
    } while (TASK_KEEP_RUNNING);

    return BSP_OK;
}

/*****************************************************************************
 �� �� ��  : manual_switch_ca_cust_nv
 ��������  : �ֶ��л���indexָ������Ӫ�̶���nv
 �������  : index: Ҫ�л���Ŀ����Ӫ��index
 �������  :  ��
 �� �� ֵ  :  BSP_OK:ִ�гɹ� BSP_ERROR:ִ��ʧ��
*****************************************************************************/
int manual_switch_ca_cust_nv(unsigned int index)
{
    int ret = BSP_ERROR;
    unsigned int oem_carrier_name = CARRIER_INDEX_MAX;
    NV_CARRIER_ADAPTER_CONFIG_STRU ca_mode;
    MTCSWT_AUTO_MANUL_STATUS_STRU ca_switch;

    /* ��ʼ�� */
    (void)memset(&ca_mode, 0, sizeof(ca_mode));
    (void)memset(&ca_switch, 0, sizeof(ca_switch));

    /* ������Ч�Լ�� */
    if (CARRIER_INDEX_MAX <= index)
    {
        carrier_adapter_log(LOG_WARN, "unknown carrier index: %d", index);
        return BSP_ERROR;
    }

    /* ��ȡ�濨ƥ�俪��NV */
    ret = bsp_nvm_read(NV_HUAWEI_MULTI_CARRIER_AUTO_SWITCH_I, (u8 *)&ca_switch, sizeof(ca_switch));

    if (NV_OK != ret)
    {
        carrier_adapter_log(LOG_ERR, "read ca_switch nv error");
        return BSP_ERROR;
    }

    /* ����濨ƥ�俪�عرգ�ֱ�ӷ���ʧ�� */
    if (CA_SWITCH_ON != ca_switch.carrier_adapter_switch)
    {
        carrier_adapter_log(LOG_ERR, "this feature is ont support");
        return BSP_ERROR;
    }

    /* ��ȡoeminfo �����洢�ĵ�ǰnv ��plmn ��Ϣ */
    ret = get_oem_carrier_name(&oem_carrier_name);

    if (BSP_OK != ret)
    {
        carrier_adapter_log(LOG_ERR, "failed to get oeminfo index");
        return BSP_ERROR;
    }
    else
    {
        carrier_adapter_log(LOG_INFO, "oeminfo carrier index is %d", oem_carrier_name);
    }

    /* oeminfo ����Ӫ�� ��indexָ������Ӫ�̲�ͬ, ���߻�ȡ
         * oeminfo ������Ӫ�����Ƴ�����������nv�л����� */
    if (index == oem_carrier_name)
    {
        carrier_adapter_log(LOG_WARN, "carrier NV already match index: %d", index);
        return BSP_OK;
    }
    else
    {
        carrier_adapter_log(LOG_INFO, "start to switch carrier NV");
    }

    /* ��ʼNV�л� */
    ret = carrier_adapter_update_nv(index);

    if (BSP_OK != ret)
    {
        carrier_adapter_log(LOG_WARN, "ca nv switch failed");
        return BSP_ERROR;
    }

    /* ��NV 50635����ȡ��ǰ�濨ƥ���л�ģʽ */
    ret = bsp_nvm_read(NV_CARRIER_ADAPTER_CONFIG, (u8 *)&ca_mode, sizeof(ca_mode));

    if (NV_OK != ret)
    {
        carrier_adapter_log(LOG_INFO, "nv read error, ret = %d", ret);
        return BSP_ERROR;
    }

    /* ����Ѿ����ֶ�ģʽ�������� */
    if (CA_NV_SWITCH_MODE_MANUAL == ca_mode.switch_mode)
    {
        carrier_adapter_log(LOG_INFO, "switch mode is manual_mode");
    }
    /* ���Ϊ�Զ�ģʽ��������Ϊ�ֶ�ģʽ */
    else
    {
        if (CA_NV_SWITCH_MODE_AUTO == ca_mode.switch_mode)
        {
            carrier_adapter_log(LOG_INFO, "switch mode is auto_mode, change to manual_mode");
        }
        else
        {
            carrier_adapter_log(LOG_WARN, "current NV config is unknown, change to manual_mode");
        }

        /* дNVΪ�ֶ�ģʽ */
        ca_mode.switch_mode = CA_NV_SWITCH_MODE_MANUAL;
        ret = bsp_nvm_write(NV_CARRIER_ADAPTER_CONFIG, (u8 *)&ca_mode, sizeof(ca_mode));

        if (NV_OK != ret)
        {
            carrier_adapter_log(LOG_ERR, "nv write error, ret = %d", ret);
            return BSP_ERROR;
        }
    }

    /* �����ݸ��µ�oeminfo���� */
    ret = set_oem_carrier_name(index);

    if (BSP_OK != ret)
    {
        carrier_adapter_log(LOG_ERR, "update oeminfo failed");
        return BSP_ERROR;
    }
    else
    {
        carrier_adapter_log(LOG_INFO, "update oeminfo successfully");
    }

#if (FEATURE_ON == MBB_DRV_SYSREBOOT_REPORT)
    /* �����ϱ���Ҫ���� */
    request_sys_reboot_report(SYS_REBOOT_REASON_CRNVSWT);
    carrier_adapter_log(LOG_INFO, "system reboot report finish");
#endif

    return BSP_OK;
}

/*****************************************************************************
 �� �� ��  : query_ca_status_info
 ��������  : ��ѯ��ǰ�濨ƥ���л�ģʽ��NV��Ӧ��index
 �������  : ��
 �������  :  mode: �濨ƥ���л�ģʽ
                           index: ��ǰ����NV��Ӧ����Ӫ��index
 �� �� ֵ  :  BSP_OK:ִ�гɹ� BSP_ERROR:ִ��ʧ��
*****************************************************************************/
int query_ca_status_info(unsigned int *mode, unsigned int *index)
{
    int ret = BSP_ERROR;
    NV_CARRIER_ADAPTER_CONFIG_STRU ca_mode;
    MTCSWT_AUTO_MANUL_STATUS_STRU ca_switch;

    /* ��ʼ�� */
    (void)memset(&ca_mode, 0, sizeof(ca_mode));
    (void)memset(&ca_switch, 0, sizeof(ca_switch));

    /* ����ж� */
    if ((NULL == mode) || (NULL == index))
    {
        carrier_adapter_log(LOG_ERR, "NULL pointer");
        return BSP_ERROR;
    }

    /* ��ȡ�濨ƥ�俪��NV */
    ret = bsp_nvm_read(NV_HUAWEI_MULTI_CARRIER_AUTO_SWITCH_I, (u8 *)&ca_switch, sizeof(ca_switch));

    if (NV_OK != ret)
    {
        carrier_adapter_log(LOG_ERR, "read ca_switch nv error");
        return BSP_ERROR;
    }

    /* ����濨ƥ�俪�عرգ�ֱ�ӷ���ʧ�� */
    if (CA_SWITCH_ON != ca_switch.carrier_adapter_switch)
    {
        carrier_adapter_log(LOG_ERR, "this feature is ont support");
        return BSP_ERROR;
    }

    /* ��ȡ��ǰ�濨ƥ���л�ģʽ */
    ret = bsp_nvm_read(NV_CARRIER_ADAPTER_CONFIG, (u8 *)&ca_mode, sizeof(ca_mode));

    if (NV_OK != ret)
    {
        carrier_adapter_log(LOG_INFO, "nv read error, ret = %d", ret);
        return BSP_ERROR;
    }

    *mode = ca_mode.switch_mode;

    /* ��ȡ��ǰNV��Ӧ����Ӫ��index */
    ret = get_oem_carrier_name(index);

    if (BSP_OK != ret)
    {
        carrier_adapter_log(LOG_ERR, "read oeminfo err");
        return BSP_ERROR;
    }
    /* ��ȡ��Ӫ��IDδ��ʼ��ʱ������Ӫ��ID����ΪĬ����Ӫ��ID */
    if (UNINIT_INDEX == *index)
    {
        set_default_data(index);
    }
    return BSP_OK;
}

/*****************************************************************************
 �� �� ��  : set_ca_nv_auto_switch
 ��������  : �����濨ƥ���л�ģʽΪ�Զ��л�
 �������  : ��
 �������  :  ��
 �� �� ֵ  :  BSP_OK:ִ�гɹ� BSP_ERROR:ִ��ʧ��
*****************************************************************************/
int set_ca_nv_auto_switch(void)
{
    unsigned int ret = NV_ERROR;
    NV_CARRIER_ADAPTER_CONFIG_STRU ca_mode;
    MTCSWT_AUTO_MANUL_STATUS_STRU ca_switch;

    /* ��ʼ�� */
    (void)memset(&ca_mode, 0, sizeof(ca_mode));
    (void)memset(&ca_switch, 0, sizeof(ca_switch));

    /* ��ȡ�濨ƥ�俪��NV */
    ret = bsp_nvm_read(NV_HUAWEI_MULTI_CARRIER_AUTO_SWITCH_I, (u8 *)&ca_switch, sizeof(ca_switch));

    if (NV_OK != ret)
    {
        carrier_adapter_log(LOG_ERR, "read ca_switch nv error");
        return BSP_ERROR;
    }

    /* ����濨ƥ�俪�عرգ�ֱ�ӷ���ʧ�� */
    if (CA_SWITCH_ON != ca_switch.carrier_adapter_switch)
    {
        carrier_adapter_log(LOG_ERR, "this feature is not support");
        return BSP_ERROR;
    }

    /* ��NV 50635����ȡ��ǰ�濨ƥ���л�ģʽ */
    ret = bsp_nvm_read(NV_CARRIER_ADAPTER_CONFIG, (u8 *)&ca_mode, sizeof(ca_mode));

    if (NV_OK != ret)
    {
        carrier_adapter_log(LOG_INFO, "nv read error, ret = %d", ret);
        return BSP_ERROR;
    }

    /* ����Ѿ����Զ�ģʽ�����سɹ� */
    if (CA_NV_SWITCH_MODE_AUTO == ca_mode.switch_mode)
    {
        carrier_adapter_log(LOG_WARN, "switch mode is already auto_mode");
        return BSP_OK;
    }
    else
    {
        if (CA_NV_SWITCH_MODE_MANUAL == ca_mode.switch_mode)
        {
            carrier_adapter_log(LOG_INFO, "switch mode is manual_mode, change to auto_mode");
        }
        else
        {
            carrier_adapter_log(LOG_WARN, "current NV config is unknown, change to auto_mode");
        }

        /* ���Ϊ�ֶ�ģʽ�������쳣��������Ϊ�Զ�ģʽ */
        ca_mode.switch_mode = CA_NV_SWITCH_MODE_AUTO;
        ret = bsp_nvm_write(NV_CARRIER_ADAPTER_CONFIG, (u8 *)&ca_mode, sizeof(ca_mode));

        if (NV_OK != ret)
        {
            carrier_adapter_log(LOG_ERR, "nv write error, ret = %d", ret);
            return BSP_ERROR;
        }

        /* �ж�PLMN��Ϣ�Ƿ���� */
        if (0 != g_plmn_info_stru.ucHplmnMncLen)
        {
            /* �����濨ƥ���Զ��л����� */
            up(&carrier_adapter_sem);
        }
        else
        {
            carrier_adapter_log(LOG_INFO, "MNC length is 0, NO PLMN information recieved");
        }
    }

    return BSP_OK;
}

/*****************************************************************************
 �� �� ��  : get_crnv_test_string
 ��������  : ����AT����ȡֵ��Χ
 �������  : out: ����ַ���
 �������  :  ��
 �� �� ֵ  :  ִ�гɹ��������ַ���ʵ�ʳ���len; ִ��ʧ��:0
*****************************************************************************/
unsigned int get_crnv_test_string(char *out)
{
    unsigned int len = 0;
    unsigned int ret = NV_ERROR;
    MTCSWT_AUTO_MANUL_STATUS_STRU ca_switch;

    /* ����ж� */
    if (NULL == out)
    {
        carrier_adapter_log(LOG_ERR, "NULL pointer!");
        return 0;
    }

    /* ��ʼ�� */
    (void)memset(&ca_switch, 0, sizeof(ca_switch));

    /* ��ȡ�濨ƥ�俪��NV */
    ret = bsp_nvm_read(NV_HUAWEI_MULTI_CARRIER_AUTO_SWITCH_I, (u8 *)&ca_switch, sizeof(ca_switch));

    if (NV_OK != ret)
    {
        carrier_adapter_log(LOG_ERR, "read ca_switch nv error");
        return 0;
    }

    /* ����濨ƥ�俪�عرգ�ֱ�ӷ���ʧ�� */
    if (CA_SWITCH_ON != ca_switch.carrier_adapter_switch)
    {
        carrier_adapter_log(LOG_ERR, "this feature is not support");
        return 0;
    }

    (void)snprintf(out, (CRNV_TEST_STR_LEN - 1), "(0,1),(0,%d)", (CARRIER_INDEX_MAX - 1));

    /* ��������ַ������� */
    len = strlen(out);
    carrier_adapter_log(LOG_INFO, "CRNV TEST string is '%s', len is %d", out, len);

    return len;
}

/*****************************************************************************
 �� �� ��  : carrier_adapter_task_init
 ��������  : �����濨ƥ������
 �������  : NULL
 �������  : ��
 �� �� ֵ  : BSP_OK:�ɹ�  BSP_ERROR:ʧ��
*****************************************************************************/
static int carrier_adapter_task_init(void)
{
    /* �����л����� */
    if (NULL == carrier_adapter_tsk)
    {
        carrier_adapter_tsk = kthread_run(carrier_adapter_task, NULL, CA_TSK_NAME);

        if (IS_ERR(carrier_adapter_tsk))
        {
            carrier_adapter_log(LOG_ERR, "fork failed for carrier_adapter_task");
            return BSP_ERROR;
        }
    }

    return BSP_OK;
}

/*****************************************************************************
 �� �� ��  : carrier_adapter_init
 ��������  : �濨ƥ��ģ���ʼ������
 �������  : NULL
 �������  : ��
 �� �� ֵ  : BSP_OK:�ɹ�  ����: ������
*****************************************************************************/
static int __init carrier_adapter_init(void)
{
    int ret = BSP_ERROR;
    /* �濨ƥ�����ϢID */
    unsigned int channel_id = ICC_CHN_IFC << 16 | IFC_RECV_FUNC_SIM_CA;
#if (FEATURE_ON == MBB_DLOAD)
    huawei_smem_info *smem_data = NULL;
#endif

    /* ��ʼ����ģ��ȫ�ֱ��� */
    (void)memset(&g_plmn_info_stru, 0, sizeof(g_plmn_info_stru));

#if (FEATURE_ON == MBB_DLOAD)
    smem_data = (huawei_smem_info *)SRAM_DLOAD_ADDR;

    if (NULL == smem_data)
    {
        carrier_adapter_log(LOG_INFO, "carrier_adapter_init: smem_data is NULL");
        return ret;
    }

    if (SMEM_DLOAD_FLAG_NUM == smem_data->smem_dload_flag)
    {
        /*����ģʽ�����α�ģ�������*/
        carrier_adapter_log(LOG_INFO, "update entry, not init carrier_adapter !");
        return ret;
    }

#endif /* MBB_DLOAD */

    /* ��ʼ��ϵͳ�� */
    wake_lock_init(&carrier_adapter_lock, WAKE_LOCK_SUSPEND, CA_MODULE_NAME);

    /* �ź�����ʼ������ */
    sema_init(&carrier_adapter_sem, 0);
    sema_init(&nv_switch_task_sem, 1);

    /* �����濨ƥ��NV �л����� */
    ret = carrier_adapter_task_init();

    if (BSP_OK != ret)
    {
        carrier_adapter_log(LOG_ERR, "running carrier adapter task failed");
        return ret;
    }

    /*ע��icc�ص�������������ȡSIM ����PLMN ��Ϣ*/
    ret = bsp_icc_event_register(channel_id, \
                                 (read_cb_func)carrier_adapter_icc_cb, NULL, NULL, NULL);

    if (BSP_OK != ret)
    {
        carrier_adapter_log(LOG_ERR, "icc event register failed.");
        return BSP_ERROR;
    }

    carrier_adapter_log(LOG_INFO, "Init OK");
    return BSP_OK;
}

module_init(carrier_adapter_init);

