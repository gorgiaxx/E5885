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
#include <linux/syscalls.h>
#include <linux/statfs.h>
#include "AtTestParaCmd.h"
#include "ATCmdProc.h"

#include "product_nv_id.h"
#include "product_nv_def.h"
#include "bsp_version.h"
#include "mdrv_version.h"
#include "mdrv_chg.h"
#include "mbb_process_start.h"


#include "LPsNvInterface.h"

#define PRIVATE_SYMBOL static

#include "MbbAtDrv.h"
#include "AtCmdFtmProc.h"

#include "mdrv_rfile_common.h"
#include "dms_core.h"



#include <linux/mlog_lib.h>



/*lint -e767 -e960 �޸���:�޽� 107747;������:���ٻ�65952;ԭ��:Log��ӡ*/
#define    THIS_FILE_ID        PS_FILE_ID_AT_EXTENDPRIVATECMD_C
/*lint +e767 +e960 �޸���:�޽� 107747;������:sunshaohua*/



/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/


#define AT_RSFR_RSFW_DATA_LEN   AT_MAX_CMD_LEN_FOR_M2M
#define OFFLINE_LOG_REF_0              201
#define OFFLINE_LOG_REF_1              202
#define UPDATE_LOG_REF_0              301
#define UPDATE_LOG_REF_1              302
#define SDT_FILE_PATH                  "/data/log/modemConfig/hids_ue_msg.cfg"
#define DUMP_FILE_PATH                 "/modem_log/Exc/"
#define OFFLINE_LOG_CNAME_DUMP          1  //dump
#define OFFLINE_LOG_CNAME_UPDATE      2 //flash log
#define OFFLINE_LOG_REF_BASE       203

#define OM_SD_LOG_PATH_MAX_LENGTH               (128)
#define OM_VENDOR_LOG_MAX_SIZE         (128 * 1024)

#define OM_FLASH_LOG_ROOT_PATH         "/online/log/balonglte"
#define OM_NFS_LOG_ROOT_PATH         "/mnt/nfs/log/balonglte"

#define OM_SD_LOG_SET_INFO_FILE         "/online/log/offlinelog.conf"


VOS_UINT32 g_EntryFlag = 0;
VOS_UINT32 g_usLength = 0;
VOS_UINT32 g_onlyonce = 0;
VOS_UINT32 g_usSeekLength = 0;
VOS_INT32 g_usCurrentSwitch = -1;
VOS_UINT32 g_packagenum = 0;
VOS_UINT32 g_packageitem = 0;
VOS_UINT32 g_SdataPackageCount = 0;
OM_FS_READ_CTRL_INFO                g_stOmFSReadCtrlInfo;
OM_FLASH_INFO_STRU              g_stFSFileInfo;
/*MBB����������log������Ϣ*/
OM_SD_LOG_SET_INFO                g_stOmSDLogSetInfo;

/*�ļ�������*/
#define OM_SD_FILE_MODE                         (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

VOS_UINT32 OM_FSReadData(
                  VOS_UINT8 *pucData, 
                  VOS_UINT32 *pulLen, 
                  VOS_UINT32 *FsTotalPack, 
                  VOS_UINT32 *FsCurPack,
                  VOS_UINT32 FileIndex,
                  VOS_UINT32 ucSubName);
VOS_UINT32 OM_FSgetLogFilePath(VOS_UINT32 ulIndex,VOS_CHAR *Path);
VOS_UINT32 OM_GetVendorLogFilePath
(
    VOS_UINT32 ulIndex,
    VOS_CHAR *Path, 
    VOS_UINT32 ucSubName
);

VOS_CHAR g_dumpfiname[2][256] = {"\0","\0"};   /* ����dump��·������ */



/*****************************************************************************
  3 ��������
*****************************************************************************/
/*lint -e64*/
/*****************************************************************************
 �� �� ��  : At_TestMdonPara
 ��������  : ^MDON��������
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
*****************************************************************************/
VOS_UINT32 At_TestMdonPara(VOS_UINT8 ucIndex)
{
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                (VOS_CHAR *)pgucAtSndCodeAddr,
                                (VOS_CHAR *)pgucAtSndCodeAddr,
                                "%s:(0,1,5)",
                                g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    return AT_OK;
}
/*lint +e64*/


VOS_UINT32 At_TestMdslPara(VOS_UINT8 ucIndex)
{
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                (VOS_CHAR *)pgucAtSndCodeAddr,
                                (VOS_CHAR *)pgucAtSndCodeAddr,
                                "%s:(0,1)",
                                g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    return AT_OK;
}


VOS_UINT32 At_TestMdsfPara(VOS_UINT8 ucIndex)
{
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                (VOS_CHAR *)pgucAtSndCodeAddr,
                                (VOS_CHAR *)pgucAtSndCodeAddr,
                                "%s:(0,1)",
                                g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    return AT_OK;
}
/*lint +e64*/

/*****************************************************************************
 �� �� ��  :  AT_TestSetMbimMode
 ��������  : ^SETMODE����Ĳ�������
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :
*****************************************************************************/
VOS_UINT32 AT_TestSetMbimMode(VOS_UINT8 ucIndex)
{
      /* ͨ����� */
    if (VOS_FALSE == AT_CheckHsicUser(ucIndex))
    {
        return AT_ERROR;
    }

    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       "%s: (0,1)",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
    return AT_OK;
}









#define AT_SPEED_NAME_LEN              (16)

VOS_UINT32 At_QrySpeed(VOS_UINT8 ucIndex)
{
    VOS_UINT32          ret;
    VOS_UCHAR            usbspeed[AT_SPEED_NAME_LEN]={'\0'};
    ret = DRV_GET_USB_SPEED(usbspeed);
    printk(KERN_ERR"%s\n",usbspeed);
    if(ret)
    {
        mlog_print(MLOG_USB_AT, mlog_lv_factory, "%s : fail to get usb speed.\n", __func__);

        return AT_ERROR;
    }
    gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                            (TAF_CHAR *)pgucAtSndCodeAddr,
                                            (TAF_CHAR *)pgucAtSndCodeAddr,
                                            "^USBSPEED: %s", usbspeed);
    return AT_OK;
}





