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
#include "ATCmdProc.h"
#include "AtParse.h"
#include "AtCheckFunc.h"
#include "AtCmdCallProc.h"
#include "AppVcApi.h"
#include "AtSndMsg.h"
#include "AtTafAgentInterface.h"
#include "AtEventReport.h"
#include "AtMsgPrint.h"
#include "MbbAudioNvXmlDec.h"
#include "MbbAtCmdCallProc.h"
#include "TafTypeDef.h"
#include "hi_list.h"
#include "AtMtaInterface.h"
#include "AtTestParaCmd.h"
#include "TafStdlib.h"
#include "CodecNvInterface.h"
#include "CodecNvId.h"
#include <linux/syscalls.h>
#include <linux/string.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

#if(FEATURE_ON == MBB_WPG_PCM)
/*****************************************************************************
2 ȫ�ֱ�������
*****************************************************************************/

#define THIS_FILE_ID PS_FILE_ID_AUDIO_NV_XML_DEC_C

/*xml decode info*/
static AUDIO_XML_DOCODE_INFO audio_xml_ctrl;

/* XML�ؼ���,������0-9,a-z,A-Z */
static VOS_INT8  g_stlaudioxmkeywordtbl[] = { '<', '>', '/', '=', '"', \
                                   ' ', '!', '?', '_', '-', \
                                   ',','{','}','[',']'};

/* NV ID��*/
VOS_UINT16  g_stlaudioxmnvIdtbl[AUDIO_CTRL_MODE_SUB_ID_NUM] \
= {en_Anfu_NB_CarFree1, en_Anfu_NB_CarFree2, en_Anfu_WB_CarFree1, en_Anfu_WB_CarFree2};

/* XML�ļ�����ʱ��״̬                  */
static AUDIO_XML_ANALYSE_STATUS_ENUM_UINT32 g_stlaudioxmlstatus = AUDIO_XML_ANASTT_ORIGINAL;

/* XML�ļ��ײ���β����ǩ������ */
static const VOS_INT8 *g_paudioxmlheadandtaillabel[AUDIO_DELTA_XML_HEAD_TAIL_LABEL_NUM] \
= {"NV_CATEGORY", "PROJECT_GROUP"};

/* NV�ڵ�����*/
static VOS_INT8 g_aclnodeaudiolabelnv[] = "NV";

/* NV id�������� */
static VOS_INT8 g_aclaudiopropertyId[] = "ID";

/*PARAM_VALUE��������*/
static VOS_INT8 g_aclpropertyparamvalue[] = "PARAM_VALUE";

/* �ڵ�ʮ������ֵ֮��ķָ��� */
static VOS_INT8 g_separator = ',';

/*����洢NVID��NV valueֵ��ȫ�ֱ���*/
static AUDIO_DELAT_XML_NV_INFO_STRU g_audio_delta_nv[AUDIO_CTRL_MODE_SUB_ID_NUM] = {0};

/*NV��Ӧ�������±�*/
static VOS_UINT16 g_nv_number = 0;

/*Global map table used to find the function according the xml analyse status.*/
/*lint -e64*/
AUDIO_XML_FUN g_uslaudioxmlanalysefuntbl[] =
{
    audio_xml_procxmlorginal,             /* ��ʼ״̬�µĴ��� */
    audio_xml_proc_xml_enter_label,       /* ����Lable��Ĵ���*/
    audio_xml_procxmlignore,              /* ���Ի�ע��״̬��ֱ������">"����*/
    audio_xml_proc_xml_node_label,        /* ��ǩ���ֿ�ʼ */
    audio_xml_proc_xmlsingle_endlabel,    /* ��׼�Ľ�β��ǩ</XXX> */
    audio_xml_procxmlend_mustberight,     /* ���� <XXX/>�ı�ǩ,�ڽ�����/��״̬ */
    audio_xml_proc_xml_propertystart,     /* ��ʼ�������� */
    audio_xml_proc_xml_propertyname,      /* ��ʼ������������*/
    audio_xml_proc_xml_propertyname_tail, /* �������ֽ������ȴ�"������ֵ��ʼ   */
    audio_xml_proc_xml_valuestart,        /* ����ֵ��ʼ*/
    audio_xml_proc_xml_valuetail,         /* ����ֵ����*/
};

/* У��XML�ļ��ײ���β����ǩ�ĺ������� */
static AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_checkheadandtaillabel(void);

/*****************************************************************************
3 ��������
*****************************************************************************/

void audio_xml_write_error_log(VOS_UINT32 ulerrorline, VOS_UINT16 ulnvid,
                                VOS_UINT32 ret)
{
    audio_xml_ctrl.g_stlxmlerrorinfo.ulxmlline = audio_xml_ctrl.g_stlxml_lineno;
    audio_xml_ctrl.g_stlxmlerrorinfo.ulstatus = g_stlaudioxmlstatus;
    audio_xml_ctrl.g_stlxmlerrorinfo.ulcodeline = ulerrorline;
    audio_xml_ctrl.g_stlxmlerrorinfo.usnvid = ulnvid;
    audio_xml_ctrl.g_stlxmlerrorinfo.ulresult = (VOS_UINT32)ret;

    return ;
}


AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_checkxmlkeyword(VOS_INT8 currentchar)
{
    VOS_UINT32 lcount = 0;

    if ((('0' <= currentchar) && ('9' >= currentchar))   /* ��Ч�ַ���0-9  */
        || (('a' <= currentchar) && ('z' >= currentchar)) /* ��Ч�ַ���a-z  */
        || (('A' <= currentchar) && ('Z' >= currentchar)))/* ��Ч�ַ���A-Z  */
    {
        return AUDIO_XML_RESULT_SUCCEED;
    }

    /* �� 0-9,a-z,A-Z ֮��� XML�ؼ��� */
    for (lcount = 0; lcount < sizeof(g_stlaudioxmkeywordtbl); lcount++)
    {
        if (currentchar == g_stlaudioxmkeywordtbl[lcount])
        {
            return AUDIO_XML_RESULT_SUCCEED;
        }
    }
 
    return AUDIO_XML_RESULT_FALIED_BAD_CHAR;
}


AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_checkcharvalidity(VOS_INT8 currentchar)
{
    AUDIO_XML_RESULT_ENUM_UINT32 returnval = 0;

    if (('\r' == currentchar)       /* ���Իس�   */
        || ('\t' == currentchar))   /* �����Ʊ�� */
    {
        return AUDIO_XML_RESULT_SUCCEED_IGNORE_CHAR;
    }

    if ('\n' == currentchar)    /* ���Ի���   */
    {
        audio_xml_ctrl.g_stlxml_lineno++;
        return AUDIO_XML_RESULT_SUCCEED_IGNORE_CHAR;
    }

    /* ��ע���е��ַ�������� */
    if ( AUDIO_XML_ANASTT_IGNORE == g_stlaudioxmlstatus)
    {
        return AUDIO_XML_RESULT_SUCCEED;
    }

    /* ���XML�Ĺؼ��� */
    returnval = audio_xml_checkxmlkeyword(currentchar);
 
    if (AUDIO_XML_RESULT_SUCCEED != returnval)
    {
        audio_xml_write_error_log(__LINE__, 0, returnval);

        return returnval;
    }

    return AUDIO_XML_RESULT_SUCCEED;
}


AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_stringtodec(VOS_INT8  *pcbuff,
                                            VOS_INT32 *pusretval)
{
    VOS_UINT32 ultemp = 0;  /* �ַ���ת������ʱ���м���� */
    VOS_INT8   currentchar;
    VOS_INT8   *pcsrc;
    VOS_BOOL   bMinus = VOS_FALSE;

    pcsrc = pcbuff;

    /* ���NV ID�ǿյģ��򷵻ش��� */
    if (0 == *pcsrc)
    {
        audio_xml_write_error_log(__LINE__, 0, AUDIO_XML_RESULT_FALIED_NV_ID_IS_NULL);

        return AUDIO_XML_RESULT_FALIED_NV_ID_IS_NULL;
    }

    if (('0' > *pcsrc || '9' < *pcsrc) && ('+' == *pcsrc || '-' == *pcsrc))
    {
        if ('-' == *pcsrc)
        {
            bMinus = VOS_TRUE;
        }
        pcsrc++;
    }

    /* ���ַ���ת��ʮ���Ƶĸ�ʽ */
    while ('\0' != *pcsrc)
    {
        currentchar = *pcsrc;

        /* �Բ���0��9֮����ַ����������� */
        if ((currentchar < '0') || (currentchar > '9'))
        {
            audio_xml_write_error_log(__LINE__, 0, AUDIO_XML_RESULT_FALIED_OUT_OF_0_9);
            return AUDIO_XML_RESULT_FALIED_OUT_OF_0_9;
        }

        
        currentchar -= '0';
        /* ת��ʮ���Ƹ�ʽ */
        ultemp = (ultemp * 10) + (VOS_UINT8)currentchar;

        pcsrc++;
    }

    /* ��� ת�����ֵ */
    *pusretval = (bMinus ? (0 - (VOS_UINT16)ultemp) : (VOS_UINT16)ultemp);

    return AUDIO_XML_RESULT_SUCCEED;
}


AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_stringtovalue(VOS_UINT8  *pucbuff)
{
    AUDIO_XML_RESULT_ENUM_UINT32 returnval = 0;
    VOS_UINT8 *pcsrc;
    VOS_UINT8 pcdest[AUDIO_DELTA_XML_NV_VALUE_NUM][AUDIO_DELTA_XML_NV_VALUE_LENGTH] = {0};
    VOS_INT32 usnvitemvalue = 0;
    VOS_INT8   currentchar;
    VOS_UINT16 i = 0;
    VOS_UINT16 j = 0;
    VOS_UINT16 nv_num = 0;
    
    pcsrc  = pucbuff;

     /* ���NV VALUE�ǿյģ��򷵻ش��� */
    if (0 == *pcsrc)
    {
        printk(KERN_ERR " the string is null!\n");

        audio_xml_write_error_log(__LINE__, 0, AUDIO_XML_RESULT_FALIED_NV_VALUE_IS_NULL);
        return AUDIO_XML_RESULT_FALIED_NV_VALUE_IS_NULL;
    }

    /* �Ȱ�ԭ�ַ����еķָ���ȥ�� */
    while ('\0' != *pcsrc)
    {
        currentchar = *pcsrc;

        if(('{' == currentchar) || ('[' == currentchar) || \
           ('}' == currentchar) || (']' == currentchar))
        {
            pcsrc++;
            continue;
        }
        
        /* �����ǰ�ַ��Ƿָ��� */
        if (g_separator == currentchar)
        {
            pcdest[nv_num][j] = '\0';

            nv_num += 1;
            j = 0;

            pcsrc++;
            continue;
        }

        pcdest[nv_num][j++] = currentchar;

        pcsrc++;

    }

    /* ���һ���ַ��������ַ���������'\0' */
    pcdest[nv_num][j] = '\0';

    /*���NV�ĸ����Ƿ���ȷ*/
    if ((AUDIO_DELTA_XML_NV_VALUE_NUM - 1) != nv_num)
    {
        audio_xml_write_error_log(__LINE__, 0, AUDIO_XML_RESULT_FALIED_MALLOC);

        return AUDIO_XML_RESULT_GET_NV_DATA_FAIL;
    }

    for (i = 0; i < AUDIO_DELTA_XML_NV_VALUE_NUM; i++)
    {
        returnval = audio_xml_stringtodec(pcdest[i],&usnvitemvalue);

        if (AUDIO_XML_RESULT_SUCCEED != returnval)
        {
            return returnval;
        }
        else
        {
            g_audio_delta_nv[g_nv_number].AudioNvValue.ashwNv[i] = (VOS_INT16)usnvitemvalue;

            usnvitemvalue = 0;
        }
    }
    
    return AUDIO_XML_RESULT_SUCCEED;
}


AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_get_nv_data(void)
{
    AUDIO_XML_RESULT_ENUM_UINT32 returnval = AUDIO_XML_RESULT_BUTT;
    VOS_INT32  usnvitemid   = 0;
    VOS_UINT32 ulPropertyIndex = 0;

    /* �������ֵΪ��,�����κδ���*/
    audio_xml_ctrl.g_stlxmlcurrentnode.stproperty[0].pcpropertyvalue[
                        audio_xml_ctrl.g_stlxmlcurrentnode.stproperty[0].ulvaluelength] = '\0';

    if ((0 == *audio_xml_ctrl.g_stlxmlcurrentnode.stproperty[0].pcpropertyvalue))
    {
        return AUDIO_XML_RESULT_SUCCEED;
    }
    for(ulPropertyIndex = 0; ulPropertyIndex <= audio_xml_ctrl.g_stlxmlcurrentnode.ulPropertyIndex; ulPropertyIndex++)
    {
        audio_xml_ctrl.g_stlxmlcurrentnode.stproperty[ulPropertyIndex].pcpropertyname[
                    audio_xml_ctrl.g_stlxmlcurrentnode.stproperty[ulPropertyIndex].ulnamelength] = '\0';

        audio_xml_ctrl.g_stlxmlcurrentnode.stproperty[ulPropertyIndex].pcpropertyvalue[
                            audio_xml_ctrl.g_stlxmlcurrentnode.stproperty[ulPropertyIndex].ulvaluelength] = '\0';

        if(!strcmp(audio_xml_ctrl.g_stlxmlcurrentnode.stproperty[ulPropertyIndex].pcpropertyname, g_aclaudiopropertyId))
        {
            /* ��id ����ֵת��NV ID */
            returnval = audio_xml_stringtodec(audio_xml_ctrl.g_stlxmlcurrentnode.stproperty[ulPropertyIndex].pcpropertyvalue,
                                        &usnvitemid);

            if (AUDIO_XML_RESULT_SUCCEED != returnval)
            {
                goto out;
            }

            /*�鿴��ǰNV ID�Ƿ�Ϊ��ƵNV ID*/
            returnval = audio_xml_nv_search_byid((VOS_UINT16)usnvitemid);

            if(AUDIO_XML_RESULT_SUCCEED == returnval)
            {
                g_audio_delta_nv[g_nv_number].nv_id = usnvitemid;
                continue;
            }
            else
            {
                return AUDIO_XML_RESULT_FALIED_NV_ID_IS_ERROR;
            }
        }
        else if(!strcmp(audio_xml_ctrl.g_stlxmlcurrentnode.stproperty[ulPropertyIndex].pcpropertyname, g_aclpropertyparamvalue))
        {
            /*��ֹû��NVID��ֻ��param value�����*/
            if( 0 == g_audio_delta_nv[g_nv_number].nv_id)
            {
                break;
            }

            /* ��param value�ַ���ת��Ϊ��ֵ*/
            returnval = audio_xml_stringtovalue(audio_xml_ctrl.g_stlxmlcurrentnode.stproperty[ulPropertyIndex].pcpropertyvalue);

            if (AUDIO_XML_RESULT_SUCCEED != returnval)
            {
                goto out;
            }
            else
            {
                g_nv_number += 1;
                break;
            }
        }
        else
        {
            continue;
        }
    }

    return AUDIO_XML_RESULT_SUCCEED;
out:
    /* ��¼�����NV ID */
    return AUDIO_XML_RESULT_GET_NV_DATA_FAIL;
}


void audio_xml_nodereset(void)
{
    VOS_UINT32 ulPropertyIndex = 0;

    /* �ڵ��ǩ��λ,��ʹ�õĳ���Ϊ0  */
    audio_xml_ctrl.g_stlxmlcurrentnode.ullabellength = 0;

    /*xml_ctrl.g_stlxmlcurrentnode.stproperty���±�������0*/
    audio_xml_ctrl.g_stlxmlcurrentnode.ulPropertyIndex = 0;
  
    for(ulPropertyIndex = 0; ulPropertyIndex < AUDIO_XML_NODE_PROPERTY_NUM; ulPropertyIndex++)
    {
        /* �ڵ���������λ,��ʹ�õĳ���Ϊ0 */
        audio_xml_ctrl.g_stlxmlcurrentnode.stproperty[ulPropertyIndex].ulnamelength = 0;

        /* �ڵ�����ֵ��λ,��ʹ�õĳ���Ϊ0 */
        audio_xml_ctrl.g_stlxmlcurrentnode.stproperty[ulPropertyIndex].ulvaluelength = 0;
    }

    return;
}


AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_write_char_to_buff(VOS_INT8   cnowchar,
                                                 VOS_INT8   *pcstrbuff,
                                                 VOS_UINT32 *plbufflength,
                                                 VOS_BOOL    ulisparamvalue)
{
    /* ���Կո� */
    if (' ' == cnowchar)
    {
        return AUDIO_XML_RESULT_SUCCEED;
    }

    /* ����ﵽ��Node Lable����param name����󳤶� */
    if ((VOS_FALSE == ulisparamvalue) \
        && (*plbufflength >= AUDIO_XML_NODE_LABEL_BUFF_LENGTH_ORIGINAL))
    {
        audio_xml_write_error_log(__LINE__, 0, AUDIO_XML_RESULT_FALIED_OUT_OF_BUFF_LEN);

        return AUDIO_XML_RESULT_FALIED_OUT_OF_BUFF_LEN;
    }

    /* ����ﵽ��param value����󳤶� */
    if ((VOS_TRUE == ulisparamvalue) \
        && (*plbufflength >= AUDIO_XML_PARAM_VALUE_BUFF_LENGTH_ORIGINAL))
    {
        audio_xml_write_error_log(__LINE__, 0, AUDIO_XML_RESULT_FALIED_OUT_OF_BUFF_LEN);

        return AUDIO_XML_RESULT_FALIED_OUT_OF_BUFF_LEN;
    }

    /* �����ַ��ӽ������� */
    *(pcstrbuff + *plbufflength) = cnowchar;

    /* ���������ȼ�1 */
    (*plbufflength)++;

    return AUDIO_XML_RESULT_SUCCEED;
}


AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_createaproperty( void )
{
    VOS_UINT32 propertyIndex = 0;

    /* ���Խڵ���±�������0 */
    audio_xml_ctrl.g_stlxmlcurrentnode.ulPropertyIndex = 0;

    for(propertyIndex = 0;propertyIndex < AUDIO_XML_NODE_PROPERTY_NUM; propertyIndex++)
    {
        /* �����������ڴ�,+1Ϊ�����ַ����������� */
        audio_xml_ctrl.g_stlxmlcurrentnode.stproperty[propertyIndex].ulnamelength = 0; /* ��ʹ�õĳ��� */

        audio_xml_ctrl.g_stlxmlcurrentnode.stproperty[propertyIndex].pcpropertyname = \
                        (VOS_INT8 *)PS_MEM_ALLOC(WUEPS_PID_AT, AUDIO_XML_NODE_LABEL_BUFF_LENGTH_ORIGINAL + 1);

        if (NULL == audio_xml_ctrl.g_stlxmlcurrentnode.stproperty[propertyIndex].pcpropertyname)
        {
             audio_xml_write_error_log(__LINE__, propertyIndex, AUDIO_XML_RESULT_FALIED_MALLOC);

            return AUDIO_XML_RESULT_FALIED_MALLOC;
        }

        /* ��������ֵ�ڴ�,+1Ϊ�����ַ����������� */
        audio_xml_ctrl.g_stlxmlcurrentnode.stproperty[propertyIndex].ulvaluelength = 0; /* ��ʹ�õĳ��� */


        audio_xml_ctrl.g_stlxmlcurrentnode.stproperty[propertyIndex].pcpropertyvalue = \
                   (VOS_INT8*)PS_MEM_ALLOC(WUEPS_PID_AT,AUDIO_XML_PARAM_VALUE_BUFF_LENGTH_ORIGINAL + 1);
        
        if (NULL == audio_xml_ctrl.g_stlxmlcurrentnode.stproperty[propertyIndex].pcpropertyvalue)
        {
            audio_xml_write_error_log(__LINE__, propertyIndex, AUDIO_XML_RESULT_FALIED_MALLOC);

            return AUDIO_XML_RESULT_FALIED_MALLOC;
        }
    }
    return AUDIO_XML_RESULT_SUCCEED;
}


AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_createanode(void)
{
    AUDIO_XML_RESULT_ENUM_UINT32 returnval = AUDIO_XML_RESULT_SUCCEED;

    /* ����һ�������� */
    returnval = audio_xml_createaproperty();

    if(AUDIO_XML_RESULT_SUCCEED != returnval)
    {
        return returnval;
    }

    /* ����ڵ��ǩ�ڴ�,+1Ϊ�����ַ�����������*/
    audio_xml_ctrl.g_stlxmlcurrentnode.ullabellength = 0; /* ��ʹ�õĳ��� */

    audio_xml_ctrl.g_stlxmlcurrentnode.pcnodelabel = (VOS_INT8*)PS_MEM_ALLOC( \
                               WUEPS_PID_AT,AUDIO_XML_NODE_LABEL_BUFF_LENGTH_ORIGINAL + 1);

    if (NULL ==  audio_xml_ctrl.g_stlxmlcurrentnode.pcnodelabel)
    {
        audio_xml_write_error_log(__LINE__, 0, AUDIO_XML_RESULT_FALIED_MALLOC);

        return AUDIO_XML_RESULT_FALIED_MALLOC;
    }
    return returnval;
}


AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_procxmlorginal(VOS_INT8 cnowchar)
{
    /* ����<�����״̬ */
    if ('<' == cnowchar)
    {
        g_stlaudioxmlstatus = AUDIO_XML_ANASTT_ENTER_LABLE;
        return AUDIO_XML_RESULT_SUCCEED;
    }

    /* �����ո������ */
    if (' ' == cnowchar)
    {
        return AUDIO_XML_RESULT_SUCCEED;
    }

    /* ����>,/,",=���ʾXML�﷨���� */
    if (('>' == cnowchar)
         || ('/' == cnowchar)
         || ('"' == cnowchar)
         || ('=' == cnowchar)
         || ('[' == cnowchar)
         || (']' == cnowchar)
         || ('}' == cnowchar)
         || ('{' == cnowchar))
    {
        audio_xml_write_error_log(__LINE__, 0, AUDIO_XML_RESULT_FALIED_BAD_SYNTAX);
        return AUDIO_XML_RESULT_FALIED_BAD_SYNTAX;
    }

    return AUDIO_XML_RESULT_SUCCEED;
}


AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_procxmlignore(VOS_INT8 cnowchar)
{

    /* ֱ��������ǩ��β������һֱ���� */
    if ('>' == cnowchar)
    {
        g_stlaudioxmlstatus = AUDIO_XML_ANASTT_ORIGINAL;
    }
    return AUDIO_XML_RESULT_SUCCEED;
}


AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_proc_xmlsingle_endlabel(VOS_INT8 cnowchar)
{
    AUDIO_XML_RESULT_ENUM_UINT32 returnval = 0;

    /* ����<�����״̬ */
    if ('>' == cnowchar)
    {
        /* ���״̬ */
        g_stlaudioxmlstatus = AUDIO_XML_ANASTT_ORIGINAL;

        /* ��鵱ǰ��β��ǩ����Ч�� */
        audio_xml_ctrl.g_stlxmlcurrentnode.pcnodelabel[audio_xml_ctrl.g_stlxmlcurrentnode.ullabellength] = '\0';
        returnval = audio_xml_checkheadandtaillabel();

        return returnval;
    }

    /* ����<,/,",=���ʾXML�﷨���� */
    if (('<' == cnowchar)
         || ('"' == cnowchar)
         || ('/' == cnowchar)
         || ('=' == cnowchar)
         || (']' == cnowchar)
         || ('[' == cnowchar)
         || ('}' == cnowchar)
         || ('{' == cnowchar))
    {
        audio_xml_write_error_log(__LINE__, 0, AUDIO_XML_RESULT_FALIED_BAD_SYNTAX);

        return AUDIO_XML_RESULT_FALIED_BAD_SYNTAX;
    }

    /* ������ֽڷŽ���ǰ�ڵ�ֵ�Ļ������� */
    returnval = audio_xml_write_char_to_buff(cnowchar,
                                     audio_xml_ctrl.g_stlxmlcurrentnode.pcnodelabel,
                                     &(audio_xml_ctrl.g_stlxmlcurrentnode.ullabellength),
                                     VOS_FALSE);

    return returnval;
}


static AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_checkheadandtaillabel(void)
{
    VOS_INT8 labelindex = 0;
    int ret = -1;

    for (labelindex = 0; labelindex < AUDIO_DELTA_XML_HEAD_TAIL_LABEL_NUM; labelindex++)
    {
        ret = strcmp(audio_xml_ctrl.g_stlxmlcurrentnode.pcnodelabel,
                         g_paudioxmlheadandtaillabel[labelindex]);

        /* �����ǰ��ǩ��XML�ļ��ײ���β����ǩ�������У���Ҳ��Ϊ�ǺϷ���ǩ */
        if (0 == ret)
        {
            /* <xx/>��ǩ����ʱ����սڵ���Ϣ */
            audio_xml_nodereset();

            return AUDIO_XML_RESULT_SUCCEED;
        }
    }

    audio_xml_write_error_log(__LINE__, 0, AUDIO_XML_RESULT_FALIED_BAD_CHAR);

    return AUDIO_XML_RESULT_FALIED_BAD_CHAR;
}


AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_checknode_rightlabel(void)
{
    AUDIO_XML_RESULT_ENUM_UINT32 returnval = 0;

    /* �жϸñ�ǩ�Ƿ���Ч */
    audio_xml_ctrl.g_stlxmlcurrentnode.pcnodelabel[audio_xml_ctrl.g_stlxmlcurrentnode.ullabellength] = '\0';

    /* �������NV��ǩ�������һ������Ƿ�����XML�ײ���ǩ */
    if (strcmp(audio_xml_ctrl.g_stlxmlcurrentnode.pcnodelabel,
                             g_aclnodeaudiolabelnv))
    {
        returnval = audio_xml_checkheadandtaillabel();

        return returnval;
    }
    else
    {
        /* д�ڵ���Ϣ��NV�� */
        returnval = audio_xml_get_nv_data();

        if (AUDIO_XML_RESULT_SUCCEED != returnval)
        {
            return returnval;
        }

        /* <xx/>��ǩ����ʱ����սڵ���Ϣ */
        audio_xml_nodereset();
    }

    return AUDIO_XML_RESULT_SUCCEED;
}


AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_procxmlend_mustberight(VOS_INT8 cnowchar)
{
    AUDIO_XML_RESULT_ENUM_UINT32 returnval = 0;

    /* ���Կո� */
    if (' ' == cnowchar)
    {
        return AUDIO_XML_RESULT_SUCCEED;
    }

    /* ��������>,���ʾXML�﷨���� */
    if ('>' != cnowchar)
    {
        audio_xml_write_error_log(__LINE__, 0, AUDIO_XML_RESULT_FALIED_BAD_SYNTAX);

        return AUDIO_XML_RESULT_FALIED_BAD_SYNTAX;
    }

    /* ���״̬ */
    g_stlaudioxmlstatus = AUDIO_XML_ANASTT_ORIGINAL;

    /*must be right need to check label*/
    returnval = audio_xml_checknode_rightlabel();

    return returnval;

}


AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_proc_xml_node_label(VOS_INT8 cnowchar)
{
    AUDIO_XML_RESULT_ENUM_UINT32 returnval = 0;

    /* ����/����>���߿ո�˵��Node�����ֽ����� */
    if ('/' == cnowchar)
    {
        /* ��������β�����ڵ�,�¸��ֽ�һ����> */
        g_stlaudioxmlstatus = AUDIO_XML_ANASTT_LABLE_END_MUST_RIGHT;

        return AUDIO_XML_RESULT_SUCCEED;
    }

    /* ��ǩ���� */
    if ('>' == cnowchar)
    {
        /* ���״̬ */
        g_stlaudioxmlstatus = AUDIO_XML_ANASTT_ORIGINAL;

        /*��鵱ǰ�ڵ����Ч��*/
        returnval = audio_xml_checknode_rightlabel();

        return returnval;
    }

    /* ��ǩ���ֽ���,�������Խ���״̬ */
    if (' ' == cnowchar)
    {
        /* ���״̬ */
        g_stlaudioxmlstatus = AUDIO_XML_ANASTT_PROPERTY_START;

        return AUDIO_XML_RESULT_SUCCEED;
    }

    /* ����<,",=���ʾXML�﷨���� */
    if (('<' == cnowchar)
         || ('"' == cnowchar)
         || ('=' == cnowchar)
         || ('[' == cnowchar)
         || (']' == cnowchar)
         || ('{' == cnowchar)
         || ('}' == cnowchar))
    {
        audio_xml_write_error_log(__LINE__, 0,AUDIO_XML_RESULT_FALIED_BAD_SYNTAX);

        return AUDIO_XML_RESULT_FALIED_BAD_SYNTAX;
    }

    /* ������ֽڷŽ���ǰ�ڵ�ֵ�Ļ������� */
    returnval = audio_xml_write_char_to_buff(cnowchar,
                                     audio_xml_ctrl.g_stlxmlcurrentnode.pcnodelabel,
                                     &(audio_xml_ctrl.g_stlxmlcurrentnode.ullabellength),
                                     VOS_FALSE);
    return returnval;
}


AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_proc_xml_enter_label(VOS_INT8 cnowchar)
{
    AUDIO_XML_RESULT_ENUM_UINT32 returnval = AUDIO_XML_RESULT_SUCCEED;

    /* �������а汾��Ϣ */
    if ('?' == cnowchar)
    {
        g_stlaudioxmlstatus = AUDIO_XML_ANASTT_IGNORE;

        return AUDIO_XML_RESULT_SUCCEED;
    }

    /* �������� */
    if ('!' == cnowchar)
    {
        g_stlaudioxmlstatus = AUDIO_XML_ANASTT_IGNORE;

        return AUDIO_XML_RESULT_SUCCEED;
    }

    /* ����β�ڵ��ǩ */
    if ('/' == cnowchar)
    {
        g_stlaudioxmlstatus = AUDIO_XML_ANASTT_SINGLE_ENDS_LABLE;

        return AUDIO_XML_RESULT_SUCCEED;
    }

    /* ������ǩ���� */
    if ('>' == cnowchar)
    {
        g_stlaudioxmlstatus = AUDIO_XML_ANASTT_ORIGINAL;

        /*��鵱ǰ�ڵ����Ч��*/
        returnval = audio_xml_checknode_rightlabel();

        return returnval;
    }

    /* ����<,",=���ʾXML�﷨���� */
    if (('<' == cnowchar)
         || ('"' == cnowchar)
         || ('=' == cnowchar)
         || ('[' == cnowchar)
         || (']' == cnowchar)
         || ('{' == cnowchar)
         || ('}' == cnowchar))
    {
        audio_xml_write_error_log(__LINE__, 0, AUDIO_XML_RESULT_FALIED_BAD_SYNTAX);
        return AUDIO_XML_RESULT_FALIED_BAD_SYNTAX;
    }

    /* �����ո� */
    if (' ' != cnowchar)
    {
        /* ���״̬����ʾ����һ���½ڵ� */
        g_stlaudioxmlstatus = AUDIO_XML_ANASTT_NODE_LABLE;

        /* ������ֽڷŽ���ǰ�ڵ�ֵ�Ļ������� */
        returnval = audio_xml_write_char_to_buff(cnowchar,
                                         audio_xml_ctrl.g_stlxmlcurrentnode.pcnodelabel,
                                         &(audio_xml_ctrl.g_stlxmlcurrentnode.ullabellength),
                                         VOS_FALSE);
    }

    return returnval;
}


AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_proc_xml_propertystart(VOS_INT8 cnowchar)
{
    AUDIO_XML_RESULT_ENUM_UINT32 returnval = AUDIO_XML_RESULT_SUCCEED;
    VOS_UINT32 ulPropertyIndex = 0;


    /* ����β�ڵ��ǩ */
    if ('/' == cnowchar)
    {
        /* ���״̬ */
        g_stlaudioxmlstatus = AUDIO_XML_ANASTT_LABLE_END_MUST_RIGHT;

        return AUDIO_XML_RESULT_SUCCEED;
    }

    /* ��ǩ���� */
    if ('>' == cnowchar)
    {
        /* ���״̬ */
        g_stlaudioxmlstatus = AUDIO_XML_ANASTT_ORIGINAL;

        /*��鵱ǰ�ڵ����Ч��*/
        returnval = audio_xml_checknode_rightlabel();

        return returnval;
    }

    /* ����<,",=���ʾXML�﷨���� */
    if (('<' == cnowchar)
         || ('"' == cnowchar)
         || ('=' == cnowchar)
         || ('[' == cnowchar)
         || (']' == cnowchar)
         || ('{' == cnowchar)
         || ('}' == cnowchar))
    {
        audio_xml_write_error_log(__LINE__, 0, AUDIO_XML_RESULT_FALIED_BAD_SYNTAX);

        return AUDIO_XML_RESULT_FALIED_BAD_SYNTAX;
    }

    /* ����״̬ */
    g_stlaudioxmlstatus = AUDIO_XML_ANASTT_PROPERTY_NAME_START;

    ulPropertyIndex = audio_xml_ctrl.g_stlxmlcurrentnode.ulPropertyIndex;

    returnval = audio_xml_write_char_to_buff(cnowchar,
                  audio_xml_ctrl.g_stlxmlcurrentnode.stproperty[ulPropertyIndex].pcpropertyname,
                  &(audio_xml_ctrl.g_stlxmlcurrentnode.stproperty[ulPropertyIndex].ulnamelength),
                  VOS_FALSE);

    return returnval;
}


AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_proc_xml_propertyname(VOS_INT8 cnowchar)
{
    AUDIO_XML_RESULT_ENUM_UINT32 returnval = AUDIO_XML_RESULT_SUCCEED;
    VOS_UINT32 ulPropertyIndex = 0;

    /* �ȴ�=��������ֵ���� */
    if ('=' == cnowchar)
    {
        /* ��״̬ */
        g_stlaudioxmlstatus = AUDIO_XML_ANASTT_PROPERTY_NAME_END;

        return AUDIO_XML_RESULT_SUCCEED;
    }

    /* ����<,>,/,"���ʾXML�﷨���� */
    if (('<' == cnowchar)
        || ('>' == cnowchar)
        || ('/' == cnowchar)
        || ('"' == cnowchar)
        || (']' == cnowchar)
        || ('[' == cnowchar)
        || ('}' == cnowchar)
        || ('{' == cnowchar))
    {
        audio_xml_write_error_log(__LINE__, 0, AUDIO_XML_RESULT_FALIED_BAD_SYNTAX);

        return AUDIO_XML_RESULT_FALIED_BAD_SYNTAX;
    }

    ulPropertyIndex = audio_xml_ctrl.g_stlxmlcurrentnode.ulPropertyIndex;

    /* �����������еĿո����, �� <nv i d="123"> */
    returnval = audio_xml_write_char_to_buff(cnowchar,
                  audio_xml_ctrl.g_stlxmlcurrentnode.stproperty[ulPropertyIndex].pcpropertyname,
                  &(audio_xml_ctrl.g_stlxmlcurrentnode.stproperty[ulPropertyIndex].ulnamelength),
                  VOS_FALSE);

    return returnval;
}


AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_proc_xml_propertyname_tail(VOS_INT8 cnowchar)
{

    /* �����ո� */
    if ( ' ' == cnowchar)
    {
        return AUDIO_XML_RESULT_SUCCEED;
    }

    /* �ȴ�" */
    if ('"' == cnowchar)
    {
        /* ����״̬ */
        g_stlaudioxmlstatus = AUDIO_XML_ANASTT_PROPERTY_VALUE_START;

        return AUDIO_XML_RESULT_SUCCEED;
    }

    /* ��������"�����ʾXML�﷨���� */
    audio_xml_write_error_log(__LINE__, 0, AUDIO_XML_RESULT_FALIED_BAD_SYNTAX);

    return AUDIO_XML_RESULT_FALIED_BAD_SYNTAX;
}


AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_proc_xml_valuestart(VOS_INT8 cnowchar)
{
    AUDIO_XML_RESULT_ENUM_UINT32 returnval = AUDIO_XML_RESULT_SUCCEED;
    VOS_UINT32 ulPropertyIndex = 0;

    /* ����" */
    if ('"' == cnowchar)
    {
        /* ��״̬,���ؿ�ʼ�������Ե�״̬ */
        g_stlaudioxmlstatus = AUDIO_XML_ANASTT_PROPERTY_VALUE_END;

        return AUDIO_XML_RESULT_SUCCEED;
    }

    /* ����<,>,/,=���ʾXML�﷨���� */
    if (('<' == cnowchar)
         || ('>' == cnowchar)
         || ('/' == cnowchar)
         || ('=' == cnowchar))
    {
        audio_xml_write_error_log(__LINE__, 0, AUDIO_XML_RESULT_FALIED_BAD_SYNTAX);

        return AUDIO_XML_RESULT_FALIED_BAD_SYNTAX;
    }

    ulPropertyIndex = audio_xml_ctrl.g_stlxmlcurrentnode.ulPropertyIndex;

    /* �ѵ�ǰ�ַ��ӵ�����ֵ�� */
    returnval = audio_xml_write_char_to_buff(cnowchar,
                  audio_xml_ctrl.g_stlxmlcurrentnode.stproperty[ulPropertyIndex].pcpropertyvalue,
                  &(audio_xml_ctrl.g_stlxmlcurrentnode.stproperty[ulPropertyIndex].ulvaluelength),
                  VOS_TRUE);

    return returnval;
}


AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_proc_xml_valuetail(VOS_INT8 cnowchar)
{
    AUDIO_XML_RESULT_ENUM_UINT32 returnval = 0;

    /* �ո���һ�����Խ�����ʼ */
    if (' ' == cnowchar)
    {
        /* ���״̬,���ؿ�ʼ�������Ե�״̬ */
        g_stlaudioxmlstatus = AUDIO_XML_ANASTT_PROPERTY_START;
        audio_xml_ctrl.g_stlxmlcurrentnode.ulPropertyIndex++;
        return AUDIO_XML_RESULT_SUCCEED;
    }

    /* ����'/' */
    if ('/' == cnowchar)
    {
        /* ���״̬ */
        g_stlaudioxmlstatus = AUDIO_XML_ANASTT_LABLE_END_MUST_RIGHT;

        return AUDIO_XML_RESULT_SUCCEED;
    }

    /* ����'>' */
    if ('>' == cnowchar)
    {
        /* ���״̬,���ؿ�ʼ�������Ե�״̬ */
        g_stlaudioxmlstatus = AUDIO_XML_ANASTT_ORIGINAL;

        /* ��鵱ǰ�ڵ���Ч */
        returnval = audio_xml_checknode_rightlabel();

        return returnval;
    }

    /* ��������>,/���ʾXML�﷨���� */
    audio_xml_write_error_log(__LINE__, 0, AUDIO_XML_RESULT_FALIED_BAD_SYNTAX);

    return AUDIO_XML_RESULT_FALIED_BAD_SYNTAX;
}


AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_analyse(VOS_INT8 cnowchar)
{
    AUDIO_XML_RESULT_ENUM_UINT32 returnval = 0;

    /* ��鵱ǰ�ַ�����Ч�� */
    returnval = audio_xml_checkcharvalidity(cnowchar);

    if (AUDIO_XML_RESULT_SUCCEED_IGNORE_CHAR == returnval)
    {
        /* ����������ԣ����������ַ� */
        return AUDIO_XML_RESULT_SUCCEED;
    }

    if (AUDIO_XML_RESULT_FALIED_BAD_CHAR == returnval)
    {
        /* ��������Ƿ��ַ�����ֹͣ���� */
        return AUDIO_XML_RESULT_FALIED_BAD_CHAR;
    }
    
    /* ����XML����ʱ����Ӧ״̬�Ķ�Ӧ���� */
    returnval = g_uslaudioxmlanalysefuntbl[g_stlaudioxmlstatus](cnowchar);

    return returnval;
}


AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_decode_xml_file(VOS_UCHAR* xmldir)
{
    VOS_INT32                     lreaded   = 0;       /* �������ֽ��� */
    VOS_INT32                     lcount    = 0;       /* ������������ */
    VOS_INT16                     fd        = 0;
    AUDIO_XML_RESULT_ENUM_UINT32  returnval = 0;

    printk(KERN_ERR " xml_decode_xml_file: xmldir = %s!\n",xmldir);

    /*���ļ�·��*/
    fd = (unsigned int)sys_open(xmldir, O_RDONLY, 0);

    if(fd < 0)
    {
        audio_xml_write_error_log(__LINE__, 0, AUDIO_XML_RESULT_FALIED_READ_FILE);
        return AUDIO_XML_RESULT_FALIED_READ_FILE;
    }

    /*��ȡ�ļ�*/
    lreaded = (int)sys_read(fd,(VOS_UINT8*)(audio_xml_ctrl.g_pclfilereadbuff),AUDIO_XML_FILE_READ_BUFF_SIZE);

    /* ��Ƶ����delta�ļ�Ϊ�յ��쳣���� */
    if (0 == lreaded)
    {
        return AUDIO_XML_RESULT_SUCCEED;
    }

    if ( (lreaded > AUDIO_XML_FILE_READ_BUFF_SIZE) || (lreaded < 0) )
    {
        audio_xml_write_error_log(__LINE__, 0, AUDIO_XML_RESULT_FALIED_READ_FILE);
        return AUDIO_XML_RESULT_FALIED_READ_FILE;
    }

    for(lcount = 0;lcount < lreaded; lcount++)
    {
        returnval = audio_xml_analyse(*(audio_xml_ctrl.g_pclfilereadbuff + lcount));

        if(AUDIO_XML_RESULT_SUCCEED != returnval)
        {
            /* ��������������ֹͣ���� */
            return returnval;
        }
    }

    return AUDIO_XML_RESULT_SUCCEED;
}


AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_procinit(void)
{
    AUDIO_XML_RESULT_ENUM_UINT32 returnval = AUDIO_XML_RESULT_SUCCEED;

    /* �����ڵ���Ϣ */
    returnval = audio_xml_createanode();

    if (AUDIO_XML_RESULT_SUCCEED != returnval)
    {
        return returnval;
    }

    /* ��ʼ����ǰ״̬ */
    g_stlaudioxmlstatus = AUDIO_XML_ANASTT_ORIGINAL;

    /* ���ڼ�¼��ȡXML�ļ������� */
    audio_xml_ctrl.g_stlxml_lineno    = 1;

    /* �����ȡ�ļ����ݵĻ�����,+1Ϊ�����ַ����������� */ 
    audio_xml_ctrl.g_pclfilereadbuff = (VOS_INT8*)PS_MEM_ALLOC( \
                               WUEPS_PID_AT,AUDIO_XML_FILE_READ_BUFF_SIZE + 1);
    
    if (NULL == audio_xml_ctrl.g_pclfilereadbuff)
    {      
        audio_xml_write_error_log(__LINE__, 0, AUDIO_XML_RESULT_FALIED_MALLOC);
        return AUDIO_XML_RESULT_FALIED_MALLOC;
    }
    
    return AUDIO_XML_RESULT_SUCCEED;
}


