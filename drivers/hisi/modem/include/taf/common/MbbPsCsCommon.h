/*
* Copyright (C) Huawei Technologies Co., Ltd. 2012-2015. All rights reserved.
* foss@huawei.com
*
* If distributed as part of the Linux kernel, the following license terms
* apply:
*
* * This program is free software; you can redistribute it and/or modify
* * it under the terms of the GNU General Public License version 2 and 
* * only version 2 as published by the Free Software Foundation.
* *
* * This program is distributed in the hope that it will be useful,
* * but WITHOUT ANY WARRANTY; without even the implied warranty of
* * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* * GNU General Public License for more details.
* *
* * You should have received a copy of the GNU General Public License
* * along with this program; if not, write to the Free Software
* * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
*
* Otherwise, the following license terms apply:
*
* * Redistribution and use in source and binary forms, with or without
* * modification, are permitted provided that the following conditions
* * are met:
* * 1) Redistributions of source code must retain the above copyright
* *    notice, this list of conditions and the following disclaimer.
* * 2) Redistributions in binary form must reproduce the above copyright
* *    notice, this list of conditions and the following disclaimer in the
* *    documentation and/or other materials provided with the distribution.
* * 3) Neither the name of Huawei nor the names of its contributors may 
* *    be used to endorse or promote products derived from this software 
* *    without specific prior written permission.
* 
* * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*/



#ifndef _MBB_PS_CS_COMMON_H__
#define _MBB_PS_CS_COMMON_H__

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "vos.h"
#include "TafTypeDef.h"
#include "TafAppMma.h"
#include "TafApsApi.h"
#include "mbb_common.h"
#include "MbbSec.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if (FEATURE_ON == MBB_WPG_COMMON)

#ifdef _EXPORT_PRIVATE
#define PRIVATE_SYMBOL
#else
#define PRIVATE_SYMBOL static
#endif

#ifdef __MBB_LINT__
#define PRIVATE_CONST
#else
#define PRIVATE_CONST const
#endif

typedef unsigned char huawei_boolean;

/*****************************************************************************
  2 �궨��
*****************************************************************************/
#define AT_NWSCAN_SET_PARA_TIME                            (60000)
#define BUFFER_LENGTH_256                                           256
#define AT_HFREQINFO_NO_REPORT            0
#define AT_HFREQINFO_REPORT                   1

#define AT_IPV6_FIRST_VERSION           (1)
#define AT_IPV6_SECOND_VERSION          (2)

/*��֧������PDP��������Ŀ��*/
#define TAF_MIN_CID                     (1)

#define AT_CRLF_MAX_LEN                     (2)

#define MN_MMA_LTE_EUTRAN_BAND1     (0x1)
#define MN_MMA_LTE_EUTRAN_BAND2     (0x2)
#define MN_MMA_LTE_EUTRAN_BAND3     (0x4)
#define MN_MMA_LTE_EUTRAN_BAND4     (0x8)
#define MN_MMA_LTE_EUTRAN_BAND5     (0x10)
#define MN_MMA_LTE_EUTRAN_BAND6     (0x20)
#define MN_MMA_LTE_EUTRAN_BAND7     (0x40)
#define MN_MMA_LTE_EUTRAN_BAND8     (0x80)
#define MN_MMA_LTE_EUTRAN_BAND9     (0x100)
#define MN_MMA_LTE_EUTRAN_BAND10    (0x200)
#define MN_MMA_LTE_EUTRAN_BAND11    (0x400)
#define MN_MMA_LTE_EUTRAN_BAND12    (0x800)
#define MN_MMA_LTE_EUTRAN_BAND13    (0x1000)
#define MN_MMA_LTE_EUTRAN_BAND14    (0x2000)
#define MN_MMA_LTE_EUTRAN_BAND15    (0x4000)
#define MN_MMA_LTE_EUTRAN_BAND16    (0x8000)
#define MN_MMA_LTE_EUTRAN_BAND17    (0x10000)
#define MN_MMA_LTE_EUTRAN_BAND18    (0x20000)
#define MN_MMA_LTE_EUTRAN_BAND19    (0x40000)
#define MN_MMA_LTE_EUTRAN_BAND20    (0x80000)
#define MN_MMA_LTE_EUTRAN_BAND21    (0x100000)
#define MN_MMA_LTE_EUTRAN_BAND22    (0x200000)
#define MN_MMA_LTE_EUTRAN_BAND23    (0x400000)
#define MN_MMA_LTE_EUTRAN_BAND24    (0x800000)
#define MN_MMA_LTE_EUTRAN_BAND25    (0x1000000)
#define MN_MMA_LTE_EUTRAN_BAND26    (0x2000000)
#define MN_MMA_LTE_EUTRAN_BAND27    (0x4000000)
#define MN_MMA_LTE_EUTRAN_BAND28    (0x8000000)
#define MN_MMA_LTE_EUTRAN_BAND29    (0x10000000)
#define MN_MMA_LTE_EUTRAN_BAND30    (0x20000000)
#define MN_MMA_LTE_EUTRAN_BAND31    (0x40000000)
#define MN_MMA_LTE_EUTRAN_BAND32    (0x80000000)
#define MN_MMA_LTE_EUTRAN_BAND33    (0x1)
#define MN_MMA_LTE_EUTRAN_BAND34    (0x2)
#define MN_MMA_LTE_EUTRAN_BAND35    (0x4)
#define MN_MMA_LTE_EUTRAN_BAND36    (0x8)
#define MN_MMA_LTE_EUTRAN_BAND37    (0x10)
#define MN_MMA_LTE_EUTRAN_BAND38    (0x20)
#define MN_MMA_LTE_EUTRAN_BAND39    (0x40)
#define MN_MMA_LTE_EUTRAN_BAND40    (0x80)
#define MN_MMA_LTE_EUTRAN_BAND41    (0x100)
#define MN_MMA_LTE_EUTRAN_BAND42    (0x200)
#define MN_MMA_LTE_EUTRAN_BAND43    (0x400)
#define MN_MMA_LTE_EUTRAN_BAND44    (0x800)
#define MN_MMA_LTE_EUTRAN_BAND45    (0x1000)    
#define MN_MMA_LTE_EUTRAN_BAND46    (0x2000)
#define MN_MMA_LTE_EUTRAN_BAND47    (0x4000)
#define MN_MMA_LTE_EUTRAN_BAND48    (0x8000)
#define MN_MMA_LTE_EUTRAN_BAND49    (0x10000)    
#define MN_MMA_LTE_EUTRAN_BAND50    (0x20000)    
#define MN_MMA_LTE_EUTRAN_BAND51    (0x40000)    
#define MN_MMA_LTE_EUTRAN_BAND52    (0x80000)   
#define MN_MMA_LTE_EUTRAN_BAND53    (0x100000)   
#define MN_MMA_LTE_EUTRAN_BAND54    (0x200000)   
#define MN_MMA_LTE_EUTRAN_BAND55    (0x400000)   
#define MN_MMA_LTE_EUTRAN_BAND56    (0x800000)    
#define MN_MMA_LTE_EUTRAN_BAND57    (0x1000000)   
#define MN_MMA_LTE_EUTRAN_BAND58    (0x2000000)   
#define MN_MMA_LTE_EUTRAN_BAND59    (0x4000000)   
#define MN_MMA_LTE_EUTRAN_BAND60    (0x8000000)    
#define MN_MMA_LTE_EUTRAN_BAND61    (0x10000000)   
#define MN_MMA_LTE_EUTRAN_BAND62    (0x20000000)    
#define MN_MMA_LTE_EUTRAN_BAND63    (0x40000000)   
#define MN_MMA_LTE_EUTRAN_BAND64    (0x80000000)    

