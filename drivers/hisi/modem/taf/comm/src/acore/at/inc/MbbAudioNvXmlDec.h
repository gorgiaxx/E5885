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

#ifndef _NV_XML_DEC_H_
#define _NV_XML_DEC_H_

/*****************************************************************************
1 ����ͷ�ļ�����
*****************************************************************************/
#include "v_typdef.h"
#include "CodecNvInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif
#pragma pack(push)
#pragma pack(4) //4�ֽڶ���

/*****************************************************************************
2 �궨��
*****************************************************************************/
#if(FEATURE_ON == MBB_WPG_PCM)
#define AUDIO_XML_FILE_READ_BUFF_SIZE              (4096)  /* ���ļ���������С */
#define AUDIO_XML_NODE_LABEL_BUFF_LENGTH_ORIGINAL  (128)   /* �ڵ��ǩ��������С*/
//#define  AUDIO_XML_NODE_NV_ITEM_LENGTH_ORIGINAL     (128)   /* �ڵ��ǩ��������С*/
#define AUDIO_XML_PARAM_VALUE_BUFF_LENGTH_ORIGINAL (2048)  /* �ڵ�ֵ��������С*/ 

#define AUDIO_CTRL_SAMPLE_RATE_NUM                 (2)     /* ֧�ֵĲ����ʸ��� 8k��16k */
#define AUDIO_XML_NODE_PROPERTY_NUM                (4)     /*NV���Եĸ���*/
#define AUDIO_CTRL_MODE_SUB_ID_NUM                 (4)     /* ͬһ��ģʽ�µ�ID���� */
#define AUDIO_DELTA_XML_NV_VALUE_NUM               (150)   /*ÿ����ЧNV��Ӧ��ֵ����*/
#define AUDIO_DELTA_XML_NV_VALUE_LENGTH            (10)    /*ÿ��NVֵ��Ӧ���ַ�������*/

#define AUDIO_DELTA_XML_HEAD_TAIL_LABEL_NUM        (2)     /* XML�ļ��ײ���β����ǩ���� */

/*****************************************************************************
3 ö�ٶ���
*****************************************************************************/

/*****************************************************************************
ö����    : XML_RESULT_ENUM_UINT32
Э����  :
ASN.1���� :
ö��˵��  : XML����������״̬����ֵ
*****************************************************************************/
enum AUDIO_XML_RESULT_ENUM
{
    AUDIO_XML_RESULT_SUCCEED                        = 0 , /* �ɹ�                      */
    AUDIO_XML_RESULT_SUCCEED_IGNORE_CHAR                , /* ���Ժ��Ե��ַ�\r\n\t      */
    AUDIO_XML_RESULT_FALIED_PARA_POINTER                , /* ����Ĳ���ָ��            */
    AUDIO_XML_RESULT_FALIED_MALLOC                      , /* �ڴ�����ʧ��*/
    AUDIO_XML_RESULT_FALIED_BAD_SYNTAX                  , /* �����XML�﷨*/
    AUDIO_XML_RESULT_FALIED_BAD_CHAR                    , /* ������ַ�*/
    AUDIO_XML_RESULT_FALIED_READ_FILE                   , /* ��ȡ�ļ�ʧ�� */
    AUDIO_XML_RESULT_FALIED_OUT_OF_BUFF_LEN             , /* ��������������*/
    AUDIO_XML_RESULT_FALIED_OUT_OF_0_9                  , /* NV IDֵ����0-9  */
    AUDIO_XML_RESULT_FALIED_OUT_OF_0_F                  , /* NV Valueֵ����0-F */
    AUDIO_XML_RESULT_FALIED_OUT_OF_2_CHAR               , /* NV Valueֵ����1Byte  */
    AUDIO_XML_RESULT_FALIED_NV_ID_IS_NULL               , /* NV IDֵΪ��*/   
    AUDIO_XML_RESULT_FALIED_NV_ID_IS_ERROR              , /* NV IDֵΪ���*/
    AUDIO_XML_RESULT_FALIED_NV_VALUE_IS_NULL            , /* NV ValueֵΪ��*/
    AUDIO_XML_RESULT_FALIED_NODE_MATCH                  , /*����NV NODE*/
    AUDIO_XML_RESULT_GET_NV_DATA_FAIL                   , /*��ȡNVֵʧ��*/
    AUDIO_XML_RESULT_WRITE_NV_DATA_FAIL                 , /*д��NVֵʧ��*/
    AUDIO_XML_RESULT_BUTT
};
typedef VOS_UINT32 AUDIO_XML_RESULT_ENUM_UINT32;
 
/*****************************************************************************
ö����    : XML_RESULT_ENUM_UINT32
Э����  :
ASN.1���� :
ö��˵��  : XML������״̬
*****************************************************************************/
enum AUDIO_XML_ANALYSE_STATUS_ENUM
{
    AUDIO_XML_ANASTT_ORIGINAL                       = 0 , /* ��ʼ                 */
    AUDIO_XML_ANASTT_ENTER_LABLE                        , /* �����ǩ             */
    AUDIO_XML_ANASTT_IGNORE                             , /* ���Ի�ע��           */
    AUDIO_XML_ANASTT_NODE_LABLE                         , /* ������ǩ��           */
    AUDIO_XML_ANASTT_SINGLE_ENDS_LABLE                  , /* ������β��ǩ         */
    AUDIO_XML_ANASTT_LABLE_END_MUST_RIGHT               , /* ��ǩ��ʼ����β       */
    AUDIO_XML_ANASTT_PROPERTY_START                     , /* ��ʼ��������         */
    AUDIO_XML_ANASTT_PROPERTY_NAME_START                , /* ��ʼ������������     */
    AUDIO_XML_ANASTT_PROPERTY_NAME_END                  , /* �������ƽ������ȴ�"  */
    AUDIO_XML_ANASTT_PROPERTY_VALUE_START               , /* "�������ȴ�="        */
    AUDIO_XML_ANASTT_PROPERTY_VALUE_END                 , /* ����ֵ�������ȴ�>    */
    AUDIO_XML_ANASTT_BUTT
};
typedef VOS_UINT32 AUDIO_XML_ANALYSE_STATUS_ENUM_UINT32;
typedef VOS_UINT32 (*AUDIO_XML_FUN)(s8 cnowchar);