/*lint -e64 -e119*/
/*****************************************************************************
 �� �� ��  : AT_QryMdonPara
 ��������  : (AT^MDON)��ѯ��ǰ����LOG��״̬
 �������  : ucIndex - �û�����
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
*****************************************************************************/
VOS_UINT32 AT_QryMdonPara(VOS_UINT8 ucIndex)
{
    VOS_INT32 ulRet = 0;
    VOS_UINT32 ustate = 0; /*��ǰ״̬*/
    VOS_UINT8  Flag = 0;
    VOS_UINT8 ucmlogflag = 0; /*mlog��ʶ*/
    ulRet = mlog_state_get(&ucmlogflag);
    if(VOS_OK != ulRet)
    {
        return AT_ERROR;
    }
    /*�ж�֧�����*/
    if(0 == ucmlogflag)
    {
        /*���ܹر�*/
        ustate = 0;
    }
    else if(1 == ucmlogflag)
    {
        /*mlog���ܿ���*/
        ustate = 5;
    }
    else
    {
        return AT_ERROR;
    }
    ulRet = OM_SDGetStorageOrFlipFlag(2, &Flag);  /* ��ȡ����log���� */
    if (VOS_OK != ulRet)
    {
        return AT_ERROR; 
    }
    if (1 == Flag)
    {
        ustate = 1;
    }

    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                            (VOS_CHAR *)pgucAtSndCodeAddr,
                                            (VOS_CHAR*)pgucAtSndCodeAddr,
                                            "%s:%d", g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                            ustate);

    return AT_OK;
}
/*lint +e64 +e119*/



VOS_UINT32 AT_QryMdslPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32  ulRet = 0;
    VOS_UINT8  Flag = 0;

    ulRet = OM_SDGetStorageOrFlipFlag(0, &Flag);
    if (VOS_OK != ulRet)
    {
        return AT_ERROR; 
    }

    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                            (VOS_CHAR *)pgucAtSndCodeAddr,
                                            (VOS_CHAR*)pgucAtSndCodeAddr,
                                            "%s:%d", g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                            Flag);

    return AT_OK;
}



VOS_UINT32 AT_QryMdStoragePara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ucState     = 0;
    VOS_INT32                           ucNandState     = -1;
    VOS_INT32                           ucSdVolume = 0;

    /*��ȡFS �洢�ռ�*/
    ucState = OM_FSGetFreeStorage( &ucNandState );
    if ( VOS_OK != ucState )
    {
        ucNandState = -1;
    }

    //��ȡSD���洢�ռ�
    /*ucState = OM_SDGetFreeStorage( &ucSdVolume );
    if ( VOS_OK != ucState )
    {
        ucSdVolume = -1;
    }*/

    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                            (VOS_CHAR *)pgucAtSndCodeAddr,
                                            (VOS_CHAR*)pgucAtSndCodeAddr,
                                            "%s:%d,%d", g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                            ucNandState,
                                            ucSdVolume);

    return AT_OK;
}



VOS_UINT32 AT_QryMdsfPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32  ulRet = 0;
    VOS_UINT8   Flag = 0;

    ulRet = OM_SDGetStorageOrFlipFlag(1, &Flag);
    if (VOS_OK != ulRet)
    {
        return AT_ERROR; 
    }
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                            (VOS_CHAR *)pgucAtSndCodeAddr,
                                            (VOS_CHAR*)pgucAtSndCodeAddr,
                                            "%s:%d", g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                            Flag);

    return AT_OK;
}


VOS_UINT32 OM_FSGetFreeStorage(VOS_INT32 *nandState)
{
    VOS_UINT32 ulfreespace = 0;/*lint !e578*/
    mm_segment_t sd_fs = 0; /*lint !e10 !e522 !e578*/
    struct statfs disk_info = {0}; /*lint !e121 !e129 !e529 !e438 !e533*/
    VOS_UINT32 ulMaxSpace = 32 * 1024;

    sd_fs = get_fs(); /*lint !e10 !e129*/
    set_fs(KERNEL_DS);  /*lint !e119 !e132 !e102*/

    /*��ִ��ʧ���򷵻�error*/
    if (0  != sys_statfs("/online",&disk_info)) /*lint !e2 !e10 !e102*/
    {
        (VOS_VOID)vos_printf("sys_statfs execute fail! \n");
        set_fs(sd_fs);
        return VOS_ERR;
    }

    ulfreespace = (VOS_UINT32)(disk_info.f_bfree * disk_info.f_bsize) / 1024; /*Byte shift to KB*/ /*lint !e10 !e101 !e129*/
    ulfreespace = ulfreespace * 9 / 10;/*����10%*/ /*lint !e10 !e129*/
    vos_printf( "ulfreespace = %lu \n",ulfreespace); /*lint !e2 !e102 !e830*/

    if (ulfreespace > ulMaxSpace)
    {
        ulfreespace = ulMaxSpace; /*lint !e63*/
    } /*lint !e533*/
    
    *nandState = (VOS_INT32)ulfreespace; /*lint !e10 !e129*/
    set_fs(sd_fs); /*lint !e132 !e578*/

    return VOS_OK; /*lint !e102*/
}

/*lint +e64 +e119*/



VOS_UINT32 SetModemLogCfgNV(VOS_UINT32 onoff)
{
    TAF_NV_PRINT_MODEM_LOG_TYPE_STRU    stPrintModemLogType;
    VOS_UINT8 switch_status = 0x00;

    if (onoff > 0)
    {
        /*����log������ӦNV��־����Ϊ0��ȫ�ֱ�������Ϊ0*/
        switch_status = 0x00;
    }
    else
    {
        /*����log������ӦNV��־����Ϊ1��ȫ�ֱ�������Ϊ1*/
        switch_status = 0x01;
    }

    TAF_MEM_SET_S(&stPrintModemLogType, sizeof(stPrintModemLogType), 0x00, sizeof(stPrintModemLogType));
    if (NV_OK != NV_Read(en_NV_Item_Print_Modem_Log_Type,
                         &stPrintModemLogType,
                         sizeof(TAF_NV_PRINT_MODEM_LOG_TYPE_STRU)))
    {
        return VOS_ERR;
    }

    stPrintModemLogType.ucPrintModemLogType = switch_status;

    /* дNV�� */
    if (NV_OK != NV_Write(en_NV_Item_Print_Modem_Log_Type,
                         &stPrintModemLogType,
                         sizeof(TAF_NV_PRINT_MODEM_LOG_TYPE_STRU)))
    {
        return VOS_ERR;
    }

    /*����ȫ�ֱ���*/
    DMS_SET_PRINT_MODEM_LOG_TYPE(switch_status);

    return VOS_OK;
}