#define MN_MMA_LTE_LOW_BAND_NONE        (0x0) 

#ifndef MBB_ERR_LOG1
#define MBB_ERR_LOG1(str,P1)                     MBB_LOG1_DEF(WUEPS_PID_TAF, 0, PS_LOG_LEVEL_ERROR, str, P1)
#define MBB_ERR_LOG1_STR(str,Ps)                     MBB_LOG1_STR_DEF(WUEPS_PID_TAF, 0, PS_LOG_LEVEL_ERROR, str, Ps)
#define    MBB_LOG1_DEF(Mod, SubMod, Level, String, Para1) \
           (VOS_VOID)DIAG_LogReport( DIAG_GEN_LOG_MODULE(VOS_GetModemIDFromPid(Mod), DIAG_MODE_UMTS, (Level)), \
                           (Mod), __FILE__, __LINE__, "%s, 0x%x \r\n", (String), (VOS_INT32)(Para1) )
#define    MBB_LOG1_STR_DEF(Mod, SubMod, Level, String, ParaStr1) \
               (VOS_VOID)DIAG_LogReport( DIAG_GEN_LOG_MODULE(VOS_GetModemIDFromPid(Mod), DIAG_MODE_UMTS, (Level)), \
                               (Mod), __FILE__, __LINE__, "%s, %s \r\n", (String), (VOS_CHAR*)(ParaStr1) )
#endif

#ifdef __NAS_OM_UT__
extern VOS_UINT32    g_nocard_value;
extern VOS_UINT32    g_nocard_flag;
#define SRAM_NOCARD_FLAG_NUM     0xAA77BB88/*NOCARD �Ѿ������ù��ı�־ħ����*/
#define NOCARD_OFF   (0)
#define NOCARD_ON    (1)
#define SRAM_NOCARD_VALUE   g_nocard_value
#define SRAM_NOCARD_FLAG    g_nocard_flag
#endif

#if (FEATURE_ON == MBB_WPG_CELLLOCK)
#define TAF_PH_EVT_SYSTEM_INFO_CELLLOCK_IND          (46)                       /*CELLLOCKϵͳ��Ϣָʾ*/
#define AT_CELLLOCK_MAX_MODE                (4)                         /* CELLLOCK֧�ֵ���ʽ���� */    
#define AT_CELLLOCK_ON_PARA_MIN_NUM         (3)                         /* CELLLOCK���������С�������� */
#define AT_CELLLOCK_ON_CI_MAX_NUM           (3)                         /* CELLLOCK����������֧�ֵ�CI���� */


enum AT_CELLLOCK_MODE_ENUM
{
    AT_CELLLOCK_MODE_GSM            = 1,                                /* GSM��ʽ */
    AT_CELLLOCK_MODE_WCDMA          = 2,                                /* WCDMA��ʽ */
    AT_CELLLOCK_MODE_TDSCDMA        = 3,                                /* TD-SCDMA��ʽ */
    AT_CELLLOCK_MODE_LTE            = 4,                                /* LTE��ʽ */
    AT_CELLLOCK_MODE_BUTT
};
typedef VOS_UINT8 AT_CELLLOCK_MODE_ENUM_UINT8;


typedef struct
{
    VOS_BOOL                                ucRegistered;                           /* ��ǰ�Ƿ�ע���� */
    AT_CELLLOCK_MODE_ENUM_UINT8             ucCurMode;                              /* ��ǰ�Ľ��뼼�� */
    VOS_UINT8                               ucReserved[2];                          /* ���� */
    VOS_UINT32                              ulCurrLAC;                              /* λ������ϢLAC,LTE�ò�����ʾ����������TAC */
    VOS_UINT32                              ulCurrCID;                              /* С��IDCI */
}AT_CELLLOCK_STATUS_INFO;


typedef struct
{    
    VOS_BOOL                                ucEnable;                               /* ʹ�ܱ�ʶ */
    VOS_UINT8                               ulCidNumber;                            /* CID���� */
    VOS_UINT8                               ucReserved[2];                          /* ���� */
    VOS_UINT32                              ulLacPara;                              /* LAC����,LTE��ʽ�±�ʾTAC���� */
    VOS_UINT32                              ulCidPara[AT_CELLLOCK_ON_CI_MAX_NUM];   /* CID���� */
}AT_GUTL_CELLLOCK_CONFIG_STRU;


typedef struct
{    
    VOS_BOOL                                ucCellLockFlag;                         /* ʹ�ܱ�ʶ */
    VOS_UINT8                               aucReserved[3];                         /* ���� */
    AT_GUTL_CELLLOCK_CONFIG_STRU            stCellLockPara[AT_CELLLOCK_MAX_MODE];   /* GUTLģС������Ϣ�ṹ */
}AT_CELLLOCK_CONFIG_STRU;


typedef struct
{
    AT_CELLLOCK_STATUS_INFO             stCellLockStatus;                           /* UEע�����״̬��Ϣ */
    AT_CELLLOCK_CONFIG_STRU             stCellLockConfig;                           /* UEС����������Ϣ */
}AT_CELLLOCK_CTX_STRU;


typedef struct
{
    VOS_UINT32                          ulMode;
    VOS_CHAR                           *acStrMode;
}AT_CELLLOCK_MODE_TBL_STRU;
#endif /* FEATURE_ON == MBB_WPG_CELLLOCK */

