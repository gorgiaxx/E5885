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

#ifndef __MBB_AT_NETSCAN_H__
#define __MBB_AT_NETSCAN_H__

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "AtCtx.h"
#include "AtParse.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)  /*4�ֽڶ���*/

#if (FEATURE_ON == MBB_WPG_M2M_NETSCAN)

/* TD_SCDMA\LTE��ȫ��Ƶ�� */
#define AT_ALL_TL_BAND                  (0xFFFFFFFF)
/* PSC�������ЧֵΪ511 */
#define AT_NETSCAN_PSC_MAX_VALUE        (511)
/* PID�������ЧֵΪ503 */
#define AT_NETSCAN_PID_MAX_VALUE        (503)
/* SYSCFGEX�ṹ���У�ucRatOrderNum�ĸ�����Ϊ1ʱ����ʾONLY */
#define AT_NETSCAN_RAT_ONLY             (1)
/* NETSCAN �л���ģ�󣬵ȴ�פ�����ʱ�� */
#define AT_NETSCAN_WAIT_CAMP_ON_TIMER_LEN (10 * 1000)


enum AT_NETSCAN_STATUS_ENUM
{
    AT_NETSCAN_STATUS_NULL = 0,                  /* NETSCAN ��ʼ״̬ */
    AT_NETSCAN_STATUS_FREQLOCK_QRY,              /* ��Ƶ��ѯ״̬ */
    AT_NETSCAN_STATUS_FREQLOCK_CNF,              /* ��Ƶ�ظ�״̬ */
    AT_NETSCAN_STATUS_QUERY_REQ,                 /* ��ѯ��ǰ�Ƿ�����NETSCAN */
    AT_NETSCAN_STATUS_QUERY_CNF,                 /* ��ѯ��ǰ�Ƿ�����NETSCAN */
    AT_NETSCAN_STATUS_SYSINFOEX_GET_REQ,         /* ��ѯSYSINFOEXϵͳģʽ״̬ */
    AT_NETSCAN_STATUS_SYSINFOEX_GET_CNF,         /* ��ȡ��SYSINFOEXϵͳģʽ״̬ */
    AT_NETSCAN_STATUS_SYSCFGEX_GET_REQ,          /* ��ѯSYSCFGEX��ǰ����״̬ */
    AT_NETSCAN_STATUS_SYSCFGEX_GET_CNF,          /* ��ȡ��SYSCFGEX��ǰ����״̬ */
    AT_NETSCAN_STATUS_CGCATT_DETACH_REQ,         /* ȥע����������״̬ */
    AT_NETSCAN_STATUS_CGCATT_DETACH_CNF,         /* ȥע������ɹ�״̬ */
    AT_NETSCAN_STATUS_SYSCFGEX_SET_REQ,          /* �л���ģ����״̬ */                                          
    AT_NETSCAN_STATUS_SYSCFGEX_SET_CNF,          /* �л���ģ�ɹ�״̬ */  
    AT_NETSCAN_STATUS_WAIT_CAMP_ON,              /* �л���ģ�󣬵ȴ�פ�������� */
    AT_NETSCAN_STATUS_CAMP_ON,                   /* �л���ģ������פ�������� */
    AT_NETSCAN_STATUS_SET_REQ,                   /* NETSCANɨƵ����״̬ */  
    AT_NETSCAN_STATUS_SET_CNF,                   /* NETSCANɨƵ�ظ�״̬ */  
    AT_NETSCAN_STATUS_CGCATT_ATTACH_REQ,         /* ע����������״̬ */
    AT_NETSCAN_STATUS_CGCATT_ATTACH_CNF,         /* ע������ɹ�״̬ */
    AT_NETSCAN_STATUS_SYSCFGEX_SET_REVERT_REQ,   /* �ָ���ģ����״̬ */ 
    AT_NETSCAN_STATUS_SYSCFGEX_SET_REVERT_CNF,   /* �ָ���ģ�ظ�״̬ */ 
    AT_NETSCAN_STATUS_END,                       /* NETSCAN����״̬ */ 
    AT_NETSCAN_STATUS_BUTT
};
typedef VOS_UINT8 AT_NETSCAN_STATUS_ENUM_UINT8;