/*****************************************************************************
4 UNION����
*****************************************************************************/

/*****************************************************************************
5 STRUCT����
*****************************************************************************/

/*****************************************************************************
�ṹ��    : XML_NODE_PROPERTY
Э����  :
ASN.1���� :
�ṹ˵��  : �ڵ���������ĵ�Ԫ
*****************************************************************************/
typedef struct
{
    VOS_UINT32 ulnamelength;                  /* pcPropertyName ����������  */
    VOS_UINT32 ulvaluelength;                 /* pcPropertyValue ���������� */
    VOS_INT8* pcpropertyname;                 /* ��������                   */
    VOS_INT8* pcpropertyvalue;                /* ����ֵ                     */
}AUDIO_XML_NODE_PROPERTY_STRU;

/*****************************************************************************
�ṹ��    : XML_NODE_STRU
Э����  :
ASN.1���� :
�ṹ˵��  : XML���Ľڵ�
*****************************************************************************/
typedef struct
{
    VOS_UINT32 ullabellength;                 /* pcNodeLabel ����������*/
    VOS_INT8*  pcnodelabel;                   /* �ڵ��ǩ*/
    VOS_UINT32 ulPropertyIndex;               /*���Խڵ�����,��¼stproperty���±�ֵ*/
    AUDIO_XML_NODE_PROPERTY_STRU stproperty[AUDIO_XML_NODE_PROPERTY_NUM]; /* ���� */
}AUDIO_XML_NODE_STRU;

/*****************************************************************************
�ṹ��    : XML_ERROR_INFO_STRU
Э����  :
ASN.1���� :
�ṹ˵��  : ���ڼ�¼XML������Ϣ
*****************************************************************************/
typedef struct
{
    VOS_UINT32 ulxmlline;      /* XML��Ӧ���к�      */
    VOS_UINT32 ulstatus;       /* XML��Ӧ�Ľ���״̬  */
    VOS_UINT32 ulcodeline;     /* ��������Ӧ���к� */
    VOS_UINT16 usnvid;         /* NV ID��ֵ          */
    VOS_UINT16 usreserve;      /* NV ID��ֵ          */
    VOS_UINT16 ulresult;       /* ���صĽ��  */
} AUDIO_XML_ERROR_INFO_STRU;

typedef struct
{
    VOS_UINT16            arraylocation;    /*NV ID��NV�����е�λ��*/
    VOS_UINT16            nv_id;            /*nv id*/
    ANFU_ECNR_NV_STRU     AudioNvValue;     /*nv value*/
}AUDIO_DELAT_XML_NV_INFO_STRU;

typedef struct
{
    AUDIO_XML_NODE_STRU g_stlxmlcurrentnode;            /*node�ṹ��*/
    VOS_INT8*  g_pclfilereadbuff;                       /*��ȡ�ļ�ʱ����buffer*/
    VOS_UINT32 g_stlxml_lineno;                         /*��¼xml�ļ��к�*/
    AUDIO_XML_ERROR_INFO_STRU    g_stlxmlerrorinfo;     /*�������ͽṹ��*/
}AUDIO_XML_DOCODE_INFO;
/*****************************************************************************
6 ��Ϣͷ����
*****************************************************************************/


/*****************************************************************************
7 ��Ϣ����
*****************************************************************************/


/*****************************************************************************
8 ȫ�ֱ�������
*****************************************************************************/


/*****************************************************************************
9 OTHERS����
*****************************************************************************/


/*****************************************************************************
10 ��������
*****************************************************************************/
AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_decode_main(VOS_UCHAR* ulFiledir);
AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_decode_xml_file(VOS_UCHAR* xmldir);
AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_procinit();
AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_analyse(VOS_INT8 cnowchar);

AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_procxmlorginal(VOS_INT8 cnowchar);
AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_proc_xml_enter_label(VOS_INT8 cnowchar);
AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_procxmlignore(VOS_INT8 cnowchar);
AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_proc_xml_node_label(VOS_INT8 cnowchar);
AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_proc_xmlsingle_endlabel(VOS_INT8 cnowchar);
AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_procxmlend_mustberight(VOS_INT8 cnowchar);
AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_proc_xml_propertystart(VOS_INT8 cnowchar);
AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_proc_xml_propertyname(VOS_INT8 cnowchar);
AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_proc_xml_propertyname_tail(VOS_INT8 cnowchar);
AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_proc_xml_valuestart(VOS_INT8 cnowchar);
AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_proc_xml_valuetail(VOS_INT8 cnowchar);
AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_nv_search_byid(VOS_UINT16 itemid);
AUDIO_XML_RESULT_ENUM_UINT32 audio_xml_write_nv(void);
void audio_xml_write_error_log(VOS_UINT32 ulerrorline, VOS_UINT16 ulnvid, VOS_UINT32 ret);
void audio_xml_help(void);
#endif /* FEATURE_ON == MBB_WPG_PCM */

#pragma pack(pop)
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* _NV_XML_DEC_H_ */