#if (FEATURE_ON == MBB_WPG_SYSCFGEX)
#define MN_MMA_LTE_LOW_BAND_ALL         (MN_MMA_LTE_EUTRAN_BAND1 | MN_MMA_LTE_EUTRAN_BAND2 \
    | MN_MMA_LTE_EUTRAN_BAND3 | MN_MMA_LTE_EUTRAN_BAND4 \
    | MN_MMA_LTE_EUTRAN_BAND5 | MN_MMA_LTE_EUTRAN_BAND6 \
    | MN_MMA_LTE_EUTRAN_BAND7 | MN_MMA_LTE_EUTRAN_BAND8 \
    | MN_MMA_LTE_EUTRAN_BAND9 | MN_MMA_LTE_EUTRAN_BAND10 \
    | MN_MMA_LTE_EUTRAN_BAND11 | MN_MMA_LTE_EUTRAN_BAND12 \
    | MN_MMA_LTE_EUTRAN_BAND13 | MN_MMA_LTE_EUTRAN_BAND14 \
    | MN_MMA_LTE_EUTRAN_BAND15 | MN_MMA_LTE_EUTRAN_BAND16 \
    | MN_MMA_LTE_EUTRAN_BAND17 | MN_MMA_LTE_EUTRAN_BAND18 \
    | MN_MMA_LTE_EUTRAN_BAND19 | MN_MMA_LTE_EUTRAN_BAND20 \
    | MN_MMA_LTE_EUTRAN_BAND21 | MN_MMA_LTE_EUTRAN_BAND22 \
    | MN_MMA_LTE_EUTRAN_BAND23 | MN_MMA_LTE_EUTRAN_BAND24 \
    | MN_MMA_LTE_EUTRAN_BAND25 | MN_MMA_LTE_EUTRAN_BAND26 \
    | MN_MMA_LTE_EUTRAN_BAND27 | MN_MMA_LTE_EUTRAN_BAND28 \
    | MN_MMA_LTE_EUTRAN_BAND29 | MN_MMA_LTE_EUTRAN_BAND30 \
    | MN_MMA_LTE_EUTRAN_BAND31 | MN_MMA_LTE_EUTRAN_BAND32)

#define MN_MMA_LTE_HIGH_BAND_ALL        (MN_MMA_LTE_EUTRAN_BAND33 | MN_MMA_LTE_EUTRAN_BAND34 \
    | MN_MMA_LTE_EUTRAN_BAND35 | MN_MMA_LTE_EUTRAN_BAND36 \
    | MN_MMA_LTE_EUTRAN_BAND37 | MN_MMA_LTE_EUTRAN_BAND38 \
    | MN_MMA_LTE_EUTRAN_BAND39 | MN_MMA_LTE_EUTRAN_BAND40 \
    | MN_MMA_LTE_EUTRAN_BAND41 | MN_MMA_LTE_EUTRAN_BAND42 \
    | MN_MMA_LTE_EUTRAN_BAND43 | MN_MMA_LTE_EUTRAN_BAND44 \
    | MN_MMA_LTE_EUTRAN_BAND45 | MN_MMA_LTE_EUTRAN_BAND46 \
    | MN_MMA_LTE_EUTRAN_BAND47 | MN_MMA_LTE_EUTRAN_BAND48 \
    | MN_MMA_LTE_EUTRAN_BAND49 | MN_MMA_LTE_EUTRAN_BAND50 \
    | MN_MMA_LTE_EUTRAN_BAND51 | MN_MMA_LTE_EUTRAN_BAND52 \
    | MN_MMA_LTE_EUTRAN_BAND53 | MN_MMA_LTE_EUTRAN_BAND54 \
    | MN_MMA_LTE_EUTRAN_BAND55 | MN_MMA_LTE_EUTRAN_BAND56 \
    | MN_MMA_LTE_EUTRAN_BAND57 | MN_MMA_LTE_EUTRAN_BAND58 \
    | MN_MMA_LTE_EUTRAN_BAND59 | MN_MMA_LTE_EUTRAN_BAND60 \
    | MN_MMA_LTE_EUTRAN_BAND61 | MN_MMA_LTE_EUTRAN_BAND62 \
    | MN_MMA_LTE_EUTRAN_BAND63 | MN_MMA_LTE_EUTRAN_BAND64)
#endif /*FEATURE_ON == MBB_WPG_SYSCFGEX*/

#define ID_MSG_L4A_HCSQ_INFO_REQ         (0x0001102A)
#define ID_MSG_L4A_HCSQ_INFO_CNF         (0x0001102B)
#define ID_MSG_L4A_HCSQ_IND                    (0x0001102C)

#define ID_MSG_L4A_LTERSRP_INFO_REQ   (0x0001102D)
#define ID_MSG_L4A_LTERSRP_INFO_CNF   (0x0001102E)
#define ID_MSG_L4A_LTERSRP_IND              (0x0001102F)

/*�豸��������С����Ϊ4,��󳤶�Ϊ8,���ܺ�ĳ���Ϊ32*/
#define MAX_DLCK_ENCODE_LEN (32)
#define MIN_DLCK_CODE_LEN (4)
#define MAX_DLCK_CODE_LEN (8)
#define TAF_OPERATOR_PERSONALISATION_DLCK       0x07   /*�Զ���:ӡ�� TATA �豸����ѯ*/
#define TAF_OPERATOR_PERSONALISATION_SETDLCK    0x08   /*�Զ���:ӡ�� TATA �豸������*/

/*�˼�ͨ��ʱ����PLMN������*/
#define MMA_PLMN_LIST_MAX_LEN    (60)     

/****************************************/
/*       EONS ���뷽ʽ����            */
/****************************************/
#define TAF_PH_EONS_GSM_7BIT_DEFAULT  (0)
#define TAF_PH_EONS_GSM_7BIT_PACK       (1)
#define TAF_PH_EONS_UCS2_COMM              (2)
#define TAF_PH_EONS_UCS2_80                   (3) 
#define TAF_PH_EONS_UCS2_81                   (4) 
#define TAF_PH_EONS_UCS2_82                   (5) 
#define TAF_PH_EONS_ASCII                        (6)
#define TAF_PH_EONS_HEX_ASCII                (7)
/****************************************/
/*       EONS ��ȡ����                            */
/****************************************/
#define AT_EONS_TYPE_DEFAULT                       (1)
#define AT_EONS_TYPE_GMM                             (2)
#define AT_EONS_TYPE_PNN                              (3)
#define AT_EONS_TYPE_LOCAL                           (4)
#define AT_EONS_TYPE_MODULE                          (5)
#define AT_ENOS_PLMN_NAME_DEFAULT_LEN                (20)