/* SYSINFOEX��ѯ����ϵͳģʽ */
enum AT_NETSCAN_SYSINFO_MODE_ENUM
{
    AT_NETSCAN_SYSINFO_MODE_NO_SERVICE = 0,
    AT_NETSCAN_SYSINFO_MODE_GSM,
    AT_NETSCAN_SYSINFO_MODE_CDMA,
    AT_NETSCAN_SYSINFO_MODE_WCDMA,
    AT_NETSCAN_SYSINFO_MODE_TD_SCDMA,
    AT_NETSCAN_SYSINFO_MODE_WIMAX,
    AT_NETSCAN_SYSINFO_MODE_LTE,
    AT_NETSCAN_SYSINFO_MODE_BUTT
};
typedef VOS_UINT8 AT_NETSCAN_SYSINFO_MODE_ENUM_UINT8;

/* NETSCAN��Ӧ״̬�Ĵ����� */
typedef VOS_VOID ( *TAF_NETSCAN_STATUS_DRIVE_FUN )(VOS_VOID);

typedef struct
{
    AT_NETSCAN_STATUS_ENUM_UINT8   ucNetscanCurrStatus;
    TAF_NETSCAN_STATUS_DRIVE_FUN   pfNetscanStatusFunc;
}TAF_NETSCAN_STATUS_DRIVE_TABLE_STRU;

/* NETSCAN��Ҫ�������ر�־λ */
typedef struct
{
    VOS_BOOL                             bIsNetscanSetenAble;      /* �Ƿ�����ִ��NETSCAN��ز��� */
    AT_NETSCAN_STATUS_ENUM_UINT8         ucNetscanCurrStatus;      /* ��ǰNETSCAN��״̬������ִ�е���һ���� */
    VOS_BOOL                             bIsSyscfgSaved;           /* �Ƿ񱣴�ԭ��SYSCFGEX���� */
    AT_NETSCAN_SYSINFO_MODE_ENUM_UINT8   ucNetscanSysinfoMode;     /* SYSINFOEX��ѯ���ĵ�ǰϵͳģʽ��Ҳ������ģ */
    TAF_MMA_SYS_CFG_PARA_STRU            stSysCfg;                 /* ����ģ�ԭ��SYSCFGEX���� */
    VOS_BOOL                             bIsNetscanSetParaSaved;   /* �Ƿ񱣴�NETSCAN���õ���ز��� */
    VOS_BOOL                             bIsDetached;              /* �Ƿ���й�ȥע����� */
    VOS_BOOL                             bIsSyscfgexChanged;       /* �Ƿ���й�SYSCFGEX���ò��� */
    VOS_BOOL                             bIsNetscanAborted;        /* �Ƿ���Ҫ�жϣ�
                                                                    �յ��жϺ�������ҪABORT�������г���ERRORҲ��ҪABORT */
    TAF_MMA_NET_SCAN_REQ_STRU            stNetScanSetPara;         /* ����ģ�NETSCAN���õ���ز��� */
    VOS_UINT32                           ulNetscanResult;          /* NETSCAN�Ľ�� AT_RRETURN_CODE_ENUM */
    VOS_UINT8                            ucIndex;                  /* NETSCAN��index */
    VOS_UINT8                            ucReserved0[3];           /*align*/
    VOS_BOOL                             bIsNetscanGotCnf;         /* �Ƿ��Ѿ���ʾ��NETSCAN��ɨƵ�����
                                                                    ����Ѿ���ʾ��ɨƵ�����������ʾulNetscanResult */
    HTIMER                               hTimer;
    VOS_UINT32                           ulTimerName;
    VOS_BOOL                             bForbReport;              /* NETSCAN�ڼ䣬���������ϱ��ı�־λ��TRUEʱ��ʾ��ֹ�ϱ� */
    VOS_BOOL                             bIsFreqlocked;            /* �Ƿ�����Ƶ */
    VOS_BOOL                             bIsTimeout;               /* �Ƿ�ʱ */
    VOS_BOOL                             bIsStateAllowed;          /* �Ƿ�������NETSCAN */ 
}TAF_NETSCAN_STATUS_STRU;

extern TAF_PH_INFO_RAT_TYPE                 g_stNetscanPara2SysMode[TAF_NETSCAN_RAT_BUTT];
extern TAF_NETSCAN_STATUS_STRU              g_stNetscanStatus;
extern TAF_MMA_SYS_CFG_PARA_STRU            g_stNetscanRatOnlySysCfg[TAF_NETSCAN_RAT_BUTT];
extern TAF_NETSCAN_STATUS_DRIVE_TABLE_STRU  g_stNetscanStatusDriveTable[AT_NETSCAN_STATUS_BUTT];
extern TAF_NAS_NETSCAN_RAT_TYPE_ENUM_UINT8  g_stNetscanUserRat2NasMmlRat[TAF_NETSCAN_RAT_BUTT];

