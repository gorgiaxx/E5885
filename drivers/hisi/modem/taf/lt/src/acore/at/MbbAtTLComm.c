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



#include "MbbPsCsCommon.h"
#include "at_lte_common.h"



#define MBB_ATTL_DEBUG_LOG()
#define MBB_ATTL_DEBUG_LOG_STR(var1)
#define MBB_ATTL_DEBUG_LOG_STR_VAR_HEX(str1, var1)
#define MBB_ATTL_DEBUG_LOG_STR_VAR(str1, var1)
#define MBB_ATTL_DEBUG_LOG_DUMP(LEN,DATA)

VOS_UINT16        gusbandwidth[]  = {1400, 3000, 5000, 10000, 15000, 20000};
LTE_BANDINFO_STRU gstAtBandinfo[] =
{
    {0,     0,     0,     0},                 /*reserved, so that bandno correspond to index directly*/
    {21100, 0,     19200, 18000},             /* FDD:Band1 */
    {19300, 600,   18500, 18600},             /* FDD:Band2 */
    {18050, 1200,  17100, 19200},             /* FDD:Band3 */
    {21100, 1950,  17100, 19950},             /* FDD:Band4 */
    {8690,  2400,  8240,  20400},             /* FDD:Band5 */
    {8750,  2650,  8300,  20650},             /* FDD:Band6 */
    {26200, 2750,  25000, 20750},             /* FDD:Band7 */
    {9250,  3450,  8800,  21450},             /* FDD:Band8 */
    {18449, 3800,  17499, 21800},             /* FDD:Band9 */
    {21100, 4150,  17100, 22150},             /* FDD:Band10 */
    {14759, 4750,  14279, 22750},             /* FDD:Band11 */
    {7290,  5010,  6990,  23010},             /* FDD:Band12 */
    {7460,  5180,  7770,  23180},             /* FDD:Band13 */
    {7580,  5280,  7880,  23280},             /* FDD:Band14 */
    {0,     0,     0,     0},                 /* reserved for b15*/
    {0,     0,     0,     0},                 /* reserved for b16*/
    {7340,  5730,  7040,  23730},             /* FDD:Band17 */
    {8600,  5850,  8150,  23850},             /* FDD:Band18 */
    {8750,  6000,  8300,  24000},             /* FDD:Band19 */
    {7910,  6150,  8320,  24150},             /* FDD:Band20 */
    {14959, 6450,  14479, 24450},             /* FDD:Band21 */
    {35100, 6600,  34100, 24600},             /* FDD:Band22*/
    {21800, 7500,  20000, 25500},             /* FDD:Band23 */
    {15250, 7700,  16265, 25700},             /* FDD:Band24 */
    {19300, 8040,  18500, 26040},             /* FDD:Band25 */
    {8590,  8690,  8140,  26690},             /* FDD:Band26 */
    {0,     0,     0,     0},                 /* reserved for b27 */
    {0,     0,     0,     0},                 /* reserved for b28 */
    {0,     0,     0,     0},                 /* reserved for b29 */
    {0,     0,     0,     0},                 /* reserved for b30 */
    {0,     0,     0,     0},                 /* reserved for b31 */
    {0,     0,     0,     0},                 /* reserved for b32 */
    {19000, 36000, 19000, 36000},             /* TDD:Band33 */
    {20100, 36200, 20100, 36200},             /* TDD:Band34 */
    {18500, 36350, 18500, 36350},             /* TDD:Band35 */
    {19300, 36950, 19300, 36950},             /* TDD:Band36 */
    {19100, 37550, 19100, 37550},             /* TDD:Band37 */
    {25700, 37750, 25700, 37750},             /* TDD:Band38 */
    {18800, 38250, 18800, 38250},             /* TDD:Band39 */
    {23000, 38650, 23000, 38650},             /* TDD:Band40 */
    {24960, 39650, 24960, 39650},             /* TDD:Band41 */
    {34000, 41590, 34000, 41590},             /* TDD:Band42 */
    {36000, 43590, 36000, 43590},             /* TDD:Band43 */
};
AT_HFREQINFO_LTE_INFO_STRU g_LastLteInfo = {0};