/****************************************/
/*       PNN��ȡ��Ϣ�洢�ṹ         */
/****************************************/
#define TAF_PH_OPER_NETWORTNAME_LEN    (32)
#define TAF_PH_OPER_SPN_LEN                      (16)
/****************************************/
/*       PLMN ID ������󳤶�             */
/****************************************/
#define AT_PLMN_ID_MAX_LEN                       (6)
/****************************************/
/*       ��Ӧ�����USC2��󳤶�  */
/****************************************/
#define TAF_PH_OPER_PNN_USC2_CODE_LEN    (128)
#define TAF_PH_OPER_SPN_USC2_CODE_LEN    (TAF_PH_SPN_NAME_MAXLEN)

/*TAF_PH_EVT_EONS_NWNAME_CNF��ֵ������TafAppMma.h�е�TAF_MMA_EVT_NET_SCAN_CNF��ֵ��ͻ*/
#define TAF_PH_EVT_EONS_NWNAME_CNF              (80) /*�ϱ�EONS ����*/

#define AT_NWSCAN_WCDMA         0
#define AT_NWSCAN_CDMA            1
#define AT_NWSCAN_TDSCDMA      2
#define AT_NWSCAN_GSM               3
#define AT_NWSCAN_EDGE             4
#define AT_NWSCAN_LTE_FDD        6
#define AT_NWSCAN_LTE_TDD        7

#define AT_NWSCAN_MAX_PARA_NUM        3
#define AT_NWSCAN_BTU_RAT_NUM          2
#define AT_NWSCAN_BTT_BTL_RAT_NUM   1

#define TAF_PH_IS_WCDMA_RAT            0
#define TAF_PH_IS_TDSCDMA_RAT          1
#if(FEATURE_ON == MBB_WPG_SIM_SWITCH)
#define SIM_SWITCH_OPEN        (1)
#endif/* FEATURE_ON == MBB_WPG_SIM_SWITCH */


#if (FEATURE_ON == MBB_WPG_HCSQ)
#define AT_HCSQ_LTE_RSSI_VALUE_MIN           (-120)
#define AT_HCSQ_LTE_RSSI_VALUE_MAX           (-25)
#define AT_HCSQ_LTE_RSSI_LEVEL_MIN            (0)
#define AT_HCSQ_LTE_RSSI_LEVEL_MAX            (96)   
#define AT_HCSQ_LTE_RSRP_VALUE_MIN           (-140)
#define AT_HCSQ_LTE_RSRP_VALUE_MAX           (-44)
#define AT_HCSQ_LTE_RSRP_LEVEL_MIN            (0)
#define AT_HCSQ_LTE_RSRP_LEVEL_MAX            (97) 
#define AT_HCSQ_LTE_RSRQ_VALUE_MIN           (-20)
#define AT_HCSQ_LTE_RSRQ_VALUE_MAX           (-3)
#define AT_HCSQ_LTE_RSRQ_LEVEL_MIN            (0)
#define AT_HCSQ_LTE_RSRQ_LEVEL_MAX           (34)
#define AT_HCSQ_LTE_SINR_VALUE_MAX            (30)
#define AT_HCSQ_LTE_SINR_VALUE_MIN           (-20)
#define AT_HCSQ_LTE_SINR_LEVEL_MAX            (251)
#define AT_HCSQ_LTE_SINR_LEVEL_MIN            (0)
#define AT_HCSQ_LTE_UNKNOWN                   (99)
#define AT_HCSQ_LTE_INVALID                   (255)
#define AT_HCSQ_LTE_SINR_LEVEL               (23)
#endif/*FEATURE_ON == MBB_WPG_HCSQ*/



#if (FEATURE_ON == MBB_MLOG)
#define AT_HIGH_QULITY_RSRP_MIN              (-110)
#endif
#if(FEATURE_ON == MBB_WPG_AC)
#define AC_CARRIER_NUM_MAX            (2) /*DC-HSPA+ ģʽ�µ�����ز���Ŀ*/
/* 200KHz bandwidth */
#define GSM_FREQ_SPREAD_HZ            (200000)
/* 5MHz bandwidth */
#define WCDMA_FREQ_SPREAD_HZ          (5000000)
/* 1.4MHz bandwidth */
#define LTE_BW_BANDWIDTH_N6           (1400000)
/* 3MHz bandwidth */
#define LTE_BW_BANDWIDTH_N15          (3000000)
/* 5MHz bandwidth */
#define LTE_BW_BANDWIDTH_N25          (5000000)
/* 10MHz bandwidth */
#define LTE_BW_BANDWIDTH_N50          (10000000)
/* 15MHz bandwidth */
#define LTE_BW_BANDWIDTH_N75          (15000000)
/* 20MHz bandwidth */
#define LTE_BW_BANDWIDTH_N100         (20000000)
/* 1.6MHz bandwidth for TDSCDMA*/
#define TDSCDMA_FREQ_SPREAD_HZ        (1600000)

/*C���ϱ���Ƶ�ʵ�λΪ100kHz*/
#define FREQ_CHANGE_TO_HZ             (100000)


typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16 usMsgName;
    VOS_UINT32 ulScellFreq[AC_CARRIER_NUM_MAX];
    VOS_UINT32 ulFreqSpread;
    VOS_UINT32 ulNoisePower;
    VOS_UINT16 usReserved;
}TAF_MMA_MSG_AC_REPORT_STRU;

typedef struct
{
    VOS_UINT32 ulScellFreq[AC_CARRIER_NUM_MAX];
    VOS_UINT32 ulFreqSpread;
    VOS_UINT32 ulNoisePower;
}AT_MMA_MSG_AC_REPORT_STRU;

enum TAF_AC_BANDWIDTH_ENUM
{
    TAF_LTE_BANDWIDTH_N6                  = 0,                /* 1.4MHz */
    TAF_LTE_BANDWIDTH_N15                 = 1,                /* 3MHz */
    TAF_LTE_BANDWIDTH_N25                 = 2,                /* 5MHz */
    TAF_LTE_BANDWIDTH_N50                 = 3,                /* 10MHz */
    TAF_LTE_BANDWIDTH_N75                 = 4,                /* 15MHz */
    TAF_LTE_BANDWIDTH_N100                = 5,                /* 20MHz */
    TAF_GSM_BANDWIDTH                     = 6,
    TAF_WCDMA_BANDWIDTH                   = 7,
    TAF_TDSCDMA_BANDWIDTH                 = 8,
    TAF_AC_BANDWIDTH_MAX
};
#endif/* FEATURE_ON == MBB_WPG_AC */

#if (FEATURE_ON == MBB_WPG_SBB_RRCSTAT)
#define AT_RRC_STAT_UNKNOWN   (255)
#endif /*(FEATURE_ON == MBB_WPG_SBB_RRCSTAT)*/