/*****************************************************************************
 �� �� ��  : AT_SetMdonPara
 ��������  :������رձ��湦��^MDON
 �������  : ucIndex
 �������  : ��
 �� �� ֵ      : AT_OK:      ���óɹ�
                AT_ERROR:   ����ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
*****************************************************************************/
VOS_UINT32 AT_SetMdonPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32 ucTmpState = 0; /*�������õ�״ֵ̬*/
    VOS_UINT8 ucmlogstat = 0; /*��д��mlog״ֵ̬*/
    VOS_UINT32 ulRst = VOS_ERR;


    VOS_UINT8 ucUpdateFlag = VOS_FALSE;
    VOS_INT ret = 0;
    VOS_UINT8 Flag = MLOG_TYPE_MAX;


    /* ������� */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }
    /* �������� */
    if (gucAtParaIndex != 1)
    {
        return AT_ERROR;
    }
    /* ��������ֻ��Ϊ1�� */
    if (gastAtParaList[0].usParaLen != 1)
    {
        return AT_ERROR;
    }
    /*�ж�datalock�Ƿ����*/
    if(VOS_TRUE == g_bAtDataLocked)
    {
        return AT_ERROR;
    }

    ucTmpState = gastAtParaList[0].aucPara[0];
    /* ucTempStateȡֵ��ΧΪ'0'~'5' */
    if (('0' <= ucTmpState) && ('5' >= ucTmpState))
    {
        ucTmpState = (VOS_UINT32)(gastAtParaList[0].aucPara[0] - '0');
    }
    else
    {
        return AT_ERROR;
    }


    /*���ݴ����״̬�ж�д��״̬*/
    switch(ucTmpState)
    {
        case MDON_TYPE_ALL_CLOSE:
        {
            /*�ر�����״̬*/
            /*���õ�ǰ״̬��NV�Լ�ȫ�ֱ�����NV��������Ч��ȫ�ֱ���������Ч*/
            ulRst = SetModemLogCfgNV(ucTmpState);
            if (VOS_OK != ulRst)
            {
                return AT_ERROR;
            }
            /* ֻ���˿�ģʽ�л�, ����NV�� ��Ҫ�µ籣�� */
            ulRst = PPM_LogPortSwitch(CPM_OM_PORT_TYPE_USB, VOS_TRUE);
            if (VOS_OK != ulRst)
            {
                return AT_ERROR;
            }
            ulRst = OM_SDGetStorageOrFlipFlag(2, &Flag); /* �������log״̬ */
            if (VOS_OK != ulRst)
            {
                return AT_ERROR;
            }
            if (1 == Flag)
            {
                /* ��Ƭ�汾��������,����CBT��λҪ�� */
                ulRst = OM_SDSetStorageOrFlipFlag(2, MDON_TYPE_ALL_CLOSE);  /* ����log�洢���� */
                if (VOS_OK == ulRst)
                {
                    ret = modemlogcat_stop_cmd();
                    if (0 != ret)
                    {
                        return AT_ERROR;
                    }
                }
                else
                {
                    return AT_ERROR;
                }
            }
            ucmlogstat = 0; /*lint !e527*/
            break;
        }
        case MDON_TYPE_OFFLINE_LOGSAVE:
            /*���ز�Ʒ�ر�mlog*/
        {
            /*���õ�ǰ״̬��NV�Լ�ȫ�ֱ�����NV��������Ч��ȫ�ֱ���������Ч*/
            ulRst = SetModemLogCfgNV(ucTmpState);
            if(VOS_OK != ulRst)
            {
                return AT_ERROR;
            }
            /* ֻ���˿�ģʽ�л�, ����NV�� ��Ҫ�µ籣�� */
            ulRst = PPM_LogPortSwitch(CPM_OM_PORT_TYPE_VCOM, VOS_TRUE);
            if (VOS_OK != ulRst)
            {
                return AT_ERROR;
            }
            /*��ѯ�Ƿ��Ѿ����ñ�־*/
            ulRst = OM_SDGetStorageOrFlipFlag(2, &Flag);
            if (VOS_OK != ulRst)
            {
                return AT_ERROR;
            }

            if (MDON_TYPE_OFFLINE_LOGSAVE == Flag)
            {
                return AT_OK;
            }
            ulRst = OM_SDSetStorageOrFlipFlag(2, MDON_TYPE_OFFLINE_LOGSAVE);  /* ����log�洢���� */
            if (VOS_OK == ulRst)
            {
                ret = modemlogcat_run_cmd();
                if (!ret)
                {
                    return AT_OK;
                }
                else
                {
                    return AT_ERROR;
                }
            }
            else
            {
                return AT_ERROR;
            }

            break; /*lint !e527*/
        }
        case MDON_TYPE_OFFLINE_DUMPSAVE:
        case MDON_TYPE_OFFLINE_LOGEXPORT:
        case MDON_TYPE_OFFLINE_ALL:
        {
            /*����log����,�˴����ر�mlog*/
            ucmlogstat = 0;
            break;
        }
        case MDON_TYPE_OFFLINE_MLOG:
        {
            /*����mlog*/
            ucmlogstat = 1;
            break;
        }
        default:
        {
            AT_ERR_LOG1("AT_SetMdonPara: not supported type, %d.\n", ucTmpState);
            return AT_ERROR;
        }
    }

    ulRst = VOS_OK;
    ulRst = (VOS_UINT32)mlog_state_set(ucmlogstat);
    if (VOS_OK == ulRst)
    {
        (void)VOS_TaskDelay(500);
        ucUpdateFlag = VOS_FALSE;
        ulRst = AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                    gastAtClientTab[ucIndex].opId,
                                    DRV_AGENT_TMODE_SET_REQ,
                                    &ucUpdateFlag,
                                    sizeof(ucUpdateFlag),
                                    I0_WUEPS_PID_DRV_AGENT);
        if (VOS_OK != ulRst)
        {
            return AT_ERROR;    /* �������Ҫ�ȴ�C�˻ظ���ֱ�ӷ���OK */
        }
        else
        {
            return AT_OK;
        }
    }
    else
    {
        return AT_ERROR;
    }

}

/*****************************************************************************
 Prototype    : modemlogcat_run_cmd
 Description  : ����ִ��ҵ��̬����log����
 Input        : Ҫִ�еĽ�����
 Output       : None
 Return Value :
 Calls        :
*****************************************************************************/
VOS_INT modemlogcat_run_cmd(VOS_VOID)
{
    VOS_INT  ret = 0;
    
    /* -r����ÿ���ļ��Ĵ�С����MΪ��λ�� -n�����ļ���Ŀ */
    char* name = "modemlogcat_lte";
    char* para  = "-n 8 -r 10";

    ret = drv_start_user_process(name, para, NO_NEED_RESULT, 0);
    
    return ret;
}

/*****************************************************************************
 Prototype    : modemlogcat_stop_cmd
 Description  : ����ɱ��ҵ��̬����log����
 Input        : None
 Output       : None
 Return Value :
 Calls        :
*****************************************************************************/

VOS_INT modemlogcat_stop_cmd(VOS_VOID)
{
    VOS_INT  ret = 0;

    char* name = "killall";
    char* para  = "modemlogcat_lte";

    ret = drv_start_user_process(name, para, NO_NEED_RESULT, 0);

    return ret;
}