VOS_UINT32 atHFreqinfoInd(VOS_VOID *pMsgBlock)
{
    VOS_UINT8  ucSumOfBand = 0;
    VOS_UINT8  ucSumOfBandwidth = 0;  
    VOS_UINT16 usLength = 0;
    VOS_UINT16 usNdl = 0;
    VOS_UINT16 usNul = 0;
    VOS_UINT16 usUlBandwidth = 0;
    VOS_UINT16 usDlBandwidth = 0;
    L4A_READ_LWCLASH_IND_STRU *pstLwclash = NULL;
    LTE_BANDINFO_STRU stCurBandinfo = {0};

    if (VOS_NULL == pMsgBlock)
    {
        return AT_ERROR;
    }
    
    pstLwclash = (L4A_READ_LWCLASH_IND_STRU *)pMsgBlock;
      
    /*��ȡ��������ĳ���*/
    ucSumOfBandwidth = sizeof(gusbandwidth) / sizeof(gusbandwidth[0]);   
    if((pstLwclash->stLwclashInfo.usUlBandwidth >= ucSumOfBandwidth) || (pstLwclash->stLwclashInfo.usDlBandwidth >= ucSumOfBandwidth))
    {
        MBB_ATTL_DEBUG_LOG_STR("the receive bandwith beyond length of array");
        return AT_ERROR;
    }
    
    /*band�Ϸ����ж�*/
    ucSumOfBand = sizeof(gstAtBandinfo) / sizeof(gstAtBandinfo[0]);/*44*/
    if(pstLwclash->stLwclashInfo.usBand >= ucSumOfBand)
    {
        MBB_ATTL_DEBUG_LOG_STR("the received band beyond length of ucSumOfBand");
        return AT_ERROR;
    }

    if((0 == pstLwclash->stLwclashInfo.usDlFreq) || (0 == pstLwclash->stLwclashInfo.usUlFreq))
    {
        /*����פ��̬�뿪ʱ���������ϱ�HFREQINFO*/
        return AT_OK;
    }

    stCurBandinfo = gstAtBandinfo[pstLwclash->stLwclashInfo.usBand];
    /*����Ƶ��Ndl = (Fdl - Fdl_low) + Noff_dl*/
    usNdl = (pstLwclash->stLwclashInfo.usDlFreq - stCurBandinfo.ulDLfreqlow) + stCurBandinfo.ulNoffdl;
    /*����Ƶ��Nul = (Ful - Ful_low) + Noff_ul*/
    usNul = (pstLwclash->stLwclashInfo.usUlFreq - stCurBandinfo.ulULfreqlow) + stCurBandinfo.ulNofful;
    usUlBandwidth = gusbandwidth[pstLwclash->stLwclashInfo.usUlBandwidth];/*ʹ��lwclash���ص�ö��ֵ��Ϊ������ȡ����*/
    usDlBandwidth = gusbandwidth[pstLwclash->stLwclashInfo.usDlBandwidth];/*ʹ��lwclash���ص�ö��ֵ��Ϊ������ȡ����*/

    /*����ֵ�Ϸ������汾���ϱ�ֵ*/
    g_LastLteInfo.usBand = pstLwclash->stLwclashInfo.usBand;
    g_LastLteInfo.usNdl = usNdl;
    g_LastLteInfo.usDlFreq = pstLwclash->stLwclashInfo.usDlFreq;
    g_LastLteInfo.usDlBandwidth = usDlBandwidth;
    g_LastLteInfo.usNul = usNul;
    g_LastLteInfo.usUlFreq = pstLwclash->stLwclashInfo.usUlFreq;
    g_LastLteInfo.usUlBandwidth = usUlBandwidth;

    if(AT_HFREQINFO_REPORT == g_AtHFreqinforeport)
    {
        usLength = (VOS_UINT16)At_sprintf( AT_CMD_MAX_LEN,(VOS_CHAR *)pgucLAtSndCodeAddr,(VOS_CHAR *)pgucLAtSndCodeAddr,
                                           "%s^HFREQINFO:%d,%d,%d,%d,%d,%d,%d,%d,%d%s",
                                           gaucAtCrLf,
                                           g_AtHFreqinforeport,
                                           AT_HFREQINFO_RAT_TYPE_LTE,
                                           pstLwclash->stLwclashInfo.usBand,
                                           usNdl,
                                           pstLwclash->stLwclashInfo.usDlFreq,
                                           usDlBandwidth,
                                           usNul,
                                           pstLwclash->stLwclashInfo.usUlFreq,
                                           usUlBandwidth,
                                           gaucAtCrLf);
        At_SendResultData(AT_BROADCAST_CLIENT_INDEX_MODEM_0, pgucLAtSndCodeAddr, usLength);
    }
    return AT_FW_CLIENT_STATUS_READY;
}