/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/

/*�豸���Ĳ�������*/
typedef enum
{
     DEVICE_LOCK_MODE_DISABLE,    /*ȥ�����豸��*/
     DEVICE_LOCK_MODE_ENABLE,     /*�����豸��*/
     DEVICE_LOCK_MODE_INPUT,      /*��������*/
     DEVICE_LOCK_MODE_MODIFY,     /*�޸�����*/
     DEVICE_LOCK_MODE_MAX
}TATA_DEVICELOCK_ENUM;

/*���䱾��PLMN LIST �б�ѡ���ʶMNCλ��*/
enum PLMN_LIST_ID_LEN
{
    PLMN_ID_LEN_5 = 5,
    PLMN_ID_LEN_6,
    PLMN_ID_LEN_MAX
};
/*���䱾��PLMN LIST �б�ѡ���ʶPLMN ��ʽ*/
enum PLMN_LIST_NETWORKMODE
{
    PLMN_LIST_NETWORKMODE_GSM = 0,
    PLMN_LIST_NETWORKMODE_UMTS,
    PLMN_LIST_NETWORKMODE_LTE,
    PLMN_LIST_NETWORKMODE_CDMA,
    PLMN_LIST_NETWORKMODE_TDSCDMA,
    PLMN_LIST_NETWORKMODE_MAX
};

enum AT_LTERSRP_RAT_ENUM
{
    AT_LTERSRP_RAT_GSM = 0,
    AT_LTERSRP_RAT_WCDMA,
    AT_LTERSRP_RAT_LTE,
    AT_LTERSRP_RAT_BUTT
};

enum AT_HFREQINFO_RAT_TYPE_ENUM
{
    AT_HFREQINFO_RAT_TYPE_GSM = 1,
    AT_HFREQINFO_RAT_TYPE_CDMA, 
    AT_HFREQINFO_RAT_TYPE_WCDMA, 
    AT_HFREQINFO_RAT_TYPE_TDSCDMA, 
    AT_HFREQINFO_RAT_TYPE_WIMAX, 
    AT_HFREQINFO_RAT_TYPE_LTE       
};


/*****************************************************************************
  4 ��Ϣͷ����
*****************************************************************************/


/*****************************************************************************
  5 ��Ϣ����
*****************************************************************************/


/*****************************************************************************
  6 STRUCT����
*****************************************************************************/
/* LOCK����ṹ���� */
typedef struct
{
    VOS_UINT32 ulCmdIndex;
    VOS_UINT32 ulChkFlag;
    const VOS_UINT8* pszCmdName;
    VOS_UINT16 reserved1;
    VOS_UINT16 reserved2;
}AT_LOCK_CMD_TAB_STRU;

typedef struct
{
    VOS_UINT32 ulPid;        /* ���AT����DIAG�Ķ�Ӧ�� L4A����Ϣ����PID*/
    VOS_UINT32 ulClientId;   /* ��AT����*/
    VOS_UINT32 ulOpId;       /* �����֣���Զ������ÿ�������һ,���û����*/
} L4A_APPCTRL_STRU;

typedef struct
{
    L4A_APPCTRL_STRU stCtrl;
    VOS_UINT32 ulErrorCode;    /*������*/
    VOS_INT16 sRssi;          /*Rssi*/
    VOS_UINT16 usLevel;        /*����ֶ�*/
    VOS_INT16 sRsrp;          /*Rsrp*/
    VOS_INT16 sRsrq;          /*Rsrq*/
    VOS_INT32 sSinr;
} L4A_HCSQ_STRU;

typedef struct
{
    L4A_APPCTRL_STRU stCtrl;
    VOS_UINT32 ulErrorCode;    /*query result*/
    VOS_UINT16 usRssi;         /*Rssi*/
    VOS_UINT16 usLevel;        /* ����ֶ�*/
    VOS_INT16 sRsrp;          
    VOS_INT16 sRsrq;           
} L4A_LTERSRP_STRU;
#if (FEATURE_ON == MBB_WPG_HFREQINFO)
/* AT_HFREQINFO_LTE_INFO_STRU����ṹ���� */
typedef struct
{
    VOS_UINT8  usBand;
    VOS_UINT8  reserved;
    VOS_UINT16 usNdl;
    VOS_UINT16 usDlFreq;
    VOS_UINT16 usDlBandwidth;
    VOS_UINT16 usNul;
    VOS_UINT16 usUlFreq;
    VOS_UINT16 usUlBandwidth;
}AT_HFREQINFO_LTE_INFO_STRU;
#endif
/*******************************************/
/*        AT^EONS����ṹ              */
/*******************************************/

typedef struct
{
    TAF_UINT16                 ucEonsType;
    TAF_UINT16                 ulLsNameLen;    
}AT_TAF_EONS_LEN_STRU;

typedef struct
{
    TAF_UINT8                  PlmnLen;                                                /* PLMN���� */
    VOS_CHAR                 PLMNId[AT_PLMN_ID_MAX_LEN + 1];          /* PLMN */
    TAF_UINT8                  EonsType;                                               /*EONS ��������*/
    AT_TAF_EONS_LEN_STRU       PlmnNameLen;  /*plmn_name_len*/
}AT_TAF_PLMN_ID;


typedef struct
{
    TAF_PLMN_ID_STRU      PlmnId;            /* MCC, MNC�洢�ṹ */
    TAF_UINT8                   PlmnLen;          /* PLMN���� */
    TAF_UINT8                   ucServiceStatus; /*��־��ǰ����״̬*/
    TAF_UINT8                   ucOnPlmn;          /*��ʶ��ǰ�Ƿ��ѯ��פPLMN*/
    TAF_UINT8                   ucEonsType;
}TAF_PH_PLMN_INFO_STRU;

typedef struct
{
    TAF_UINT8                   ucNameLen;                                 /* ���� */
    TAF_UINT8                   ucNamecode;                                /* ���� */
    TAF_UINT8                   aucName[TAF_PH_OPER_PNN_USC2_CODE_LEN + 1];     /* �����ַ��� */
    TAF_UINT8                   align;
}TAF_PH_NWNAME_INFO_STRU;

typedef struct
{
    TAF_PH_NWNAME_INFO_STRU  stLNameInfo;
    TAF_PH_NWNAME_INFO_STRU  stSNameInfo;
}TAF_PH_FSNWNAME_INFO_STRU;

