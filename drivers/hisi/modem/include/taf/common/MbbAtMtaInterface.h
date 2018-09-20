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


#ifndef __MBB_ATMTAINTERFACE_H__
#define __MBB_ATMTAINTERFACE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "PsTypeDef.h"
#include "AtMnInterface.h"
#include "AtMtaInterface.h"
//#include "MtaPhyInterface.h"
#include "NasNvInterface.h"
/*****************************************************************************
  2 �궨��
*****************************************************************************/

#if(FEATURE_ON == MBB_WPG_SIMSQ)
enum USIMM_SIMSQ_ENUM
{
    USIMM_SIMSQ_NOT_INSERTED              = 0,
    USIMM_SIMSQ_INSERTED                      = 1,
    USIMM_SIMSQ_PIN_PUK                        = 2,
    USIMM_SIMSQ_SIMLOCK                        = 3,
    USIMM_SIMSQ_INITIALIZING                 = 10,
    USIMM_SIMSQ_INITIALIZED                   = 11,
    USIMM_SIMSQ_READY                            = 12,
    USIMM_SIMSQ_PUKLOCK_DAMAGE         = 98,
    USIMM_SIMSQ_REMOVED                       = 99,
    USIMM_SIMSQ_INIT_FAIL                      = 100,
    USIMM_SIMSQ_BUTT
};
#endif/*FEATURE_ON == MBB_WPG_SIMSQ*/

#if (FEATURE_ON == MBB_WPG_JD)

enum MTA_AT_JAM_DETECT_RAT_ENUM
{
   MTA_AT_JAM_DETECT_RAT_GSM         = 2,
   MTA_AT_JAM_DETECT_RAT_WCDMA,
   MTA_AT_JAM_DETECT_RAT_TD_SCDMA,
   MTA_AT_JAM_DETECT_RAT_LTE,
   MTA_AT_JAM_DETECT_RAT_ALL,
   MTA_AT_JAM_DETECT_RAT_BUTT
};
typedef VOS_UINT8 MTA_AT_JAM_DETECT_RAT_ENUM_UINT8;
#endif /* FEATURE_ON == MBB_WPG_JD */

/*****************************************************************************
  4 ȫ�ֱ�������
*****************************************************************************/
/*****************************************************************************
  5 ��Ϣͷ����
*****************************************************************************/
/*****************************************************************************
  6 ��Ϣ����
*****************************************************************************/
/*****************************************************************************
  7 STRUCT����
*****************************************************************************/
#if (FEATURE_ON == MBB_WPG_JD)

typedef struct
{
    VOS_UINT8        ucRssiSrhThreshold;/* ����RSSIʱ�������Ҫ�ﵽ��Ƶ�����ֵ��ȡֵ��Χ[0,70]��ʵ���õ�ʱ���70ʹ�� */
    VOS_UINT8        ucRssiSrhFreqPercent;/* ����RSSIʱ�����Ҫ�ﵽ��Ƶ�����ռƵ��������BAND�͸����ź�ȡ�������İٷֱȣ�ȡֵ��Χ��[0,100] */
    VOS_UINT16       usPschSrhThreshold;/* ����PSCHʱ�����ҪС�ڻ���ڵ�Ƶ�����ֵ��ȡֵ��Χ:[0,65535] */
    VOS_UINT8        ucPschSrhFreqPercent;/* ����PSCHʱ�����Ҫ�ﵽ��Ƶ�����ռƵ��������BAND�͸����ź�ȡ�������İٷֱȣ�ȡֵ��Χ:[0,100] */
    VOS_UINT8        aucResert[3];
} AT_MTA_SET_JAM_DETECT_WCDMA_STRU;


typedef struct
{
    VOS_UINT8                           ucFlag;
    VOS_UINT8                           ucMethod;
    VOS_UINT8                           ucThreshold;
    VOS_UINT8                           ucFreqNum;
#if (FEATURE_ON == MBB_WPG_JD)
    MTA_AT_JAM_DETECT_RAT_ENUM_UINT8    enRat;        /* ��������ģʽ�ĸ��ż����������ֵ�ǰ���õ�ģʽ���� */
    VOS_UINT8                           aucResert[3];
    AT_MTA_SET_JAM_DETECT_WCDMA_STRU    stWcdmaPara;
#endif /* FEATURE_ON == MBB_WPG_JD */
} MBB_AT_MTA_SET_JAM_DETECT_REQ_STRU;

#endif /* FEATURE_ON == MBB_WPG_JD */

#if (FEATURE_ON == MBB_WPG_M2M_XCELL)

enum AT_MTA_XCELLINFO_RPT_ENUM
{
    AT_MTA_XCELLINFO_RPT_OFF            = 0,                                /* �����ϱ��ر� */
    AT_MTA_XCELLINFO_RPT_ON             = 1,                                /* �����ϱ����� */
    AT_MTA_XCELLINFO_RPT_BUTT
};
typedef VOS_UINT8 AT_MTA_XCELLINFO_RPT_ENUM_UINT8;


enum AT_MTA_XCELLINFO_TYPE_ENUM
{
    AT_MTA_XCELLINFO_TYPE_GSM               = 0,                                /* GSM��ʽ */
    AT_MTA_XCELLINFO_TYPE_WCDMA             = 2,                                /* WCDMA��ʽ */
    AT_MTA_XCELLINFO_TYPE_LTE               = 5,                                /* LTE��ʽ */
    AT_MTA_XCELLINFO_TYPE_NULL,                                                 /* δע��ɹ� */    
    AT_MTA_XCELLINFO_TYPE_BUTT
};
typedef VOS_UINT8 AT_MTA_XCELLINFO_TYPE_ENUM_UINT8;