VOS_UINT32 atLwclashCnfProcMbb(VOS_VOID *pMsgBlock)
{
    VOS_UINT8  ucIndexId = 0;
    VOS_UINT8  ucSumOfBandwidth = 0;
    VOS_UINT8  ucSumOfBand = 0;
    VOS_UINT16 usNdl = 0;
    VOS_UINT16 usNul = 0;
    VOS_UINT16 usUlBandwidth = 0;
    VOS_UINT16 usDlBandwidth = 0;
    VOS_UINT32 ulResult = AT_FAILURE;
    L4A_READ_LWCLASH_CNF_STRU *pstLwclash = NULL;
    LTE_BANDINFO_STRU stCurBandinfo = {0};
    VOS_UINT16 usLength = 0;

    if (VOS_NULL == pMsgBlock)
    {
        return AT_ERROR;
    }

    pstLwclash = (L4A_READ_LWCLASH_CNF_STRU *)pMsgBlock;
    ulResult = At_ClientIdToUserId(pstLwclash->usClientId, &ucIndexId);
    
    if(AT_FAILURE == ulResult)
    {
        return AT_ERROR;
    }
    
    /*��ȡ��������ĳ���*/
    ucSumOfBandwidth = sizeof(gusbandwidth) / sizeof(gusbandwidth[0]);   
    if((pstLwclash->stLwclashInfo.usUlBandwidth >= ucSumOfBandwidth) || (pstLwclash->stLwclashInfo.usDlBandwidth >= ucSumOfBandwidth))
    {
        MBB_ATTL_DEBUG_LOG_STR("the receive bandwith beyond length of array");
        return AT_ERROR;
    }
    
    /*band�Ϸ����ж�*/
    ucSumOfBand = sizeof(gstAtBandinfo) / sizeof(gstAtBandinfo[0]);/*44*/
    if(pstLwclash->stLwclashInfo.usBand >= ucSumOfBand)
    {
        MBB_ATTL_DEBUG_LOG_STR("the received band beyond length of ucSumOfBand");
        return AT_ERROR;
    }
      
    if(VOS_OK == VOS_StrCmp("^HFREQINFO", (VOS_CHAR *)g_stParseContext[ucIndexId].pstCmdElement->pszCmdName))
    {
        stCurBandinfo = gstAtBandinfo[pstLwclash->stLwclashInfo.usBand];
        /*����Ƶ��Ndl = (Fdl - Fdl_low) + Noff_dl*/
        usNdl = (pstLwclash->stLwclashInfo.usDlFreq - stCurBandinfo.ulDLfreqlow) + stCurBandinfo.ulNoffdl;
        /*����Ƶ��Nul = (Ful - Ful_low) + Noff_ul*/
        usNul = (pstLwclash->stLwclashInfo.usUlFreq - stCurBandinfo.ulULfreqlow) + stCurBandinfo.ulNofful;
        usUlBandwidth = gusbandwidth[pstLwclash->stLwclashInfo.usUlBandwidth];/*ʹ��lwclash���ص�ö��ֵ��Ϊ������ȡ����*/
        usDlBandwidth = gusbandwidth[pstLwclash->stLwclashInfo.usDlBandwidth];/*ʹ��lwclash���ص�ö��ֵ��Ϊ������ȡ����*/

        if((0 == pstLwclash->stLwclashInfo.usBand)
        || (0 == pstLwclash->stLwclashInfo.usDlFreq)
        ||(0 == pstLwclash->stLwclashInfo.usUlFreq))
        {
            usLength = (VOS_UINT16)At_sprintf( AT_CMD_MAX_LEN, (VOS_CHAR *)pgucLAtSndCodeAddr, (VOS_CHAR *)pgucLAtSndCodeAddr,
                                           "^HFREQINFO:%d,%d,%d,%d,%d,%d,%d,%d,%d",
                                           g_AtHFreqinforeport,
                                           AT_HFREQINFO_RAT_TYPE_LTE,
                                           g_LastLteInfo.usBand,
                                           g_LastLteInfo.usNdl,
                                           g_LastLteInfo.usDlFreq, 
                                           g_LastLteInfo.usDlBandwidth,
                                           g_LastLteInfo.usNul,
                                           g_LastLteInfo.usUlFreq,
                                           g_LastLteInfo.usUlBandwidth);
        }
        else
        {
            usLength = (VOS_UINT16)At_sprintf( AT_CMD_MAX_LEN, (VOS_CHAR *)pgucLAtSndCodeAddr, (VOS_CHAR *)pgucLAtSndCodeAddr,
                                           "^HFREQINFO:%d,%d,%d,%d,%d,%d,%d,%d,%d",
                                           g_AtHFreqinforeport, 
                                           AT_HFREQINFO_RAT_TYPE_LTE,
                                           pstLwclash->stLwclashInfo.usBand,
                                           usNdl,
                                           pstLwclash->stLwclashInfo.usDlFreq, 
                                           usDlBandwidth,
                                           usNul,
                                           pstLwclash->stLwclashInfo.usUlFreq,
                                           usUlBandwidth);

            /*����ֵ�Ϸ������汾���ϱ�ֵ*/
            g_LastLteInfo.usBand = pstLwclash->stLwclashInfo.usBand;
            g_LastLteInfo.usNdl = usNdl;
            g_LastLteInfo.usDlFreq = pstLwclash->stLwclashInfo.usDlFreq;
            g_LastLteInfo.usDlBandwidth = usDlBandwidth;
            g_LastLteInfo.usNul = usNul;
            g_LastLteInfo.usUlFreq = pstLwclash->stLwclashInfo.usUlFreq;
            g_LastLteInfo.usUlBandwidth = usUlBandwidth;
        }
        CmdErrProc((VOS_UINT8)(pstLwclash->usClientId), pstLwclash->ulErrorCode, usLength, pgucLAtSndCodeAddr);
    }
    else
    {
        usLength = (VOS_UINT16)At_sprintf( AT_CMD_MAX_LEN,
                (VOS_CHAR *)pgucLAtSndCodeAddr,
                (VOS_CHAR *)pgucLAtSndCodeAddr,
                "^LWCLASH: %d,%d,%d,%d,%d,%d",
                pstLwclash->stLwclashInfo.enState,
                pstLwclash->stLwclashInfo.usUlFreq, pstLwclash->stLwclashInfo.usUlBandwidth,
                pstLwclash->stLwclashInfo.usDlFreq, pstLwclash->stLwclashInfo.usDlBandwidth,
                pstLwclash->stLwclashInfo.usBand);

        CmdErrProc((VOS_UINT8)(pstLwclash->usClientId), pstLwclash->ulErrorCode, usLength, pgucLAtSndCodeAddr);
    }
    return AT_FW_CLIENT_STATUS_READY;
}


