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

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "RnicCtx.h"
#include "RnicProcMsg.h"
#include "AtRnicInterface.h"
#include "v_typdef.h"
#include "RnicTimerMgmt.h"
#include "mdrv.h"
#include "RnicEntity.h"
#include "AdsDeviceInterface.h"
#include "RnicLog.h"
#include "PsTypeDef.h"
#include "PsCommonDef.h"
#include "RnicDebug.h"
#include <linux/wakelock.h>

#include "AcpuReset.h"

#include "ImsaRnicInterface.h"

#include "RnicCdsInterface.h"



/*****************************************************************************
    Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_RNIC_PROCMSG_C

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
extern struct wake_lock rnic_wakelock;
/* RNICģ�鶨ʱ����ʱ��Ϣ����������Ӧ�� */
const RNIC_RCV_TI_EXPRIED_PROC_STRU g_astRnicTiExpriedProcTab[]=
{
    /* ��ϢID */                            /* ��ʱ����ʱ�������� */
    {TI_RNIC_DSFLOW_STATS_0,                RNIC_RcvTiDsflowStatsExpired},
    {TI_RNIC_DSFLOW_STATS_1,                RNIC_RcvTiDsflowStatsExpired},
    {TI_RNIC_DSFLOW_STATS_2,                RNIC_RcvTiDsflowStatsExpired},
    {TI_RNIC_DSFLOW_STATS_3,                RNIC_RcvTiDsflowStatsExpired},
    {TI_RNIC_DSFLOW_STATS_4,                RNIC_RcvTiDsflowStatsExpired},
    {TI_RNIC_DEMAND_DIAL_DISCONNECT,        RNIC_RcvTiDemandDialDisconnectExpired},
    {TI_RNIC_DEMAND_DIAL_PROTECT,           RNIC_RcvTiDemandDialProtectExpired}
 };


/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/


VOS_VOID RNIC_MNTN_TraceDialConnEvt(VOS_VOID)
{
    RNIC_MNTN_DIAL_CONN_EVT_STRU       *pstDialEvt = VOS_NULL_PTR;

    /* ������Ϣ */
    pstDialEvt = (RNIC_MNTN_DIAL_CONN_EVT_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                        ACPU_PID_RNIC,
                        sizeof(RNIC_MNTN_DIAL_CONN_EVT_STRU));
    if (VOS_NULL_PTR == pstDialEvt)
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_MNTN_TraceDialEvent: Memory alloc failed.");
        return;
    }

    /* ��д��Ϣ */
    pstDialEvt->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstDialEvt->ulReceiverPid   = ACPU_PID_RNIC;
    pstDialEvt->enMsgId         = ID_RNIC_MNTN_EVT_DIAL_CONNECT;

    /* ������ά�ɲ���Ϣ */
    if (VOS_OK != PS_SEND_MSG(ACPU_PID_RNIC, pstDialEvt))
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_MNTN_TraceDialConnEvt():WARNING:SEND MSG FIAL");
    }

    return;
}


VOS_VOID RNIC_MNTN_TraceDialDisconnEvt(
    VOS_UINT32                          ulPktNum,
    VOS_UINT32                          ulUsrExistFlg
)
{
    RNIC_MNTN_DIAL_DISCONN_EVT_STRU    *pstDialEvt = VOS_NULL_PTR;

    /* ������Ϣ */
    pstDialEvt = (RNIC_MNTN_DIAL_DISCONN_EVT_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                        ACPU_PID_RNIC,
                        sizeof(RNIC_MNTN_DIAL_DISCONN_EVT_STRU));
    if (VOS_NULL_PTR == pstDialEvt)
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_MNTN_TraceDialEvent: Memory alloc failed.");
        return;
    }

    /* ��д��Ϣͷ */
    pstDialEvt->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstDialEvt->ulReceiverPid   = ACPU_PID_RNIC;
    pstDialEvt->enMsgId         = ID_RNIC_MNTN_EVT_DIAL_DISCONNECT;

    /* ��д��Ϣ���� */
    pstDialEvt->ulPktNum        = ulPktNum;
    pstDialEvt->ulUsrExistFlg   = ulUsrExistFlg;

    /* ������ά�ɲ���Ϣ */
    if (VOS_OK != PS_SEND_MSG(ACPU_PID_RNIC, pstDialEvt))
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_MNTN_TraceDialDisconnEvt():WARNING:SEND MSG FIAL");
    }

    return;
}


VOS_UINT32 RNIC_SendDialEvent(
    VOS_UINT32                          ulDeviceId,
    VOS_UINT32                          ulEventId
)
{
    VOS_UINT32                          ulRet;
    VOS_UINT32                          ulSize;
    DEVICE_EVENT                        stEvent;

    ulSize                              = sizeof(DEVICE_EVENT);
    stEvent.device_id                   = (DEVICE_ID)ulDeviceId;
    stEvent.event_code                  = (VOS_INT)ulEventId;
    stEvent.len                         = 0;

    /* �ϱ����貦���¼�*/
    ulRet = (VOS_UINT32)device_event_report(&stEvent, (VOS_INT)ulSize);

    if (VOS_OK != ulRet)
    {
        RNIC_WARNING_LOG(ACPU_PID_RNIC, "RNIC_SendDialEvent: can't add event");
        return VOS_ERR;
    }

    RNIC_INFO_LOG2(ACPU_PID_RNIC, "RNIC_SendDialEvent Done ulEventId  ulDeviceId",ulEventId,ulDeviceId);

    return VOS_OK;

}


RNIC_TIMER_ID_ENUM_UINT16 RNIC_GetDsflowTimerIdByNetId(VOS_UINT8 ucRmNetId)
{
    RNIC_TIMER_ID_ENUM_UINT16           enTimerId;

    switch (ucRmNetId)
    {
        case RNIC_RMNET_ID_0 :
            enTimerId = TI_RNIC_DSFLOW_STATS_0;
            break;

        case RNIC_RMNET_ID_1 :
            enTimerId = TI_RNIC_DSFLOW_STATS_1;
            break;

        case RNIC_RMNET_ID_2 :
            enTimerId = TI_RNIC_DSFLOW_STATS_2;
            break;

        case RNIC_RMNET_ID_3 :
            enTimerId = TI_RNIC_DSFLOW_STATS_3;
            break;

        case RNIC_RMNET_ID_4 :
            enTimerId = TI_RNIC_DSFLOW_STATS_4;
            break;

        default :
            enTimerId = TI_RNIC_TIMER_BUTT;
            break;
    }

    return enTimerId;
}


VOS_UINT8 RNIC_GetNetIdByTimerId(VOS_UINT32 ulMsgId)
{
    VOS_UINT8                           ucRmNedId;

    switch (ulMsgId)
    {
        case TI_RNIC_DSFLOW_STATS_0 :
            ucRmNedId = RNIC_RMNET_ID_0;
            break;

        case TI_RNIC_DSFLOW_STATS_1 :
            ucRmNedId = RNIC_RMNET_ID_1;
            break;

        case TI_RNIC_DSFLOW_STATS_2 :
            ucRmNedId = RNIC_RMNET_ID_2;
            break;

        case TI_RNIC_DSFLOW_STATS_3 :
            ucRmNedId = RNIC_RMNET_ID_3;
            break;

        case TI_RNIC_DSFLOW_STATS_4 :
            ucRmNedId = RNIC_RMNET_ID_4;
            break;

        case TI_RNIC_DEMAND_DIAL_DISCONNECT :
        case TI_RNIC_DEMAND_DIAL_PROTECT :
            ucRmNedId = RNIC_RMNET_ID_0;
            break;

        default :
            ucRmNedId = RNIC_RMNET_ID_BUTT;
            break;
    }

    return ucRmNedId;
}