typedef struct
{
    VOS_UINT32                          ulMcc;                          /* �ƶ������� */
    VOS_UINT32                          ulMnc;                          /* �ƶ������� */
    VOS_UINT32                          ulCellId;                       /* С��ID */
    VOS_UINT16                          usLac;                          /* λ������ */                          
    VOS_UINT16                          usArfcn;                        /* С������Ƶ���, ȡֵ��Χ[0,1023] */
    VOS_UINT8                           ucBsic;                         /* С��BSIC, ȡֵ��Χ[0,63] */
    VOS_UINT8                           ucRxlev;                        /* С��������ƽ, ȡֵ��ΧΪ[0,63] */
    VOS_UINT8                           aucReserved[2];                 /* ���� */
}AT_MTA_XCELLINFO_GSM_IND_STRU;


typedef struct
{
    VOS_UINT32                          ulMcc;                          /* �ƶ������� */
    VOS_UINT32                          ulMnc;                          /* �ƶ������� */
    VOS_UINT32                          ulCellId;                       /* С��ID */
    VOS_UINT16                          usLac;                          /* λ������ */                          
    VOS_UINT8                           aucReserved[2];                 /* ���� */
}AT_MTA_XCELLINFO_WCDMA_IND_STRU;


typedef struct
{
    VOS_UINT32                          ulMcc;                          /* �ƶ������� */
    VOS_UINT32                          ulMnc;                          /* �ƶ������� */
    VOS_UINT32                          ulCellId;                       /* С��ID */
}AT_MTA_XCELLINFO_LTE_IND_STRU;


typedef struct
{
    AT_MTA_XCELLINFO_RPT_ENUM_UINT8             ucEnable;                           /* �����ϱ����� */
    VOS_UINT8                                   aucReserved[3];                     /* ���� */
}AT_MTA_XCELLINFO_SET_REQ_STRU;


typedef struct
{
    MTA_AT_RESULT_ENUM_UINT32           ulResult;
} MTA_AT_SET_XCELLINFO_CNF_STRU;


typedef struct
{
    MTA_AT_RESULT_ENUM_UINT32                   ulResult;
    AT_MTA_XCELLINFO_RPT_ENUM_UINT8             ucEnable;                           /* �����ϱ����� */
    AT_MTA_XCELLINFO_TYPE_ENUM_UINT8            ucCurType;                          /* ��ǰפ��С����ʽ */            
    VOS_UINT8                                   aucReserved[2];                     /* ���� */

    union
    {
        AT_MTA_XCELLINFO_GSM_IND_STRU           stGsmInfo;                          /* GSMС����Ϣ */
        AT_MTA_XCELLINFO_WCDMA_IND_STRU         stWcdmaInfo;                        /* WCDMAС����Ϣ */
        AT_MTA_XCELLINFO_LTE_IND_STRU           stLteInfo;                          /* LTEС����Ϣ */
    }u;
} MTA_AT_XCELLINFO_IND_STRU;
#endif /* (FEATURE_ON == MBB_WPG_M2M_XCELL) */

#if(FEATURE_ON == MBB_WPG_WIRELESSPARAM)
typedef struct
{
    MTA_AT_RESULT_ENUM_UINT32          enResult;  /* MTA������Ϣ��� */
    VOS_INT16     stxPwr;                         /* GU���书�ʣ���λ:0.1dBm */   
    VOS_INT16     sPuschPwr;                      /* LTE PPusch�ŵ����书�ʣ���λ:1dBm */
    VOS_INT16     sPucchPwr;                      /* LTE PPucch�ŵ� ���书�ʣ���λ:1dBm */
    VOS_INT16     sSrsPwr;                        /* LTE PSrs�ŵ����书�ʣ���λ:1dBm */
    VOS_INT16     sPrachPwr;                      /* LTE PPrach�ŵ����书�ʣ���λ:1dBm */
    VOS_UINT8     urev[2];
}AT_MTA_TX_PWR_QRY_CNF;

#define MCS_INFO_MAXNUM  (5)
typedef struct
{
    VOS_UINT16     usUlMcs;                       /* ����MCS���ƽ��� */
    VOS_UINT16     usDlMcs[2];                    /* ����MCS���ƽ�����MCS[0]: ����0��MCS[1]: ����1 */
    VOS_UINT8      aucReserved[1];
}AT_MCS_INFO_STRU;

typedef struct
{
    MTA_AT_RESULT_ENUM_UINT32          enResult;  /* MTA������Ϣ��� */
    AT_MCS_INFO_STRU stMcsInfo[MCS_INFO_MAXNUM];  /* ������MCS ֵ��ţ�2��5ĿǰԤ�� */
}AT_MTA_MCS_QRY_CNF;

typedef struct
{
    MTA_AT_RESULT_ENUM_UINT32          enResult; /* MTA������Ϣ��� */
    VOS_UINT8   ucSubframeAssign;                /* ��֡���䷽ʽ */
    VOS_UINT8   ucSubframePatterns;              /* ��֡���� */
    VOS_UINT8   aucReserved[2];
}AT_MTA_TDD_QRY_CNF;
#endif /* FEATURE_ON == MBB_WPG_WIRELESSPARAM */

/* copy  include/taf/ccore/mtarrcinterface.h �к�netmonitor�й����� */

/*****************************************************************************
  10 ��������
*****************************************************************************/
#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