typedef struct
{
    TAF_UINT8                   ucDispMode;                     /* spn��ʾģʽ */
    TAF_UINT8                   ucSpnLen;                       /* spn���� */
    TAF_UINT8                   ucSPNCode;                      /* spn���뷽ʽ */
    TAF_UINT8                   aucSpn[TAF_PH_OPER_SPN_USC2_CODE_LEN + 1];      /* SPN���� */
}TAF_PH_EONS_SPN_INFO_STRU;

typedef struct
{
    TAF_PH_PLMN_INFO_STRU             stPlmnInfo;    /* PLMN��Ϣ */
    TAF_PH_EONS_SPN_INFO_STRU         stSpnInfo;     /* SPN��Ϣ */
    TAF_PH_FSNWNAME_INFO_STRU         stNWName;      /* ����������Ϣ */
}TAF_PH_EONS_INFO_STRU;


typedef struct
{
    TAF_PHONE_EVENT                  PhoneEvent;
    MN_OPERATION_ID_T               OpId;
    MN_CLIENT_ID_T                     ClientId;
    TAF_PH_EONS_INFO_STRU        stEonsInfo;          /* EONS��ѯ��Ϣ */
}TAF_PH_EONS_INFO_RSP_STRU;

/*��ȡPLMN LISTֻ��Ҫ��ȡ�������������¶���ṹ��
ֻ��ID�ͳ����Լ�MNC����*/
typedef struct
{
    TAF_UINT8                   PlmnLen;
    TAF_PLMN_ID_STRU            PlmnId;
    TAF_CHAR                    aucOperatorNameLong[TAF_PH_OPER_NAME_LONG ];
}TAF_PH_PLMN_NAME_STRU;
/*˫�˼�ͨ�Žṹ�壬��Ҫ��ʶ��ǰCALL BACK���ͼ�
Я����PLMN ��Ϣ����*/
typedef struct
{
    TAF_PHONE_EVENT             PhoneEvent;
    MN_OPERATION_ID_T           OpId;
    MN_CLIENT_ID_T              ClientId;
    TAF_UINT16                  usPlmnListNums;
    TAF_UINT16                  usPlmnListtotalNums;
    TAF_PH_PLMN_NAME_STRU       PlmnListInfo[MMA_PLMN_LIST_MAX_LEN];
}TAF_PH_PLMN_INFO_LIST_STRU; 


typedef struct
{    
    TAF_UINT8                 ucRssiValue;          /*ת���ȼ�����ź�ǿ��*/
    TAF_UINT8                 ucRSCP;                 /*3G�µ�RSCP��4G�µ�RSRP*/
    TAF_UINT8                 ucEcio;                  /*3G�µ�ECIO��4G�µ�SINR*/
    TAF_UINT8                 ucRsrq;                  /*4G�µ�RSRQ*/
    TAF_UINT8                 ucSysMode;           /*ϵͳģʽ*/
    TAF_UINT8                 ucSinr;                   /*CDMA�µ�SINR��Ԥ��*/
}TAF_PH_HCSQ_STRU;

typedef VOS_UINT8 AT_LTERSRP_RAT_ENUM_UINT8;
typedef VOS_UINT8 AT_HFREQINFO_REPORT_TYPE;
typedef VOS_UINT8 AT_HFREQINFO_RAT_TYPE_ENUM_UINT8;
typedef struct 
{
    VOS_UINT32 ulDLfreqlow;/* ��λΪ100khz*/
    VOS_UINT32 ulNoffdl;
    VOS_UINT32 ulULfreqlow;/* ��λΪ100khz*/
    VOS_UINT32 ulNofful;
}LTE_BANDINFO_STRU;

#if (FEATURE_ON == MBB_WPG_AT)
typedef VOS_UINT32 (*AT_L4A_MSG_FUN)(VOS_VOID* pMsgBlock);
typedef struct
{
    VOS_UINT32 ulMsgId;
    AT_L4A_MSG_FUN atL4aMsgProc;
}AT_L4A_MSG_FUN_TABLE_STRU;
extern AT_L4A_MSG_FUN_TABLE_STRU* atL4aGetCnfMsgFunMbb(VOS_UINT32 ulMsgId);
extern AT_L4A_MSG_FUN_TABLE_STRU* atL4aGetIndMsgFunMbb(VOS_UINT32 ulMsgId);
#endif/*FEATURE_ON == MBB_WPG_AT*/

typedef enum{
    TIME_FORMAT_RPT_TIME,
    TIME_FORMAT_RPT_NWTIME,
    TIME_FORMAT_QRY_NWTIME,
    TIME_FORMAT_QRY_CCLK,
}TIME_FORMAT_ENUM;
typedef VOS_UINT32 TIME_FORMAT_ENUM_UINT32;

enum  APP_VCOM_DISABLE_ENUM {
    VCOM_DISABLE_NONE                   = 0,
    VCOM_DISABLE_WRITE                  = (1 << 0),
    VCOM_DISABLE_READ                   = (1 << 1),

    VCOM_DISABLE_ALL                    = 0xffff
};


#if(FEATURE_ON == MBB_WPG_SIM_SWITCH)
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16                  usMsgName;
    VOS_UINT8                   ucCardState;
    VOS_UINT8                   ucCardSwitch;
}MN_MSG_SIM_SWITCH_STRU;
#endif/* FEATURE_ON == MBB_WPG_SIM_SWITCH */

#if (FEATURE_ON == MBB_FEATURE_GPS)
#define MN_MSG_EFSMS_LEN                 176 /*EFSMS�ļ��ĳ���*/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16 usMsgName;
    VOS_UINT8  aucSmContent[MN_MSG_EFSMS_LEN];
    VOS_UINT16 usSmContentLen;
    VOS_UINT8  aucNIMsgData[MN_MSG_EFSMS_LEN];
    VOS_UINT16 usNIMsgDataLen;
}MN_MSG_NI_DATA_STRU;
#endif /*FEATURE_ON == MBB_FEATURE_GPS*/

#if (FEATURE_ON == MBB_WPG_SBB_RRCSTAT)

typedef struct
{
    MSG_HEADER_STRU               MsgHeader; 
    TAF_MMA_CTRL_STRU            stCtrl;
    VOS_UINT32                           isRrcConnExist;
}MMA_MSCC_RRC_STAT_CNF_STRU;

typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    TAF_MMA_MSG_TYPE_ENUM_UINT32        ulMsgName;                              /*_H2ASN_Skip*/
    TAF_MMA_CTRL_STRU                   stCtrl;
}TAF_MMA_RRC_STAT_QRY_REQ_STRU;
#endif /*(FEATURE_ON == MBB_WPG_SBB_RRCSTAT)*/

#if (FEATURE_ON == MBB_WPG_CFUN)

typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgName;                              /*_H2ASN_Skip*/
    VOS_UINT8                           ucOpid;
    VOS_UINT8                           aucReserve[1];
    USIMM_CARD_SERVIC_ENUM_UINT32       ucSimServiceStatus;  /* SIM ������״̬*/
}TAF_MMA_CFUN_USIM_STATUS_IND_STRU;
#endif/*FEATURE_ON == MBB_WPG_CFUN*/
/*****************************************************************************
  7 UNION����
*****************************************************************************/


/*****************************************************************************
  8 OTHERS����
*****************************************************************************/


/*****************************************************************************
  9 ȫ�ֱ�������
*****************************************************************************/
extern VOS_UINT32                                                    g_ulAtUsbDebugFlag;
extern VOS_CHAR                                                      g_szBufForDebug[BUFFER_LENGTH_256 + 1];

#if (FEATURE_ON == MBB_WPG_MODULE_AT)
extern VOS_UINT8                            gaucAtCmdNotSupportStrNV[];   /*NV�洢�����֧����ʾ�ִ�*/
#endif /* FEATURE_ON == MBB_WPG_MODULE_AT */
/*****************************************************************************
  10 ��������
*****************************************************************************/
#if (FEATURE_ON == MBB_WPG_EONS)
extern TAF_UINT16 At_TafCallBackNWNameProc(TAF_UINT8* pData);
#endif/*FEATURE_ON == MBB_WPG_EONS*/

#if (FEATURE_ON == MBB_WPG_DIAL)
extern VOS_VOID AT_PS_HangupAllCall(VOS_UINT16 usClientId);
VOS_UINT32 TAF_PS_GetDsFlowInfo(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucUsrCid,
    VOS_UINT8                           ucOpId
);
#endif/*FEATURE_ON == MBB_WPG_DIAL*/

#if (FEATURE_ON == MBB_WPG_IMSI_POLLING)
extern VOS_VOID SI_PIH_InitImsiCheckTimer(VOS_VOID);
#endif/*FEATURE_ON == MBB_WPG_IMSI_POLLING*/

#if (FEATURE_ON == MBB_WPG_NWTIME)
extern TAF_UINT16 At_PrintNwTimeInfo(
    NAS_MM_INFO_IND_STRU                *pstMmInfo,
    TAF_UINT16          usLength,
    CONST_T VOS_CHAR *cmd,
    CONST_T VOS_CHAR *cmd_sep,
    CONST_T VOS_CHAR *ending_str,
    TIME_FORMAT_ENUM_UINT32 eTimeFormat);
#endif/*FEATURE_ON == MBB_WPG_NWTIME*/
#if (FEATURE_ON == MBB_WPG_COMMON)
extern TAF_UINT32 At_AsciiNum2HexString(TAF_UINT8 *pucSrc, TAF_UINT16 *pusSrcLen);
#endif /* FEATURE_ON == MBB_WPG_COMMON */
#if (FEATURE_ON == MBB_FEATURE_BIP)
extern VOS_VOID  MN_PH_GetSysInfoRoamStatus(TAF_PH_SYSINFO_STRU *pstSysInfo);
extern VOS_UINT8 TAF_SDC_GetServiceStatus(VOS_VOID);
#endif /* FEATURE_ON == MBB_FEATURE_BIP */
#if (FEATURE_ON == MBB_WPG_MODULE_AT)
extern VOS_VOID  AT_MBB_Report_CPBF(VOS_UINT8 ucIndex, VOS_UINT16 *usBufLen);
/*****************************************************************************
 �� �� ��  : AT_ModemCheckPdpIdleState
 ��������  : �����Ƿ���idel��deacting��PDP
 �������  : NULL
 �������  : NULL
 �� �� ֵ  :
 ���ú���  :
 ��������  :
*****************************************************************************/
VOS_UINT32 AT_ModemCheckPdpIdleState(VOS_VOID);
TAF_UINT32 At_MBB_SetFPara(TAF_UINT8 ucIndex);
#endif /* FEATURE_ON == MBB_WPG_MODULE_AT */

#if (FEATURE_ON == MBB_WPG_AT)
VOS_BOOL AT_CheckIsQuietModeOfAtQ(VOS_UINT8 ucIndex);
VOS_BOOL AT_CheckIsBrevityModeOfAtV(VOS_UINT8 ucIndex);
#endif

#if (FEATURE_ON == MBB_WPG_HFREQINFO)
extern AT_HFREQINFO_REPORT_TYPE g_AtHFreqinforeport;
extern VOS_UINT32 atLwclashCnfProcMbb(VOS_VOID *pMsgBlock);
#endif/*FEATURE_ON == MBB_WPG_HFREQINFO*/


extern VOS_UINT32 AT_QryCurrSysMode( VOS_UINT8 ucIndex );
#if (FEATURE_ON == MBB_WPG_AT_DEBUG)
#define AT_USB_DEBUG_PRINT()        {\
                                        if (VOS_TRUE == g_ulAtUsbDebugFlag)\
                                        {\
                                            (VOS_VOID)vos_printf("===>%s,%d \n",__func__, __LINE__);\
                                        }\
                                    }
 
#define AT_USB_DEBUG_PRINT_1(var1)  {\
                                        if (VOS_TRUE == g_ulAtUsbDebugFlag)\
                                        {\
                                            (VOS_VOID)vos_printf("===>%s, %d msg id = %d\n", __func__, __LINE__, var1);\
                                        }\
                                    }

#define AT_USB_DEBUG_PRINT_3(var1, var2, var3) {\
    if (VOS_TRUE == g_ulAtUsbDebugFlag)\
    {\
        MBB_MEM_SET_S(g_szBufForDebug, (BUFFER_LENGTH_256 + 1), 0, (BUFFER_LENGTH_256 + 1));\
        MBB_STR_NCPY_S(g_szBufForDebug, (BUFFER_LENGTH_256 + 1), var3, (var2 > BUFFER_LENGTH_256) ? BUFFER_LENGTH_256 : (var2 > 0 ? (var2 - 1) : 0));\
        (VOS_VOID)vos_printf("===>%s: %d, PortNo = %d, length = %d, data = [%s]\n",\
         __func__, __LINE__, var1, var2, g_szBufForDebug);\
    }\
}