extern VOS_BOOL isNetscanEnabled(VOS_VOID);
extern VOS_BOOL isNetscanAbort(VOS_VOID);
extern VOS_BOOL isNetscanNeedRevert(VOS_VOID);
extern VOS_BOOL isNetscanGotCnf(VOS_VOID);
extern VOS_VOID AT_NetscanStatusDrive(VOS_VOID);
extern VOS_VOID AT_NetscanStatusClean(VOS_VOID);
extern VOS_VOID AT_NetscanAbortEnable(VOS_VOID);
extern VOS_VOID AT_NetscanResultSet(VOS_UINT32 ulResult);
extern VOS_VOID AT_NetscanDetached(VOS_VOID);
extern VOS_VOID AT_NetscanStatusEnable(VOS_VOID);
extern VOS_VOID AT_NetscanGotCnf(VOS_VOID);
extern VOS_VOID AT_NetscanSaveSetPara(TAF_MMA_NET_SCAN_REQ_STRU *pstNetScanSetPara);
extern VOS_VOID AT_NetscanSaveSysPara(TAF_MMA_SYS_CFG_PARA_STRU *pstSysPara);
extern VOS_VOID AT_NetscanSaveSysinfoModePara(AT_NETSCAN_SYSINFO_MODE_ENUM_UINT8 ucSysMode);
extern AT_NETSCAN_STATUS_ENUM_UINT8 AT_NetscanStatusGet(VOS_VOID);
extern VOS_VOID AT_NetscanSaveucIndex(VOS_UINT8 ucIndex);
extern VOS_VOID AT_NetscanStatusSet(AT_NETSCAN_STATUS_ENUM_UINT8 ucNetscanCurrStatus);
extern VOS_BOOL AT_NetscanStatusCheck(AT_NETSCAN_STATUS_ENUM_UINT8 ucNetscanCurrStatus);
extern VOS_BOOL AT_NetscanSendReq (VOS_UINT8 ucIndex, TAF_MMA_NET_SCAN_REQ_STRU *pstNetScanSetPara);
extern VOS_BOOL AT_NetscanSysinfoex(VOS_UINT8 ucIndex);
extern VOS_BOOL AT_NetscanSyscfgexRead(VOS_UINT8 ucIndex);
extern VOS_BOOL AT_NetscanSyscfgexSetRatOnly(VOS_UINT8 ucIndex);
extern VOS_BOOL AT_NetscanSyscfgexSetRevert(VOS_UINT8 ucIndex);
extern VOS_BOOL AT_NetscanCgcattDetach(VOS_UINT8 ucIndex);
extern VOS_BOOL AT_NetscanCgcattAttach(VOS_UINT8 ucIndex);
extern VOS_VOID AT_NetscanStatusNullProc(VOS_VOID);
extern VOS_VOID AT_NetscanStatusSysinfoexCnfProc(VOS_VOID);
extern VOS_VOID AT_NetscanStatusSyscfgexGetCnfProc(VOS_VOID);
extern VOS_VOID AT_NetscanStatusDetachCnfProc(VOS_VOID);
extern VOS_VOID AT_NetscanStatusAttachCnfProc(VOS_VOID);
extern VOS_VOID AT_NetscanStatusSyscfgexSetCnfProc(VOS_VOID);
extern VOS_VOID AT_NetscanStatusCampOnProc(VOS_VOID);
extern VOS_VOID AT_NetscanStatusSetCnfProc(VOS_VOID);
extern VOS_VOID AT_NetscanStatusSyscfgexSetRevertCnfProc(VOS_VOID);


extern VOS_UINT32 AT_RcvMmaNetScanQryCnf(VOS_VOID *pData);


VOS_VOID AT_NetscanStatusFreqlockCnfProc(VOS_VOID);
VOS_VOID AT_NetscanStatusQueryCnfProc(VOS_VOID);
#endif/* FEATURE_ON == MBB_WPG_M2M_NETSCAN */


#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of AtCmdImsProc.h */