VOS_UINT32 atSetLtersrpCnfSameProc(VOS_VOID *pMsgBlock)
{
    
    VOS_UINT16 usLength = 0;    
    VOS_UINT32 ulResult = 0;    
    VOS_INT16 sRsrp = 0;    
    VOS_INT16 sRsrq = 0;
    AT_ANLEVEL_INFO_CNF_STRU stLteRsrpInfo = {0};

    if (VOS_NULL == pMsgBlock)
    {
        return AT_FW_CLIENT_STATUS_READY;
    }
    
    /*�������нӿں������LteRsrpInfo�ṹ��*/
    ulResult = at_CsqInfoProc(pMsgBlock,&stLteRsrpInfo);   
    if(ERR_MSP_SUCCESS == ulResult)    
    {
      
        sRsrp = stLteRsrpInfo.sRsrp;        
        sRsrq = stLteRsrpInfo.sRsrq;        
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR *)pgucLAtSndCodeAddr,                    
                                        (VOS_CHAR *)pgucLAtSndCodeAddr + usLength,"%s%d,%d",
                                         "^LTERSRP:",sRsrp,
                                        sRsrq);
    }
    else
    {
        MBB_ATTL_DEBUG_LOG_STR_VAR(" ulResult",ulResult);   
    }
    CmdErrProc((VOS_UINT8)(stLteRsrpInfo.usClientId), stLteRsrpInfo.ulErrorCode,                        
                            usLength, pgucLAtSndCodeAddr);
    
    return AT_FW_CLIENT_STATUS_READY;
}