void audio_xml_freemem(void)
{
    VOS_UINT32 ulPropertyIndex = 0;

    for(ulPropertyIndex = 0; ulPropertyIndex < AUDIO_XML_NODE_PROPERTY_NUM; ulPropertyIndex++)
    {
        PS_MEM_FREE(WUEPS_PID_AT,audio_xml_ctrl.g_stlxmlcurrentnode.stproperty[ulPropertyIndex].pcpropertyname);

        PS_MEM_FREE(WUEPS_PID_AT,audio_xml_ctrl.g_stlxmlcurrentnode.stproperty[ulPropertyIndex].pcpropertyvalue);
    }
    PS_MEM_FREE(WUEPS_PID_AT,audio_xml_ctrl.g_stlxmlcurrentnode.pcnodelabel);

    PS_MEM_FREE(WUEPS_PID_AT,audio_xml_ctrl.g_pclfilereadbuff);
}


void audio_xml_decodefail_freemem(void)
{
    VOS_UINT32 ulPropertyIndex = 0;

    for(ulPropertyIndex = 0; ulPropertyIndex < AUDIO_XML_NODE_PROPERTY_NUM; ulPropertyIndex++)
    {
        if(NULL != audio_xml_ctrl.g_stlxmlcurrentnode.stproperty[ulPropertyIndex].pcpropertyname)
        {
            PS_MEM_FREE(WUEPS_PID_AT,audio_xml_ctrl.g_stlxmlcurrentnode.stproperty[ulPropertyIndex].pcpropertyname);
            audio_xml_ctrl.g_stlxmlcurrentnode.stproperty[ulPropertyIndex].pcpropertyname = NULL;
        }
    }

    if(NULL != audio_xml_ctrl.g_stlxmlcurrentnode.pcnodelabel)
    {
        PS_MEM_FREE(WUEPS_PID_AT,audio_xml_ctrl.g_stlxmlcurrentnode.pcnodelabel);
        audio_xml_ctrl.g_stlxmlcurrentnode.pcnodelabel = NULL;
    }

    if(NULL != audio_xml_ctrl.g_pclfilereadbuff)
    {
        PS_MEM_FREE(WUEPS_PID_AT,audio_xml_ctrl.g_pclfilereadbuff);
        audio_xml_ctrl.g_pclfilereadbuff = NULL;
    }

}


AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_decode_main(VOS_UCHAR* ulFiledir)
{
    AUDIO_XML_RESULT_ENUM_UINT32 returnval = AUDIO_XML_RESULT_SUCCEED;

    /*��ʼ��ȫ�ֱ���*/
    returnval = audio_xml_procinit();

    if (AUDIO_XML_RESULT_SUCCEED != returnval)
    {
        goto out;
    }

    /* ����xnv.xml�ļ�  */
    returnval = audio_xml_decode_xml_file(ulFiledir);

    if (AUDIO_XML_RESULT_SUCCEED != returnval)
    {
        goto out;
    }

    /*д��Ч��ص�NV*/
    returnval = audio_xml_write_nv();

    if (AUDIO_XML_RESULT_SUCCEED != returnval)
    {
        goto out;
    }

    /*����¼NV������ȫ�ֱ�����Ϊ0*/
    g_nv_number = 0;
    
    /* �ͷ��ѷ�����ڴ� */
    audio_xml_freemem();
 
    return AUDIO_XML_RESULT_SUCCEED;
out:
    audio_xml_help();
    audio_xml_decodefail_freemem();
    return returnval;
}


AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_nv_search_byid(VOS_UINT16 itemid)
{
    VOS_UINT16 lcount = 0;

    for (lcount = 0; lcount < ( sizeof(g_stlaudioxmnvIdtbl) / sizeof(g_stlaudioxmnvIdtbl[0]) ); lcount++)
    {
        if (itemid == g_stlaudioxmnvIdtbl[lcount])
        {
            g_audio_delta_nv[g_nv_number].arraylocation = lcount;
            return AUDIO_XML_RESULT_SUCCEED;
        }
    }
    
    printk(KERN_ERR "audio_xml_nv_search_byid: err char :%d\n",itemid);
    return AUDIO_XML_RESULT_FALIED_NV_ID_IS_NULL;
}


AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_write_nv(void)
{
    VOS_UINT16            lcount = 0;

    for (lcount = 0; lcount < g_nv_number; lcount++)
    {
        if (NV_OK != NV_WriteEx(MODEM_ID_0,g_stlaudioxmnvIdtbl[g_audio_delta_nv[lcount].arraylocation], \
                   g_audio_delta_nv[lcount].AudioNvValue.ashwNv,AUDIO_DELTA_XML_NV_VALUE_NUM*sizeof(VOS_INT16)))
        {
            printk(KERN_ERR "audio_xml_write_nv():ERROR: the nv id is %d\n",g_audio_delta_nv[lcount].nv_id);
            return AUDIO_XML_RESULT_WRITE_NV_DATA_FAIL;
        }
    }

    return AUDIO_XML_RESULT_SUCCEED;
}


void audio_xml_help(void)
{
    printk(KERN_ERR "audio_xml_ctrl.g_stlxmlerrorinfo.ulxmlline  =  %d\n",audio_xml_ctrl.g_stlxmlerrorinfo.ulxmlline);
    printk(KERN_ERR "audio_xml_ctrl.g_stlxmlerrorinfo.ulstatus   =  %d\n",audio_xml_ctrl.g_stlxmlerrorinfo.ulstatus);
    printk(KERN_ERR "audio_xml_ctrl.g_stlxmlerrorinfo.ulcodeline =  %d\n",audio_xml_ctrl.g_stlxmlerrorinfo.ulcodeline);
    printk(KERN_ERR "audio_xml_ctrl.g_stlxmlerrorinfo.usnvid     =  %d\n",audio_xml_ctrl.g_stlxmlerrorinfo.usnvid);
    printk(KERN_ERR "audio_xml_ctrl.g_stlxmlerrorinfo.ulresult   =  %d\n",audio_xml_ctrl.g_stlxmlerrorinfo.ulresult);
    printk(KERN_ERR "\n");
    printk(KERN_ERR "g_stlaudioxmlstatus                         = %d\n",g_stlaudioxmlstatus);
    printk(KERN_ERR "audio_xml_ctrl.g_stlxml_lineno              = %d\n",audio_xml_ctrl.g_stlxml_lineno);
}

EXPORT_SYMBOL(audio_xml_analyse);
EXPORT_SYMBOL(audio_xml_help);
EXPORT_SYMBOL(audio_xml_write_error_log);
EXPORT_SYMBOL(audio_xml_procinit);
#endif /* FEATURE_ON == MBB_WPG_PCM */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