#define AT_USB_DEBUG_PRINT_5(var1, var2, var3, var4, var5)  {\
    if (VOS_TRUE == g_ulAtUsbDebugFlag)\
    {\
        MBB_MEM_SET_S(g_szBufForDebug, (BUFFER_LENGTH_256 + 1), 0, (BUFFER_LENGTH_256 + 1));\
        MBB_STR_NCPY_S(g_szBufForDebug, (BUFFER_LENGTH_256 + 1), var4, (var5 > BUFFER_LENGTH_256) ? BUFFER_LENGTH_256 : (var5 > 0 ? (var5 - 1) : 0));\
        (VOS_VOID)vos_printf("===>%s, %d mode=%d, user = %d, port_no = %d, buffer = [%s]\n",\
        __func__, __LINE__, var1, var2, var3, g_szBufForDebug);\
    }\
}
 
#define AT_USB_WORK_MODE_PRINT(ucIndex,modetype,mode)       {\
    if (VOS_TRUE == g_ulAtUsbDebugFlag)\
    {\
        (VOS_VOID)vos_printf("===>%s, %d index = %d, modetype = %d mode = %d\n",__func__, __LINE__, ucIndex, modetype, mode);\
    }\
}

#define  AT_CLIENT_STATUS_PRINT_2(ucIndex,client_status) {\
    if (VOS_TRUE == g_ulAtUsbDebugFlag)\
    {\
        (VOS_VOID)vos_printf("===>%s, %d AT_FW_CLIENT_STATUS : %d Index=%d\n",__func__, __LINE__, client_status, ucIndex);\
    }\
}

#if (FEATURE_ON == MBB_ATNLPROXY)
extern int g_current_user_at;
#define AT_STOP_TIMER_CMD_READY(ucIndex)  {\
                            (VOS_VOID)AT_StopRelTimer(ucIndex, &gastAtClientTab[ucIndex].hTimer);\
                            g_stParseContext[ucIndex].ucClientStatus = AT_FW_CLIENT_STATUS_READY;\
                            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CURRENT_OPT_BUTT;\
                            gastAtClientTab[ucIndex].opId = 0;\
                            if (VOS_TRUE == g_ulAtUsbDebugFlag)\
                            {\
                                (VOS_VOID)vos_printf("===>%s, %d AT_FW_CLIENT_STATUS : %d Index = %d\n", __func__, __LINE__, \
                                                                        g_stParseContext[ucIndex].ucClientStatus, ucIndex);\
                            }\
                            if(g_current_user_at == ucIndex)\
                            { gastAtClientTab[AT_CLIENT_ID_APP].usIsWaitAts = 0;g_current_user_at = -1;}\
                        }
#else
#define AT_STOP_TIMER_CMD_READY(ucIndex)  {\
                            (VOS_VOID)AT_StopRelTimer(ucIndex, &gastAtClientTab[ucIndex].hTimer);\
                            g_stParseContext[ucIndex].ucClientStatus = AT_FW_CLIENT_STATUS_READY;\
                            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CURRENT_OPT_BUTT;\
                            gastAtClientTab[ucIndex].opId = 0;\
                            if (VOS_TRUE == g_ulAtUsbDebugFlag)\
                            {\
                                (VOS_VOID)vos_printf("===>%s, %d AT_FW_CLIENT_STATUS : %d Index = %d\n", __func__, __LINE__, \
                                                                        g_stParseContext[ucIndex].ucClientStatus, ucIndex);\
                            }\
                        }
#endif/*FEATURE_ON == MBB_ATNLPROXY*/
#endif/*FEATURE_ON == MBB_WPG_AT_DEBUG*/

#if ((FEATURE_ON == FEATURE_HISOCKET) || (VOS_WIN32 == VOS_OS_VER))
extern VOS_VOID AT_SuspendSockOmServer(VOS_VOID);
extern VOS_VOID AT_WakeUpSockOmServer(VOS_VOID);
extern VOS_VOID AT_InitSockOmRunSem(VOS_VOID);
extern VOS_BOOL AT_IsSockForbidAtCmd(const VOS_UINT8 ucIndex, const VOS_CHAR *pszCmdName);
VOS_UINT32 PPM_SockOmGetIpaddr(VOS_VOID);
VOS_VOID PPM_RegisterInetAddrNotifier(VOS_VOID);
VOS_VOID AT_InitIpSockOmRunSem(VOS_VOID);
VOS_VOID AT_GetIpSuspendSockOmServer(VOS_VOID);
#endif /*((FEATURE_ON == FEATURE_HISOCKET) || (VOS_WIN32 == VOS_OS_VER))*/

#endif/*FEATURE_ON == MBB_WPG_COMMON*/
#if (FEATURE_OFF == MBB_WPG_AT_DEBUG)
#define AT_USB_DEBUG_PRINT()  
#define AT_USB_DEBUG_PRINT_1(var1)  
#define AT_USB_DEBUG_PRINT_3(var1, var2, var3)
#define AT_USB_DEBUG_PRINT_4(var1, var2, var3, var4) 
#define AT_USB_DEBUG_PRINT_5(var1, var2, var3, var4, var5) 
#define AT_USB_WORK_MODE_PRINT(ucIndex,modetype,mode)
#define  AT_CLIENT_STATUS_PRINT_2(ucIndex,client_status) 
#endif/*FEATURE_ON == MBB_WPG_AT_DEBUG*/

#if (FEATURE_ON == MBB_WPG_M2M_NETSCAN)

enum TAF_PH_SIGN_SYS_CFG_SET_ENUM
{
    TAF_PH_SIGN_SYS_CFG_SET_NONE,               /* ��ǰ����NETSCAN */
    TAF_PH_SIGN_NET_SCAN_SYS_CFG_SET_RAT_ONLY,  /* ��ǰNETSCAN�����޸�ϵͳģʽ */
    TAF_PH_SIGN_NET_SCAN_SYS_CFG_SET_REVERT,    /* ��ǰNETSCAN���ڻָ�ϵͳģʽ */
    TAF_PH_SIGN_SYS_CFG_SET_BUTT
};
typedef VOS_UINT8 TAF_PH_SIGN_SYS_CFG_SET_ENUM_U8;
#endif /* FEATURE_ON == MBB_WPG_M2M_NETSCAN */

#if (FEATURE_ON == MBB_WPG_MODULE_AT)

enum TAF_LTE_DUPLEX_MODE_ENUM
{
    TAF_LTE_DUPLEX_MODE_NONE        = 0,
    TAF_LTE_DUPLEX_MODE_FDD         = 1,
    TAF_LTE_DUPLEX_MODE_TDD         = 2,
    TAF_LTE_DUPLEX_MODE_BUTT
};
typedef VOS_UINT8 TAF_LTE_DUPLEX_MODE_ENUM_UINT8;
#endif /* FEATURE_ON == MBB_WPG_MODULE_AT */

#ifdef __cplusplus
}
#endif


#endif /*__AT_H__
 */