VOS_UINT32 AT_SetMdslPara(VOS_UINT8 ucIndex)
{
    VOS_UINT8                           ucRRCVer;
    VOS_UINT32                          ulRst;

    /* ������� */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* �������� */
    if (gucAtParaIndex != 1)
    {
        return AT_ERROR;
    }

    /* ��������ֻ��Ϊ1�� */
    if (gastAtParaList[0].usParaLen != 1)
    {
        return AT_ERROR;
    }

    ucRRCVer = gastAtParaList[0].aucPara[0];
    /* ���ز�ƷucRRCVerȡֵ��ΧΪ'0','2';������Ʒ'0'~'1' */
    if (('0' <= ucRRCVer) && ('1' >= ucRRCVer))
    {
        ucRRCVer = gastAtParaList[0].aucPara[0] - '0';
    }
    else
    {
        return AT_ERROR;
    }
    ulRst = OM_SDSetStorageOrFlipFlag(0,ucRRCVer);
    if (VOS_OK == ulRst)
    {
        return AT_OK;    /* �������Ҫ�ȴ�C�˻ظ���ֱ�ӷ���OK */
    }
    else
    {
        return AT_ERROR;
    }
}



VOS_UINT32 AT_SetMdStoragePara(VOS_UINT8 ucIndex) /*lint !e830*/
{
    VOS_UINT8 ucRRCVer = 0;
    mm_segment_t sd_fs = 0; /*lint !e10 !e522 !e578*/
    int retRm = 0;
    DRV_DIR_S *dirp = NULL;
    DRV_DIRENT_S *read_dirent = NULL;
    VOS_CHAR path[OM_SD_LOG_PATH_MAX_LENGTH] = {0};

    /* ������� */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* �������� */
    if (gucAtParaIndex != 1)
    {
        return AT_ERROR;
    }

    /* ��������ֻ��Ϊ1�� */
    if (gastAtParaList[0].usParaLen != 1)
    {
        return AT_ERROR;
    }

    ucRRCVer = gastAtParaList[0].aucPara[0];
    /* ���ز�ƷucRRCVerȡֵ��ΧΪ'0'~'2'��������Ʒ'0'~'1' */
    if (('0' <= ucRRCVer) && ('1' >= ucRRCVer))
    {
        ucRRCVer = gastAtParaList[0].aucPara[0] - '0';
    }
    else
    {
        return AT_ERROR;
    }
    if(1 == ucRRCVer)
    {
        return AT_ERROR;
    }
    if(0 == ucRRCVer)
    {
        dirp = mdrv_file_opendir(OM_FLASH_LOG_ROOT_PATH);
        if (NULL == dirp)
        {
            (VOS_VOID)vos_printf("OM_FSReadData:open file path %s is null\n", OM_FLASH_LOG_ROOT_PATH); /*lint !e119*/
            return AT_ERROR;
        }


        /* �ı��ڴ����Ȩ�� */
        sd_fs = get_fs(); /*lint !e63*/
        set_fs(KERNEL_DS);

        while (1)
        {
            read_dirent = mdrv_file_readdir(dirp);
            if(NULL == read_dirent)
            {
                break;
            }

            (VOS_VOID)At_sprintf(OM_SD_LOG_PATH_MAX_LENGTH,
                                        path, path, "%s/%s",
                                        OM_FLASH_LOG_ROOT_PATH, read_dirent->d_name);
            (void)mdrv_file_remove(path);

        }

        if (NULL != dirp)
        {
            (void)mdrv_file_closedir(dirp);
        }
        retRm = mdrv_file_rmdir(OM_FLASH_LOG_ROOT_PATH);
        if (retRm != 0)
        {
            printk("mdrv_file_rmdir faild %d\n", retRm);
        }

        set_fs(sd_fs);
        return AT_OK;
    }

}/*lint !e533*/


VOS_UINT32 AT_SetMdsfPara(VOS_UINT8 ucIndex)
{
    VOS_UINT8                           ucRRCVer;
    VOS_UINT32                          ulRst;

    /* ������� */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* �������� */
    if (gucAtParaIndex != 1)
    {
        return AT_ERROR;
    }

    /* ��������ֻ��Ϊ1�� */
    if (gastAtParaList[0].usParaLen != 1)
    {
        return AT_ERROR;
    }

    ucRRCVer = gastAtParaList[0].aucPara[0];
    /* ucRRCVerȡֵ��ΧΪ'0'~'1' */
    if (('0' <= ucRRCVer) && ('1' >= ucRRCVer))
    {
        ucRRCVer = gastAtParaList[0].aucPara[0] - '0';
    }
    else
    {
        return AT_ERROR;
    }
    ulRst = OM_SDSetStorageOrFlipFlag(1,ucRRCVer);
    if (VOS_OK == ulRst)
    {
        return AT_OK;    /* �������Ҫ�ȴ�C�˻ظ���ֱ�ӷ���OK */
    }
    else
    {
        return AT_ERROR;
    }
}


VOS_UINT32 OM_SDSetStorageOrFlipFlag(VOS_UINT8 storageOrFlip, VOS_UINT8 flag)
{
    FILE*                       fp = NULL;
    VOS_INT32                   lReadSize = 0;
    mm_segment_t                sd_fs = 0; /*lint !e10 !e522 !e578*/

    /* �ı��ڴ����Ȩ�� */
    sd_fs = get_fs(); /*lint !e63*/
    set_fs(KERNEL_DS);

    /*��ȡ���������ļ�*/
    fp = mdrv_file_open(OM_SD_LOG_SET_INFO_FILE, "rb");
    if (NULL == fp)
    {
        (VOS_VOID)vos_printf("OM_SDSetStorageOrFlipFlag: The config file is not exsit, create it!\n"); /*lint !e119*/
        MBB_MEM_SET_S(&g_stOmSDLogSetInfo, sizeof(OM_SD_LOG_SET_INFO), 0, sizeof(OM_SD_LOG_SET_INFO));
    }
    else
    {
        lReadSize = mdrv_file_read((void *)&g_stOmSDLogSetInfo, 1, sizeof(OM_SD_LOG_SET_INFO), fp);
        if (sizeof(OM_SD_LOG_SET_INFO) != lReadSize)
        {
            (VOS_VOID)vos_printf("OM_SDSetStorageOrFlipFlag: The config file read fail!\n"); /*lint !e119*/
            (void)mdrv_file_close(fp);
            set_fs(sd_fs);
            return VOS_ERR;
        }
    
        (void)mdrv_file_close(fp);
    }
    /*�����Ƿ�ѭ���洢*/
    if ( 1 == storageOrFlip )
    {
        if ( g_stOmSDLogSetInfo.ucOfflineCleanStorage != flag)
        {
            g_stOmSDLogSetInfo.ucOfflineCleanStorage = flag;
        }
    }
    else if ( 0 == storageOrFlip )/*���ô洢λ��*/
    {
        if ( g_stOmSDLogSetInfo.ucStorageLocation != flag)
        {
            g_stOmSDLogSetInfo.ucStorageLocation = flag;
        }
    }
    else if ( 2 == storageOrFlip )/*���ô洢LOG����*/
    {
        if ( g_stOmSDLogSetInfo.ucOfflineSwitch != flag)
        {
            g_stOmSDLogSetInfo.ucOfflineSwitch = flag;
        }
    }

        /*д�뿪�������ļ�*/
    fp = mdrv_file_open(OM_SD_LOG_SET_INFO_FILE, "wb");
    if (NULL == fp)
    {
        (VOS_VOID)vos_printf("OM_SDSetStorageOrFlipFlag: The config file is not exsit!\n"); /*lint !e119*/
        set_fs(sd_fs);
        return VOS_ERR;
    }

    lReadSize = mdrv_file_write((void *)&g_stOmSDLogSetInfo, 1, sizeof(OM_SD_LOG_SET_INFO), fp);
    if (sizeof(OM_SD_LOG_SET_INFO) != lReadSize)
    {
        (VOS_VOID)vos_printf("OM_SDSetStorageOrFlipFlag: The config file write fail!\n"); /*lint !e119*/
        (void)mdrv_file_close(fp);
        set_fs(sd_fs);
        return VOS_ERR;
    }

    (void)mdrv_file_close(fp);
    set_fs(sd_fs);
    return VOS_OK;
}