VOS_UINT32 RNIC_BuildRabIdByModemId(
    MODEM_ID_ENUM_UINT16                enModemId,
    VOS_UINT8                           ucRabId,
    VOS_UINT8                          *pucRabId
)
{
    if (MODEM_ID_0 == enModemId)
    {
        /* Modem0��RABID�ĸ���λ��00��ʾ */
        *pucRabId = ucRabId;
    }
    else
    {
        /* �Ȳ���Modem0Ҳ����Modem1�ģ�����ʧ�� */
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 RNIC_SaveNetIdByRabId(
    MODEM_ID_ENUM_UINT16                enModemId,
    VOS_UINT8                           ucRabId,
    VOS_UINT8                           ucRmNetId
)
{
    RNIC_RABID_INFO_STAU               *pstRabIdInfo;

    if ((ucRabId < RNIC_RAB_ID_MIN)
     || (ucRabId > RNIC_RAB_ID_MAX))
    {
        return VOS_ERR;
    }

    /* ��ȡָ��Modem��RABID��Ϣ */
    pstRabIdInfo = RNIC_GET_SPEC_MODEM_RABID_INFO(enModemId);

    pstRabIdInfo->aucRmNetId[ucRabId - RNIC_RAB_ID_OFFSET] = ucRmNetId;

    return VOS_OK;

}



VOS_UINT32 RNIC_RcvAtIpv4PdpActInd(
    MsgBlock                           *pstMsg
)
{
    AT_RNIC_IPV4_PDP_ACT_IND_STRU      *pstRcvInd;
    RNIC_PDP_CTX_STRU                  *pstPdpAddr;
    RNIC_TIMER_ID_ENUM_UINT16           enTimerId;
    RNIC_SPEC_CTX_STRU                 *pstSpecNetCardCtx;
    VOS_UINT8                           ucRabid;
    VOS_UINT8                           ucRmNetId;
    pstRcvInd                           = (AT_RNIC_IPV4_PDP_ACT_IND_STRU *)pstMsg;
    ucRmNetId                           = pstRcvInd->ucRmNetId;

    /* ��ȡPDP�����ĵ�ַ */
    pstPdpAddr                          = RNIC_GetPdpCtxAddr(ucRmNetId);
    pstSpecNetCardCtx                   = RNIC_GetSpecNetCardCtxAddr(ucRmNetId);

    ucRabid                             = RNIC_RAB_ID_INVALID;

    /* ����modem id������id�����Rabid */
    if (VOS_OK != RNIC_BuildRabIdByModemId(pstSpecNetCardCtx->enModemId,
                                           pstRcvInd->ucRabId,
                                           &ucRabid))
    {
        RNIC_INFO_LOG1(ACPU_PID_RNIC, "RNIC_RcvAtIpv4PdpActInd, modemId:%d", pstSpecNetCardCtx->enModemId);
        return VOS_ERR;
    }

    /* �洢RABID��Ӧ������ID */
    if (VOS_OK != RNIC_SaveNetIdByRabId(pstSpecNetCardCtx->enModemId,
                                        pstRcvInd->ucRabId,
                                        ucRmNetId))
    {
        RNIC_INFO_LOG1(ACPU_PID_RNIC, "RNIC_RcvAtIpv4PdpActInd, RabId:%d", pstRcvInd->ucRabId);
        return VOS_ERR;
    }

    /* Modified by l60609 for L-C��������Ŀ, 2014-1-14, begin */
    pstSpecNetCardCtx->enModemType        = RNIC_MODEM_TYPE_INSIDE;
    /* Modified by l60609 for L-C��������Ŀ, 2014-1-14, end */

    /* ����PDP��������Ϣ */
    pstPdpAddr->stIpv4PdpInfo.enRegStatus = RNIC_PDP_REG_STATUS_ACTIVE;
    pstPdpAddr->stIpv4PdpInfo.ucRabId     = pstRcvInd->ucRabId;
    pstPdpAddr->stIpv4PdpInfo.ulIpv4Addr  = pstRcvInd->ulIpv4Addr;



    /* ��������ID��ȡ����ͳ�ƵĶ�ʱ��ID */
    enTimerId = RNIC_GetDsflowTimerIdByNetId(ucRmNetId);

    /* ��������ͳ�ƶ�ʱ�� */
    RNIC_StartTimer(enTimerId, TI_RNIC_DSFLOW_STATS_LEN);

    /* ֹͣ���貦�ű�����ʱ�� */
    if (RNIC_RMNET_ID_0 == ucRmNetId)
    {
        RNIC_StopTimer(TI_RNIC_DEMAND_DIAL_PROTECT);
    }


    /* ע�����з��ͺ�����ADS����ע��ĺ��������������� */
    ADS_DL_RegDlDataCallback(ucRabid, RNIC_RcvAdsDlData, ucRmNetId);


    return VOS_OK;
}


VOS_UINT32 RNIC_RcvAtIpv6PdpActInd(
    MsgBlock                           *pstMsg
)
{
    AT_RNIC_IPV6_PDP_ACT_IND_STRU      *pstRcvInd;
    RNIC_PDP_CTX_STRU                  *pstPdpAddr;
    RNIC_TIMER_ID_ENUM_UINT16           enTimerId;
    RNIC_SPEC_CTX_STRU                 *pstSpecNetCardCtx;
    VOS_UINT8                           ucRabid;
    VOS_UINT8                           ucRmNetId;

    pstRcvInd                           = (AT_RNIC_IPV6_PDP_ACT_IND_STRU *)pstMsg;
    ucRmNetId                           = pstRcvInd->ucRmNetId;

    /* ��ȡPDP�����ĵ�ַ */
    pstPdpAddr                          = RNIC_GetPdpCtxAddr(ucRmNetId);
    pstSpecNetCardCtx                   = RNIC_GetSpecNetCardCtxAddr(ucRmNetId);

    ucRabid                             = RNIC_RAB_ID_INVALID;

    /* ����modem id������id�����Rabid */
    if (VOS_OK != RNIC_BuildRabIdByModemId(pstSpecNetCardCtx->enModemId,
                                           pstRcvInd->ucRabId,
                                           &ucRabid))
    {
        RNIC_INFO_LOG1(ACPU_PID_RNIC, "RNIC_RcvAtIpv6PdpActInd, modemId:%d", pstSpecNetCardCtx->enModemId);
        return VOS_ERR;
    }

    /* �洢RABID��Ӧ������ID */
    if (VOS_OK != RNIC_SaveNetIdByRabId(pstSpecNetCardCtx->enModemId,
                                        pstRcvInd->ucRabId,
                                        ucRmNetId))
    {
        RNIC_INFO_LOG1(ACPU_PID_RNIC, "RNIC_RcvAtIpv6PdpActInd, RabId:%d", pstRcvInd->ucRabId);
        return VOS_ERR;
    }

    /* Modified by l60609 for L-C��������Ŀ, 2014-1-14, begin */
    pstSpecNetCardCtx->enModemType        = RNIC_MODEM_TYPE_INSIDE;
    /* Modified by l60609 for L-C��������Ŀ, 2014-1-14, end */

    /* ����PDP��������Ϣ */
    pstPdpAddr->stIpv6PdpInfo.enRegStatus = RNIC_PDP_REG_STATUS_ACTIVE;
    pstPdpAddr->stIpv6PdpInfo.ucRabId     = pstRcvInd->ucRabId;
    TAF_MEM_CPY_S(pstPdpAddr->stIpv6PdpInfo.aucIpv6Addr,
               sizeof(pstPdpAddr->stIpv6PdpInfo.aucIpv6Addr),
               pstRcvInd->aucIpv6Addr,
               RNICITF_MAX_IPV6_ADDR_LEN);


    /* ��������ID��ȡ����ͳ�ƵĶ�ʱ��ID */
    enTimerId = RNIC_GetDsflowTimerIdByNetId(ucRmNetId);

    /* ��������ͳ�ƶ�ʱ�� */
    RNIC_StartTimer(enTimerId, TI_RNIC_DSFLOW_STATS_LEN);

    /* ע�����з��ͺ�����ADS����ע��ĺ��������������� */


    ADS_DL_RegDlDataCallback(ucRabid, RNIC_RcvAdsDlData, ucRmNetId);



    return VOS_OK;

}


VOS_UINT32 RNIC_RcvAtIpv4v6PdpActInd(
    MsgBlock                           *pstMsg
)
{
    AT_RNIC_IPV4V6_PDP_ACT_IND_STRU    *pstRcvInd;
    RNIC_PDP_CTX_STRU                  *pstPdpAddr;
    RNIC_TIMER_ID_ENUM_UINT16           enTimerId;
    RNIC_SPEC_CTX_STRU                 *pstSpecNetCardCtx;
    VOS_UINT8                           ucRabid;
    VOS_UINT8                           ucRmNetId;

    pstRcvInd                           = (AT_RNIC_IPV4V6_PDP_ACT_IND_STRU *)pstMsg;
    ucRmNetId                           = pstRcvInd->ucRmNetId;

    /* ��ȡPDP�����ĵ�ַ */
    pstPdpAddr                          = RNIC_GetPdpCtxAddr(ucRmNetId);
    pstSpecNetCardCtx                   = RNIC_GetSpecNetCardCtxAddr(ucRmNetId);

    ucRabid                             = RNIC_RAB_ID_INVALID;

    /* ����modem id������id�����Rabid */
    if (VOS_OK != RNIC_BuildRabIdByModemId(pstSpecNetCardCtx->enModemId,
                                           pstRcvInd->ucRabId,
                                           &ucRabid))
    {
        RNIC_INFO_LOG1(ACPU_PID_RNIC, "RNIC_RcvAtIpv4v6PdpActInd, modemId:%d", pstSpecNetCardCtx->enModemId);
        return VOS_ERR;
    }

    /* �洢RABID��Ӧ������ID */
    if (VOS_OK != RNIC_SaveNetIdByRabId(pstSpecNetCardCtx->enModemId,
                                        pstRcvInd->ucRabId,
                                        ucRmNetId))
    {
        RNIC_INFO_LOG1(ACPU_PID_RNIC, "RNIC_RcvAtIpv4v6PdpActInd, modemId:%d", pstSpecNetCardCtx->enModemId);
        return VOS_ERR;
    }

    /* Modified by l60609 for L-C��������Ŀ, 2014-1-14, begin */
    pstSpecNetCardCtx->enModemType          = RNIC_MODEM_TYPE_INSIDE;
    /* Modified by l60609 for L-C��������Ŀ, 2014-1-14, end */

    /* ����PDP��������Ϣ */
    pstPdpAddr->stIpv4v6PdpInfo.enRegStatus = RNIC_PDP_REG_STATUS_ACTIVE;
    pstPdpAddr->stIpv4v6PdpInfo.ucRabId     = pstRcvInd->ucRabId;
    pstPdpAddr->stIpv4PdpInfo.ucRabId       = pstRcvInd->ucRabId;
    pstPdpAddr->stIpv6PdpInfo.ucRabId       = pstRcvInd->ucRabId;



    /* ��������ID��ȡ����ͳ�ƵĶ�ʱ��ID */
    enTimerId = RNIC_GetDsflowTimerIdByNetId(ucRmNetId);

    /* ��������ͳ�ƶ�ʱ�� */
    RNIC_StartTimer(enTimerId, TI_RNIC_DSFLOW_STATS_LEN);

    /* ֹͣ���貦�ű�����ʱ�� */
    RNIC_StopTimer(TI_RNIC_DEMAND_DIAL_PROTECT);


    /* ע�����з��ͺ�����ADS����ע��ĺ��������������� */
    ADS_DL_RegDlDataCallback(ucRabid, RNIC_RcvAdsDlData, ucRmNetId);


    return VOS_OK;

}



VOS_UINT32 RNIC_RcvAtPdpDeactInd(
    MsgBlock                           *pstMsg
)
{
    AT_RNIC_PDP_DEACT_IND_STRU         *pstRcvInd;
    RNIC_PDP_CTX_STRU                  *pstPdpCtxAddr;
    RNIC_SPEC_CTX_STRU                 *pstSpecNetCardCtx;
    RNIC_TIMER_ID_ENUM_UINT16           enTimerId;
    VOS_UINT8                           ucRmNetId;

    pstRcvInd                           = (AT_RNIC_PDP_DEACT_IND_STRU *)pstMsg;
    ucRmNetId                           = pstRcvInd->ucRmNetId;

    /* ��ȡPDP�����ĵ�ַ */
    pstPdpCtxAddr                       = RNIC_GetPdpCtxAddr(ucRmNetId);
    pstSpecNetCardCtx                   = RNIC_GetSpecNetCardCtxAddr(ucRmNetId);

    /* ��������ID��ȡ����ͳ�ƵĶ�ʱ��ID */
    enTimerId = RNIC_GetDsflowTimerIdByNetId(ucRmNetId);

    /* �����IPV4 PDPȥ���� */
    if ((pstPdpCtxAddr->stIpv4PdpInfo.ucRabId == pstRcvInd->ucRabId)
     && (RNIC_PDP_REG_STATUS_ACTIVE == pstPdpCtxAddr->stIpv4PdpInfo.enRegStatus))
    {
        /* ���IPV4 PDP��������Ϣ */
        RNIC_InitIpv4PdpCtx(&pstPdpCtxAddr->stIpv4PdpInfo);

        /* ������0�ϲ��а��貦�ŵĹ��� */
        if (RNIC_RMNET_ID_0 == ucRmNetId)
        {
            /* ֹͣ���貦�ŶϿ���ʱ�� */
            RNIC_StopTimer(TI_RNIC_DEMAND_DIAL_DISCONNECT);

            /* ��ղ��ŶϿ���ʱ����ʱͳ�� */
            RNIC_ClearTiDialDownExpCount();
        }
    }

    if ((pstPdpCtxAddr->stIpv6PdpInfo.ucRabId == pstRcvInd->ucRabId)
     && (RNIC_PDP_REG_STATUS_ACTIVE == pstPdpCtxAddr->stIpv6PdpInfo.enRegStatus))
    {
        /* ���IPV6 PDP��������Ϣ */
        RNIC_InitIpv6PdpCtx(&pstPdpCtxAddr->stIpv6PdpInfo);
    }

    if ((pstPdpCtxAddr->stIpv4v6PdpInfo.ucRabId == pstRcvInd->ucRabId)
     && (RNIC_PDP_REG_STATUS_ACTIVE == pstPdpCtxAddr->stIpv4v6PdpInfo.enRegStatus))
    {
        /* ���IPV4V6 PDP��������Ϣ */
        RNIC_InitIpv4v6PdpCtx(&pstPdpCtxAddr->stIpv4v6PdpInfo, ucRmNetId);
    }

    /* PDP��δ�����ʱ�� */
    if ((RNIC_PDP_REG_STATUS_ACTIVE != pstPdpCtxAddr->stIpv4PdpInfo.enRegStatus)
     && (RNIC_PDP_REG_STATUS_ACTIVE != pstPdpCtxAddr->stIpv6PdpInfo.enRegStatus)
     && (RNIC_PDP_REG_STATUS_ACTIVE != pstPdpCtxAddr->stIpv4v6PdpInfo.enRegStatus))
    {
        /* ֹͣ����ͳ�ƶ�ʱ�� */
        RNIC_StopTimer(enTimerId);

        /* �������ͳ����Ϣ */
        RNIC_ClearNetDsFlowStats(ucRmNetId);

        /* Modified by l60609 for L-C��������Ŀ, 2014-1-14, begin */
        pstSpecNetCardCtx->enModemType = RNIC_MODEM_TYPE_INSIDE;
        /* Modified by l60609 for L-C��������Ŀ, 2014-1-14, end */
    }

    /* �ڶϿ����ųɹ�ʱ���ͶƱ˯�ߣ��Ա��ܽ�����˯ */
    DRV_PWRCTRL_SLEEPVOTE_UNLOCK(PWRCTRL_SLEEP_RNIC);

    /* ���RABID��Ӧ������ID */
    if (VOS_OK != RNIC_SaveNetIdByRabId(pstSpecNetCardCtx->enModemId,
                                        pstRcvInd->ucRabId,
                                        RNIC_RMNET_ID_BUTT))
    {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 RNIC_RcvAtDialModeReq(
    MsgBlock                           *pstMsg
)
{
    AT_RNIC_DIAL_MODE_REQ_STRU         *pstRcvInd;
    RNIC_DIAL_MODE_STRU                *pstDialMode;
    RNIC_AT_DIAL_MODE_CNF_STRU         *pstSndMsg;

    /* �ڴ���� */
    pstSndMsg = (RNIC_AT_DIAL_MODE_CNF_STRU *)PS_ALLOC_MSG(ACPU_PID_RNIC,
                        sizeof(RNIC_AT_DIAL_MODE_CNF_STRU) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == pstSndMsg)
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_RcvAtDialModeReq: Malloc failed!");
        return VOS_ERR;
    }

    pstRcvInd                           = (AT_RNIC_DIAL_MODE_REQ_STRU *)pstMsg;

    /* ��ȡ���貦�ŵ�ģʽ�Լ�ʱ���ĵ�ַ */
    pstDialMode                         = RNIC_GetDialModeAddr();
    pstSndMsg->clientId                 = pstRcvInd->clientId;
    pstSndMsg->ulDialMode               = pstDialMode->enDialMode;
    pstSndMsg->ulIdleTime               = pstDialMode->ulIdleTime;
    pstSndMsg->ulEventReportFlag        = pstDialMode->enEventReportFlag;

    /* ͨ��ID_RNIC_AT_DIAL_MODE_CNF��Ϣ���͸�ATģ�� */
    /* �����Ϣ */
    pstSndMsg->ulSenderCpuId            = VOS_LOCAL_CPUID;
    pstSndMsg->ulSenderPid              = ACPU_PID_RNIC;
    pstSndMsg->ulReceiverCpuId          = VOS_LOCAL_CPUID;
    pstSndMsg->ulReceiverPid            = WUEPS_PID_AT;
    pstSndMsg->enMsgId                  = ID_RNIC_AT_DIAL_MODE_CNF;

    /* ������Ϣ */
    if (VOS_OK != PS_SEND_MSG(ACPU_PID_RNIC, pstSndMsg))
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_RcvAtDialModeReq: Send msg failed!");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 RNIC_RcvAtDsflowInd(
    MsgBlock                           *pstMsg
)
{
    AT_RNIC_DSFLOW_IND_STRU            *pstRcvInd;
    RNIC_PDP_CTX_STRU                  *pstPdpAddr;
    RNIC_AT_DSFLOW_RSP_STRU            *pstDsflowRsp;

    /* �ڴ���� */
    pstDsflowRsp = (RNIC_AT_DSFLOW_RSP_STRU *)PS_ALLOC_MSG(ACPU_PID_RNIC,
                        sizeof(RNIC_AT_DSFLOW_RSP_STRU) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == pstDsflowRsp)
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_RcvAtDsflowInd: Malloc failed!");
        return VOS_ERR;
    }

    pstRcvInd                               = (AT_RNIC_DSFLOW_IND_STRU *)pstMsg;
    pstDsflowRsp->clientId                  = pstRcvInd->clientId;

    pstPdpAddr                              = RNIC_GetPdpCtxAddr(pstRcvInd->enRnicRmNetId);

    /* ��ƷҪ��δ���Ϻţ�����Ϊ0 */
    pstDsflowRsp->stRnicDataRate.ulDLDataRate = 0;
    pstDsflowRsp->stRnicDataRate.ulULDataRate = 0;

    /* PDP�����ʱ�򣬻�ȡ��ǰ������������ */
    if ((RNIC_PDP_REG_STATUS_ACTIVE == pstPdpAddr->stIpv4PdpInfo.enRegStatus)
     || (RNIC_PDP_REG_STATUS_ACTIVE == pstPdpAddr->stIpv6PdpInfo.enRegStatus)
     || (RNIC_PDP_REG_STATUS_ACTIVE == pstPdpAddr->stIpv4v6PdpInfo.enRegStatus))
    {
        pstDsflowRsp->stRnicDataRate.ulDLDataRate = RNIC_GetCurrentDlRate(pstRcvInd->enRnicRmNetId);
        pstDsflowRsp->stRnicDataRate.ulULDataRate = RNIC_GetCurrentUlRate(pstRcvInd->enRnicRmNetId);
    }

    /* ͨ��ID_RNIC_AT_DSFLOW_RSP��Ϣ���͸�ATģ�� */
    /* �����Ϣ */
    pstDsflowRsp->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstDsflowRsp->ulSenderPid                 = ACPU_PID_RNIC;
    pstDsflowRsp->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstDsflowRsp->ulReceiverPid               = WUEPS_PID_AT;
    pstDsflowRsp->enMsgId                     = ID_RNIC_AT_DSFLOW_RSP;

    /* ������Ϣ */
    if (VOS_OK != PS_SEND_MSG(ACPU_PID_RNIC, pstDsflowRsp))
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_RcvAtDsflowInd: Send msg failed!");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 RNIC_RcvAtPdnInfoCfgInd(
    MsgBlock                           *pstMsg
)
{
    AT_RNIC_PDN_INFO_CFG_IND_STRU      *pstRnicPdnCfgInd;
    RNIC_PDP_CTX_STRU                  *pstPdpAddr;
    RNIC_TIMER_ID_ENUM_UINT16           enTimerId;
    RNIC_SPEC_CTX_STRU                 *pstSpecNetCardCtx;
    ADS_FILTER_IP_ADDR_INFO_STRU        stFilterIpAddr;
    VOS_UINT8                           ucRabid;
    VOS_UINT8                           ucRmNetId;

    TAF_MEM_SET_S(&stFilterIpAddr, sizeof(stFilterIpAddr), 0x00, sizeof(ADS_FILTER_IP_ADDR_INFO_STRU));
    pstRnicPdnCfgInd = (AT_RNIC_PDN_INFO_CFG_IND_STRU *)pstMsg;
    ucRmNetId        = pstRnicPdnCfgInd->ucRmNetId;

    /* ��ȡPDP�����ĵ�ַ */
    pstPdpAddr                          = RNIC_GetPdpCtxAddr(ucRmNetId);
    pstSpecNetCardCtx                   = RNIC_GetSpecNetCardCtxAddr(ucRmNetId);
    ucRabid                             = RNIC_RAB_ID_INVALID;

    /* ����modem id���Rabid */
    if (VOS_OK != RNIC_BuildRabIdByModemId(pstSpecNetCardCtx->enModemId,
                                           pstRnicPdnCfgInd->ucRabId,
                                           &ucRabid))
    {
        RNIC_INFO_LOG1(ACPU_PID_RNIC, "RNIC_RcvAtPdnInfoCfgInd, modemId:%d", pstSpecNetCardCtx->enModemId);
        return VOS_ERR;
    }

    /* �洢RABID��Ӧ������ID */
    if (VOS_OK != RNIC_SaveNetIdByRabId(pstSpecNetCardCtx->enModemId,
                                        pstRnicPdnCfgInd->ucRabId,
                                        ucRmNetId))
    {
        RNIC_INFO_LOG1(ACPU_PID_RNIC, "RNIC_RcvAtPdnInfoCfgInd, RabId:%d", pstRnicPdnCfgInd->ucRabId);
        return VOS_ERR;
    }

    /* Modified by l60609 for L-C��������Ŀ, 2014-1-14, begin */
    pstSpecNetCardCtx->enModemType        = RNIC_MODEM_TYPE_INSIDE;
    /* Modified by l60609 for L-C��������Ŀ, 2014-1-14, end */

    if (VOS_TRUE == pstRnicPdnCfgInd->bitOpIpv4PdnInfo)
    {
        /* ����PDP��������Ϣ */
        pstPdpAddr->stIpv4PdpInfo.enRegStatus = RNIC_PDP_REG_STATUS_ACTIVE;
        pstPdpAddr->stIpv4PdpInfo.ucRabId     = pstRnicPdnCfgInd->ucRabId;
        pstPdpAddr->stIpv4PdpInfo.ulIpv4Addr  = pstRnicPdnCfgInd->stIpv4PdnInfo.ulPdnAddr;

        /* ���¹���IP��ַ��Ϣ */
        stFilterIpAddr.bitOpIpv4Addr          = VOS_TRUE;
        TAF_MEM_CPY_S(stFilterIpAddr.aucIpv4Addr,
                   sizeof(stFilterIpAddr.aucIpv4Addr),
                   (VOS_UINT8 *)&pstRnicPdnCfgInd->stIpv4PdnInfo.ulPdnAddr,
                   RNIC_IPV4_ADDR_LEN);
    }

    if (VOS_TRUE == pstRnicPdnCfgInd->bitOpIpv6PdnInfo)
    {
        /* ����PDP��������Ϣ */
        pstPdpAddr->stIpv6PdpInfo.enRegStatus = RNIC_PDP_REG_STATUS_ACTIVE;
        pstPdpAddr->stIpv6PdpInfo.ucRabId     = pstRnicPdnCfgInd->ucRabId;
        TAF_MEM_CPY_S(pstPdpAddr->stIpv6PdpInfo.aucIpv6Addr,
                   sizeof(pstPdpAddr->stIpv6PdpInfo.aucIpv6Addr),
                   pstRnicPdnCfgInd->stIpv6PdnInfo.aucPdnAddr,
                   RNICITF_MAX_IPV6_ADDR_LEN);

        /* ���¹���IP��ַ��Ϣ */
        stFilterIpAddr.bitOpIpv6Addr          = VOS_TRUE;
        TAF_MEM_CPY_S(stFilterIpAddr.aucIpv6Addr,
                   sizeof(pstPdpAddr->stIpv6PdpInfo.aucIpv6Addr),
                   pstRnicPdnCfgInd->stIpv6PdnInfo.aucPdnAddr,
                   RNIC_IPV6_ADDR_LEN);
    }

    /* ��������ID��ȡ����ͳ�ƵĶ�ʱ��ID */
    enTimerId = RNIC_GetDsflowTimerIdByNetId(ucRmNetId);

    /* ��������ͳ�ƶ�ʱ�� */
    RNIC_StartTimer(enTimerId, TI_RNIC_DSFLOW_STATS_LEN);

    /* ע�����й��˻ص�������ADS����ע��ĺ��������������� */
    ADS_DL_RegFilterDataCallback(ucRabid, &stFilterIpAddr, (RCV_DL_DATA_FUNC)RNIC_RcvAdsDlData);

    return VOS_OK;
}


VOS_UINT32 RNIC_RcvAtPdnInfoRelInd(
    MsgBlock                           *pstMsg
)
{
    AT_RNIC_PDN_INFO_REL_IND_STRU      *pstRnicPdnRelInd;
    RNIC_PDP_CTX_STRU                  *pstPdpCtxAddr;
    RNIC_SPEC_CTX_STRU                 *pstSpecNetCardCtx;
    RNIC_TIMER_ID_ENUM_UINT16           enTimerId;
    VOS_UINT8                           ucRmNetId;

    pstRnicPdnRelInd = (AT_RNIC_PDN_INFO_REL_IND_STRU *)pstMsg;

    ucRmNetId                           = pstRnicPdnRelInd->ucRmNetId;

    /* ��ȡPDP�����ĵ�ַ */
    pstPdpCtxAddr                       = RNIC_GetPdpCtxAddr(ucRmNetId);
    pstSpecNetCardCtx                   = RNIC_GetSpecNetCardCtxAddr(ucRmNetId);

    /* ��������ID��ȡ����ͳ�ƵĶ�ʱ��ID */
    enTimerId = RNIC_GetDsflowTimerIdByNetId(ucRmNetId);

    /* ����PDP��������Ϣ */
    if ((pstPdpCtxAddr->stIpv4PdpInfo.ucRabId == pstRnicPdnRelInd->ucRabId)
     && (RNIC_PDP_REG_STATUS_ACTIVE == pstPdpCtxAddr->stIpv4PdpInfo.enRegStatus))
    {
        /* ���IPV4 PDP��������Ϣ */
        RNIC_InitIpv4PdpCtx(&pstPdpCtxAddr->stIpv4PdpInfo);
    }

    if ((pstPdpCtxAddr->stIpv6PdpInfo.ucRabId == pstRnicPdnRelInd->ucRabId)
     && (RNIC_PDP_REG_STATUS_ACTIVE == pstPdpCtxAddr->stIpv6PdpInfo.enRegStatus))
    {
        /* ���IPV6 PDP��������Ϣ */
        RNIC_InitIpv6PdpCtx(&pstPdpCtxAddr->stIpv6PdpInfo);
    }

    if ((pstPdpCtxAddr->stIpv4v6PdpInfo.ucRabId == pstRnicPdnRelInd->ucRabId)
     && (RNIC_PDP_REG_STATUS_ACTIVE == pstPdpCtxAddr->stIpv4v6PdpInfo.enRegStatus))
    {
        /* ���IPV4V6 PDP��������Ϣ */
        RNIC_InitIpv4v6PdpCtx(&pstPdpCtxAddr->stIpv4v6PdpInfo, ucRmNetId);
    }

    /* PDP��δ�����ʱ�� */
    if ((RNIC_PDP_REG_STATUS_ACTIVE != pstPdpCtxAddr->stIpv4PdpInfo.enRegStatus)
     && (RNIC_PDP_REG_STATUS_ACTIVE != pstPdpCtxAddr->stIpv6PdpInfo.enRegStatus)
     && (RNIC_PDP_REG_STATUS_ACTIVE != pstPdpCtxAddr->stIpv4v6PdpInfo.enRegStatus))
    {
        /* ֹͣ����ͳ�ƶ�ʱ�� */
        RNIC_StopTimer(enTimerId);

        /* Modified by l60609 for L-C��������Ŀ, 2014-1-14, begin */
        pstSpecNetCardCtx->enModemType        = RNIC_MODEM_TYPE_INSIDE;
        /* Modified by l60609 for L-C��������Ŀ, 2014-1-14, end */

        /* �������ͳ����Ϣ */
        RNIC_ClearNetDsFlowStats(ucRmNetId);
    }

    /* ���RABID��Ӧ������ID */
    if (VOS_OK != RNIC_SaveNetIdByRabId(pstSpecNetCardCtx->enModemId,
                                        pstRnicPdnRelInd->ucRabId,
                                        RNIC_RMNET_ID_BUTT))
    {
        return VOS_ERR;
    }

    /* ȥע�����й��˻ص����� */
    ADS_DL_DeregFilterDataCallback(pstRnicPdnRelInd->ucRabId);

    return VOS_OK;
}


VOS_UINT32  RNIC_RcvTiDsflowStatsExpired(
    MsgBlock                           *pstMsg,
    VOS_UINT8                           ucRmNetId
)
{
    RNIC_SPEC_CTX_STRU                 *pstNetCntxt = VOS_NULL_PTR;
    VOS_UINT32                          ulTaBytes;
    VOS_UINT32                          ulRate;
    RNIC_TIMER_ID_ENUM_UINT16           enTimerId;

    /* ��ȡ�����ĵ�ַ */
    pstNetCntxt = RNIC_GET_SPEC_NET_CTX(ucRmNetId);

    /* ��������ID��ȡ����ͳ�ƵĶ�ʱ��ID */
    enTimerId   = RNIC_GetDsflowTimerIdByNetId(ucRmNetId);

    RNIC_StopTimer(enTimerId);

    /* ��ȡ2����������� */
    ulTaBytes   = pstNetCntxt->stDsFlowStats.ulTotalRecvFluxLow;

    /* ��ƷҪ��ÿ2���Ӽ���һ��,��λΪ:byte/s */
    ulRate = ulTaBytes>>1;
    RNIC_SetCurrentDlRate(ulRate, ucRmNetId);

    /* ��ȡ2����������� */
    ulTaBytes   = pstNetCntxt->stDsFlowStats.ulTotalSendFluxLow;

    /* ��ƷҪ��ÿ2���Ӽ���һ��,��λΪ:byte/s */
    ulRate      = ulTaBytes>>1;
    RNIC_SetCurrentUlRate(ulRate, ucRmNetId);

    /* ÿ������ͳ�����ڽ�������Ҫ������ͳ��Byte����� */
    pstNetCntxt->stDsFlowStats.ulTotalRecvFluxLow = 0;
    pstNetCntxt->stDsFlowStats.ulTotalSendFluxLow = 0;

    RNIC_StartTimer(enTimerId, TI_RNIC_DSFLOW_STATS_LEN);

    return VOS_OK;
}


VOS_UINT32 RNIC_RcvTiDemandDialDisconnectExpired(
    MsgBlock                           *pstMsg,
    VOS_UINT8                           ucRmNetId
)
{
    RNIC_DIAL_MODE_STRU                *pstDialMode = VOS_NULL_PTR;
    RNIC_SPEC_CTX_STRU                 *pstNetCntxt = VOS_NULL_PTR;
    VOS_UINT32                          ulPktsNum;
    VOS_UINT32                          ulCount;
    VOS_UINT32                          ulUserExistFlg;
    VOS_UINT32                          ulExpiredCount;

    /* ��ȡ�����ĵ�ַ */
    pstNetCntxt     = RNIC_GET_SPEC_NET_CTX(ucRmNetId);

    /* ��ȡ���貦�ŵ�ģʽ�Լ�ʱ���ĵ�ַ */
    pstDialMode     = RNIC_GetDialModeAddr();

    /* ���û����õ�ʱ������ʱ��ʱ���ȷ� */
    ulExpiredCount  = (pstDialMode->ulIdleTime * TI_RNIC_UNIT) / TI_RNIC_DEMAND_DIAL_DISCONNECT_LEN;

    RNIC_StopTimer(TI_RNIC_DEMAND_DIAL_DISCONNECT);

    /* �����ǰ���ֶ�����,��ֱ�ӷ���,����Ҫ֪ͨ */
    if (RNIC_DIAL_MODE_DEMAND_CONNECT != pstDialMode->enDialMode)
    {
        RNIC_NORMAL_LOG(ACPU_PID_RNIC, "RNIC_RcvTiDemandDialDisconnectExpired: Dialmode is manual!");
        return VOS_OK;
    }

    /* ��ȡ�ڶ�ʱ��������������ͳ�Ƶ����ݰ����Լ���ǰ�û�����״̬ */
    ulPktsNum = pstNetCntxt->stDsFlowStats.ulPeriodSendPktNum;
    ulUserExistFlg  = DRV_AT_GET_USER_EXIST_FLAG();

    /* ���貦�ŶϿ��߼�����:
         ���û������������ݰ�:
             ����������
         �������:
             ������++
       ���³����޷�ʶ��:
          ��ǰ����USB�����û������ݷ��ͣ��������������·�������Ҳ����Ϊ
          �����û������������ݰ��������¼�����
    */
    if ((0 != ulPktsNum)
     && (VOS_TRUE == ulUserExistFlg))
    {
        RNIC_ClearTiDialDownExpCount();
    }
    else
    {
        RNIC_IncTiDialDownExpCount();
    }

    /* �ж�ͳ�Ƽ������Ƿ���ڵ����û����öϿ�����ʱ���붨ʱ��ʱ���ȷ� */
    ulCount = RNIC_GetTiDialDownExpCount();

    if ((ulCount >= ulExpiredCount)
       &&(RNIC_ALLOW_EVENT_REPORT == pstDialMode->enEventReportFlag))
    {
        /*֪ͨӦ�öϿ����� */
        if (VOS_OK == RNIC_SendDialEvent(DEVICE_ID_WAN, RNIC_DAIL_EVENT_DOWN))
        {
            /* �ϱ��Ͽ������¼���ͶƱ������˯�ߣ��ڶϿ����ųɹ�ʱ��� */
            DRV_PWRCTRL_SLEEPVOTE_LOCK(PWRCTRL_SLEEP_RNIC);
             wake_lock_timeout(&rnic_wakelock, (long)msecs_to_jiffies(5000));
            RNIC_DBG_SEND_APP_DIALDOWN_SUCC_NUM(1, ucRmNetId);

            RNIC_WARNING_LOG(ACPU_PID_RNIC, "RNIC_ProcUlIpv4DataInPdpDeactive:Send Act PDP Msg to APP");
        }
        else
        {
            RNIC_DBG_SEND_APP_DIALDOWN_FAIL_NUM(1, ucRmNetId);
        }

        RNIC_MNTN_TraceDialDisconnEvt(ulPktsNum, ulUserExistFlg);
    }

    RNIC_StartTimer(TI_RNIC_DEMAND_DIAL_DISCONNECT, TI_RNIC_DEMAND_DIAL_DISCONNECT_LEN);

    return VOS_OK;
}


VOS_UINT32 RNIC_RcvTiDemandDialProtectExpired(
    MsgBlock                           *pstMsg,
    VOS_UINT8                           ucRmNetId
)
{
    /* ֹͣ���貦�ű�����ʱ�� */
    RNIC_StopTimer(TI_RNIC_DEMAND_DIAL_PROTECT);

    return VOS_OK;
}


RNIC_RCV_TI_EXPRIED_PROC_FUNC RNIC_GetTiExpiredFuncByMsgId(VOS_UINT32 ulMsgId)
{
    VOS_UINT8                           i;
    VOS_UINT32                          ulTableSize;

    /* ��g_astRnicTiExpriedProcTab�л�ȡ�������� */
    ulTableSize = sizeof(g_astRnicTiExpriedProcTab)/sizeof(RNIC_RCV_TI_EXPRIED_PROC_STRU);

    /* g_astRnicTiExpriedProcTab�����������Ϣ�����ķַ� */
    for (i = 0; i < ulTableSize; i++)
    {
        if (g_astRnicTiExpriedProcTab[i].ulMsgId== ulMsgId)
        {
            return g_astRnicTiExpriedProcTab[i].pTiExpriedProcFunc;
        }
    }

    return VOS_NULL_PTR;
}


VOS_UINT32 RNIC_RcvCcpuResetStartInd(
    MsgBlock                           *pstMsg
)
{
    VOS_UINT8                           ucIndex;
    RNIC_CTX_STRU                      *pstRnicCtx;

    pstRnicCtx                          = RNIC_GetRnicCtxAddr();

    printk("\n RNIC_RcvCcpuResetStartInd enter, %u \n", VOS_GetSlice());

    /* ֹͣ���������Ķ�ʱ�� */
    RNIC_StopAllTimer();

    for (ucIndex = 0 ; ucIndex < RNIC_NET_ID_MAX_NUM ; ucIndex++)
    {
        /* �������ͳ����Ϣ */
        RNIC_ClearNetDsFlowStats(ucIndex);

        /* ��ʼ��RNIC PDP������ */
        RNIC_InitPdpCtx(&(pstRnicCtx->astSpecCtx[ucIndex].stPdpCtx), ucIndex);

        /* ��ʼ������״̬ */
        pstRnicCtx->astSpecCtx[ucIndex].enFlowCtrlStatus = RNIC_FLOW_CTRL_STATUS_STOP;

        /* ��ʼ��ģ��ID */
        /* ��ʼ����ʱ����Ѿ�ȷ��������������Ӧ�ĸ�MODEM */
        pstRnicCtx->astSpecCtx[ucIndex].enModemId        = RNIC_GET_MODEM_ID_BY_NET_ID(ucIndex);

        pstRnicCtx->astSpecCtx[ucIndex].enModemType      = RNIC_MODEM_TYPE_INSIDE;
    }

    /* ��ʼ��RABID��Ϣ */
    for (ucIndex = 0 ; ucIndex < RNIC_MODEM_ID_MAX_NUM ; ucIndex++)
    {
        RNIC_InitRabidInfo(&pstRnicCtx->astRabIdInfo[ucIndex]);
    }

    /* ��ʼ��PDNID��Ϣ */
    RNIC_InitPdnIdInfo(&(pstRnicCtx->stPdnIdInfo));

    /* ��ʼ��RNIC��ʱ�������� */
    RNIC_InitAllTimers(pstRnicCtx->astTimerCtx);

    /* ��ʼ������ģʽ��Ϣ */
    RNIC_ResetDialMode(&(pstRnicCtx->stDialMode));

    /* ��ʼ�����ŶϿ���ʱ����ʱ��������ͳ�� */
    RNIC_ClearTiDialDownExpCount();

    /* ��ʼ��TIMER4���ѱ�־ */
    RNIC_SetTimer4WakeFlg(VOS_FALSE);

    /* �ͷ��ź�����ʹ�õ���API����������� */
    VOS_SmV(RNIC_GetResetSem());

    /* �˴��Ƿ�Ҫ���sdio����ע�ắ�� */
    printk("\n RNIC_RcvCcpuResetStartInd leave, %u \n", VOS_GetSlice());
    return VOS_OK;
}



/* Modified by l60609 for L-C��������Ŀ, 2014-01-06, Begin */

VOS_VOID RNIC_ProcInsideModemIpv4ActInd(
    RNIC_RMNET_CONFIG_REQ_STRU         *pstPdpStatusInd
)
{
    RNIC_TIMER_ID_ENUM_UINT16           enTimerId;
    VOS_UINT8                           ucRabid;
    VOS_UINT8                           ucRmNetId;
    RNIC_PDP_CTX_STRU                  *pstPdpAddr          = VOS_NULL_PTR;
    RNIC_SPEC_CTX_STRU                 *pstSpecNetCardCtx   = VOS_NULL_PTR;

    ucRmNetId                           = pstPdpStatusInd->ucRmNetId;

    /* ��ȡPDP�����ĵ�ַ */
    pstPdpAddr                          = RNIC_GetPdpCtxAddr(ucRmNetId);
    pstSpecNetCardCtx                   = RNIC_GetSpecNetCardCtxAddr(ucRmNetId);

    ucRabid                             = RNIC_RAB_ID_INVALID;

    /* ����modem id������id�����Rabid */
    if (VOS_OK != RNIC_BuildRabIdByModemId(pstSpecNetCardCtx->enModemId,
                                           pstPdpStatusInd->ucRabId,
                                           &ucRabid))
    {
        RNIC_INFO_LOG1(ACPU_PID_RNIC, "RNIC_ProcInsideModemIpv4ActInd, modemId:%d", pstSpecNetCardCtx->enModemId);
        return;
    }

    /* �洢RABID��Ӧ������ID */
    if (VOS_OK != RNIC_SaveNetIdByRabId(pstSpecNetCardCtx->enModemId,
                                        pstPdpStatusInd->ucRabId,
                                        ucRmNetId))
    {
        RNIC_INFO_LOG1(ACPU_PID_RNIC, "RNIC_ProcInsideModemIpv4ActInd, RabId:%d", pstPdpStatusInd->ucRabId);
        return;
    }

    pstSpecNetCardCtx->enModemType        = RNIC_MODEM_TYPE_INSIDE;

    /* ����PDP��������Ϣ */
    pstPdpAddr->stIpv4PdpInfo.enRegStatus = RNIC_PDP_REG_STATUS_ACTIVE;
    pstPdpAddr->stIpv4PdpInfo.ucRabId     = pstPdpStatusInd->ucRabId;

    /* ��������ID��ȡ����ͳ�ƵĶ�ʱ��ID */
    enTimerId = RNIC_GetDsflowTimerIdByNetId(ucRmNetId);

    /* ��������ͳ�ƶ�ʱ�� */
    RNIC_StartTimer(enTimerId, TI_RNIC_DSFLOW_STATS_LEN);

    /* ֹͣ���貦�ű�����ʱ�� */
    if (RNIC_RMNET_ID_0 == ucRmNetId)
    {
        RNIC_StopTimer(TI_RNIC_DEMAND_DIAL_PROTECT);
    }

    /* ע�����з��ͺ�����ADS����ע��ĺ��������������� */
    ADS_DL_RegDlDataCallback(ucRabid, RNIC_RcvAdsDlData, ucRmNetId);

    return;
}


VOS_VOID RNIC_ProcInsideModemIpv6ActInd(
    RNIC_RMNET_CONFIG_REQ_STRU           *pstPdpStatusInd
)
{
    RNIC_TIMER_ID_ENUM_UINT16           enTimerId;
    VOS_UINT8                           ucRabid;
    VOS_UINT8                           ucRmNetId;
    RNIC_PDP_CTX_STRU                  *pstPdpAddr          = VOS_NULL_PTR;
    RNIC_SPEC_CTX_STRU                 *pstSpecNetCardCtx   = VOS_NULL_PTR;


    ucRmNetId                           = pstPdpStatusInd->ucRmNetId;

    /* ��ȡPDP�����ĵ�ַ */
    pstPdpAddr                          = RNIC_GetPdpCtxAddr(ucRmNetId);
    pstSpecNetCardCtx                   = RNIC_GetSpecNetCardCtxAddr(ucRmNetId);

    ucRabid                             = RNIC_RAB_ID_INVALID;

    /* ����modem id������id�����Rabid */
    if (VOS_OK != RNIC_BuildRabIdByModemId(pstSpecNetCardCtx->enModemId,
                                           pstPdpStatusInd->ucRabId,
                                           &ucRabid))
    {
        RNIC_INFO_LOG1(ACPU_PID_RNIC, "RNIC_ProcInsideModemIpv6ActInd, modemId:%d", pstSpecNetCardCtx->enModemId);
        return;
    }

    /* �洢RABID��Ӧ������ID */
    if (VOS_OK != RNIC_SaveNetIdByRabId(pstSpecNetCardCtx->enModemId,
                                        pstPdpStatusInd->ucRabId,
                                        ucRmNetId))
    {
        RNIC_INFO_LOG1(ACPU_PID_RNIC, "RNIC_ProcInsideModemIpv6ActInd, RabId:%d", pstPdpStatusInd->ucRabId);
        return;
    }

    pstSpecNetCardCtx->enModemType        = RNIC_MODEM_TYPE_INSIDE;

    /* ����PDP��������Ϣ */
    pstPdpAddr->stIpv6PdpInfo.enRegStatus = RNIC_PDP_REG_STATUS_ACTIVE;
    pstPdpAddr->stIpv6PdpInfo.ucRabId     = pstPdpStatusInd->ucRabId;

    /* ��������ID��ȡ����ͳ�ƵĶ�ʱ��ID */
    enTimerId = RNIC_GetDsflowTimerIdByNetId(ucRmNetId);

    /* ��������ͳ�ƶ�ʱ�� */
    RNIC_StartTimer(enTimerId, TI_RNIC_DSFLOW_STATS_LEN);

    /* ע�����з��ͺ�����ADS����ע��ĺ��������������� */
    ADS_DL_RegDlDataCallback(ucRabid, RNIC_RcvAdsDlData, ucRmNetId);

    return;
}


VOS_VOID RNIC_ProcRnicPdpActInd(
    RNIC_RMNET_CONFIG_REQ_STRU           *pstPdpStatusInd
)
{
    /* Ĭ��ipv4��ipv6����ɹ��������·� */
    if (RNIC_MODEM_TYPE_INSIDE == pstPdpStatusInd->enModemType)
    {
        if (RNIC_IP_TYPE_IPV4 == pstPdpStatusInd->enIpType)
        {
            /* balong modem ipv4���� */
            RNIC_ProcInsideModemIpv4ActInd(pstPdpStatusInd);
        }

        if (RNIC_IP_TYPE_IPV6 == pstPdpStatusInd->enIpType)
        {
            /* balong modem ipv6���� */
            RNIC_ProcInsideModemIpv6ActInd(pstPdpStatusInd);
        }
    }


    return;
}



VOS_VOID RNIC_ProcInsideModemDeactInd(
    RNIC_RMNET_CONFIG_REQ_STRU         *pstPdpStatusInd
)
{
    RNIC_TIMER_ID_ENUM_UINT16           enTimerId;
    VOS_UINT8                           ucRmNetId;
    RNIC_PDP_CTX_STRU                  *pstPdpCtxAddr       = VOS_NULL_PTR;
    RNIC_SPEC_CTX_STRU                 *pstSpecNetCardCtx   = VOS_NULL_PTR;

    ucRmNetId                           = pstPdpStatusInd->ucRmNetId;

    /* ��ȡPDP�����ĵ�ַ */
    pstPdpCtxAddr                       = RNIC_GetPdpCtxAddr(ucRmNetId);
    pstSpecNetCardCtx                   = RNIC_GetSpecNetCardCtxAddr(ucRmNetId);

    /* ��������ID��ȡ����ͳ�ƵĶ�ʱ��ID */
    enTimerId = RNIC_GetDsflowTimerIdByNetId(ucRmNetId);

    /* �����IPV4 PDPȥ���� */
    if ((pstPdpCtxAddr->stIpv4PdpInfo.ucRabId == pstPdpStatusInd->ucRabId)
     && (RNIC_PDP_REG_STATUS_ACTIVE == pstPdpCtxAddr->stIpv4PdpInfo.enRegStatus))
    {
        /* ���IPV4 PDP��������Ϣ */
        RNIC_InitIpv4PdpCtx(&pstPdpCtxAddr->stIpv4PdpInfo);

        /* ������0�ϲ��а��貦�ŵĹ��� */
        if (RNIC_RMNET_ID_0 == ucRmNetId)
        {
            /* ֹͣ���貦�ŶϿ���ʱ�� */
            RNIC_StopTimer(TI_RNIC_DEMAND_DIAL_DISCONNECT);

            /* ��ղ��ŶϿ���ʱ����ʱͳ�� */
            RNIC_ClearTiDialDownExpCount();
        }
    }

    if ((pstPdpCtxAddr->stIpv6PdpInfo.ucRabId == pstPdpStatusInd->ucRabId)
     && (RNIC_PDP_REG_STATUS_ACTIVE == pstPdpCtxAddr->stIpv6PdpInfo.enRegStatus))
    {
        /* ���IPV6 PDP��������Ϣ */
        RNIC_InitIpv6PdpCtx(&pstPdpCtxAddr->stIpv6PdpInfo);
    }

    if ((pstPdpCtxAddr->stIpv4v6PdpInfo.ucRabId == pstPdpStatusInd->ucRabId)
     && (RNIC_PDP_REG_STATUS_ACTIVE == pstPdpCtxAddr->stIpv4v6PdpInfo.enRegStatus))
    {
        /* ���IPV4V6 PDP��������Ϣ */
        RNIC_InitIpv4v6PdpCtx(&pstPdpCtxAddr->stIpv4v6PdpInfo, ucRmNetId);
    }

    /* PDP��δ�����ʱ�� */
    if ((RNIC_PDP_REG_STATUS_ACTIVE != pstPdpCtxAddr->stIpv4PdpInfo.enRegStatus)
     && (RNIC_PDP_REG_STATUS_ACTIVE != pstPdpCtxAddr->stIpv6PdpInfo.enRegStatus)
     && (RNIC_PDP_REG_STATUS_ACTIVE != pstPdpCtxAddr->stIpv4v6PdpInfo.enRegStatus))
    {
        /* ֹͣ����ͳ�ƶ�ʱ�� */
        RNIC_StopTimer(enTimerId);

        /* �������ͳ����Ϣ */
        RNIC_ClearNetDsFlowStats(ucRmNetId);

        pstSpecNetCardCtx->enModemType = RNIC_MODEM_TYPE_INSIDE;
    }

    /* �ڶϿ����ųɹ�ʱ���ͶƱ˯�ߣ��Ա��ܽ�����˯ */
    DRV_PWRCTRL_SLEEPVOTE_UNLOCK(PWRCTRL_SLEEP_RNIC);

    /* ���RABID��Ӧ������ID */
    if (VOS_OK != RNIC_SaveNetIdByRabId(pstSpecNetCardCtx->enModemId,
                                        pstPdpStatusInd->ucRabId,
                                        RNIC_RMNET_ID_BUTT))
    {
        return;
    }

    return;
}


VOS_VOID RNIC_ProcRnicPdpDeactInd(
    RNIC_RMNET_CONFIG_REQ_STRU           *pstPdpStatusInd
)
{
    /* ��ն�Ӧ������PDP������ */
    if (RNIC_MODEM_TYPE_INSIDE == pstPdpStatusInd->enModemType)
    {
        /* ����ڲ�modem PDPȥ������������� */
        RNIC_ProcInsideModemDeactInd(pstPdpStatusInd);
    }
    return;
}


VOS_UINT32 RNIC_RcvRnicRmnetConfigReq(
    MsgBlock                           *pstMsg
)
{
    VOS_UINT8                           ucRmNetId;
    RNIC_RMNET_CONFIG_REQ_STRU         *pstRmnetConfigReq = VOS_NULL_PTR;
    RNIC_SPEC_CTX_STRU                 *pstSpecNetCardCtx = VOS_NULL_PTR;

    pstRmnetConfigReq       = (RNIC_RMNET_CONFIG_REQ_STRU *)pstMsg;
    ucRmNetId               = pstRmnetConfigReq->ucRmNetId;
    pstSpecNetCardCtx       = RNIC_GetSpecNetCardCtxAddr(ucRmNetId);

    /* pdp����ɹ���rnic����up */
    if (RNIC_RMNET_STATUS_UP == pstRmnetConfigReq->enRmnetStatus)
    {
        /* RNIC_MODEM_TYPE_BUTT�޸�ΪRNIC_MODEM_TYPE_INSIDE����ҪΪ������v3r3���貦�Ź��� */
        if ((pstSpecNetCardCtx->enModemType == pstRmnetConfigReq->enModemType)
         || (RNIC_MODEM_TYPE_INSIDE == pstSpecNetCardCtx->enModemType))
        {
            RNIC_ProcRnicPdpActInd(pstRmnetConfigReq);
        }
    }
    /* pdpȥ����ɹ���rnic����down */
    else
    {
        if (pstSpecNetCardCtx->enModemType == pstRmnetConfigReq->enModemType)
        {
            RNIC_ProcRnicPdpDeactInd(pstRmnetConfigReq);
        }
    }

    return VOS_OK;
}
/* Modified by l60609 for L-C��������Ŀ, 2014-01-06, End */



VOS_UINT32 RNIC_RcvAtMsg(MsgBlock *pstMsg)
{
    MSG_HEADER_STRU                    *pstMsgHeader;

    pstMsgHeader = (MSG_HEADER_STRU *)pstMsg;

    switch(pstMsgHeader->ulMsgName)
    {
        case ID_AT_RNIC_IPV4_PDP_ACT_IND:
            RNIC_RcvAtIpv4PdpActInd(pstMsg);
            break;

        case ID_AT_RNIC_IPV6_PDP_ACT_IND:
            RNIC_RcvAtIpv6PdpActInd(pstMsg);
            break;

        case ID_AT_RNIC_IPV4V6_PDP_ACT_IND:
            RNIC_RcvAtIpv4v6PdpActInd(pstMsg);
            break;

        case ID_AT_RNIC_PDP_DEACT_IND:
            RNIC_RcvAtPdpDeactInd(pstMsg);
            break;

        case ID_AT_RNIC_DSFLOW_IND:
            RNIC_RcvAtDsflowInd(pstMsg);
            break;

        case ID_AT_RNIC_DIAL_MODE_REQ:
            RNIC_RcvAtDialModeReq(pstMsg);
            break;

        case ID_AT_RNIC_PDN_INFO_CFG_IND:
            RNIC_RcvAtPdnInfoCfgInd(pstMsg);
            break;

        case ID_AT_RNIC_PDN_INFO_REL_IND:
            RNIC_RcvAtPdnInfoRelInd(pstMsg);
            break;

        default:
            RNIC_INFO_LOG1(ACPU_PID_RNIC, "RNIC_RcvAtMsg:MsgId", pstMsgHeader->ulMsgName);
            break;
    }

    return VOS_OK;
}


VOS_UINT32 RNIC_RcvTimerMsg(MsgBlock *pstMsg)
{
    REL_TIMER_MSG                      *pstRcvMsg;
    VOS_UINT8                           ucRmNetId;
    RNIC_RCV_TI_EXPRIED_PROC_FUNC       pTiExpriedProcFunc;
    VOS_UINT32                          ulRst;

    pTiExpriedProcFunc = VOS_NULL_PTR;
    pstRcvMsg = (REL_TIMER_MSG *)pstMsg;

    /* ���ݶ�ʱ��ID��ȡ����ID */
    ucRmNetId = RNIC_GetNetIdByTimerId(pstRcvMsg->ulName);

    /* ���Ҷ�ʱ����ʱ����������Ϣ�������� */
    pTiExpriedProcFunc = RNIC_GetTiExpiredFuncByMsgId(pstRcvMsg->ulName);
    if (VOS_NULL_PTR != pTiExpriedProcFunc)
    {
        ulRst = pTiExpriedProcFunc(pstMsg, ucRmNetId);
        if (VOS_ERR == ulRst)
        {
            RNIC_INFO_LOG(ACPU_PID_RNIC, "RNIC_RcvTimerMsg:proc func error! \r\n");
            return VOS_ERR;
        }
    }
    else
    {
        RNIC_INFO_LOG1(ACPU_PID_RNIC, "RNIC_RcvTimerMsg:pstRcvMsg->ulName\r\n", pstRcvMsg->ulName);
    }

    return VOS_OK;

}



VOS_UINT32 RNIC_RcvRnicMsg(MsgBlock *pstMsg)
{
    MSG_HEADER_STRU                    *pstMsgHeader;

    pstMsgHeader = (MSG_HEADER_STRU *)pstMsg;

    switch(pstMsgHeader->ulMsgName)
    {
        case ID_RNIC_CCPU_RESET_START_IND:
            RNIC_RcvCcpuResetStartInd(pstMsg);
            break;

        case ID_RNIC_CCPU_RESET_END_IND:

            /* do nothing */
            RNIC_NORMAL_LOG(ACPU_PID_RNIC, "RNIC_RcvRnicMsg: rcv ID_CCPU_RNIC_RESET_END_IND");
            break;


        /* Modified by l60609 for L-C��������Ŀ, 2014-01-06, Begin */
        case ID_RNIC_RMNET_CONFIG_REQ:
            RNIC_RcvRnicRmnetConfigReq(pstMsg);
            break;

        /* Modified by l60609 for L-C��������Ŀ, 2014-01-06, End */

        default:
            RNIC_NORMAL_LOG1(ACPU_PID_RNIC, "RNIC_RcvRnicMsg: rcv error msg id %d\r\n", pstMsgHeader->ulMsgName);
            break;
    }

    return VOS_OK;
}



VOS_UINT32 RNIC_ProcMsg (MsgBlock *pstMsg)
{
    if (VOS_NULL_PTR == pstMsg)
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_ProcMsg: Msg is invalid!");
        return VOS_ERR;
    }

    /* ��Ϣ�ķַ����� */
    switch (pstMsg->ulSenderPid)
    {
        case WUEPS_PID_AT:

            /* ����AT��Ϣ */
            RNIC_RcvAtMsg(pstMsg);
            break;

        case VOS_PID_TIMER:

            /* ���ն�ʱ����ʱ��Ϣ */
            RNIC_RcvTimerMsg(pstMsg);
            break;

        case ACPU_PID_RNIC:

            /* ����RNIC����Ϣ*/
            RNIC_RcvRnicMsg(pstMsg);
            break;


        default:
            RNIC_INFO_LOG1(ACPU_PID_RNIC, "RNIC_ProcMsg:SendPid", pstMsg->ulSenderPid);
            break;
    }

    return VOS_OK;
}

