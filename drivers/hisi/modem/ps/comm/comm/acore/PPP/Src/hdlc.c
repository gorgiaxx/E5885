/*-
 * Copyright (c) 1996 - 2001 Brian Somers <brian@Awfulhak.org>
 *          based on work by Toshiharu OHNO <tony-o@iij.ad.jp>
 *                           Internet Initiative Japan, Inc (IIJ)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD: stable/9/usr.sbin/ppp/hdlc.c 134789 2004-09-05 01:46:52Z brian $
 */

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/******************************************************************************
   ͷ�ļ�����
******************************************************************************/
#include "mdrv.h"
#include "TTFComm.h"
#include "soc_sctrl_interface.h"
#include "PPP/Inc/ppp_public.h"
#include "PPP/Inc/layer.h"
#include "PPP/Inc/ppp_mbuf.h"
#include "PPP/Inc/ppp_fsm.h"
#include "PPP/Inc/hdlc.h"
#include "PPP/Inc/throughput.h"
#include "PPP/Inc/lcp.h"
#include "PPP/Inc/async.h"
#include "PPP/Inc/auth.h"
#include "PPP/Inc/ipcp.h"
#include "PPP/Inc/pppid.h"
#include "PPP/Inc/link.h"
#include "PPP/Inc/ppp_init.h"
#if (FEATURE_ON == FEATURE_HARDWARE_HDLC_FUNC)
#include "PPP/Inc/hdlc_hardware.h"
#endif
	
/*****************************************************************************
    Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_HDLC_C

#if(FEATURE_ON == FEATURE_PPP)

VOS_UINT16 const fcstab[256] = {
   /* 00 */ 0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
   /* 08 */ 0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
   /* 10 */ 0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
   /* 18 */ 0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
   /* 20 */ 0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
   /* 28 */ 0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
   /* 30 */ 0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
   /* 38 */ 0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
   /* 40 */ 0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
   /* 48 */ 0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
   /* 50 */ 0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
   /* 58 */ 0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
   /* 60 */ 0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
   /* 68 */ 0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
   /* 70 */ 0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
   /* 78 */ 0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
   /* 80 */ 0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
   /* 88 */ 0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
   /* 90 */ 0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
   /* 98 */ 0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
   /* a0 */ 0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
   /* a8 */ 0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
   /* b0 */ 0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
   /* b8 */ 0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
   /* c0 */ 0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
   /* c8 */ 0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
   /* d0 */ 0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
   /* d8 */ 0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
   /* e0 */ 0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
   /* e8 */ 0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
   /* f0 */ 0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
   /* f8 */ 0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

#if (FEATURE_ON == FEATURE_HARDWARE_HDLC_FUNC)
VOS_UINT_PTR    g_ulPppVirtAddr         = 0;
VOS_UINT_PTR    g_ulPppPhyAddr          = 0;
VOS_UINT32      g_ulPppTotalBufLen      = 0;
#endif

/******************************************************************************
   5 ����ʵ��
******************************************************************************/
void
hdlc_Init(struct hdlc *hdlc, struct lcp *lcp)
{
  PSACORE_MEM_SET(hdlc, sizeof(struct hdlc), '\0', sizeof(struct hdlc));
  hdlc->lqm.owner = lcp;
}

/*
 *  HDLC FCS computation. Read RFC 1171 Appendix B and CCITT X.25 section
 *  2.27 for further details.
 */
VOS_UINT16
hdlc_Fcs(VOS_CHAR *cp, VOS_UINT32 len)
{
  VOS_UINT16 fcs = INITFCS;

  while (len--)
    fcs = (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff];

  return fcs;
}

struct ppp_mbuf *
hdlc_LayerPush(struct link *l, struct ppp_mbuf *bp,
               VOS_INT32 pri, VOS_UINT16 *proto)
{
  return bp;
}

PPP_ZC_STRU *hdlc_LayerPull(struct link *l, PPP_ZC_STRU *pstMem, VOS_UINT16 *proto)
{
  VOS_UINT16    usFcs;
  VOS_UINT16    usLen;

  usLen = PPP_ZC_GET_DATA_LEN(pstMem);

  usFcs = hdlc_Fcs(PPP_ZC_GET_DATA_PTR(pstMem), usLen);

  /* PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, PS_PRINT_NORMAL, "hdlc_LayerPull: fcs = <1>\r\n",usFcs); */

  l->hdlc.lqm.ifInOctets += usLen + 1;        /* plus 1 flag octet! */

  if (usFcs != GOODFCS)
  {
    l->hdlc.lqm.ifInErrors++;
    l->hdlc.stats.badfcs++;
    PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING, "bad hdlc fcs\r\n");
    PPP_MemFree(pstMem);

    return VOS_NULL_PTR;
  }

  /* Either done here or by the sync layer */
  l->hdlc.lqm.lqr.InGoodOctets += usLen + 1;  /* plus 1 flag octet! */
  l->hdlc.lqm.ifInUniPackets++;

  /* rfc1662 section 4.3 */
  if (usLen < 4)
  {
    PPP_MemFree(pstMem);

    return VOS_NULL_PTR;
  }

  PPP_MemCutTailData(&pstMem, (VOS_UINT8 *)(&usFcs), 2, PPP_ZC_UL_RESERVE_LEN);

  return pstMem;
}

struct layer hdlclayer = { LAYER_HDLC, "hdlc", hdlc_LayerPush, hdlc_LayerPull };

#if (FEATURE_ON == FEATURE_HARDWARE_HDLC_FUNC)

VOS_UINT_PTR PPP_PHY_TO_VIRT(VOS_UINT_PTR ulPAddr)
{
    return (VOS_UINT_PTR)phys_to_virt(ulPAddr);
}


VOS_UINT_PTR PPP_VIRT_TO_PHY(VOS_UINT_PTR ulVAddr)
{
    return (VOS_UINT_PTR)virt_to_phys((void *)ulVAddr);
}


VOS_UINT_PTR PPP_HDLC_NODE_PHY_TO_VIRT(VOS_UINT_PTR ulPAddr)
{
    return VOS_UncacheMemPhyToVirt((VOS_UINT8 *)ulPAddr, (VOS_UINT8 *)g_ulPppPhyAddr, (VOS_UINT8 *)g_ulPppVirtAddr, g_ulPppTotalBufLen);
}


VOS_UINT_PTR PPP_HDLC_NODE_VIRT_TO_PHY(VOS_UINT_PTR ulVAddr)
{
    return VOS_UncacheMemVirtToPhy((VOS_UINT8 *)ulVAddr, (VOS_UINT8 *)g_ulPppPhyAddr, (VOS_UINT8 *)g_ulPppVirtAddr, g_ulPppTotalBufLen);
}