VOS_UINT32 OM_SDGetStorageOrFlipFlag(VOS_UINT8 storageOrFlip, VOS_UINT8 *flag)
{
    FILE*                       fp = DRV_FILE_NULL;
    VOS_INT32                   lReadSize = 0;
    mm_segment_t                sd_fs = 0; /*lint !e10 !e522 !e578*/

    /* �ı��ڴ����Ȩ�� */
    sd_fs = get_fs(); /*lint !e63*/
    set_fs(KERNEL_DS);

    /*��ȡ�����ļ�*/
    fp = mdrv_file_open(OM_SD_LOG_SET_INFO_FILE, "r");
    if (NULL == fp)
    {
        (VOS_VOID)vos_printf("OM_SDSetStorageOrFlipFlag: The config file is not exsit, return defualt!\n"); /*lint !e119*/
        /*Ĭ����ѭ���洢/Flash�ķ�ʽ*/
        *flag = 0;
        set_fs(sd_fs);
        return VOS_OK;
    }
    else
    {
        lReadSize = mdrv_file_read((VOS_CHAR*)&g_stOmSDLogSetInfo, 1, sizeof(OM_SD_LOG_SET_INFO), fp);
        if (sizeof(OM_SD_LOG_SET_INFO) != lReadSize)
        {
            (VOS_VOID)vos_printf("OM_SDSetStorageOrFlipFlag: The config file read fail!\n"); /*lint !e119*/
            (void)mdrv_file_close(fp);
            set_fs(sd_fs);
            return VOS_ERR;
        }
        (void)mdrv_file_close(fp);
    }

    /*��ѯ�Ƿ�ѭ���洢*/
    if ( 1 == storageOrFlip )
    {
        *flag = g_stOmSDLogSetInfo.ucOfflineCleanStorage;
    }
    else if ( 0 == storageOrFlip )/*��ѯ�洢λ��*/
    {
        *flag = g_stOmSDLogSetInfo.ucStorageLocation;
    }
    else if ( 2 == storageOrFlip ) /* ��ѯ�洢LOG���� */
    {
        *flag = g_stOmSDLogSetInfo.ucOfflineSwitch;
    }
    set_fs(sd_fs);
    return VOS_OK;
}











VOS_UINT32 At_SetHgmrPara(VOS_UINT8 ucIndex)
{
    VOS_UINT8 version_type = SOFTWARE_VERSION_MAX;
    VOS_CHAR  version_info[VERSION_MAX_LEN] = {0};
    VOS_UINT8 ulRst = AT_SUCCESS;
    /* ������� */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* ����������Ϊһ�� */
    if (gucAtParaIndex != 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }
    version_type = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    switch(version_type)
    {
        /*��ѯ��������汾��*/
        case BASE_SOFTWARE_VERSION:
        {
            /* �ȴ�׮��������Ӿ���Ľӿ� */
           // PS_MEM_CPY(version_info, "V700R200C00B001", AT_STRLEN("V700R200C00B001"));
            ulRst = BSP_DLOAD_BaselineVer(version_info,VERSION_MAX_LEN);
            if (AT_SUCCESS != ulRst)
            {
                AT_ERR_LOG("At_SetHgmrPara: Get BaselineVer failed!");
                return AT_ERROR;
            }
            break;
        }
        /*��ѯƽ̨����汾��*/
        case PLATFORM_SOFTWARE_VERSION:
        {
           // PS_MEM_CPY(version_info, "21.090.00.00.000", AT_STRLEN("21.090.00.00.000"));
            ulRst = mdrv_dload_getsoftver(version_info,VERSION_MAX_LEN);
            if (AT_SUCCESS != ulRst)
            {
                AT_ERR_LOG("At_SetHgmrPara: GetSoftwareVer failed!");
                return AT_ERROR;
            }
            break;
        }
        /*��ѯ��Ʒ����汾�ţ�Ŀǰ����PDU��ѯ�İ汾��������*/
        case PRODUCT_SOFTWARE_VERSION:
            break;
        /*��ѯPC VENDOR ����汾�ţ�balongƽ̨��֧��*/
        case PC_VENDOR_SOFTWARE_VERSION:
        default:
            break;
    }
    gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                (TAF_CHAR *)pgucAtSndCodeAddr,
                                (TAF_CHAR *)pgucAtSndCodeAddr,
                                "%s:%s",
                                g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                version_info);
    return AT_OK;
    /*��ѯ��������汾��*/
}


VOS_UINT32 At_QryHgmrPara(VOS_UINT8 ucIndex)
{
    gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                 (TAF_CHAR *)pgucAtSndCodeAddr,
                                 (TAF_CHAR *)pgucAtSndCodeAddr,
                                 "%s:%d,%d,%d",
                                 g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                 BASE_SOFTWARE_VERSION,
                                 PLATFORM_SOFTWARE_VERSION,
                                 PRODUCT_SOFTWARE_VERSION);
    return AT_OK;
}





/*****************************************************************************
 �� �� ��  : At_SetMbimMode
 ��������  : AT^SETMODE=<value>������������ģʽ�л���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :
*****************************************************************************/
VOS_UINT32 At_SetMbimMode(VOS_UINT8 ucIndex)
{   
    int switch_mode = 0;
    /* ������� */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* �������� */
    if (gucAtParaIndex > 1)
    {
        return AT_ERROR;
    }
    
    /*�������Ȳ�Ϊ1*/
    if (1 != gastAtParaList[0].usParaLen)
    {
        return AT_ERROR;
    }

    /*1��ʾ��Ҫ�л�Ϊdebugģʽ��0��ʾ��Ҫ�л�ΪNORMALģʽ*/
    if('1' == gastAtParaList[0].aucPara[0])
    {
        switch_mode = 1;
       
    }
    else if('0' == gastAtParaList[0].aucPara[0])
    {
        switch_mode = 0;
    }
    else
    {

        return AT_ERROR;
    }
    
    DRV_USB_MBIM_SET_MODE(switch_mode); 
    return AT_OK;
}