VOS_UINT32 atLtersrpIndProc(VOS_VOID *pMsgBlock)
{
    VOS_UINT16 usLength = 0;    
    VOS_UINT32 ulResult = 0;    
    VOS_INT16 sRsrp = 0;    
    VOS_INT16 sRsrq = 0;    
    AT_ANLEVEL_INFO_CNF_STRU stLteRsrpInfo = {0};

    if (VOS_NULL == pMsgBlock)
    {
        return AT_FW_CLIENT_STATUS_READY;
    }
  
    /*�������нӿں������LteRsrpInfo�ṹ��*/
    
    ulResult = at_CsqInfoProc(pMsgBlock,&stLteRsrpInfo);
    if(ERR_MSP_SUCCESS == ulResult)    
    {
        
        sRsrp = stLteRsrpInfo.sRsrp;
        sRsrq = stLteRsrpInfo.sRsrq;        
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR *)pgucLAtSndCodeAddr,                                                                    
                            (VOS_CHAR *)pgucLAtSndCodeAddr + usLength,"%s%s%d,%d%s",                                                                    
                            gaucAtCrLf, "^LTERSRP:",                                                                     
                            sRsrp,                                                                    
                            sRsrq,gaucAtCrLf);   
    }    
    else    
    {        
        MBB_ATTL_DEBUG_LOG_STR_VAR("ulResult", ulResult);   
    }
    
    At_SendResultData(AT_BROADCAST_CLIENT_INDEX_MODEM_0,pgucLAtSndCodeAddr,usLength);    
    return AT_FW_CLIENT_STATUS_READY;
}

/* MBB�Լ����ӵ�AT  ������Ӧ����*/
static const AT_L4A_MSG_FUN_TABLE_STRU g_astAtL4aCnfMsgFunTableMbb[] = { 
    {ID_MSG_L4A_LTERSRP_INFO_CNF, atSetLtersrpCnfSameProc     },
};

static const AT_L4A_MSG_FUN_TABLE_STRU g_astAtL4aIndMsgFunTableMbb[] = {
    {ID_MSG_L4A_LWCLASH_IND,    atHFreqinfoInd},
    {ID_MSG_L4A_LTERSRP_IND ,    atLtersrpIndProc},   
};

AT_L4A_MSG_FUN_TABLE_STRU* atL4aGetCnfMsgFunMbb(VOS_UINT32 ulMsgId)
{
    VOS_UINT32 i = 0;
    VOS_UINT32 ulTableLen = 0;
    const AT_L4A_MSG_FUN_TABLE_STRU* pTable;

    pTable = g_astAtL4aCnfMsgFunTableMbb;
    ulTableLen = sizeof(g_astAtL4aCnfMsgFunTableMbb);

    for(i = 0; i < (ulTableLen/sizeof(AT_L4A_MSG_FUN_TABLE_STRU)); i++)
    {
        if(ulMsgId == (pTable + i)->ulMsgId)
        {
            return (AT_L4A_MSG_FUN_TABLE_STRU*)(pTable + i);
        }
    }
    return NULL;
}

AT_L4A_MSG_FUN_TABLE_STRU* atL4aGetIndMsgFunMbb(VOS_UINT32 ulMsgId)
{
    VOS_UINT32 i = 0;
    VOS_UINT32 ulTableLen = 0;
    const AT_L4A_MSG_FUN_TABLE_STRU* pTable;

    pTable = g_astAtL4aIndMsgFunTableMbb;
    ulTableLen = sizeof(g_astAtL4aIndMsgFunTableMbb);

    for(i = 0; i < (ulTableLen/sizeof(AT_L4A_MSG_FUN_TABLE_STRU)); i++)
    {
        if(ulMsgId == (pTable + i)->ulMsgId)
        {
            return (AT_L4A_MSG_FUN_TABLE_STRU*)(pTable + i);
        }
    }
    return NULL;
}