VOS_UINT32 PPP_HDLC_HARD_InitBuf(VOS_VOID)
{
    /* �����ڴ泤�ȣ�ʵ��ʹ�ó���Ϊ0x6140�����밴��0x8000(32K) */
    g_ulPppTotalBufLen  = PAGE_ALIGN(TTF_HDLC_MASTER_LINK_TOTAL_LEN + PAGE_SIZE);

    /* ����ϵͳ�ӿ�dma_alloc_coherent����uncache�ڴ� */
    g_ulPppVirtAddr     = (VOS_UINT_PTR)VOS_UnCacheMemAlloc(g_ulPppTotalBufLen, &g_ulPppPhyAddr);
    if (VOS_NULL_PTR == g_ulPppVirtAddr)
    {
        PPP_HDLC_ERROR_LOG("PPP_HDLC_HARD_InitBuf, ERROR, VOS_UnCacheMemAlloc Fail\r\n");
        return VOS_ERR;
    }

    PSACORE_MEM_SET((void *)g_ulPppVirtAddr, g_ulPppTotalBufLen, 0, g_ulPppTotalBufLen);

    /* ����TtfMemoryMap.h��ʼ��HDLC�����ڴ� */
    g_pstHdlcDefBufInfo = (HDLC_DEF_BUFF_INFO_STRU *)g_ulPppVirtAddr;
    g_pstHdlcFrmBufInfo = (HDLC_FRM_BUFF_INFO_STRU *)(g_ulPppVirtAddr + sizeof(HDLC_DEF_BUFF_INFO_STRU));

    /* TTF_HDLC_MASTER_DEF_BUF_LEN������ṹHDLC_DEF_BUFF_INFO_STRU�Ĵ�Сһ�� */
    if (PPP_HEDL_HRPD_CHECK_STRU_SIZE(TTF_HDLC_MASTER_DEF_BUF_LEN , HDLC_DEF_BUFF_INFO_STRU))
    {
        PPP_HDLC_ERROR_LOG2("PPP_HDLC_HARD_InitBuf, ERROR, TTF_HDLC_MASTER_DEF_BUF_LEN %d sizeof(HDLC_DEF_BUFF_INFO_STRU) %d\r\n",
                      TTF_HDLC_MASTER_DEF_BUF_LEN, sizeof(HDLC_DEF_BUFF_INFO_STRU));
        return VOS_ERR;
    }

    /* TTF_HDLC_MASTER_FRM_BUF_LEN�ձ�����ṹHDLC_FRM_BUFF_INFO_STRU�Ĵ�Сһ�� */
    if (PPP_HEDL_HRPD_CHECK_STRU_SIZE(TTF_HDLC_MASTER_FRM_BUF_LEN, HDLC_FRM_BUFF_INFO_STRU))
    {
        PPP_HDLC_ERROR_LOG2("PPP_HDLC_HARD_InitBuf, ERROR, TTF_HDLC_MASTER_FRM_BUF_LEN %d sizeof(HDLC_FRM_BUFF_INFO_STRU) %d\r\n",
                      TTF_HDLC_MASTER_FRM_BUF_LEN, sizeof(HDLC_FRM_BUFF_INFO_STRU));
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_VOID PPP_HDLC_HARD_MntnDefTraceInput
(
    HDLC_DEF_BUFF_INFO_STRU         *pstDefBuffInfo,
    HDLC_PARA_LINK_BUILD_INFO_STRU  *pstBuildInfo
)
{
    VOS_UINT32                          ulNodeLoop;
    HDLC_PARA_LINK_NODE_STRU           *pstParaNode;

    if ((g_stHdlcConfigInfo.ulHdlcMntnTraceCfg & PPP_HDLC_MNTN_TRACE_PARA) != 0)
    {
        PPP_HDLC_HARD_MntnTraceInputParaLink(ID_HDLC_MNTN_DEF_INPUT_PARA,
                                             pstBuildInfo->ulInputLinkNodeCnt,
                                             pstBuildInfo->ulInputLinkTotalSize,
                                             &(pstDefBuffInfo->astInputParaLinkNodeBuf[0]));
    }

    if ((g_stHdlcConfigInfo.ulHdlcMntnTraceCfg & PPP_HDLC_MNTN_TRACE_DATA) != 0)
    {
        /* ��¼���ϱ������������нڵ���������ݣ�ÿ���ڵ���һ��IP�� */
        for ( ulNodeLoop = 0; ulNodeLoop < pstBuildInfo->ulInputLinkNodeCnt; ulNodeLoop++ )
        {
            pstParaNode = &(pstDefBuffInfo->astInputParaLinkNodeBuf[ulNodeLoop]);

            PPP_HDLC_HARD_MntnTraceSingleData(pstParaNode->usDataLen, (VOS_UINT8*)PPP_PHY_TO_VIRT((VOS_UINT32)(pstParaNode->pucDataAddr)),
                                              ID_HDLC_MNTN_DEF_INPUT_DATA, ulNodeLoop);

            /* ��Ҫ������д��DDR��HDLC��DDR�ж����� */
            PPP_HDLC_CACHE_FLUSH((VOS_UINT8 *)PPP_PHY_TO_VIRT((VOS_UINT32)(pstParaNode->pucDataAddr)), pstParaNode->usDataLen);
        }
    }

    return;
}


VOS_VOID PPP_HDLC_HARD_MntnDefTraceOutput
(
    VOS_UINT16                          usValidFrameNum,
    HDLC_DEF_BUFF_INFO_STRU            *pstDefBuffInfo
)
{
    HDLC_MNTN_DEF_OUTPUT_PARA_STRU     *pstOutputPara;
    VOS_UINT32                          ulDataLen;
    VOS_UINT32                          ulNodeLoop;
    HDLC_DEF_RPT_NODE_STRU             *pstRptNode;
    VOS_UINT16                          usMaxFrameNum;


    /* ���װ�ϱ��ռ��ά�ɲ� */
    if ((g_stHdlcConfigInfo.ulHdlcMntnTraceCfg & PPP_HDLC_MNTN_TRACE_PARA) != 0)
    {
        usMaxFrameNum = (HDLC_MNTN_ALLOC_MEM_MAX_SIZE - sizeof(HDLC_MNTN_DEF_OUTPUT_PARA_STRU)) /
                         sizeof(HDLC_DEF_RPT_NODE_STRU);
        usMaxFrameNum = TTF_MIN(usMaxFrameNum, usValidFrameNum);

        ulDataLen     = sizeof(HDLC_MNTN_DEF_OUTPUT_PARA_STRU) + usMaxFrameNum * sizeof(HDLC_DEF_RPT_NODE_STRU);
        pstOutputPara = (HDLC_MNTN_DEF_OUTPUT_PARA_STRU *)PS_MEM_ALLOC(PS_PID_PPP_HDLC, ulDataLen);

        if (VOS_NULL_PTR == pstOutputPara)
        {
            PPP_HDLC_NORMAL_LOG1("PPP_HDLC_HARD_MntnDefTraceOutput, NORMAL, Alloc mem failed ulDataLen %!\r\n", ulDataLen);
            return;
        }

        pstOutputPara->usDefValidNum = usValidFrameNum;
        pstOutputPara->usTraceNum    = usMaxFrameNum;

        VOS_MemCpy_s((VOS_UINT8 *)(pstOutputPara + 1),
                    usMaxFrameNum * sizeof(HDLC_DEF_RPT_NODE_STRU),
                   (VOS_UINT8 *)(&(pstDefBuffInfo->astRptNodeBuf[0])),
                   usMaxFrameNum * sizeof(HDLC_DEF_RPT_NODE_STRU));

        PPP_HDLC_HARD_MntnTraceMsg((HDLC_MNTN_TRACE_HEAD_STRU *)pstOutputPara,
                                   ID_HDLC_MNTN_DEF_OUTPUT_PARA, ulDataLen);

        PS_MEM_FREE(PS_PID_PPP_HDLC, pstOutputPara);
    }

    /* ���װĿ�Ŀռ���ÿ����Ч֡��ά�ɲ� */
    if ((g_stHdlcConfigInfo.ulHdlcMntnTraceCfg & PPP_HDLC_MNTN_TRACE_DATA) != 0)
    {
        for ( ulNodeLoop = 0; ulNodeLoop < usValidFrameNum; ulNodeLoop++ )
        {
            pstRptNode = &(pstDefBuffInfo->astRptNodeBuf[ulNodeLoop]);

            PPP_HDLC_HARD_MntnTraceSingleData(pstRptNode->usDefOutOneLen, (VOS_UINT8*)PPP_HDLC_NODE_PHY_TO_VIRT((VOS_UINT32)(pstRptNode->pucDefOutOneAddr)),
                                              ID_HDLC_MNTN_DEF_OUTPUT_DATA, ulNodeLoop);
        }
    }

    return;
}


VOS_VOID PPP_HDLC_HARD_MntnFrmTraceInput
(
    HDLC_FRM_BUFF_INFO_STRU         *pstFrmBuffInfo,
    HDLC_PARA_LINK_BUILD_INFO_STRU  *pstBuildInfo
)
{
    VOS_UINT32                          ulNodeLoop;
    HDLC_PARA_LINK_NODE_STRU           *pstParaNode;


    if ((g_stHdlcConfigInfo.ulHdlcMntnTraceCfg & PPP_HDLC_MNTN_TRACE_PARA) != 0)
    {
        PPP_HDLC_HARD_MntnTraceInputParaLink(ID_HDLC_MNTN_FRM_INPUT_PARA,
                                             pstBuildInfo->ulInputLinkNodeCnt,
                                             pstBuildInfo->ulInputLinkTotalSize,
                                             &(pstFrmBuffInfo->astInputParaLinkNodeBuf[0]));
    }

    if ((g_stHdlcConfigInfo.ulHdlcMntnTraceCfg & PPP_HDLC_MNTN_TRACE_DATA) != 0)
    {
        /* ��¼���ϱ������������нڵ���������ݣ�ÿ���ڵ���һ��IP�� */
        for ( ulNodeLoop = 0; ulNodeLoop < pstBuildInfo->ulInputLinkNodeCnt; ulNodeLoop++ )
        {
            pstParaNode = &(pstFrmBuffInfo->astInputParaLinkNodeBuf[ulNodeLoop]);

            PPP_HDLC_HARD_MntnTraceSingleData(pstParaNode->usDataLen, (VOS_UINT8*)PPP_PHY_TO_VIRT((VOS_UINT32)(pstParaNode->pucDataAddr)),
                                              ID_HDLC_MNTN_FRM_INPUT_DATA, ulNodeLoop);

            /* ��Ҫ������д��DDR��HDLC��DDR�ж����� */
            PPP_HDLC_CACHE_FLUSH((VOS_UINT8*)PPP_PHY_TO_VIRT((VOS_UINT32)(pstParaNode->pucDataAddr)), pstParaNode->usDataLen);
        }
    }

    return;
}


VOS_VOID PPP_HDLC_HARD_MntnFrmTraceOutput
(
    VOS_UINT8                           ucFrmValidNum,
    VOS_UINT16                          usFrmOutSegNum,
    HDLC_FRM_BUFF_INFO_STRU            *pstFrmBuffInfo,
    HDLC_PARA_LINK_BUILD_INFO_STRU     *pstBuildInfo
)
{
    HDLC_MNTN_FRM_OUTPUT_PARA_STRU      stOutputPara;
    HDLC_MNTN_FRM_OUTPUT_PARA_STRU     *pstOutputPara = &stOutputPara;
    VOS_UINT32                          ulDataLen;
    VOS_UINT32                          ulNodeLoop;
    HDLC_PARA_LINK_NODE_STRU           *pstParaNode;

    if ((g_stHdlcConfigInfo.ulHdlcMntnTraceCfg & PPP_HDLC_MNTN_TRACE_PARA) != 0)
    {
        ulDataLen     = sizeof(HDLC_MNTN_FRM_OUTPUT_PARA_STRU);

        pstOutputPara->ulOutputLinkNodeCnt   = pstBuildInfo->ulOutputLinkNodeCnt;
        pstOutputPara->ulOutputLinkTotalSize = pstBuildInfo->ulOutputLinkTotalSize;
        pstOutputPara->ucFrmValidNum         = ucFrmValidNum;
        pstOutputPara->usOutputNodeUsedCnt   = usFrmOutSegNum;

        VOS_MemCpy_s((VOS_UINT8 *)(&(pstOutputPara->astOutputParaLinkNodeBuf[0])),
                    TTF_HDLC_FRM_OUTPUT_PARA_LINK_MAX_NUM * sizeof(HDLC_PARA_LINK_NODE_STRU),
                   (VOS_UINT8 *)(&(pstFrmBuffInfo->astOutputParaLinkNodeBuf[0])),
                   TTF_HDLC_FRM_OUTPUT_PARA_LINK_MAX_NUM * sizeof(HDLC_PARA_LINK_NODE_STRU));

        VOS_MemCpy_s((VOS_UINT8 *)(&(pstOutputPara->astRptNodeBuf[0])),
                    TTF_HDLC_FRM_RPT_MAX_NUM * sizeof(HDLC_FRM_RPT_NODE_STRU),
                   (VOS_UINT8 *)(&(pstFrmBuffInfo->astRptNodeBuf[0])),
                   TTF_HDLC_FRM_RPT_MAX_NUM * sizeof(HDLC_FRM_RPT_NODE_STRU));

        PPP_HDLC_HARD_MntnTraceMsg((HDLC_MNTN_TRACE_HEAD_STRU *)pstOutputPara,
                                        ID_HDLC_MNTN_FRM_OUTPUT_PARA, ulDataLen);
    }

    if ((g_stHdlcConfigInfo.ulHdlcMntnTraceCfg & PPP_HDLC_MNTN_TRACE_DATA) != 0)
    {
        for ( ulNodeLoop = 0; ulNodeLoop < usFrmOutSegNum; ulNodeLoop++ )
        {
            pstParaNode = &(pstFrmBuffInfo->astOutputParaLinkNodeBuf[ulNodeLoop]);

            PPP_HDLC_HARD_MntnTraceSingleData(pstParaNode->usDataLen, (VOS_UINT8*)PPP_PHY_TO_VIRT((VOS_UINT32)(pstParaNode->pucDataAddr)),
                                              ID_HDLC_MNTN_FRM_OUTPUT_DATA, ulNodeLoop);
        }
    }

    return;
}


VOS_VOID PPP_HDLC_HARD_CommReleaseLink
(
    PPP_ZC_STRU     **ppstLinkNode,
    VOS_UINT32        ulRelCnt
)
{
    VOS_UINT32                          ulNodeLoop;


    for ( ulNodeLoop = 0; ulNodeLoop < ulRelCnt; ulNodeLoop++ )
    {
        PPP_MemFree(ppstLinkNode[ulNodeLoop]);

        ppstLinkNode[ulNodeLoop] = VOS_NULL_PTR;
    }

    return;
}


PPP_ZC_STRU * PPP_HDLC_HARD_DefProcRptNode
(
    HDLC_DEF_RPT_NODE_STRU             *pstRptNode
)
{
    PPP_ZC_STRU                        *pstMem;
    VOS_UINT32                          usFistSegLen;
    VOS_UINT8                          *pucDefOutOneAddr;


    if ( (0 == pstRptNode->usDefOutOneLen) || (HDLC_DEF_OUT_PER_MAX_CNT < pstRptNode->usDefOutOneLen) )
    {
        PPP_HDLC_WARNING_LOG1("PPP_HDLC_HARD_DefProcValidFrames, WARNING, invalid usDefOutOneLen %d\r\n",
                      pstRptNode->usDefOutOneLen);

        return VOS_NULL_PTR;
    }

    pstMem = PPP_MemAlloc(pstRptNode->usDefOutOneLen, PPP_ZC_UL_RESERVE_LEN);

    if (VOS_NULL_PTR == pstMem)
    {
        return VOS_NULL_PTR;
    }

    pucDefOutOneAddr    = (VOS_UINT8*)PPP_HDLC_NODE_PHY_TO_VIRT((VOS_UINT32)pstRptNode->pucDefOutOneAddr);
    /* �жϸ�֡��ʼ�ӳ����Ƿ񳬹�����ռ�β�����������ƻش��� */
    if ((pucDefOutOneAddr + pstRptNode->usDefOutOneLen) >
        HDLC_DEF_OUTPUT_BUF_END_ADDR)
    {
        if (pucDefOutOneAddr <= HDLC_DEF_OUTPUT_BUF_END_ADDR)
        {
            usFistSegLen = (VOS_UINT32)(HDLC_DEF_OUTPUT_BUF_END_ADDR - pucDefOutOneAddr);

            /* ��������ʼ��ַ������ռ�β�������� */
            PPP_MemSingleCopy(PPP_ZC_GET_DATA_PTR(pstMem), pucDefOutOneAddr,
                              usFistSegLen);

            /* ����������ռ��ײ���ʣ������ */
            PPP_MemSingleCopy(PPP_ZC_GET_DATA_PTR(pstMem) + usFistSegLen, HDLC_DEF_OUTPUT_BUF_START_ADDR,
                              pstRptNode->usDefOutOneLen - usFistSegLen);
        }
        else
        {
            PPP_MemFree(pstMem);

            PPP_HDLC_ERROR_LOG2("\r\nPPP, PPP_HDLC_HARD_DefProcRptNode, ERROR, Error pucDefOutOneAddr.\r\n",
                    pucDefOutOneAddr, HDLC_DEF_OUTPUT_BUF_END_ADDR);

            PPP_HDLC_HARD_MntnShowDefReg();

            return VOS_NULL_PTR;
        }
    }
    else
    {
        PPP_MemSingleCopy(PPP_ZC_GET_DATA_PTR(pstMem), pucDefOutOneAddr,
                          pstRptNode->usDefOutOneLen);
    }

    /* �����㿽�����ݳ��� */
    PPP_ZC_SET_DATA_LEN(pstMem, pstRptNode->usDefOutOneLen);

    return pstMem;
}


VOS_VOID PPP_HDLC_HARD_DefProcValidFrames
(
    VOS_UINT32          ulMode,
        PPP_ID          usPppId,
    struct link        *pstLink
)
{
    VOS_UINT16                          usValidFrameNum;
    HDLC_DEF_BUFF_INFO_STRU            *pstDefBuffInfo;
    VOS_UINT32                          ulFrameLoop;
    HDLC_DEF_RPT_NODE_STRU             *pstRptNode;
    PPP_ZC_STRU                        *pstMem;


    pstDefBuffInfo  = HDLC_DEF_GET_BUF_INFO(usPppId);
    usValidFrameNum = (VOS_UINT16)TTF_Read32RegByBit(SOC_ARM_HDLC_DEF_STATUS_ADDR(HDLC_IP_BASE_ADDR), 8, 23);

    /* �ϱ����װ�����ݿ�ά�ɲ�:�ϱ��ռ���Ϣ��������� */
    PPP_HDLC_HARD_MntnDefTraceOutput(usValidFrameNum, pstDefBuffInfo);

    /* ��Ч֡�����ֵ��� */
    if (TTF_HDLC_DEF_RPT_MAX_NUM < usValidFrameNum)
    {
        PPP_HDLC_WARNING_LOG2("PPP_HDLC_HARD_DefProcValidFrames, WARNING, usValidFrameNum = %d > TTF_HDLC_DEF_RPT_MAX_NUM = %d",
                      usValidFrameNum, TTF_HDLC_DEF_RPT_MAX_NUM);
        return;
    }

    g_PppHdlcHardStat.ulDefMaxValidCntOnce = TTF_MAX(g_PppHdlcHardStat.ulDefMaxValidCntOnce, usValidFrameNum);

    /* ��Ŀ�Ŀռ����Ч֡���ݿ������㿽���ڴ棬���ݲ������͵������з����ӿ� */
    for ( ulFrameLoop = 0 ; ulFrameLoop < usValidFrameNum; ulFrameLoop++ )
    {
        pstRptNode = &(pstDefBuffInfo->astRptNodeBuf[ulFrameLoop]);

        pstMem = PPP_HDLC_HARD_DefProcRptNode(pstRptNode);

        /* ���벻�����ݻ��ϱ���Ϣ���󣬶�������Ч֡ */
        if (VOS_NULL_PTR == pstMem)
        {
            continue;
        }

        if (HDLC_IP_MODE == ulMode)
        {
            PPP_HDLC_ProcIpModeUlData(pstLink, pstMem, pstRptNode->usDefOutOnePro);
        }
        else
        {
            PPP_HDLC_ProcPppModeUlData(usPppId, pstMem);
        }
    }

    return;
}


VOS_VOID PPP_HDLC_HARD_DefProcErrorFrames
(
    struct link        *pstLink
)
{
    VOS_UINT8                           ucErrType;
    VOS_UINT8                           ucMask;
    VOS_UINT32                          ulErrTypeLoop;
    VOS_UINT8                           ucResult;
    HDLC_DEF_ERR_FRAMES_CNT_STRU        stErrCnt;


    /* ��ѯ״̬�Ĵ���hdlc_def_status (0x88)�ĵ�24:30��Ӧ����λΪ1��ʾ��ĳ�ִ���֡�����
       Ϊ0��ʾ��֡��� */
    ucErrType = (VOS_UINT8)TTF_Read32RegByBit(SOC_ARM_HDLC_DEF_STATUS_ADDR(HDLC_IP_BASE_ADDR) , 24, 30);

    if (0 == ucErrType)
    {
        return;
    }

    /* get fcs error count */
    stErrCnt.usFCSErrCnt        = (VOS_UINT16)TTF_Read32RegByBit(SOC_ARM_HDLC_DEF_ERR_INFO_0_ADDR(HDLC_IP_BASE_ADDR), 0, 15);

    /* get frame too long error count */
    stErrCnt.usLenLongCnt       = (VOS_UINT16)TTF_Read32RegByBit(SOC_ARM_HDLC_DEF_ERR_INFO_0_ADDR(HDLC_IP_BASE_ADDR), 16, 31);

    /* get frame too short error count */
    stErrCnt.usLenShortCnt      = (VOS_UINT16)TTF_Read32RegByBit(SOC_ARM_HDLC_DEF_ERR_INFO_1_ADDR(HDLC_IP_BASE_ADDR), 0, 15);

    /* get error protocol count */
    stErrCnt.usErrProtocolCnt   = (VOS_UINT16)TTF_Read32RegByBit(SOC_ARM_HDLC_DEF_ERR_INFO_1_ADDR(HDLC_IP_BASE_ADDR), 16, 31);

    /* get error control count */
    stErrCnt.usErrCtrlCnt       = (VOS_UINT16)TTF_Read32RegByBit(SOC_ARM_HDLC_DEF_ERR_INFO_2_ADDR(HDLC_IP_BASE_ADDR), 0, 15);

    /* get error address count */
    stErrCnt.usErrAddrCnt       = (VOS_UINT16)TTF_Read32RegByBit(SOC_ARM_HDLC_DEF_ERR_INFO_2_ADDR(HDLC_IP_BASE_ADDR), 16, 31);

    /* get error flag position count */
    stErrCnt.usFlagPosErrCnt    = (VOS_UINT16)TTF_Read32RegByBit(SOC_ARM_HDLC_DEF_ERR_INFO_3_ADDR(HDLC_IP_BASE_ADDR), 0, 15);
    for (ulErrTypeLoop = 0UL; ulErrTypeLoop < HDLC_DEF_MAX_TYPE_CNT; ulErrTypeLoop++)
    {
        ucMask   = SET_BITS_VALUE_TO_BYTE(0x01, ulErrTypeLoop);    /* �������� */
        ucResult = (VOS_UINT8)GET_BITS_FROM_BYTE(ucErrType, ucMask);

        if (0 != ucResult)      /* ���ڴ������ */
        {
            if (0UL == ulErrTypeLoop)   /* ��������0: CRCУ����� */
            {
                pstLink->hdlc.stats.badfcs       += stErrCnt.usFCSErrCnt;
                pstLink->hdlc.lqm.ifInErrors   += stErrCnt.usFCSErrCnt;
                PPP_HDLC_WARNING_LOG("bad hdlc fcs\r\n");
            }
            else if (1UL == ulErrTypeLoop)    /* ��������1: ���װ��֡�ֽ�������1502bytes */
            {
                pstLink->hdlc.lqm.ifInErrors += stErrCnt.usLenLongCnt;
                PPP_HDLC_WARNING_LOG("bad hdlc frame length too long\r\n");
            }
            else if (2UL == ulErrTypeLoop)    /* ��������2: ���װ��֡�ֽ���С��4bytes */
            {
                pstLink->hdlc.lqm.ifInErrors += stErrCnt.usLenShortCnt;
                PPP_HDLC_WARNING_LOG("bad hdlc frame length too short\r\n");
            }
            else if (3UL == ulErrTypeLoop)    /* ��������3: ��P�������ʱ, �յ��Ƿ���Protocol��ֵ(��*******0 *******1��ʽ) */
            {
                pstLink->hdlc.lqm.ifInErrors += stErrCnt.usErrProtocolCnt;
                PPP_HDLC_WARNING_LOG("bad hdlc frame protocol\r\n");
            }
            else if (4UL == ulErrTypeLoop)    /* ��������4: ��AC����ѹ��ʱ, Control��ֵ��0x03 */
            {
                pstLink->hdlc.lqm.ifInErrors += stErrCnt.usErrCtrlCnt;
                pstLink->hdlc.stats.badcommand += stErrCnt.usErrCtrlCnt;
                PPP_HDLC_NORMAL_LOG("bad hdlc frame control\r\n");
            }
            else if (5UL == ulErrTypeLoop)    /* ��������5: ��AC����ѹ��ʱ, Address��ֵ��0xFF */
            {
                pstLink->hdlc.lqm.ifInErrors += stErrCnt.usErrAddrCnt;
                pstLink->hdlc.stats.badaddr    += stErrCnt.usErrAddrCnt;
                PPP_HDLC_NORMAL_LOG("bad hdlc frame address\r\n");
            }
            else if (6UL == ulErrTypeLoop)    /* ��������6: ת���ַ�0x7D�����һ��Flag�� */
            {
                pstLink->hdlc.lqm.ifInErrors += stErrCnt.usFlagPosErrCnt;
                PPP_HDLC_NORMAL_LOG("bad hdlc frame flag position\r\n");
            }
            else
            {
                PPP_HDLC_NORMAL_LOG("link_HDLCDefDealErr err\r\n");
            }
        }
    }

    return;
}


VOS_UINT32 PPP_HDLC_HARD_DefCfgReg
(
    VOS_UINT32                      ulMode,
    struct link                    *pstLink,
    HDLC_DEF_UNCOMPLETED_INFO_STRU *pstDefUncompletedInfo
)
{
    /*
        hdlc_def_cfg  (0x70)
         31                           4 3     2   1     0
        |-------------------------------|-------|-----|-----|
        |              Rsv              |  Pfc  | Acfc| ago |
        Reserved             [31:4]  28'b0   h/s R/W  ����λ����ʱ����0��дʱ��Ӱ�졣
        def_pfc              [3:2]   2'b0    h/s R/W  P��ѹ��ָʾ��00��P����ѹ��������룻01��P��ѹ��������룻11��P�򲻰��룻��������Ч��
        def_acfc             [1]     1'b0    h/s R/W  AC��ѹ��ָʾ��0��AC����ѹ����1����ʾAC��ѹ����
        def_uncompleted_ago  [0]     1'b0    h/s R/W  ����ָʾ��Ӧ��ǰ���װ��������ͬһPPP/IP���ŵ���ǰ������װ���������Ƿ��н��������֡��
                                                      Ϊ��֧�ֶ��PPP/IP���Ŷ����ӵ����ã�0��û�У�1����
        */

    VOS_UINT32       ulDeframerCfg;
    VOS_UINT8        ucACComp;
    VOS_UINT8        ucPComp;
    VOS_UINT8        ucLowByte;
    VOS_UINT16       usLowWord;

    /* 1.����ulMode��P���AC���Ƿ�ѹ������hdlc_def_cfg (0x70) */

    /* ��ȡAC��ѹ��ָʾ, P��ѹ��ָʾ */
    ucACComp = (1 == pstLink->lcp.want_acfcomp) ? 1 : 0;

    if (HDLC_IP_MODE == ulMode) /* IPģʽ: P�򲻺��� */
    {
        ucPComp = (1 == pstLink->lcp.want_protocomp)
                   ? HDLC_PROTOCOL_REMOVE_WITH_COMPRESS
                   : HDLC_PROTOCOL_REMOVE_WITHOUT_COMPRESS;
    }
    else                        /* PPPģʽ: P����� */
    {
        ucPComp = HDLC_PROTOCOL_NO_REMOVE;
    }

    /* ��ucPComp���õ�һ���ֽڵĵ�2, 3λ�� */
    ucLowByte = SET_BITS_VALUE_TO_BYTE(ucPComp, HDLC_DEF_PFC_BITPOS);

    if (1 == ucACComp)  /* AC��ѹ�� */
    {
        SET_BIT_TO_BYTE(ucLowByte, HDLC_DEF_ACFC_BITPOS);
    }

    /* 2.���÷�����֡�����Ϣ */
    if ( (VOS_NULL_PTR != pstDefUncompletedInfo) &&
        (HDLC_DEF_UNCOMPLETED_EXIST == pstDefUncompletedInfo->ucExistFlag) )
    {
        /* def_uncompleted_ago��1��ʾ���ϴ�����ķ�����֡���뱾�ν��װ */
        SET_BIT_TO_BYTE(ucLowByte, HDLC_DEF_IS_UNCOMPLETED_AGO_BITPOS);

        /*
        def_uncompleted_len  (0x74)
         31                 16  15                  0
        |---------------------|----------------------|
        |         Rsv         |         Len          |
        Reserved             [31:16] 16'b0   h/s R/W  ����λ����ʱ����0��дʱ��Ӱ�졣
        def_uncompleted_len  [15:0]  16'b0   h/s R/W  ��Ӧ��ǰ���װ��������ͬһPPP/IP���ŵ���ǰ������װ����������������֡�ĳ��ȣ�Ϊ��֧�ֶ��PPP/IP���Ŷ����ӵ�����
        */
        PPP_HDLC_WRITE_32REG(SOC_ARM_HDLC_DEF_UNCOMPLETED_LEN_ADDR(HDLC_IP_BASE_ADDR),
                        (VOS_UINT32)pstDefUncompletedInfo->usDefOutOneLen & 0xFFFF);

        /*
        def_uncompleted_pro  (0x78)
         31                 16  15                  0
        |---------------------|----------------------|
        |         Rsv         |         Pro          |
        Reserved             [31:16] 16'b0   h/s R/W  ����λ����ʱ����0��дʱ��Ӱ�졣
        def_uncompleted_pro  [15:0]  16'b0   h/s R/W  ��Ӧ��ǰ���װ��������ͬһPPP/IP���ŵ���ǰ������װ����������������֡��
                                                      Э�飬Ϊ��֧�ֶ��PPP/IP���Ŷ����ӵ����ã��������е�0Byte��1Byte��2Byte��Ч��
        */
        PPP_HDLC_WRITE_32REG(SOC_ARM_HDLC_DEF_UNCOMPLETED_PRO_ADDR(HDLC_IP_BASE_ADDR),
                        (VOS_UINT32)pstDefUncompletedInfo->usDefOutOnePro & 0xFFFF);

        /*
        def_uncompleted_addr  (0x7C)
         31                  0
        |----------------------|
        |         Addr         |
        def_uncompleted_addr [31:0]  32'b0   h/s R/W  ��Ӧ��ǰ���װ��������ͬһPPP/IP���ŵ���ǰ������װ����������������֡��
                                                      �ⲿ�洢��ʼ��ַ��Ϊ��֧�ֶ��PPP/IP���Ŷ����ӵ����ã��õ�ַ��������ԭ���ϱ���ͬ���µ�ַ��
        */
        PPP_HDLC_WRITE_32REG(SOC_ARM_HDLC_DEF_UNCOMPLETED_ADDR(HDLC_IP_BASE_ADDR),
                        (VOS_UINT32)pstDefUncompletedInfo->pucDefOutOneAddr);

        /*
        def_uncomplet_st_ago  (0x80)
         31                  16 15             5 4     0
        |----------------------|----------------|-------|
        |         Ago          |       Rsv      |  Ago  |
        crc16_result_ago     [31:16] 16'b0   h/s R/W  �뵱ǰ���װ��������ͬһPPP/IP���ŵ���ǰ������׽��װ��������������������֡ʱ��CRCУ��ֵ
        Reserved             [15:5]  11'b0   h/s R/W  ����λ����ʱ����0��дʱ��Ӱ�졣
        def_data_st_curr_ago [4:0]   5'b0    h/s R/W  �뵱ǰ���װ��������ͬһPPP/IP���ŵ���ǰ������׽��װ��������������������֡ʱ������״̬����ǰ״̬
        */
        PPP_HDLC_WRITE_32REG(SOC_ARM_HDLC_DEF_UNCOMPLETED_ST_AGO_ADDR(HDLC_IP_BASE_ADDR),
                        (VOS_UINT32)pstDefUncompletedInfo->ulDefStAgo);

        /*
        def_info_frl_cnt_ago  (0xC0)
         31        27 26                 16 15   11 10              0
        |------------|---------------------|-------|-----------------|
        |    Rsv     |         Ago         |  Rsv  |       Ago       |
        Reserved             [31:27] 5'b0    h/s R/W  ����λ����ʱ����0��дʱ��Ӱ�졣
        def_framel_cnt_ago   [26:16] 11'b0   h/s R/W  �뵱ǰ���װ��������ͬһPPP/IP���ŵ���ǰ������׽��װ��������������������֡ʱ��֡����
        Reserved             [15:11] 5'b0    h/s R/W  ����λ����ʱ����0��дʱ��Ӱ�졣
        def_info_cnt_ago     [10:0]  11'b0   h/s R/W  �뵱ǰ���װ��������ͬһPPP/IP���ŵ���ǰ������׽��װ��������������������֡ʱ����Ϣ����
        */
        PPP_HDLC_WRITE_32REG(SOC_ARM_HDLC_DEF_INFO_FRL_CNT_AGO_ADDR(HDLC_IP_BASE_ADDR),
                        (VOS_UINT32)pstDefUncompletedInfo->ulDefInfoFrlCntAgo);
    }

    usLowWord     = HDLC_MAKE_WORD(0x00, ucLowByte);
    ulDeframerCfg = HDLC_MAKE_DWORD(0x00, usLowWord);

    /* �����ý��д��Ĵ��� */
    PPP_HDLC_WRITE_32REG(SOC_ARM_HDLC_DEF_CFG_ADDR(HDLC_IP_BASE_ADDR),ulDeframerCfg);

    /* ��������������󵥰����� */
    PPP_HDLC_WRITE_32REG(SOC_ARM_HDLC_DEF_IN_PKT_LEN_MAX_ADDR(HDLC_IP_BASE_ADDR),
                    (VOS_UINT32)HDLC_DEF_IN_PER_MAX_CNT);

    return VOS_OK;
}


VOS_VOID PPP_HDLC_HARD_DefSaveUncompletedInfo
(
    PPP_ID          usPppId
)
{
    HDLC_DEF_UNCOMPLETED_INFO_STRU     *pstUncompletedInfo;
    VOS_UINT32                          ulStatus;                /* ���װ״̬ */
    HDLC_DEF_BUFF_INFO_STRU            *pstDefBuffInfo;
    VOS_UINT32                          ulValidNum;
    VOS_UINT8                          *pucDefOutOneAddr;
    HDLC_DEF_RPT_NODE_STRU             *pstRptNode;


    pstDefBuffInfo     = HDLC_DEF_GET_BUF_INFO(usPppId);
    pstUncompletedInfo = HDLC_DEF_GET_UNCOMPLETED_INFO(usPppId);

    ulStatus  =   PPP_HDLC_READ_32REG(SOC_ARM_HDLC_DEF_STATUS_ADDR(HDLC_IP_BASE_ADDR));

    /* ��ѯ״̬�Ĵ���hdlc_def_status (0x88)�ĵ�2λ
       Ϊ1��ʾ���ν��װ�з�����֡�����
       Ϊ0��ʾ�޷�����֡��� */
    if (0 == (ulStatus & 0x4))
    {
        pstUncompletedInfo->ucExistFlag = HDLC_DEF_UNCOMPLETED_NOT_EXIST;

        return;
    }

    g_PppHdlcHardStat.ulDefUncompleteCnt++;

    /* def_valid_num        [23:8]  16'b0   h/s RO   ��֡�ϱ�ʱ����Ч֡��Ŀ�������������һ�����ܵķ�����֡�� */
    ulValidNum = (ulStatus & 0xFFFF00) >> 8;

    /* ������֡���ϱ���Ϣ����Ч֡���棬���ǲ�������Ч֡��Ŀ�� */
    if (TTF_HDLC_DEF_RPT_MAX_NUM <= ulValidNum)
    {
        PPP_HDLC_ERROR_LOG2("PPP_HDLC_HARD_DefSaveUncompletedInfo, ERROR, ulValidNum %d >= TTF_HDLC_DEF_RPT_MAX_NUM %d!\r\n",
                     ulValidNum, TTF_HDLC_DEF_RPT_MAX_NUM);

        pstUncompletedInfo->ucExistFlag = HDLC_DEF_UNCOMPLETED_NOT_EXIST;

        return;
    }

    /* �з�����֡ʱ��Ҫ��ȡ������def_uncomplet_st_now(0x8C)��def_info_frl_cnt_now(0xC4)
       ��ȡ�������ϱ��ռ���Ч֮֡��ķ�����֡���ȡ�Э������ݵ�ַ */
    pstUncompletedInfo->ucExistFlag = HDLC_DEF_UNCOMPLETED_EXIST;

    pstRptNode = &(pstDefBuffInfo->astRptNodeBuf[ulValidNum]);

    pucDefOutOneAddr = (VOS_UINT8*)PPP_HDLC_NODE_PHY_TO_VIRT((VOS_UINT32)pstRptNode->pucDefOutOneAddr);

    if (pucDefOutOneAddr != HDLC_DEF_OUTPUT_BUF_START_ADDR)
    {
        if ((pucDefOutOneAddr - HDLC_DEF_OUTPUT_BUF_START_ADDR) >= pstRptNode->usDefOutOneLen)
        {
            mdrv_memcpy(HDLC_DEF_OUTPUT_BUF_START_ADDR, pucDefOutOneAddr, pstRptNode->usDefOutOneLen);
        }
        else
        {
            PSACORE_MEM_MOVE(HDLC_DEF_OUTPUT_BUF_START_ADDR, pstRptNode->usDefOutOneLen, pucDefOutOneAddr, pstRptNode->usDefOutOneLen);
        }
#ifndef PPP_ST_TEST
        pstRptNode->pucDefOutOneAddr = (VOS_UINT8*)PPP_HDLC_NODE_VIRT_TO_PHY((VOS_UINT32)HDLC_DEF_OUTPUT_BUF_START_ADDR);
#endif
    }


    /* ������֡��Э�顢���Ⱥ��ڴ洢�ռ�ĵ�ַ�����ֻ���ݴ���Щ��Ϣ�����½��װ��ʱ����ԭ�����HDLC */
    pstUncompletedInfo->usDefOutOnePro   = pstRptNode->usDefOutOnePro;
    pstUncompletedInfo->usDefOutOneLen   = pstRptNode->usDefOutOneLen;
    pstUncompletedInfo->pucDefOutOneAddr = pstRptNode->pucDefOutOneAddr;

    /* ���ֻ���ݴ���Щ��Ϣ�����½��װ��ʱ����ԭ�����HDLC */
    pstUncompletedInfo->ulDefStAgo         = PPP_HDLC_READ_32REG(SOC_ARM_HDLC_DEF_UNCOMPLETED_ST_NOW_ADDR(HDLC_IP_BASE_ADDR));
    pstUncompletedInfo->ulDefInfoFrlCntAgo = PPP_HDLC_READ_32REG(SOC_ARM_HDLC_DEF_INFO_FRL_CNT_NOW_ADDR(HDLC_IP_BASE_ADDR));

    PPP_HDLC_HARD_MntnDefTraceUncompleteInfo(pstUncompletedInfo);

    return;
}


VOS_VOID PPP_HDLC_HARD_DefWaitAndProc
(
    VOS_UINT32          ulMode,
    VOS_UINT32          ulEnableInterrupt,
    PPP_ID              usPppId,
    struct link        *pstLink
)
{
    VOS_UINT32                          ulDefStatus;
    VOS_UINT32                          ulContinue;

    for (; ;)
    {
        /* ʹ���жϣ���ȴ��жϸ������ͷ��ź�����������ѯ���װ״̬�Ĵ��� */
        ulDefStatus = PPP_HDLC_HARD_DefWaitResult(ulEnableInterrupt);

        switch ( ulDefStatus )
        {
            case HDLC_DEF_STATUS_PAUSE_RPT_SPACE_FULL :
            case HDLC_DEF_STATUS_PAUSE_OUTPUT_SPACE_FULL :
                /* ������Ч֡������GO_ON�Ĵ��� */
                PPP_HDLC_HARD_DefProcValidFrames(ulMode, usPppId, pstLink);
                PPP_HDLC_HARD_DefCfgGoOnReg(ulDefStatus);

                ulContinue = VOS_TRUE;
                g_PppHdlcHardStat.ulDefFullPauseCnt++;
                break;
            case HDLC_DEF_STATUS_PAUSE_LCP :
                /* ������Ч֡��LCP֡���������üĴ���������GO_ON�Ĵ��� */
                PPP_HDLC_HARD_DefProcValidFrames(ulMode, usPppId, pstLink);
                PPP_HDLC_HARD_DefCfgReg(ulMode, pstLink, VOS_NULL_PTR);
                PPP_HDLC_HARD_DefCfgGoOnReg(ulDefStatus);

                ulContinue = VOS_TRUE;
                g_PppHdlcHardStat.ulDefLcpPauseCnt++;
                break;
            case HDLC_DEF_STATUS_DONE_WITHOUT_FRAMES :
                /* ���ݷ�����ָ֡ʾ�����������֡��Ϣ */
                PPP_HDLC_HARD_DefSaveUncompletedInfo(usPppId);

                ulContinue = VOS_FALSE;
                break;
            case HDLC_DEF_STATUS_DONE_WITH_FRAMES :
                /* ������Ч֡������֡��LCP֡(���������һ֡)�����ݷ�����ָ֡ʾ�����������֡��Ϣ */
                PPP_HDLC_HARD_DefProcValidFrames(ulMode, usPppId, pstLink);
                PPP_HDLC_HARD_DefProcErrorFrames(pstLink);
                PPP_HDLC_HARD_DefSaveUncompletedInfo(usPppId);
                ulContinue = VOS_FALSE;
                break;
            case HDLC_DEF_STATUS_DOING :
            default:
                /* ��ӡ�쳣��־������PPP���� */
                PPP_HDLC_HARD_DefProcException(ulDefStatus, ulEnableInterrupt);

                ulContinue = VOS_FALSE;
                break;
        }

        /* ��ͣ״̬��Ҫ������������״̬���װ����˳� */
        if (VOS_TRUE != ulContinue)
        {
            break;
        }
    }

    g_stHdlcRegSaveInfo.ulHdlcDefRawInt = 0xFFFFFFFFU;
    g_stHdlcRegSaveInfo.ulHdlcDefStatus = 0xFFFFFFFFU;

    return;
}


PPP_HDLC_PARA_CHECK_RESULT_ENUM_UINT32 PPP_HDLC_HARD_DefCheckPara
(
    PPP_ZC_STRU                 *pstMem,
    VOS_UINT32                   ulDataLen,
    PPP_DATA_TYPE_ENUM_UINT8     ucDataType
)
{
    PPP_DATA_TYPE_ENUM_UINT8     ucCurrDataType;


    /* ��ȡ����װ���ݰ����� */
    ucCurrDataType = (PPP_ZC_GET_DATA_APP(pstMem) & 0x00FF);

    if ( ucDataType != ucCurrDataType )
    {
        return PPP_HDLC_PARA_CHECK_FAIL_KEEP;
    }

    /* ���װ������볤�����ڴ�ģ���������󳤶ȣ�Ŀǰ��1536B */
    /* ���HDLC Bug�ڴ��������1 */
    if ( (0 == ulDataLen) || (HDLC_DEF_IN_PER_MAX_CNT < ulDataLen) )
    {
        PPP_HDLC_NORMAL_LOG1("PPP_HDLC_HARD_DefCheckPara, NORMAL, invalid data length %d\r\n", ulDataLen);

        return PPP_HDLC_PARA_CHECK_FAIL_DISCARD;
    }

    return PPP_HDLC_PARA_CHECK_PASS;
}



VOS_VOID PPP_HDLC_HARD_ForbiddenHdlcBug(PPP_ZC_STRU **ppstMem)
{
    PPP_ZC_STRU *pstMem;
    PPP_ZC_STRU *pstTmpMem;
    VOS_UINT8   *pstData;
    VOS_UINT16   usLen;
    const VOS_UINT8    ucEndByte = 0x7e;

    pstMem  = *ppstMem;

    if (VOS_NULL_PTR == pstMem)
    {
        return;
    }

    usLen   = PPP_ZC_GET_DATA_LEN(pstMem);
    pstData = PPP_ZC_GET_DATA_PTR(pstMem);

    if (usLen <= 1)
    {
        return;
    }

    if (ucEndByte != pstData[usLen - 2])
    {
        /* �����ڶ����ֽڲ�Ϊ7e���ô��� */
        return;
    }
    else if ((usLen >= 3) && (ucEndByte == pstData[usLen - 3]))
    {
        /* �����������ֽ�Ϊ7e���ô��� */
        return;
    }
    else
    {
        /*
            �����ڶ����ֽ�Ϊ7e,�����������ֽڲ�Ϊ7e�ĳ���
            �����һ���ֽں�����һ���ֽ�,ͬʱ�ڴ�ĳ���+1

            skb������ڴ�ṹ����
           |--------data(uslen)--------|----32bytes align(0~31)-----|--skb_share_info(256)----|
        */
        if (PPP_ZC_GET_RESERVE_ROOM(pstMem) > 0)
        {
            INSERT_BYTE_LAST(pstMem, pstData, usLen, ucEndByte);
            g_PppHdlcHardStat.ulForbidHdlcBugNoCpy++;
        }
        else
        {
            /* ����һ������1�ֽڵ��ڴ� */

            pstTmpMem = PPP_MemAlloc(usLen + 1, 0);
            if (VOS_NULL_PTR != pstTmpMem)
            {
                PPP_MemWriteData(pstTmpMem, pstData, usLen);

                pstData = PPP_ZC_GET_DATA_PTR(pstTmpMem);

                INSERT_BYTE_LAST(pstTmpMem, pstData, usLen, ucEndByte);
                g_PppHdlcHardStat.ulForbidHdlcBugCpy++;
            }

            /* ������������ڴ��Ƿ�Ϊ�ն�����ȥ,�����洦�� */
            *ppstMem = pstTmpMem;

            /* ��ԭ�ڴ��ͷ� */
            PPP_MemFree(pstMem);
        }
    }
}


VOS_UINT32 PPP_HDLC_HARD_DefBuildInputParaLink
(
    HDLC_PARA_LINK_BUILD_PARA_STRU  *pstBuildPara,
    HDLC_PARA_LINK_BUILD_INFO_STRU  *pstBuildInfo
)
{
    VOS_UINT32                              ulLinkLoop;
    PPP_ZC_STRU                            *pstMem;
    PPP_DATA_TYPE_ENUM_UINT8                ucDataType;
    PPP_HDLC_PARA_CHECK_RESULT_ENUM_UINT32  enCheckResult;
    VOS_UINT16                              usDataLen;
    HDLC_PARA_LINK_NODE_STRU               *pstParaNode;
    VOS_UINT32                              ulNodeIndex;
    HDLC_DEF_BUFF_INFO_STRU                *pstDefBuffInfo;
    HDLC_DEF_UNCOMPLETED_INFO_STRU         *pstUncompletedInfo;
    VOS_UINT32                              ulMaxDataLen1Time = TTF_HDLC_DEF_INPUT_PARA_LINK_MAX_SIZE;

    /* ��������Ϣ */
    VOS_MemSet_s(pstBuildInfo, sizeof(HDLC_PARA_LINK_BUILD_INFO_STRU),
                 0, sizeof(HDLC_PARA_LINK_BUILD_INFO_STRU));

    pstUncompletedInfo = HDLC_DEF_GET_UNCOMPLETED_INFO(pstBuildPara->usPppId);

    if (HDLC_DEF_UNCOMPLETED_EXIST == pstUncompletedInfo->ucExistFlag)
    {
        ulMaxDataLen1Time = TTF_HDLC_DEF_INPUT_PARA_LINK_MAX_SIZE - pstUncompletedInfo->usDefOutOneLen;
    }


    /* ����usPppId�ҵ���Ӧ���ڴ�  */
    pstDefBuffInfo = HDLC_DEF_GET_BUF_INFO(pstBuildPara->usPppId);
    ucDataType     = pstBuildPara->ucDataType;

    for ( ulLinkLoop = 0 ; ulLinkLoop < TTF_HDLC_DEF_INPUT_PARA_LINK_MAX_NUM; ulLinkLoop++ )
    {
        pstMem     = (PPP_ZC_STRU *)PPP_ZC_PEEK_QUEUE_HEAD(pstBuildPara->pstDataQ);

        /* ��û�е����ڵ��������Ƕ������Ѿ�û������ */
        if( VOS_NULL_PTR == pstMem )
        {
            break;
        }

        usDataLen     = PPP_ZC_GET_DATA_LEN(pstMem);
        enCheckResult = PPP_HDLC_HARD_DefCheckPara(pstMem, usDataLen, ucDataType);

        if( PPP_HDLC_PARA_CHECK_FAIL_DISCARD == enCheckResult )
        {
            pstMem = PPP_ZC_DEQUEUE_HEAD(pstBuildPara->pstDataQ);
            PPP_MemFree(pstMem);
            pstBuildInfo->ulDealCnt++;

            g_PppHdlcHardStat.ulDefInputDiscardCnt++;
            continue;
        }

        if( PPP_HDLC_PARA_CHECK_FAIL_KEEP == enCheckResult )
        {
            break;
        }

        /* ��PPP����ȡ��ͷ��㣬������������� pstBuildInfo->apstInputLinkNode */
        pstMem  = PPP_ZC_DEQUEUE_HEAD(pstBuildPara->pstDataQ);

        /* ���HDLC BUG,�ڷ���HDLC BUG�����¶�ԭ�����ݽ����滻,
           �����pstMemΪ�滻����ڴ�ָ��,ԭʼ�ڴ�ָ������Ѿ����ͷ�
         */
        PPP_HDLC_HARD_ForbiddenHdlcBug(&pstMem);

        /* �滻�ڴ��ʱ�������ڴ�ʧ��,ֱ�Ӵ�����һ������  */
        if (VOS_NULL_PTR == pstMem)
        {
            continue;
        }

        /* �ڴ���ܷ����滻,����ȡ���� */
        usDataLen = PPP_ZC_GET_DATA_LEN(pstMem);

        /* һ������������д���װ���ݰ��ܳ����ܳ���15KB */
        /* ���HDLC BUG,�������ݱ��޸Ĺ�,����������1�ֽ�,��Ҫ�ŵ������ж� */
        if( pstBuildInfo->ulInputLinkTotalSize + usDataLen  > ulMaxDataLen1Time )
        {
            /* �ڴ�Żض���,�´��ٴ��� */
            PPP_ZC_ENQUEUE_HEAD(pstBuildPara->pstDataQ, pstMem);

            break;
        }

        /* ��ȡ��ǰҪ��ӽڵ���±� */
        ulNodeIndex = pstBuildInfo->ulInputLinkNodeCnt;

        /* ����Ҫ����Ĳ����ڵ� */
        pstParaNode = &(pstDefBuffInfo->astInputParaLinkNodeBuf[ulNodeIndex]);

        /* �ѽڵ�������� */
        if( 0 != ulNodeIndex )
        {
            pstDefBuffInfo->astInputParaLinkNodeBuf[ulNodeIndex - 1].pstNextNode = (HDLC_PARA_LINK_NODE_STRU *)PPP_HDLC_NODE_VIRT_TO_PHY((VOS_UINT32)pstParaNode);
        }

        /* ��д������������������ */
        pstParaNode->pucDataAddr = (VOS_UINT8 *)PPP_VIRT_TO_PHY((VOS_UINT_PTR)PPP_ZC_GET_DATA_PTR(pstMem));
        pstParaNode->usDataLen   = usDataLen;
        pstParaNode->pstNextNode = VOS_NULL_PTR;

        pstBuildInfo->apstInputLinkNode[ulNodeIndex] = pstMem;
        pstBuildInfo->ulInputLinkTotalSize          += (VOS_UINT32)usDataLen;
        pstBuildInfo->ulInputLinkNodeCnt++;
        pstBuildInfo->ulDealCnt++;

        /* ��Ҫ������д��DDR��HDLC��DDR�ж����� */
        PPP_HDLC_CACHE_FLUSH(PPP_ZC_GET_DATA_PTR(pstMem), PPP_ZC_GET_DATA_LEN(pstMem));
    }

    if (0 == pstBuildInfo->ulInputLinkNodeCnt)
    {
        return VOS_ERR;
    }

    /* �ϱ���������������ݿ�ά�ɲ� */
    PPP_HDLC_HARD_MntnDefTraceInput(pstDefBuffInfo, pstBuildInfo);

    return VOS_OK;
}


VOS_VOID PPP_HDLC_HARD_DefCfgBufReg(HDLC_DEF_BUFF_INFO_STRU *pstDefBuffInfo)
{
    /* ����װ��������������ʼ��ַ���ø��Ĵ���def_in_lli_addr(0x90) */
    PPP_HDLC_WRITE_32REG(SOC_ARM_HDLC_DEF_IN_LLI_ADDR(HDLC_IP_BASE_ADDR),
                    PPP_HDLC_NODE_VIRT_TO_PHY((VOS_UINT32)&(pstDefBuffInfo->astInputParaLinkNodeBuf[0])));

    /* ����װ��������������ʼ��ַ���ø��Ĵ���def_out_spc_addr(0xA0) */
    PPP_HDLC_WRITE_32REG(SOC_ARM_HDLC_DEF_OUT_SPC_ADDR(HDLC_IP_BASE_ADDR),
                    PPP_HDLC_NODE_VIRT_TO_PHY((VOS_UINT32)&(pstDefBuffInfo->aucOutputDataBuf[0])));

    /* ����װ��������������ʼ��ַ���ø��Ĵ���def_out_space_dep(0xA4)��16λ */
    PPP_HDLC_WRITE_32REG(SOC_ARM_HDLC_DEF_OUT_SPACE_DEP_ADDR(HDLC_IP_BASE_ADDR),
                    (VOS_UINT32)TTF_HDLC_DEF_OUTPUT_DATA_BUF_LEN & 0xFFFF);

    /* ����װ��Ч֡�����Ϣ�ϱ��ռ���ʼ��ַ���ø��Ĵ���def_rpt_addr(0xA8) */
    PPP_HDLC_WRITE_32REG(SOC_ARM_HDLC_DEF_RPT_ADDR(HDLC_IP_BASE_ADDR),
                    PPP_HDLC_NODE_VIRT_TO_PHY((VOS_UINT32)&(pstDefBuffInfo->astRptNodeBuf[0])));

    /* ����װ��Ч֡�����Ϣ�ϱ��ռ�������ø��Ĵ���def_rpt_dep (0xAC)��16λ */
    PPP_HDLC_WRITE_32REG(SOC_ARM_HDLC_DEF_RPT_DEP_ADDR(HDLC_IP_BASE_ADDR),
                    (VOS_UINT32)TTF_HDLC_DEF_RPT_BUF_LEN & 0xFFFF);

    return;
}


VOS_UINT32 PPP_HDLC_HARD_DefPacket
(
    PPP_ID              usPppId,
    struct link        *pstLink,
    PPP_ZC_QUEUE_STRU  *pstDataQ,
    VOS_UINT32         *pulDealCnt
)
{
    VOS_UINT32                          ulEnableInterrupt;
    HDLC_PARA_LINK_BUILD_INFO_STRU      stBuildInfo;
    HDLC_PARA_LINK_BUILD_PARA_STRU      stBuildPara;
    VOS_UINT32                          ulBuildResult;
    HDLC_DEF_BUFF_INFO_STRU            *pstDefBuffInfo;

    pstDefBuffInfo = HDLC_DEF_GET_BUF_INFO(usPppId);

    stBuildPara.ucDataType = PPP_PULL_PACKET_TYPE;
    stBuildPara.usProtocol = PROTO_IP;
    stBuildPara.pstDataQ   = pstDataQ;
    stBuildPara.usPppId    = usPppId;

    /* ������������������ϱ������ڴ��ά�ɲ� */
    ulBuildResult = PPP_HDLC_HARD_DefBuildInputParaLink(&stBuildPara, &stBuildInfo);

    *pulDealCnt = stBuildInfo.ulDealCnt;

    if (VOS_OK != ulBuildResult)
    {
        PPP_HDLC_WARNING_LOG("PPP_HDLC_HARD_DefPacket, WARNING, BuildParaLink Fail.\r\n");
        return ulBuildResult;
    }

    g_PppHdlcHardStat.ulDefMaxInputCntOnce  = TTF_MAX(g_PppHdlcHardStat.ulDefMaxInputCntOnce, stBuildInfo.ulInputLinkNodeCnt);
    g_PppHdlcHardStat.ulDefMaxInputSizeOnce = TTF_MAX(g_PppHdlcHardStat.ulDefMaxInputSizeOnce, stBuildInfo.ulInputLinkTotalSize);

    /* ���÷�װ�����װͨ�üĴ��� */
    PPP_HDLC_HARD_CommCfgReg();

    /* �����ڴ���ؼĴ��� */
    PPP_HDLC_HARD_DefCfgBufReg(pstDefBuffInfo);

    /* ����ѹ��ָʾ��������֡�����Ϣ�Ĵ��� */
    PPP_HDLC_HARD_DefCfgReg(HDLC_IP_MODE, pstLink, HDLC_DEF_GET_UNCOMPLETED_INFO(usPppId));

    /* ����ʹ�ܼĴ��������ϱ�ʹ��ǰ�Ĵ�����ά�ɲ� */
    ulEnableInterrupt = PPP_HDLC_HARD_DefCfgEnReg(stBuildInfo.ulInputLinkTotalSize);

    /* �ȴ����װ��ͣ����ɣ�Ȼ����������ݣ����ܻ��ж��ͣ�ȵĹ��� */
    PPP_HDLC_HARD_DefWaitAndProc(HDLC_IP_MODE, ulEnableInterrupt, usPppId, pstLink);

    /* �ͷ��ѽ��װ��ɵ����� */
    PPP_HDLC_HARD_CommReleaseLink(stBuildInfo.apstInputLinkNode, stBuildInfo.ulInputLinkNodeCnt);

    return VOS_OK;
}


VOS_UINT32 PPP_HDLC_HARD_DefRawData
(
    PPP_ID              usPppId,
    struct link        *pstLink,
    PPP_ZC_QUEUE_STRU  *pstDataQ,
    VOS_UINT32         *pulDealCnt
)
{
    VOS_UINT32                          ulEnableInterrupt;
    HDLC_PARA_LINK_BUILD_INFO_STRU      stBuildInfo;
    HDLC_PARA_LINK_BUILD_PARA_STRU      stBuildPara;
    VOS_UINT32                          ulBuildResult;
    HDLC_DEF_BUFF_INFO_STRU            *pstDefBuffInfo;

    pstDefBuffInfo = HDLC_DEF_GET_BUF_INFO(usPppId);

    VOS_MemSet_s(&stBuildInfo, sizeof(HDLC_PARA_LINK_BUILD_INFO_STRU),
                 0, sizeof(HDLC_PARA_LINK_BUILD_INFO_STRU));

    stBuildPara.ucDataType = PPP_PULL_RAW_DATA_TYPE;
    stBuildPara.usProtocol = PROTO_IP;
    stBuildPara.pstDataQ   = pstDataQ;
    stBuildPara.usPppId    = usPppId;

    /* ������������������ϱ������ڴ��ά�ɲ� */
    ulBuildResult = PPP_HDLC_HARD_DefBuildInputParaLink(&stBuildPara, &stBuildInfo);

    *pulDealCnt = stBuildInfo.ulDealCnt;

    if (VOS_OK != ulBuildResult)
    {
        return ulBuildResult;
    }

    g_PppHdlcHardStat.ulDefMaxInputCntOnce  = TTF_MAX(g_PppHdlcHardStat.ulDefMaxInputCntOnce, stBuildInfo.ulInputLinkNodeCnt);
    g_PppHdlcHardStat.ulDefMaxInputSizeOnce = TTF_MAX(g_PppHdlcHardStat.ulDefMaxInputSizeOnce, stBuildInfo.ulInputLinkTotalSize);

    /* ���÷�װ�����װͨ�üĴ��� */
    PPP_HDLC_HARD_CommCfgReg();

    /* �����ڴ���ؼĴ��� */
    PPP_HDLC_HARD_DefCfgBufReg(pstDefBuffInfo);

    /* ����ѹ��ָʾ��������֡�����Ϣ�Ĵ��� */
    PPP_HDLC_HARD_DefCfgReg(HDLC_PPP_MODE, pstLink, HDLC_DEF_GET_UNCOMPLETED_INFO(usPppId));

    /* ����ʹ�ܼĴ��������ϱ�ʹ��ǰ�Ĵ�����ά�ɲ� */
    ulEnableInterrupt = PPP_HDLC_HARD_DefCfgEnReg(stBuildInfo.ulInputLinkTotalSize);

    /* �ȴ����װ��ͣ����ɣ�Ȼ����������ݣ����ܻ��ж��ͣ�ȵĹ��� */
    PPP_HDLC_HARD_DefWaitAndProc(HDLC_PPP_MODE, ulEnableInterrupt, usPppId, pstLink);

    /* �ͷ��ѽ��װ��ɵ����� */
    PPP_HDLC_HARD_CommReleaseLink(stBuildInfo.apstInputLinkNode, stBuildInfo.ulInputLinkNodeCnt);

    return VOS_OK;
}


VOS_UINT32 PPP_HDLC_HARD_FrmUpdateLink
(
    VOS_UINT32                      ulAllocMemCnt,
    VOS_UINT32 *                    aulAllocLen,
    PPP_ZC_STRU                   **ppstAllocedMem,
    HDLC_PARA_LINK_BUILD_PARA_STRU *pstBuildPara,
    HDLC_PARA_LINK_BUILD_INFO_STRU *pstBuildInfo
)
{
    VOS_UINT32                          ulMemLoop;
    VOS_UINT32                          ulNodeIndex;
    HDLC_PARA_LINK_NODE_STRU           *pstOutputParaNode;
    HDLC_FRM_BUFF_INFO_STRU            *pstFrmBuffInfo;


    /* ����usPppId�ҵ���Ӧ���ڴ�  */
    pstFrmBuffInfo          = HDLC_FRM_GET_BUF_INFO(pstBuildPara->usPppId);

    if( (pstBuildInfo->ulOutputLinkNodeCnt + ulAllocMemCnt) > TTF_HDLC_FRM_OUTPUT_PARA_LINK_MAX_NUM )
    {
        g_PppHdlcHardStat.ulFrmOutputLinkFullCnt++;

        PPP_HDLC_HARD_CommReleaseLink(ppstAllocedMem, ulAllocMemCnt);

        return VOS_ERR;
    }

    /* ����Ŀ�Ŀռ����� apstOutputLinkNode�������ͷ�Ŀ�Ŀռ� */
    VOS_MemCpy_s(&(pstBuildInfo->apstOutputLinkNode[pstBuildInfo->ulOutputLinkNodeCnt]),
               ulAllocMemCnt * sizeof(PPP_ZC_STRU *),
               &(ppstAllocedMem[0]),
               ulAllocMemCnt * sizeof(PPP_ZC_STRU *));

    /* ����װ�������������ĸ�����Ա��ֵ */
    for ( ulMemLoop = 0; ulMemLoop < ulAllocMemCnt; ulMemLoop++ )
    {
        ulNodeIndex = pstBuildInfo->ulOutputLinkNodeCnt;

        pstOutputParaNode = &(pstFrmBuffInfo->astOutputParaLinkNodeBuf[ulNodeIndex]);

        /* �����ϸ���������������pstNextNode�� */
        if (0 != ulNodeIndex)
        {
            pstFrmBuffInfo->astOutputParaLinkNodeBuf[ulNodeIndex - 1].pstNextNode = (HDLC_PARA_LINK_NODE_STRU*)PPP_HDLC_NODE_VIRT_TO_PHY((VOS_UINT32)pstOutputParaNode);
        }

        pstOutputParaNode->pucDataAddr = (VOS_UINT8 *)PPP_VIRT_TO_PHY((VOS_UINT_PTR)PPP_ZC_GET_DATA_PTR(ppstAllocedMem[ulMemLoop]));
        pstOutputParaNode->usDataLen   = (VOS_UINT16)aulAllocLen[ulMemLoop];
        pstOutputParaNode->pstNextNode = VOS_NULL_PTR;

        pstBuildInfo->ulOutputLinkNodeCnt++;
        pstBuildInfo->ulOutputLinkTotalSize += aulAllocLen[ulMemLoop];

        PPP_HDLC_CACHE_INVALID((VOS_UINT8 *)PPP_PHY_TO_VIRT((VOS_UINT32)(pstOutputParaNode->pucDataAddr)), pstOutputParaNode->usDataLen);
    }

    return VOS_OK;
}


VOS_UINT32 PPP_HDLC_HARD_FrmOutputMemAlloc
(
    VOS_UINT16                           usLen,
    HDLC_PARA_LINK_BUILD_PARA_STRU      *pstBuildPara,
    HDLC_PARA_LINK_BUILD_INFO_STRU      *pstBuildInfo
)
{
    VOS_UINT16                  usFrmedMaxLen;                                  /* ��װ��Ŀ��ܵ���󳤶� */
    VOS_UINT16                  usAllocLen;
    VOS_UINT16                  usAllocLoop;
    VOS_UINT16                  usLoopCnt;
    VOS_UINT16                  usAllocMemCnt;                                  /* ����������ڴ���� */
    VOS_UINT32                  aulAllocLen[HDLC_OUTPUT_PARA_LINK_MAX_SIZE];    /* ��¼��������ĸ����ڴ�鳤�� */
    PPP_ZC_STRU *               apstAllocedMem[HDLC_OUTPUT_PARA_LINK_MAX_SIZE]; /* ��¼��������ĸ����ڴ��ָ�� */
    PPP_ZC_STRU                *pstMem;

    /* ��װ����ܵ�������ݳ���(2*ԭʼ���ݳ���+13B) */
    usFrmedMaxLen = (VOS_UINT16)HDLC_FRM_GET_MAX_FRAMED_LEN(usLen);
    usAllocMemCnt = 0;
    usLoopCnt     = TTF_CEIL(usFrmedMaxLen, PPP_ZC_MAX_DATA_LEN);

    for ( usAllocLoop = 0 ; usAllocLoop < usLoopCnt; usAllocLoop++ )
    {
        usAllocLen  = PPP_ZC_MAX_DATA_LEN;
        pstMem      = PPP_MemAlloc(usAllocLen, PPP_ZC_DL_RESERVE_LEN);

        if (VOS_NULL_PTR == pstMem)
        {
            break;
        }

        usAllocMemCnt++;
        aulAllocLen[usAllocLoop]    = usAllocLen;
        apstAllocedMem[usAllocLoop] = pstMem;
    }

    /* ����������ڴ�ʧ�ܵ���������ͷű����Ѿ�������ڴ� */
    if (usLoopCnt > usAllocMemCnt)
    {
        PPP_HDLC_HARD_CommReleaseLink(&(apstAllocedMem[0]), usAllocMemCnt);
        return VOS_ERR;
    }

    return PPP_HDLC_HARD_FrmUpdateLink(usAllocMemCnt,
                                       &(aulAllocLen[0]), &(apstAllocedMem[0]),
                                       pstBuildPara, pstBuildInfo);
}



VOS_UINT32 PPP_HDLC_HARD_FrmGetProtocol
(
    PPP_ZC_STRU     *pstMem,
    VOS_UINT16      *pusProtocol
)
{
    VOS_UINT8           aucProto[2] = {0, 0};
    VOS_UINT32          ulRtn;


    ulRtn = PPP_MemGet(pstMem, 0, aucProto, 2);

    if ( PS_FAIL == ulRtn )
    {
        return VOS_ERR;
    }

    if ( 0x01 == GET_BITS_FROM_BYTE(aucProto[0], 0x01) )
    {
        *pusProtocol = HDLC_MAKE_WORD(0x00, aucProto[0]);
    }
    else
    {
        if ( 0x01 == GET_BITS_FROM_BYTE(aucProto[1], 0x01) )
        {
            *pusProtocol = HDLC_MAKE_WORD(aucProto[0], aucProto[1]);
        }
        else
        {
            PPP_HDLC_WARNING_LOG("PPP_HDLC_HARD_FrmGetProtocol, Warning, PPP mode framer data protocol error\r\n");
            return VOS_ERR;
        }
    }

    return VOS_OK;
}


PPP_HDLC_PARA_CHECK_RESULT_ENUM_UINT32 PPP_HDLC_HARD_FrmCheckPara
(
    PPP_ZC_STRU                 *pstMem,
    VOS_UINT32                   ulDataLen,
    PPP_DATA_TYPE_ENUM_UINT8     ucDataType,
    VOS_UINT16                   usProtocol
)
{
    PPP_DATA_TYPE_ENUM_UINT8     ucCurrDataType;
    VOS_UINT32                   ulRlst;
    VOS_UINT16                   usCurrProtocol;


    /* ��ȡ����װ���ݰ����� */
    ucCurrDataType = (PPP_ZC_GET_DATA_APP(pstMem) & 0x00FF);

    if ( ucDataType != ucCurrDataType )
    {
        return PPP_HDLC_PARA_CHECK_FAIL_KEEP;
    }

    /* ����װ���ݰ������쳣 */
    if ( (0 == ulDataLen) || (HDLC_FRM_IN_PER_MAX_CNT < ulDataLen) )
    {
        PPP_HDLC_NORMAL_LOG1("PPP_HDLC_HARD_FrmCheckPara,IP mode framer data len wrong %d\r\n", ulDataLen);

        return PPP_HDLC_PARA_CHECK_FAIL_DISCARD;
    }

    /* �������PPPģʽ */
    if( PPP_PUSH_RAW_DATA_TYPE != ucCurrDataType )
    {
        return PPP_HDLC_PARA_CHECK_PASS;
    }

    ulRlst  = PPP_HDLC_HARD_FrmGetProtocol(pstMem, &usCurrProtocol);

    if( VOS_OK != ulRlst )
    {
        PPP_HDLC_WARNING_LOG("PPP_HDLC_HARD_FrmCheckPara, Warning, PPP mode get framer data protocol error\r\n");
        return PPP_HDLC_PARA_CHECK_FAIL_DISCARD;
    }

    /* ��֤һ���������ù����з�װЭ��ֵһ�� */
    if( usProtocol != usCurrProtocol )
    {
        return PPP_HDLC_PARA_CHECK_FAIL_KEEP;
    }

    return PPP_HDLC_PARA_CHECK_PASS;
}


VOS_VOID PPP_HDLC_HARD_FrmStartTimer
(
    HDLC_PARA_LINK_BUILD_PARA_STRU      *pstBuildPara
)
{
    if( VOS_NULL_PTR != g_stHdlcConfigInfo.stHdlcFrmTimerHandle )
    {
        return;
    }

    if (VOS_OK != VOS_StartRelTimer(&g_stHdlcConfigInfo.stHdlcFrmTimerHandle, PS_PID_APP_PPP,
                     HDLC_FRM_TIME_INTERVAL,
                     TIMER_HDLC_FRM_OUTPUT_SPACE_ALLOC_FAIL,
                     (VOS_UINT32)pstBuildPara->usPppId,
                     VOS_RELTIMER_NOLOOP, VOS_TIMER_PRECISION_0))
    {
        PPP_HDLC_WARNING_LOG("PPP_HDLC_HARD_FrmStartTimer, Warning, start reltimer error\r\n");
    }

    return;
}


VOS_UINT32 PPP_HDLC_HARD_FrmBuildParaLink
(
 HDLC_PARA_LINK_BUILD_PARA_STRU      *pstBuildPara,
 HDLC_PARA_LINK_BUILD_INFO_STRU      *pstBuildInfo
 )
{
    PPP_ZC_STRU                             *pstMem;
    VOS_UINT32                               ulLoop;
    VOS_UINT16                               usUnFrmLen;
    VOS_UINT32                               ulAllocResult;
    HDLC_PARA_LINK_NODE_STRU                *pstParaNode;
    VOS_UINT32                               ulNodeIndex;
    PPP_HDLC_PARA_CHECK_RESULT_ENUM_UINT32   enCheckResult;
    VOS_UINT32                               ulAllocMemFail;
    HDLC_FRM_BUFF_INFO_STRU                 *pstFrmBuffInfo;


    VOS_MemSet_s(pstBuildInfo, sizeof(HDLC_PARA_LINK_BUILD_INFO_STRU),
                 0, sizeof(HDLC_PARA_LINK_BUILD_INFO_STRU));

    /* ����usPppId�ҵ���Ӧ���ڴ�  */
    pstFrmBuffInfo = HDLC_FRM_GET_BUF_INFO(pstBuildPara->usPppId);
    ulAllocMemFail = VOS_FALSE;

    for(ulLoop = 0; ulLoop < TTF_HDLC_FRM_INPUT_PARA_LINK_MAX_NUM; ulLoop++)
    {
        pstMem = (PPP_ZC_STRU *)PPP_ZC_PEEK_QUEUE_HEAD(pstBuildPara->pstDataQ);

        if( VOS_NULL_PTR == pstMem )
        {
            break;
        }

        usUnFrmLen    = PPP_ZC_GET_DATA_LEN(pstMem);
        enCheckResult = PPP_HDLC_HARD_FrmCheckPara(pstMem, usUnFrmLen,
                                                   pstBuildPara->ucDataType,
                                                   pstBuildPara->usProtocol);

        if( PPP_HDLC_PARA_CHECK_FAIL_DISCARD == enCheckResult )
        {
            pstMem = PPP_ZC_DEQUEUE_HEAD(pstBuildPara->pstDataQ);
            PPP_MemFree(pstMem);
            pstBuildInfo->ulDealCnt++;

            g_PppHdlcHardStat.ulFrmInputDiscardCnt++;
            continue;
        }

        if( PPP_HDLC_PARA_CHECK_FAIL_KEEP == enCheckResult )
        {
            break;
        }

        /* һ������������д���װ���ݰ��ܳ����ܳ���15KB */
        if( pstBuildInfo->ulInputLinkTotalSize + usUnFrmLen > TTF_HDLC_FRM_INPUT_PARA_LINK_MAX_SIZE )
        {
            break;
        }

        /* �����װĿ�Ŀռ� */
        ulAllocResult = PPP_HDLC_HARD_FrmOutputMemAlloc(usUnFrmLen, pstBuildPara, pstBuildInfo);

        if( VOS_OK != ulAllocResult )
        {
            ulAllocMemFail = VOS_TRUE;

            g_PppHdlcHardStat.ulFrmAllocOutputMemFailCnt++;
            break;
        }

        /* ��PPP����ȡ��ͷ��㣬������������� pstBuildInfo->apstInputLinkNode */
        pstMem = PPP_ZC_DEQUEUE_HEAD(pstBuildPara->pstDataQ);

        /* ��ȡ��ǰҪ��ӽڵ���±� */
        ulNodeIndex = pstBuildInfo->ulInputLinkNodeCnt;

        /* ����Ҫ����Ĳ����ڵ� */
        pstParaNode = &(pstFrmBuffInfo->astInputParaLinkNodeBuf[ulNodeIndex]);

        /* ��д������������������ */
        if( 0 != ulNodeIndex )
        {
            pstFrmBuffInfo->astInputParaLinkNodeBuf[ulNodeIndex - 1].pstNextNode = (HDLC_PARA_LINK_NODE_STRU*)PPP_HDLC_NODE_VIRT_TO_PHY((VOS_UINT32)pstParaNode);
        }

        pstParaNode->pucDataAddr = (VOS_UINT8*)PPP_VIRT_TO_PHY((VOS_UINT_PTR)PPP_ZC_GET_DATA_PTR(pstMem));
        pstParaNode->usDataLen   = usUnFrmLen;
        pstParaNode->pstNextNode = VOS_NULL_PTR;

        pstBuildInfo->apstInputLinkNode[ulNodeIndex] = pstMem;
        pstBuildInfo->ulInputLinkTotalSize          += (VOS_UINT32)usUnFrmLen;
        pstBuildInfo->ulInputLinkNodeCnt++;
        pstBuildInfo->ulDealCnt++;

        /* ��Ҫ������д��DDR��HDLC��DDR�ж����� */
        PPP_HDLC_CACHE_FLUSH(PPP_ZC_GET_DATA_PTR(pstMem), PPP_ZC_GET_DATA_LEN(pstMem));
    }

    if (0 == pstBuildInfo->ulInputLinkNodeCnt)
    {
        if (VOS_TRUE == ulAllocMemFail)
        {
            /* �ȴ�һ��ʱ������³��������ڴ��ٷ�װ */
            PPP_HDLC_HARD_FrmStartTimer(pstBuildPara);

            g_PppHdlcHardStat.ulFrmAllocFirstMemFailCnt++;
        }

        return VOS_ERR;
    }

    /* ��������������������ݿ�ά�ɲ� */
    PPP_HDLC_HARD_MntnFrmTraceInput(pstFrmBuffInfo, pstBuildInfo);

    return VOS_OK;
}


VOS_VOID PPP_HDLC_HARD_FrmCfgBufReg(HDLC_FRM_BUFF_INFO_STRU *pstFrmBuffInfo)
{
    /* ����װ��������������ʼ��ַ���ø��Ĵ���frm_in_lli_addr */
    PPP_HDLC_WRITE_32REG(SOC_ARM_HDLC_FRM_IN_LLI_ADDR(HDLC_IP_BASE_ADDR),
                    PPP_HDLC_NODE_VIRT_TO_PHY((VOS_UINT32)&(pstFrmBuffInfo->astInputParaLinkNodeBuf[0])));

    /* ����װ��������������ʼ��ַ���ø��Ĵ���frm_out_lli_addr */
    PPP_HDLC_WRITE_32REG(SOC_ARM_HDLC_FRM_OUT_LLI_ADDR(HDLC_IP_BASE_ADDR),
                    PPP_HDLC_NODE_VIRT_TO_PHY((VOS_UINT32)&(pstFrmBuffInfo->astOutputParaLinkNodeBuf[0])));

    /* ����װ��Ч֡�����Ϣ�ϱ��ռ���ʼ��ַ���ø��Ĵ���frm_rpt_addr */
    PPP_HDLC_WRITE_32REG(SOC_ARM_HDLC_FRM_RPT_ADDR(HDLC_IP_BASE_ADDR),
                    PPP_HDLC_NODE_VIRT_TO_PHY((VOS_UINT32)&(pstFrmBuffInfo->astRptNodeBuf[0])));

    /* ����װ��Ч֡�����Ϣ�ϱ��ռ�������ø��Ĵ���frm_rpt_dep��[15:0]λ */
    PPP_HDLC_WRITE_32REG(SOC_ARM_HDLC_FRM_RPT_DEP_ADDR(HDLC_IP_BASE_ADDR),
                    (VOS_UINT32)TTF_HDLC_FRM_RPT_BUF_LEN & 0xFFFF);

    return;
}


VOS_VOID PPP_HDLC_HARD_FrmCfgReg
(
    struct link        *pstLink,
    VOS_UINT16          usProtocol
)
{
    /*
    hdlc_frm_cfg   (0x20)
      31              16 15           4 3      2   1      0
    |-------------------|--------------|--------|------|------|
    |      protocol     |      Rsv     |   Pfc  | Acfc |1dor2d|

    frm_protocol         [31:16] 16'b0   h/s R/W  ��װЭ��ֵ����ЧЭ��ֵ�涨�μ�����б�
    Reserved             [15:4]  12'b0   h/s R/W  ����λ����ʱ����0��дʱ��Ӱ�졣
    frm_pfc              [3:2]   2'b0    h/s R/W  P��ѹ��ָʾ��00��Ӳ��ģ�����P��P����ѹ��;
                                                               01��Ӳ��ģ�����P��P��ѹ��;
                                                               11��Ӳ��ģ�鲻���P��;
                                                               ��������Ч;
    frm_acfc             [1]     1'b0    h/s R/W  AC��ѹ��ָʾ��0��AC����ѹ��;1����ʾAC��ѹ��;
    frm_in_lli_1dor2d    [0]     1'b0    h/s R/W  ��װ����һά���ά����ѡ��ָʾ�Ĵ�����
                                                                0Ϊһά;1Ϊ��ά;

    IPģʽһ�����P��,PPPģʽһ�������P��
      LCP֡: AC��ѹ����P��ѹ��
    */

    VOS_UINT32              ulFrmCfg;


    /* ���Ĵ���hdlc_frm_cfg��[0]λfrm_in_lli_1dor2d����Ϊ0 */
    ulFrmCfg = 0x0;

    /* ����hdlc_frm_cfg�� P��� AC�� */
    if (PROTO_LCP != usProtocol)
    {
        if ( 1 == pstLink->lcp.his_acfcomp )
        {
            ulFrmCfg |= (1 << HDLC_FRM_ACFC_BITPOS);
        }

        if ( 1 == pstLink->lcp.his_protocomp )
        {
            ulFrmCfg |= (1 << HDLC_FRM_PFC_BITPOS);
        }

        /* ����hdlc_frm_accm */
        PPP_HDLC_WRITE_32REG(SOC_ARM_HDLC_FRM_ACCM_ADDR(HDLC_IP_BASE_ADDR),
                            pstLink->lcp.his_accmap);
    }
    else
    {
        PPP_HDLC_WRITE_32REG(SOC_ARM_HDLC_FRM_ACCM_ADDR(HDLC_IP_BASE_ADDR), 0xFFFFFFFFU);
    }

    /* ���üĴ���hdlc_frm_cfg��[31:16]λfrm_protocolΪusProtocol */
    ulFrmCfg |= ( ((VOS_UINT32)usProtocol) << 16 );

    PPP_HDLC_WRITE_32REG(SOC_ARM_HDLC_FRM_CFG_ADDR(HDLC_IP_BASE_ADDR), ulFrmCfg);

    return;
}


VOS_VOID PPP_HDLC_HARD_FrmRawCfgReg
(
    struct link        *pstLink,
    VOS_UINT16          usProtocol
)
{
    /*
    hdlc_frm_cfg   (0x20)
      31              16 15           4 3      2   1      0
    |-------------------|--------------|--------|------|------|
    |      protocol     |      Rsv     |   Pfc  | Acfc |1dor2d|

    frm_protocol         [31:16] 16'b0   h/s R/W  ��װЭ��ֵ����ЧЭ��ֵ�涨�μ�����б�
    Reserved             [15:4]  12'b0   h/s R/W  ����λ����ʱ����0��дʱ��Ӱ�졣
    frm_pfc              [3:2]   2'b0    h/s R/W  P��ѹ��ָʾ��00��Ӳ��ģ�����P��P����ѹ��;
                                                               01��Ӳ��ģ�����P��P��ѹ��;
                                                               11��Ӳ��ģ�鲻���P��;
                                                               ��������Ч;
    frm_acfc             [1]     1'b0    h/s R/W  AC��ѹ��ָʾ��0��AC����ѹ��;1����ʾAC��ѹ��;
    frm_in_lli_1dor2d    [0]     1'b0    h/s R/W  ��װ����һά���ά����ѡ��ָʾ�Ĵ�����
                                                                0Ϊһά;1Ϊ��ά;

    IPģʽһ�����P��,PPPģʽһ�������P��
      LCP֡: AC��ѹ����P��ѹ��
    */

    VOS_UINT32              ulFrmCfg;
    /*PS_BOOL_ENUM_UINT8      enPComp;       �Ƿ�ѹ��Э���ֶ�, �� - PS_TRUE */
    PS_BOOL_ENUM_UINT8      enACComp;     /* �Ƿ�ѹ����ַ�Ϳ����ֶ�, �� - PS_TRUE */
    VOS_UINT32              ulACCM;
    VOS_UINT8               ucACComp;
    VOS_UINT8               ucPComp;
    VOS_UINT8               ucLowByte;
    VOS_UINT16              usLowWord;


    /* ���Ĵ���hdlc_frm_cfg��[0]λfrm_in_lli_1dor2d����Ϊ0 */
    ulFrmCfg    = 0x0;

    if (PROTO_LCP == usProtocol)
    {
        /* LCP֡P��ѹ��, AC��ѹ��, ACCMΪȫת�� */
        /*enPComp     = PS_FALSE; */
        enACComp    = PS_FALSE;
        ulACCM      = 0xFFFFFFFFU;
    }
    else
    {
        /* ʹ��Ĭ��ֵ */
        /*enPComp     = (1 == pstLink->lcp.his_protocomp) ? PS_TRUE : PS_FALSE; */
        enACComp    = (1 == pstLink->lcp.his_acfcomp) ? PS_TRUE : PS_FALSE;
        /* ����PPP���Ų�֪��ACCM, ������ΪЭ��Ĭ��ֵ */
        ulACCM      = 0xFFFFFFFFU;
    }

    /* ��ȡAC��ѹ��ָʾ, P��ѹ��ָʾ, Э��ֵ, ACCM */
    ucACComp = (PS_TRUE == enACComp) ? 1 : 0;
    ucPComp = HDLC_PROTOCOL_NO_ADD;

    /* ����accm */
    PPP_HDLC_WRITE_32REG(SOC_ARM_HDLC_FRM_ACCM_ADDR(HDLC_IP_BASE_ADDR),ulACCM);

    /* ��ucPComp���õ�һ���ֽڵĵ�2, 3λ�� */
    ucLowByte = SET_BITS_VALUE_TO_BYTE(ucPComp, HDLC_FRM_PFC_BITPOS);

    if (1 == ucACComp)  /* AC��ѹ�� */
    {
        SET_BIT_TO_BYTE(ucLowByte, HDLC_FRM_ACFC_BITPOS);
    }
    else
    {
        CLEAR_BIT_TO_BYTE(ucLowByte, HDLC_FRM_ACFC_BITPOS);
    }

    usLowWord   = HDLC_MAKE_WORD(0x00, ucLowByte);
    ulFrmCfg    = HDLC_MAKE_DWORD(usProtocol, usLowWord);

    /* ����AC��ѹ��ָʾ, P��ѹ��ָʾ, Э��ֵ��ʹ�ܷ�װ���, accm */
    PPP_HDLC_WRITE_32REG(SOC_ARM_HDLC_FRM_CFG_ADDR(HDLC_IP_BASE_ADDR),ulFrmCfg);

    return;
}


VOS_VOID PPP_HDLC_HARD_FrmProcValidFrames
(
    PPP_ID                               usPppId,
    HDLC_PARA_LINK_BUILD_INFO_STRU      *pstBuildInfo
)
{
    PPP_ZC_STRU                        *pstMem;
    VOS_UINT16                          usFrmOutSegNum;
    VOS_UINT8                           ucFrmValidNum;
    VOS_UINT32                          ulFrmOutSpaceCnt;
    HDLC_FRM_BUFF_INFO_STRU            *pstFrmBuffInfo;
    HDLC_PARA_LINK_NODE_STRU           *pstOutputParaLink;
    HDLC_FRM_RPT_NODE_STRU             *pstFrmRptNodeStru;
    PPP_ZC_STRU                       **ppstOutputLinkNode;
    VOS_UINT32                          ucRptSpaceIndex;
    VOS_UINT32                          ulOutputLinkIndex;
    VOS_UINT8                          *pucFrmOutAddr;
    VOS_UINT16                          usFrmOutLen;
    VOS_UINT16                          usDataLen;


    pstFrmBuffInfo     = HDLC_FRM_GET_BUF_INFO(usPppId);                   /* ����usPppId�ҵ���Ӧ���ڴ�  */
    pstFrmRptNodeStru  = &(pstFrmBuffInfo->astRptNodeBuf[0]);              /* ��װ�ϱ��ռ��׵�ַ */
    pstOutputParaLink  = &(pstFrmBuffInfo->astOutputParaLinkNodeBuf[0]);   /* ��װ��������׵�ַ */
    ppstOutputLinkNode = &(pstBuildInfo->apstOutputLinkNode[0]);           /* ��װ�������ڵ��Ӧ�㿽���ڴ������׵�ַ */
    ulFrmOutSpaceCnt   = pstBuildInfo->ulOutputLinkNodeCnt;                /* ��װ�������ڵ�ĸ��� */

    /* ��Ч֡���ռ��Ƭ�θ���usFrmOutSegNum= hdlc_frm_status�Ĵ���[31:16]λ��ֵ */
    usFrmOutSegNum = (VOS_UINT16)TTF_Read32RegByBit(SOC_ARM_HDLC_FRM_STATUS_ADDR(HDLC_IP_BASE_ADDR), 16, 31);

    /* ��Ч֡����usFrmValidNum= hdlc_frm_status�Ĵ���[15:8]λ��ֵ */
    ucFrmValidNum  = (VOS_UINT8)TTF_Read32RegByBit(SOC_ARM_HDLC_FRM_STATUS_ADDR(HDLC_IP_BASE_ADDR), 8, 15);

    /* �ϱ���װ�����ݿ�ά�ɲ�:�ϱ��ռ���Ϣ������������� */
    PPP_HDLC_HARD_MntnFrmTraceOutput(ucFrmValidNum, usFrmOutSegNum, pstFrmBuffInfo, pstBuildInfo);

    /* ucFrmValidNum�϶�ҪС�ڵ���ʹ�õ��ڴ����usFrmOutSegNum */
    if( ucFrmValidNum > usFrmOutSegNum )
    {
        PPP_HDLC_ERROR_LOG2("PPP_HDLC_HARD_FrmProcValidFram, ERROR, frm_valid_num = %d > usFrmOutSegNum = %d",
                      ucFrmValidNum, usFrmOutSegNum);
        PPP_HDLC_HARD_CommReleaseLink(ppstOutputLinkNode, ulFrmOutSpaceCnt);
        return;
    }

    /* ucFrmValidNumӦ��������������ͬulInputLinkNodeCnt��������װ������ܻ�Ⱥ���С */
    if( ucFrmValidNum > pstBuildInfo->ulInputLinkNodeCnt )
    {
        PPP_HDLC_ERROR_LOG2("PPP_HDLC_HARD_FrmProcValidFram, ERROR, frm_valid_num = %d > InputLinkNodeCnt = %d",
                      ucFrmValidNum, pstBuildInfo->ulInputLinkNodeCnt);
        PPP_HDLC_HARD_CommReleaseLink(ppstOutputLinkNode, ulFrmOutSpaceCnt);
        return;
    }

    /* usFrmOutSegNumʹ�õ��ڴ�����϶�С�ڵ���ulOutputLinkNodeCnt */
    if( usFrmOutSegNum  > ulFrmOutSpaceCnt )
    {
        PPP_HDLC_ERROR_LOG2("PPP_HDLC_HARD_FrmProcValidFram, ERROR, frm_out_seg_num = %d > OutputLinkNodeCnt = %d",
                      usFrmOutSegNum, ulFrmOutSpaceCnt);
        PPP_HDLC_HARD_CommReleaseLink(ppstOutputLinkNode, ulFrmOutSpaceCnt);
        return;
    }

    ucRptSpaceIndex   = 0;
    ulOutputLinkIndex = 0;

    while( ucRptSpaceIndex < ucFrmValidNum )
    {
        pucFrmOutAddr    = (VOS_UINT8*)PPP_PHY_TO_VIRT((VOS_UINT32)pstFrmRptNodeStru[ucRptSpaceIndex].pucFrmOutOneAddr);
        usFrmOutLen      = pstFrmRptNodeStru[ucRptSpaceIndex].usFrmOutOneLen;

        if ( (0 == usFrmOutLen) || (HDLC_FRM_OUT_PER_MAX_CNT < usFrmOutLen) )
        {
            PPP_HDLC_ERROR_LOG1("PPP_HDLC_HARD_FrmProcValidFrames, ERROR, invalid usFrmOutOneLen %d\r\n", usFrmOutLen);

            /* �ͷ������Ŀ�Ŀռ� */
            PPP_HDLC_HARD_CommReleaseLink(ppstOutputLinkNode + ulOutputLinkIndex,
                                          ulFrmOutSpaceCnt - ulOutputLinkIndex);
            return;
        }

        pstMem      = ppstOutputLinkNode[ulOutputLinkIndex];

        if( pucFrmOutAddr != PPP_ZC_GET_DATA_PTR(pstMem) )
        {
            PPP_HDLC_ERROR_LOG("PPP_HDLC_HARD_FrmProcValidFram, ERROR, SOC copy error!\r\n");

            /* �ͷ������Ŀ�Ŀռ� */
            PPP_HDLC_HARD_CommReleaseLink(ppstOutputLinkNode + ulOutputLinkIndex,
                                          ulFrmOutSpaceCnt - ulOutputLinkIndex);
            return;
        }

        while( usFrmOutLen > 0 )
        {
            /* ��pstBuildInfo->apstOutputLinkNodeȡ��ͷ��� pstMem */
            pstMem    = ppstOutputLinkNode[ulOutputLinkIndex];

            /* ���������������ָĿ�Ŀռ�Ĵ�С */
            usDataLen = pstOutputParaLink[ulOutputLinkIndex].usDataLen;

            if( usFrmOutLen > usDataLen )
            {
                PPP_ZC_SET_DATA_LEN(pstMem, usDataLen);
                usFrmOutLen -= usDataLen;
            }
            else
            {
                PPP_ZC_SET_DATA_LEN(pstMem, usFrmOutLen);
                usFrmOutLen  = 0;
            }

            PPP_HDLC_ProcDlData(usPppId, pstMem);

            g_PppDataQCtrl.stStat.ulDownlinkSndDataCnt++;

            /* ������������ݰ���apstOutputLinkNode����� */
            ppstOutputLinkNode[ulOutputLinkIndex] = VOS_NULL_PTR;

            ulOutputLinkIndex++;
        }

        ucRptSpaceIndex++;
    }

    /* �ͷ�ʣ��δ���õ�Ŀ�Ŀռ� */
    PPP_HDLC_HARD_CommReleaseLink(ppstOutputLinkNode + ulOutputLinkIndex,
                                  ulFrmOutSpaceCnt - ulOutputLinkIndex);

    return;
}


VOS_VOID PPP_HDLC_HARD_FrmWaitAndProc
(
    VOS_UINT32                           ulEnableInterrupt,
    PPP_ID                               usPppId,
    struct link                         *pstLink,
    HDLC_PARA_LINK_BUILD_INFO_STRU      *pstBuildInfo
)
{
    VOS_UINT32                          ulFrmStatus;


    /* ʹ���жϣ���ȴ��жϸ������ͷ��ź�����������ѯ���װ״̬�Ĵ��� */
    ulFrmStatus = PPP_HDLC_HARD_FrmWaitResult(ulEnableInterrupt);

    if ( HDLC_FRM_ALL_PKT_DONE == ulFrmStatus )
    {
        /* ������Ч֡�����ͷ�ʣ���ڴ� */
        PPP_HDLC_HARD_FrmProcValidFrames(usPppId, pstBuildInfo);
    }
    else
    {
        /* �ͷ������Ŀ�Ŀռ� */
        PPP_HDLC_HARD_CommReleaseLink(pstBuildInfo->apstOutputLinkNode, pstBuildInfo->ulOutputLinkNodeCnt);

        /* ��ӡ�쳣��־������PPP���� */
        PPP_HDLC_HARD_FrmProcException(ulFrmStatus, ulEnableInterrupt);
    }

    g_stHdlcRegSaveInfo.ulHdlcFrmRawInt = 0xFFFFFFFFU;
    g_stHdlcRegSaveInfo.ulHdlcFrmStatus = 0xFFFFFFFFU;

    return;
}


VOS_UINT32 PPP_HDLC_HARD_FrmPacket
(
    PPP_ID              usPppId,
    VOS_UINT16          usProtocol,
    struct link        *pstLink,
    PPP_ZC_QUEUE_STRU  *pstDataQ,
    VOS_UINT32         *pulDealCurCnt
)
{
    VOS_UINT32                          ulEnableInterrupt;
    HDLC_PARA_LINK_BUILD_INFO_STRU      stBuildInfo;
    HDLC_PARA_LINK_BUILD_PARA_STRU      stBuildPara;
    VOS_UINT32                          ulBuildResult;
    HDLC_FRM_BUFF_INFO_STRU            *pstFrmBuffInfo;


    stBuildPara.ucDataType = PPP_PUSH_PACKET_TYPE;
    stBuildPara.usProtocol = usProtocol;
    stBuildPara.usPppId    = usPppId;
    stBuildPara.pstDataQ   = pstDataQ;

    /* ����usPppId�ҵ���Ӧ���ڴ� */
    pstFrmBuffInfo = HDLC_FRM_GET_BUF_INFO(usPppId);

    /* ����������������������ϱ������ڴ��ά�ɲ� */
    ulBuildResult  = PPP_HDLC_HARD_FrmBuildParaLink(&stBuildPara, &stBuildInfo);

    *pulDealCurCnt = stBuildInfo.ulDealCnt;

    if (VOS_OK != ulBuildResult)
    {
        PPP_HDLC_WARNING_LOG("PPP_HDLC_HARD_FrmPacket, WARNING, BuildParaLink Fail.\r\n");
        return ulBuildResult;
    }

    g_PppHdlcHardStat.ulFrmMaxInputCntOnce   = TTF_MAX(g_PppHdlcHardStat.ulFrmMaxInputCntOnce, stBuildInfo.ulInputLinkNodeCnt);
    g_PppHdlcHardStat.ulFrmMaxInputSizeOnce  = TTF_MAX(g_PppHdlcHardStat.ulFrmMaxInputSizeOnce, stBuildInfo.ulInputLinkTotalSize);
    g_PppHdlcHardStat.ulFrmMaxOutputCntOnce  = TTF_MAX(g_PppHdlcHardStat.ulFrmMaxOutputCntOnce, stBuildInfo.ulOutputLinkNodeCnt);
    g_PppHdlcHardStat.ulFrmMaxOutputSizeOnce = TTF_MAX(g_PppHdlcHardStat.ulFrmMaxOutputSizeOnce, stBuildInfo.ulOutputLinkTotalSize);

    /* ���÷�װ�����װͨ�üĴ��� */
    PPP_HDLC_HARD_CommCfgReg();

    /* �����ڴ���ؼĴ��� */
    PPP_HDLC_HARD_FrmCfgBufReg(pstFrmBuffInfo);

    /* ���÷�װ��ؼĴ��� */
    PPP_HDLC_HARD_FrmCfgReg(pstLink, usProtocol);

    /* ����ʹ�ܼĴ��������ϱ�ʹ��ǰ�Ĵ�����ά�ɲ� */
    ulEnableInterrupt = PPP_HDLC_HARD_FrmCfgEnReg(stBuildInfo.ulInputLinkTotalSize);

    /* �ȴ���װ��ɣ�Ȼ�����������,�ͷŶ����Ŀ�Ŀռ��ڴ� */
    PPP_HDLC_HARD_FrmWaitAndProc(ulEnableInterrupt, usPppId, pstLink, &stBuildInfo);

    /* �ͷ�pstBuildInfo->apstInputLinkNode�еĽ���ڴ� */
    PPP_HDLC_HARD_CommReleaseLink(stBuildInfo.apstInputLinkNode, stBuildInfo.ulInputLinkNodeCnt);

    return VOS_OK;
}


VOS_UINT32 PPP_HDLC_HARD_FrmRawData
(
    PPP_ID              usPppId,
    VOS_UINT16          usProtocol,
    struct link        *pstLink,
    PPP_ZC_QUEUE_STRU  *pstDataQ,
    VOS_UINT32         *pulDealCurCnt
)
{
    VOS_UINT32                          ulEnableInterrupt;
    HDLC_PARA_LINK_BUILD_INFO_STRU      stBuildInfo;
    HDLC_PARA_LINK_BUILD_PARA_STRU      stBuildPara;
    VOS_UINT32                          ulBuildResult;
    HDLC_FRM_BUFF_INFO_STRU            *pstFrmBuffInfo;


    stBuildPara.ucDataType = PPP_PUSH_RAW_DATA_TYPE;
    stBuildPara.usProtocol = usProtocol;
    stBuildPara.usPppId    = usPppId;
    stBuildPara.pstDataQ   = pstDataQ;

    /* ����usPppId�ҵ���Ӧ���ڴ�  */
    pstFrmBuffInfo         = HDLC_FRM_GET_BUF_INFO(usPppId);

    /* ����������������������ϱ������ڴ��ά�ɲ� */
    ulBuildResult  = PPP_HDLC_HARD_FrmBuildParaLink(&stBuildPara, &stBuildInfo);

    *pulDealCurCnt = stBuildInfo.ulDealCnt;

    if (VOS_OK != ulBuildResult)
    {
        return ulBuildResult;
    }


    g_PppHdlcHardStat.ulFrmMaxInputCntOnce   = TTF_MAX(g_PppHdlcHardStat.ulFrmMaxInputCntOnce, stBuildInfo.ulInputLinkNodeCnt);
    g_PppHdlcHardStat.ulFrmMaxInputSizeOnce  = TTF_MAX(g_PppHdlcHardStat.ulFrmMaxInputSizeOnce, stBuildInfo.ulInputLinkTotalSize);
    g_PppHdlcHardStat.ulFrmMaxOutputCntOnce  = TTF_MAX(g_PppHdlcHardStat.ulFrmMaxOutputCntOnce, stBuildInfo.ulOutputLinkNodeCnt);
    g_PppHdlcHardStat.ulFrmMaxOutputSizeOnce = TTF_MAX(g_PppHdlcHardStat.ulFrmMaxOutputSizeOnce, stBuildInfo.ulOutputLinkTotalSize);

    /* ���÷�װ�����װͨ�üĴ��� */
    PPP_HDLC_HARD_CommCfgReg();

    /* �����ڴ���ؼĴ��� */
    PPP_HDLC_HARD_FrmCfgBufReg(pstFrmBuffInfo);

    /* ���÷�װ��ؼĴ��� */
    PPP_HDLC_HARD_FrmRawCfgReg(pstLink, usProtocol);

    /* ����ʹ�ܼĴ��������ϱ�ʹ��ǰ�Ĵ�����ά�ɲ� */
    ulEnableInterrupt = PPP_HDLC_HARD_FrmCfgEnReg(stBuildInfo.ulInputLinkTotalSize);

    /* �ȴ���װ��ɣ�Ȼ�����������,�ͷŶ����Ŀ�Ŀռ��ڴ� */
    PPP_HDLC_HARD_FrmWaitAndProc(ulEnableInterrupt, usPppId, pstLink, &stBuildInfo);

    /* �ͷ�pstBuildInfo->apstInputLinkNode�еĽ���ڴ� */
    PPP_HDLC_HARD_CommReleaseLink(stBuildInfo.apstInputLinkNode, stBuildInfo.ulInputLinkNodeCnt);

    return VOS_OK;
}


PPP_HDLC_RESULT_TYPE_ENUM_UINT32 PPP_HDLC_HARD_ProcData
(
    PPP_ID              usPppId,
    struct link        *pstLink,
    PPP_ZC_QUEUE_STRU  *pstDataQ
)
{
    PPP_ZC_STRU                        *pstMem;
    VOS_UINT32                          ulDealTotalCnt;
    VOS_UINT32                          ulDealCurCnt;
    VOS_UINT32                          ulDealResult;
    VOS_UINT16                          usProtocol;
    PPP_DATA_TYPE_ENUM_UINT8            ucDataType;


    g_PppHdlcHardStat.ulHdlcHardProcCnt++;

    ulDealTotalCnt  = 0;
    ulDealCurCnt    = 0;

    for (;;)
    {
        pstMem  = (PPP_ZC_STRU *)PPP_ZC_PEEK_QUEUE_HEAD(pstDataQ);

        /* ����Ϊ�յ�ʱ�򷵻ؿ�ָ�� */
        if ( VOS_NULL_PTR == pstMem )
        {
            return PPP_HDLC_RESULT_COMM_FINISH;
        }

        /*���ѭ������Ľ����������˶���һ�����������������
          ���˳�ѭ��������PPP_DATA_PROC_NOTIFY��Ϣ*/
        if ( (ulDealTotalCnt >= PPP_ONCE_DEAL_MAX_CNT) )
        {
            g_PppHdlcHardStat.ulContinueCnt++;
            return PPP_HDLC_RESULT_COMM_CONTINUE;
        }

        /*����ý��(�����ͷŶ����Ѿ��ڸ��������ڲ���ɣ��������ͷŽ��)*/
        ucDataType   = (PPP_ZC_GET_DATA_APP(pstMem) & 0x00FF);
        ulDealResult = VOS_OK;
        ulDealCurCnt = 0;

        switch ( ucDataType )
        {
            case PPP_PULL_PACKET_TYPE:

                ulDealResult = PPP_HDLC_HARD_DefPacket(usPppId, pstLink, pstDataQ, &ulDealCurCnt);

                g_PppHdlcHardStat.ulDefIpDataProcCnt += ulDealCurCnt;

                LcpEchoAdjust(pstLink);

                break;
            case PPP_PUSH_PACKET_TYPE:

                ulDealResult = PPP_HDLC_HARD_FrmPacket(usPppId, PROTO_IP, pstLink, pstDataQ, &ulDealCurCnt);

                g_PppHdlcHardStat.ulFrmIpDataProcCnt += ulDealCurCnt;
                break;

            case PPP_PULL_RAW_DATA_TYPE:

                ulDealResult = PPP_HDLC_HARD_DefRawData(usPppId, pstLink, pstDataQ, &ulDealCurCnt);

                g_PppHdlcHardStat.ulDefIpDataProcCnt += ulDealCurCnt;

                LcpEchoAdjust(pstLink);

                break;

            case PPP_PUSH_RAW_DATA_TYPE:

                /* ��ȡ����װ���ݰ���Э��ֵusProtocol */
                if (VOS_OK == PPP_HDLC_HARD_FrmGetProtocol(pstMem, &usProtocol))
                {
                    ulDealResult = PPP_HDLC_HARD_FrmRawData(usPppId, usProtocol, pstLink, pstDataQ, &ulDealCurCnt);

                    g_PppHdlcHardStat.ulFrmPppDataProcCnt += ulDealCurCnt;
                }
                else
                {
                    pstMem  = (PPP_ZC_STRU *)PPP_ZC_DEQUEUE_HEAD(pstDataQ);
                    PPP_MemFree(pstMem);
                }
                break;

            default:
                pstMem  = (PPP_ZC_STRU *)PPP_ZC_DEQUEUE_HEAD(pstDataQ);
                PPP_MemFree(pstMem);
                PPP_HDLC_WARNING_LOG1("PPP_HDLC_HARD_ProcData, WARNING, ucDataType %d is Abnormal!", ucDataType);
                break;
        }

        /*ͳ��*/
        ulDealTotalCnt += ulDealCurCnt;

        if ( ulDealTotalCnt > g_PppHdlcHardStat.ulMaxCntOnce )
        {
            g_PppHdlcHardStat.ulMaxCntOnce = ulDealTotalCnt;
        }

        if (VOS_OK != ulDealResult)
        {
            PPP_HDLC_WARNING_LOG("PPP_HDLC_HARD_ProcData, WARNING, ulDealResult = ERROR!");
            break;
        }
    } /* for (;;) */

    return PPP_HDLC_RESULT_COMM_ERROR;
}


VOS_UINT32 PPP_HDLC_HARD_SendAsFrmPacketMsg
(
    VOS_UINT16       usPppId,
    VOS_UINT16       usProtocol,
    PPP_ZC_STRU     *pstMem
)
{
    HDLC_PROC_AS_FRM_PACKET_IND_MSG_STRU    *pstMsg;
    VOS_UINT32                               ulLength;


    ulLength    = sizeof(HDLC_PROC_AS_FRM_PACKET_IND_MSG_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg      = (HDLC_PROC_AS_FRM_PACKET_IND_MSG_STRU *)PS_ALLOC_MSG(PS_PID_PPP_HDLC, ulLength);

    if (VOS_NULL_PTR == pstMsg)
    {
        /*��ӡ������Ϣ---������Ϣ��ʧ��:*/
        PPP_HDLC_ERROR_LOG("PPP_HDLC_HARD_SendMsg, ERROR: PS_ALLOC_MSG Failed!\r\n");
        return VOS_ERR;
    }

    /*��д��Ϣͷ:*/
    pstMsg->MsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulSenderPid     = PS_PID_PPP_HDLC;
    pstMsg->MsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid   = PS_PID_PPP_HDLC;
    pstMsg->MsgHeader.ulLength        = ulLength;

    pstMsg->MsgHeader.ulMsgName       = PPP_HDLC_PROC_AS_FRM_PACKET_IND;
    /*��д��Ϣ��:*/
    pstMsg->usPppId                   = usPppId;
    pstMsg->usProtocol                = usProtocol;
    pstMsg->pstMem                    = pstMem;

    /*���͸���Ϣ*/
    if (VOS_OK != PS_SEND_MSG(PS_PID_PPP_HDLC, pstMsg))
    {
        /*��ӡ������Ϣ---������Ϣʧ��:*/
        PPP_HDLC_ERROR_LOG("PPP_HDLC_HARD_SendAsFrmPacketMsg, ERROR : PS_SEND_MSG Failed!");
        PPP_MemFree(pstMem);
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_VOID PPP_HDLC_HARD_ProcAsFrmPacket
(
    VOS_UINT16       usPppId,
    VOS_UINT16       usProtocol,
    PPP_ZC_STRU     *pstMem
)
{
    VOS_UINT32              ulDealCurCnt;
    PPP_ZC_QUEUE_STRU       stDataQ;
    struct link            *pstLink;
    VOS_UINT32              ulDealResult;

    pstLink = PPP_LINK(usPppId);

    ulDealCurCnt = 0;

    /* ��װ������IP�������ݰ� */
    PPP_ZC_SET_DATA_APP(pstMem, (VOS_UINT16)(usPppId << 8) | (VOS_UINT16)PPP_PUSH_PACKET_TYPE);

    /* ��ʼ������stDataQ, ���� pstMem ��� */
    PPP_ZC_QUEUE_INIT(&stDataQ);
    PPP_ZC_ENQUEUE_TAIL(&stDataQ, pstMem);

    /* ֱ�ӽ������ݰ���װ���͸�PC */
    ulDealResult = PPP_HDLC_HARD_FrmPacket(usPppId, usProtocol, pstLink, &stDataQ, &ulDealCurCnt);

    if ((VOS_OK != ulDealResult) || (1 != ulDealCurCnt))
    {
        PPP_HDLC_ERROR_LOG2("PPP_HDLC_HARD_MakeFrmPacket, ERROR, ulDealResult %d ulDealCurCnt %d!\r\n",
                      ulDealResult, ulDealCurCnt);

        /* ˵����ĳ�ִ���������û�дӶ������Ƴ�����Ҫ�ͷ� */
        if (0 < PPP_ZC_GET_QUEUE_LEN(&stDataQ))
        {
            PPP_MemFree(pstMem);
        }

        return;
    }

    g_PppHdlcHardStat.ulFrmIpDataProcCnt++;

    return;
}


VOS_VOID PPP_HDLC_HARD_ProcProtocolPacket
(
    struct link     *pstLink,
    struct ppp_mbuf *pstMbuf,
    VOS_INT32        ulPri,
    VOS_UINT16       usProtocol
)
{
    PPP_ZC_STRU            *pstMem;
    VOS_UINT16              usPppId;
    VOS_UINT32              ulFrmResult;
    VOS_UINT32              ulDefResult;


    usPppId = (VOS_UINT16)PPP_LINK_TO_ID(pstLink);

    pstMem  = ppp_m_alloc_ttfmem_from_mbuf(pstMbuf);

    ppp_m_freem(pstMbuf);
    pstMbuf  = VOS_NULL_PTR;

    /* for lint 438 */
    (VOS_VOID)pstMbuf;

    if (VOS_NULL_PTR == pstMem)
    {
        PPP_HDLC_WARNING_LOG("PPP_HDLC_HARD_ProcProtocolPacket, WARNING, pstMem is NULL!\r\n");
        return;
    }

    /* PPPЭ��ջ�ظ���Э�̰���Ҫ��װ�󷢸�PC��ȷ�ϴ�ʱ�Ƿ��װ����װ���Ѿ���� */
    ulFrmResult     = PPP_HDLC_HARD_FrmIsEnabled();
    ulDefResult     = PPP_HDLC_HARD_DefIsEnabled();

    /* �˺��������н��Э�̰�ʱ��PPPЭ��ջ����Ӧ�����ʱ���װ��Ӧ��ʹ�� */
    if( (VOS_TRUE == ulFrmResult) || (VOS_TRUE == ulDefResult) )
    {
        PPP_HDLC_WARNING_LOG2("PPP_HDLC_HARD_ProcProtocolPacket, WARNING, ulFrmResult %d, ulDefResult %d!\r\n",
                      ulFrmResult, ulDefResult);

        PPP_HDLC_HARD_SendAsFrmPacketMsg(usPppId, usProtocol, pstMem);

        return;
    }

    /* �´�PPP������ȵ�ʱ���ٷ�װ��Э�̣���ʱ��װ����װ��δʹ�� */
    PPP_HDLC_HARD_ProcAsFrmPacket(usPppId, usProtocol, pstMem);

    return;
}


VOS_VOID PPP_HDLC_HARD_Help(VOS_VOID)
{
    vos_printf("********************PPP�����Ϣ************************\n");
    vos_printf("PPP_HDLC_HARD_MntnShowStatInfo      ��ӡͳ����Ϣ\n");
    vos_printf("PPP_INPUT_ShowStatInfo              ��ӡ g_PppDataQCtrl��Ϣ\n");
    vos_printf("PPP_HDLC_HARD_MntnSetConfig         ���ÿ�ά�ɲ�ȼ�:\n");
    vos_printf("                                    1--������2--�Ĵ�����4--����\n");
    vos_printf("PPP_HDLC_HARD_MntnShowDefReg        ��ӡ���װ�Ĵ�����Ϣ\n");
    vos_printf("PPP_HDLC_HARD_MntnShowFrmReg        ��ӡ��װ�Ĵ�����Ϣ\n");
    vos_printf("PPP_HDLC_HARD_MntnSetDefIntLimit    ���ý��װ�ж�ˮ��\n");
    vos_printf("PPP_HDLC_HARD_MntnSetFrmIntLimit    ���÷�װ�ж�ˮ��\n");
    vos_printf("PPP_HDLC_HARD_MntnShowConfigInfo    ��ӡ������Ϣ\n");

    vos_printf("PPP_Info                            ���ַ  :0x%x\n", g_ulPppVirtAddr);
    vos_printf("PPP_Info                            �����ַ:0x%x\n", g_ulPppPhyAddr);
    vos_printf("PPP_Info                            �ڴ泤��:0x%x\n", g_ulPppTotalBufLen);

    return;
}

#endif  /* #if(FEATURE_ON == FEATURE_HARDWARE_HDLC_FUNC) */

#endif /* #if(FEATURE_ON == FEATURE_PPP) */

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