/*****************************************************************************
 �� �� ��  : At_QryMbimMode
 ��������  : ^SETMODE��ѯ�������
 �������  : TAF_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :
*****************************************************************************/
VOS_UINT32   At_QryMbimMode(VOS_UINT8 ucIndex)
{
    VOS_UINT32 ulResult = 0;
    ulResult = DRV_USB_MBIM_GET_MODE();
    
    /* 0��NORMALģʽ(����������ģʽ) 1��DEBUGģʽ(������ģʽ)*/
    gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                              (TAF_CHAR *)pgucAtSndCodeAddr,
                                              (TAF_CHAR*)pgucAtSndCodeAddr,
                                              "%s: %d",
                                              g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                              ulResult);
    return AT_OK;
}






















VOS_UINT32 OM_FSReadData(
                  VOS_UINT8 *pucData, 
                  VOS_UINT32 *pulLen, 
                  VOS_UINT32 *FsTotalPack, 
                  VOS_UINT32 *FsCurPack,
                  VOS_UINT32 FileIndex,
                  VOS_UINT32 ucSubName)
{
    FILE                             *fp = NULL;
    VOS_INT32                   iRet = 0;
    VOS_UINT32                  uFsTotalPack = 0;
    mm_segment_t                sd_fs = 0; /*lint !e10 !e522 !e578*/
    VOS_CHAR                    path[OM_SD_LOG_PATH_MAX_LENGTH] = {0};
    VOS_UINT32                  packet_len = *pulLen;
    DRV_DIR_S                   *dirp = NULL;
    DRV_DIRENT_S              *read_dirent = NULL;
    int max_file_id = 0;
    int min_file_id = 0;
    int temp_id = 0;

    /*�״ζ�ȡ��ʼ�������ݰ��������ļ�����*/
    if (0 == g_stOmFSReadCtrlInfo.isInited)
    {
        g_stOmFSReadCtrlInfo.ulTotalLength = 0;
        g_stOmFSReadCtrlInfo.ulFileCurId = 0;
        g_stOmFSReadCtrlInfo.ulCurPack = 0;

        if ( FILE_TYPE_FS == FileIndex )
        {
            dirp = mdrv_file_opendir(OM_FLASH_LOG_ROOT_PATH);
            if (NULL == dirp)
            {
                (VOS_VOID)vos_printf("OM_FSReadData:open file path %s is null\n", OM_FLASH_LOG_ROOT_PATH); /*lint !e119*/
                return AT_ERROR;
            }
        }
        else if ( FILE_TYPE_VENDOR == FileIndex )
        {
            return AT_ERROR;
        }
        else
        {
            return AT_ERROR;
        }
        
        /* �ı��ڴ����Ȩ�� */
        sd_fs = get_fs(); /*lint !e63*/
        set_fs(KERNEL_DS);

        while (1)
        {
            read_dirent = mdrv_file_readdir(dirp);
            if(NULL == read_dirent)
            {
                break;
            }
            if(!sscanf(read_dirent->d_name, "ltelogfile_%05d", &temp_id))
            {
                continue;
            }
            (VOS_VOID)At_sprintf(OM_SD_LOG_PATH_MAX_LENGTH,
                                        path, path, "%s/%s",
                                        OM_FLASH_LOG_ROOT_PATH, read_dirent->d_name);

            fp = mdrv_file_open(path, "r+");
            if (NULL == fp)
            {
                continue;
            }
            iRet = mdrv_file_seek(fp, 0, SEEK_END);
            if (0 > iRet)
            {
                (VOS_VOID)vos_printf("%s:%d:SD_FS_LSEEK is wrong\n", __FUNCTION__, __LINE__); /*lint !e119*/
                goto error2;
            }
            iRet = mdrv_file_tell(fp);
            
            //iRet = OM_FSgetLogFileLength(path, FileIndex);
            if (iRet < sizeof(OM_UCMX_HEAD_INFO_STRU)) /*lint !e574*/
            {
                continue;
            }

            if (0 != g_stOmFSReadCtrlInfo.ulFileCurId)
            {
                iRet -= sizeof(OM_UCMX_HEAD_INFO_STRU);
            }

            if (temp_id > max_file_id)
            {
                max_file_id = temp_id;
            }
            
            if ((min_file_id > temp_id) || (0 == min_file_id))
            {
                min_file_id = temp_id;
            }

            g_stOmFSReadCtrlInfo.ulFileLength = iRet;
            uFsTotalPack += (g_stOmFSReadCtrlInfo.ulFileLength + packet_len -1) / packet_len;
            g_stOmFSReadCtrlInfo.ulTotalLength +=  g_stOmFSReadCtrlInfo.ulFileLength;
            g_stOmFSReadCtrlInfo.ulFileCurId++;
        
        }
        g_stFSFileInfo.ulFileMinId = min_file_id;
        g_stFSFileInfo.ulFileMaxId = max_file_id;

        if (NULL != fp)
        {
            (void)mdrv_file_close(fp);
        }
        if (NULL != dirp)
        {
            (void)mdrv_file_closedir(dirp);
        }
        set_fs(sd_fs);
        if (0 == uFsTotalPack)
        {
            return VOS_ERR;
        }
        g_stOmFSReadCtrlInfo.ulTotalPack = uFsTotalPack;
        g_stOmFSReadCtrlInfo.ulFileCurId = 0;
        g_stOmFSReadCtrlInfo.ulDataInBuf = 0;
        g_stOmFSReadCtrlInfo.ulFileLength = 0;
        g_stOmFSReadCtrlInfo.isInited = 1;
    }

    (VOS_VOID)At_sprintf(OM_SD_LOG_PATH_MAX_LENGTH,
                                        path, path, "%s/ltelogfile_%05d.lpd",
                                        OM_FLASH_LOG_ROOT_PATH,
                                        (g_stFSFileInfo.ulFileMinId + g_stOmFSReadCtrlInfo.ulFileCurId));
    /*������״ζ�ȡ���ļ����ȳ�ʼ���ļ����ȵ�*/
    if (0 == g_stOmFSReadCtrlInfo.ulFileLength)
    {
        //iRet = OM_FSgetLogFileLength(path, FileIndex);
        sd_fs = get_fs(); /*lint !e63*/
        set_fs(KERNEL_DS);
        fp = mdrv_file_open(path, "r+");
        if (NULL == fp)
        {
            set_fs(sd_fs);
            return VOS_ERR;
        }
        
        iRet = mdrv_file_seek(fp, 0, SEEK_END);
        if (0 > iRet)
        {
            (VOS_VOID)vos_printf("%s:%d:SD_FS_LSEEK is wrong\n", __FUNCTION__, __LINE__); /*lint !e119*/
            goto error2;
        }
        iRet = mdrv_file_tell(fp);
        (void)mdrv_file_close(fp);
        set_fs(sd_fs);
        if (iRet < sizeof(OM_UCMX_HEAD_INFO_STRU)) /*lint !e574*/
        {
            return VOS_ERR;
        }
//#if (FEATURE_ON == FEATURE_COMPRESS_WRITE_LOG_FILE)
        if (0 != g_stOmFSReadCtrlInfo.ulFileCurId) 
        {
            iRet -= sizeof(OM_UCMX_HEAD_INFO_STRU);
        }
//#endif
        g_stOmFSReadCtrlInfo.ulFileLength = iRet;

        g_stOmFSReadCtrlInfo.ulFileOffset = 0;
//#if (FEATURE_ON == FEATURE_COMPRESS_WRITE_LOG_FILE)
        /* LOGѹ���㷨��������Ҫ��ȥͷ�ļ���Ϣ */
        if (0 != g_stOmFSReadCtrlInfo.ulFileCurId)
        {
            g_stOmFSReadCtrlInfo.ulFileOffset += sizeof(OM_UCMX_HEAD_INFO_STRU);
            (VOS_VOID)vos_printf("COMPRESS:g_stOmFSReadCtrlInfo.ulFileOffset= %d \n",g_stOmFSReadCtrlInfo.ulFileOffset); /*lint !e119*/
        }
//#endif
    }
    /*��ȡ�ļ����ݵ�buf*/
    sd_fs = get_fs(); /*lint !e63*/
    set_fs(KERNEL_DS);
    fp = mdrv_file_open(path, "r+");
    if (NULL == fp)
    {
        (VOS_VOID)vos_printf("SD_FS_OPEN(%s, \"r\") is NULL\n",path); /*lint !e119*/
        goto error1;
    }
    
    iRet = mdrv_file_seek(fp, g_stOmFSReadCtrlInfo.ulFileOffset , SEEK_SET);
    if(iRet < 0)
    {
        (VOS_VOID)vos_printf("%s:%d:SD_FS_LSEEK is wrong\n",__FUNCTION__,__LINE__); /*lint !e119*/
        goto error2;
    }
    
    if (g_stOmFSReadCtrlInfo.ulFileLength < packet_len)
    {
        g_stOmFSReadCtrlInfo.ulDataInBuf = g_stOmFSReadCtrlInfo.ulFileLength;
    }
    else
    {
        g_stOmFSReadCtrlInfo.ulDataInBuf = packet_len;
    }
    
    iRet = mdrv_file_read(pucData,  sizeof(VOS_UINT8), g_stOmFSReadCtrlInfo.ulDataInBuf, fp);
    if(iRet != g_stOmFSReadCtrlInfo.ulDataInBuf)
    {
        (VOS_VOID)vos_printf("%s:BSP_fread() is wrong\n",__FUNCTION__); /*lint !e119*/
        goto error2;
    }
    
    g_stOmFSReadCtrlInfo.ulFileLength -= g_stOmFSReadCtrlInfo.ulDataInBuf;
    g_stOmFSReadCtrlInfo.ulFileOffset += g_stOmFSReadCtrlInfo.ulDataInBuf;
    g_stOmFSReadCtrlInfo.ulCurPack ++;
    (void)mdrv_file_close(fp);
    set_fs(sd_fs);

    *pulLen = g_stOmFSReadCtrlInfo.ulDataInBuf;
    *FsTotalPack = g_stOmFSReadCtrlInfo.ulTotalPack;
    *FsCurPack = g_stOmFSReadCtrlInfo.ulCurPack;

    if (0 == g_stOmFSReadCtrlInfo.ulFileLength)
    {
        g_stOmFSReadCtrlInfo.ulFileCurId ++;
    }

    if (g_stOmFSReadCtrlInfo.ulCurPack == g_stOmFSReadCtrlInfo.ulTotalPack)
    {
        g_stOmFSReadCtrlInfo.ulCurPack = 0;
        g_stOmFSReadCtrlInfo.ulFileCurId = 0;
        g_stOmFSReadCtrlInfo.ulTotalLength = 0;
        g_stOmFSReadCtrlInfo.ulTotalPack = 0;
        g_stOmFSReadCtrlInfo.isInited = 0;
        /*if ( FILE_TYPE_VENDOR == FileIndex )
        {
            OM_SetVendorLogStop(0);
        }*/
    }

    return VOS_OK;
error2:
    (void)mdrv_file_close(fp);
error1:
    set_fs(sd_fs);
    (VOS_VOID)vos_printf("%s:return VOS_ERR\n",__FUNCTION__); /*lint !e119*/
    return VOS_ERR;
}
/*lint +e64 +e119*/

