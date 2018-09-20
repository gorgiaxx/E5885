/*
 *
 * All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses. You may choose this file to be licensed under the terms
 * of the GNU General Public License (GPL) Version 2 or the 2-clause
 * BSD license listed below:
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS "AS IS" AND
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
 */



/*****************************************************************************
    Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
*****************************************************************************/
/*lint -e767  �޸���: z57034; ������: g45205 ԭ�����: �����־�ļ���ID���� */
#define    THIS_FILE_ID        PS_FILE_ID_PPP_ATCMD_C
/*lint +e767  �޸���: z57034; ������: g45205 */

#include "product_config.h"

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "AtPppInterface.h"

/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/

/*****************************************************************************
 Prototype      : Ppp_CreatePppReq
 Description    : ΪATģ��"����PPP��·"�ṩ��Ӧ��API������

 Input          : ---
 Output         : ---�����ɹ��󷵻ص�PPP ID
 Return Value   : ---VOS_UINT32
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-11-18
    Author      : ---
    Modification: Created function
*****************************************************************************/
VOS_UINT32 Ppp_CreatePppReq ( VOS_UINT16 *pusPppId)
{
    return VOS_OK;
}


/*****************************************************************************
 Prototype      : Ppp_CreateRawDataPppReq
 Description    : ����PDP����ΪPPP��PPPʵ�壬��������·����ֻ�����ݵķ�װ�ͽ��װ

 Input          : ---
 Output         : ---�����ɹ��󷵻ص�PPP ID
 Return Value   : ---VOS_UINT32
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-11-18
    Author      : ---
    Modification: Created function
*****************************************************************************/
VOS_UINT32 Ppp_CreateRawDataPppReq ( VOS_UINT16 *pusPppId)
{
    return VOS_OK;
}


/*****************************************************************************
 Prototype      : Ppp_RcvConfigInfoInd
 Description    : ΪATģ��"PPPģ���������ָʾ��������Ϣ"�ṩ��Ӧ��API������
                  ��AT��GGSN��֤�ɹ��󣬵��ô˺�����PPP��ָʾ��

 Input          : usPppId---Ҫ��ָʾ��PPP��·���ڵ�PPP ID
                  pPppIndConfigInfo---��GGSN�����ĸ�PPP��·��IP��ַ����Ϣ
 Output         : ---
 Return Value   : ---VOS_UINT32
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-11-18
    Author      : ---
    Modification: Created function
*****************************************************************************/
VOS_UINT32 Ppp_RcvConfigInfoInd
(
    PPP_ID usPppId,
    AT_PPP_IND_CONFIG_INFO_STRU         *pstAtPppIndConfigInfo
)
{
    return VOS_OK;
}


/*****************************************************************************
 Prototype      : PPP_RcvAtCtrlOperEvent
 Description    : ΪATģ��"��PPP���Ϳ��Ʋ���"�ṩ��Ӧ��API������
 Input          : usPppId---Ҫ��ָʾ��PPP��·���ڵ�PPP ID
                  ulCtrlOperType---AT��PPP���͵Ŀ��Ʋ�������
 Output         : ---
 Return Value   : VOS_OK        --�����ɹ�
                  VOS_ERR       --����ʧ��
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2008-04-17
    Author      : L47619
    Modification: Created function
*****************************************************************************/
VOS_UINT32 PPP_RcvAtCtrlOperEvent(VOS_UINT16 usPppId, VOS_UINT32 ulCtrlOperType)
{
    return VOS_OK;
}

/*****************************************************************************
 Prototype      : Ppp_RegDlDataCallback
 Description    : ΪATģ���ṩע�����з������ݵ�API

 Input          : usPppId---Ҫ��ָʾ��PPP��·���ڵ�PPP ID
 Output         : ---
 Return Value   : ---VOS_UINT32
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2013-06-04
    Author      : ---
    Modification: Created function
*****************************************************************************/
VOS_UINT32 Ppp_RegDlDataCallback(PPP_ID usPppId)
{
    return VOS_OK;
}