/*****************************************************************************
 �� �� ��  : Mbb_AT_QryRsfrLogFs
 ��������  : ��������log
 �������  : ucIndex - �û�����
 �������  : ��
 �� �� ֵ  : �����ɹ�: AT_OK, ʧ��: AT_ERROR
 ���ú���  : OM_FSReadData
             AtBase64Encode
 ��������  : AT_SetRsfrPara
 �޸���ʷ  :
*****************************************************************************/
VOS_UINT32 Mbb_AT_QryRsfrLogFs(VOS_CHAR* cSubName, VOS_CHAR* cName)
{
    VOS_UINT32 ucSubName = 0;
    VOS_UINT32 buflenght = 0;


    VOS_UINT32 uFsdataCurPackNum = 0;
    VOS_UINT32 uFsdataTotalPack = 0;
    VOS_UINT32 uFileIndex = 0;
    VOS_UINT8 *pcRsfrSrcItem = NULL;
    VOS_UINT8 *pcRsfrDstItem = NULL;
    /*lint -e64*/
    (void)At_Auc2ul(cSubName,gastAtParaList[1].usParaLen,&ucSubName);   
    /*lint +e64*/
    if (ERR_MSP_SUCCESS == AT_STRCMP(cName, "LOGSAVE_FS"))
    {
        uFileIndex = FILE_TYPE_FS;
    }
    else if (ERR_MSP_SUCCESS == AT_STRCMP(cName, "LOGSAVE_VENDOR"))
    {
        //uFileIndex = FILE_TYPE_VENDOR;
    }

    pcRsfrSrcItem = (VOS_UINT8*)PS_MEM_ALLOC(WUEPS_PID_AT, AT_RSFR_RSFW_MAX_LEN);
    if (VOS_NULL_PTR == pcRsfrSrcItem )
    {
        return AT_ERROR;
    }

    PS_MEM_SET(pcRsfrSrcItem, 0, AT_RSFR_RSFW_MAX_LEN);

    buflenght = AT_RSFR_RSFW_DATA_LEN;
    if (VOS_OK != OM_FSReadData(pcRsfrSrcItem,&buflenght,
        &uFsdataTotalPack,&uFsdataCurPackNum, uFileIndex, ucSubName))
    {
        PS_MEM_FREE(WUEPS_PID_AT, pcRsfrSrcItem); /*lint !e830*/
        return AT_ERROR;
    }
    pcRsfrDstItem = (VOS_UINT8 *)PS_MEM_ALLOC(WUEPS_PID_AT,AT_RSFR_RSFW_MAX_LEN);
    if(pcRsfrDstItem == NULL)
    {
        PS_MEM_FREE(WUEPS_PID_AT, pcRsfrSrcItem);
        return AT_ERROR;
    }
    PS_MEM_SET(pcRsfrDstItem, 0, AT_RSFR_RSFW_MAX_LEN);

    /*�����ݽ���Base64����*/
    AtBase64Encode(pcRsfrSrcItem, buflenght, pcRsfrDstItem);/*lint !e516*/

    /*lint -e64 -e119*/   
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr,
                                (VOS_CHAR *)pgucAtSndCodeAddr,
                                "^RSFR:\"%s\",\"%s\",%d,%d,%d,",
                                cName,cSubName,OFFLINE_LOG_REF_BASE,uFsdataTotalPack,uFsdataCurPackNum);
                                   
    gstAtSendData.usBufLen += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr,
                                (VOS_CHAR *)pgucAtSndCodeAddr + gstAtSendData.usBufLen,
                                "\"%s\",%s", pcRsfrDstItem,gaucAtCrLf);
    /*lint +e64 +e119*/
    PS_MEM_FREE(WUEPS_PID_AT, pcRsfrSrcItem);
    PS_MEM_FREE(WUEPS_PID_AT, pcRsfrDstItem);
    return AT_OK;
}










/******************************************************************************
                       MBB DRV AT list
******************************************************************************/
AT_PAR_CMD_ELEMENT_STRU g_astDrvAtPrivateCmdTbl[] = {

/* HUAWEI �ƶ������Ʒ�ն��豸һ�������ӿڹ淶V1.07.doc �ĵ��漰���������δ��һ: V700R001�汾ʹ��LTE��ʵ�֣�V300R002�汾ʹ��GUģ��ʵ�� */
    {AT_CMD_SUPINFO,
    atSetUpInfo, AT_UPGRADE_TIME_5S, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
    VOS_NULL_PTR,   AT_NOT_SET_TIME,
    AT_ERROR, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_E5_IS_LOCKED,
    (VOS_UINT8*)"^SUPINFO", (VOS_UINT8*)"(@time),(@ver),(1-10)"},

    {AT_CMD_CHKUPINFO,
    atSetUpInfoNum, AT_UPGRADE_TIME_5S, atQryUpInfo, AT_UPGRADE_TIME_5S, VOS_NULL_PTR, AT_NOT_SET_TIME,
    VOS_NULL_PTR,   AT_NOT_SET_TIME,
    AT_ERROR, CMD_TBL_PIN_IS_LOCKED | CMD_TBL_E5_IS_LOCKED,
    (VOS_UINT8*)"^CHKUPINFO", (VOS_UINT8*)"(1-10)"},

    {AT_CMD_USBSPEED,
    VOS_NULL_PTR,           AT_SET_PARA_TIME,   At_QrySpeed,          AT_QRY_PARA_TIME,   VOS_NULL_PTR, AT_NOT_SET_TIME,
    VOS_NULL_PTR,   AT_NOT_SET_TIME,
    AT_CME_INCORRECT_PARAMETERS, CMD_TBL_LIMITED_NULL | CMD_TBL_PIN_IS_LOCKED,
    (VOS_UINT8*)"^USBSPEED",    TAF_NULL_PTR},



    /*lint -e10 -e64 -e133*/
    /*����log��mlog���ؿ���AT����*/
    {AT_CMD_MDON,
    AT_SetMdonPara, AT_NOT_SET_TIME, AT_QryMdonPara, AT_QRY_PARA_TIME, At_TestMdonPara, AT_NOT_SET_TIME,
    VOS_NULL_PTR, AT_NOT_SET_TIME,
    AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
    (VOS_UINT8*)"^MDON",   (TAF_UINT8*)"(0,1,5)"},
    /*lint +e10 +e64 +e133*/
    {AT_CMD_MDSL,
    AT_SetMdslPara, AT_NOT_SET_TIME, AT_QryMdslPara, AT_QRY_PARA_TIME, At_TestMdslPara, AT_NOT_SET_TIME,
    VOS_NULL_PTR, AT_NOT_SET_TIME,
    AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
    (VOS_UINT8*)"^MDSL", (TAF_UINT8*)"(0,1)"},

    {AT_CMD_MDSTORAGE,
    AT_SetMdStoragePara, AT_NOT_SET_TIME, AT_QryMdStoragePara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
    VOS_NULL_PTR, AT_NOT_SET_TIME,
    AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
    (VOS_UINT8*)"^MDSTORAGE", (TAF_UINT8*)"(0,1)"},

    {AT_CMD_MDSF,
    AT_SetMdsfPara, AT_NOT_SET_TIME, AT_QryMdsfPara, AT_QRY_PARA_TIME, At_TestMdsfPara, AT_NOT_SET_TIME,
    VOS_NULL_PTR, AT_NOT_SET_TIME,
    AT_CME_INCORRECT_PARAMETERS, CMD_TBL_PIN_IS_LOCKED,
    (VOS_UINT8*)"^MDSF", (TAF_UINT8*)"(0,1)"},
    /*lint +e10 +e64 +e133*/



    {AT_CMD_HGMR,
    At_SetHgmrPara,     AT_NOT_SET_TIME,    At_QryHgmrPara,    AT_NOT_SET_TIME,    
    VOS_NULL_PTR ,    AT_NOT_SET_TIME,
    VOS_NULL_PTR,AT_NOT_SET_TIME,
    AT_CME_INCORRECT_PARAMETERS,    CMD_TBL_PIN_IS_LOCKED,
    (VOS_UINT8*)"^HGMR",  (VOS_UINT8 *)"(1-4)"},















}; /* g_astDrvAtPrivateCmdTbl */

VOS_UINT32 At_RegisterMbbDrvPrivateCmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_astDrvAtPrivateCmdTbl, sizeof(g_astDrvAtPrivateCmdTbl)/sizeof(g_astDrvAtPrivateCmdTbl[0]));
}



