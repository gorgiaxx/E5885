 /****************************************************************************
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
 ******************************************************************************/


/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/fs.h>
#include <linux/of_gpio.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/irqreturn.h>
#include <linux/interrupt.h>
#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/seq_file.h>
#include <linux/init.h>
#include <linux/mutex.h>
#include <mdrv_nfc.h>
#include "drv_m24sr.h"
#include "libndefuri.h"
#include "libndefwifi.h"
#include "drv_m24sr.h"
#include "libtagtype4.h"
#include <linux/netlink.h>

/*----------------------------------------------*
 * ö�ٶ���                                     *
 *----------------------------------------------*/
 /*��ӡ�ȼ�����*/
enum NFC_DEBUG_LEVEL
{
    HUAWEI_NFC_ERR,           /* error conditions */
    HUAWEI_NFC_WARNING,       /* warning conditions */
    HUAWEI_NFC_NOTICE,        /* normal but significant condition */
    HUAWEI_NFC_INFO,          /* informational */
    HUAWEI_NFC_DEBUG,         /* debug-level messages */
    HUAWEI_NFC_MAX,
};

/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
#define NFC_ERROR(fmt,args...)          nfc_print_message(HUAWEI_NFC_ERR,    fmt,## args)
#define NFC_WARNING(fmt,args...)        nfc_print_message(HUAWEI_NFC_WARNING,fmt,## args)
#define NFC_NOTICE(fmt,args...)         nfc_print_message(HUAWEI_NFC_NOTICE, fmt,## args)
#define NFC_INFO(fmt,args...)           nfc_print_message(HUAWEI_NFC_INFO,   fmt,## args)
#define NFC_DEBUG(fmt,args...)          nfc_print_message(HUAWEI_NFC_DEBUG,  fmt,## args)

#if(FEATURE_ON == MBB_FACTORY)
static sWifiUrlToken g_FactoryVerifyInfo;
#define HUAWEI_NFC_FACTORY_DELAY        (500)
#define HUAWEI_NFC_INT_WAKE_TIMEOUT     (HUAWEI_NFC_FACTORY_DELAY * 2)
#else
#define HUAWEI_NFC_INT_WAKE_TIMEOUT     (20 * 1000)
#endif

/*----------------------------------------------*
 * ȫ�ֱ���                                     *
 *----------------------------------------------*/
/*NFC LOG��ӡ���ƣ�Ĭ�ϴ�ӡERR����LOG*/
/*echo 5 > /sys/module/huawei_nfc_i2c_driver/parameters/g_nfc_log_level �����д�ӡ*/
static int32_t g_nfc_log_level = HUAWEI_NFC_MAX;

module_param_named(
    g_nfc_log_level, g_nfc_log_level, int, S_IRUGO | S_IWUSR | S_IWGRP
);
/*nfc_operate_mutex is used to protect the operation of NFC's reading and writting*/
static DEFINE_SEMAPHORE(nfc_operate_mutex);
static int32_t g_nfc_init = FALSE;
static int32_t g_nfc_write_flag = FALSE;
/*����sysfsʹ��*/
static struct class* g_nfc_class = NULL;
/*����I2C�豸�ľ��*/
struct i2c_client* g_huawei_nfc_client = NULL;
/*NFC�豸����*/
struct huawei_nfc_device* g_huawei_nfc_device = NULL;

/*----------------------------------------------*
 * �ṹ�嶨��                                   *
 *----------------------------------------------*/
#if (FEATURE_ON == MBB_NFC_PARSE_URL)
/*URL��ֵ����������URL*/
static URI_StringDef URI_Table[URI_RFU] =
{
    {URI_ID_0x01, URI_ID_0x01_STRING},
    {URI_ID_0x01, URI_ID_0x01_STRING},
    {URI_ID_0x02, URI_ID_0x02_STRING},
    {URI_ID_0x03, URI_ID_0x03_STRING},
    {URI_ID_0x04, URI_ID_0x04_STRING},
    {URI_ID_0x05, URI_ID_0x05_STRING},
    {URI_ID_0x06, URI_ID_0x06_STRING},
    {URI_ID_0x07, URI_ID_0x07_STRING},
    {URI_ID_0x08, URI_ID_0x08_STRING},
    {URI_ID_0x09, URI_ID_0x09_STRING},
    {URI_ID_0x0A, URI_ID_0x0A_STRING},
    {URI_ID_0x0B, URI_ID_0x0B_STRING},
    {URI_ID_0x0C, URI_ID_0x0C_STRING},
    {URI_ID_0x0D, URI_ID_0x0D_STRING},
    {URI_ID_0x0E, URI_ID_0x0E_STRING},
    {URI_ID_0x0F, URI_ID_0x01_STRING},
    {URI_ID_0x10, URI_ID_0x10_STRING},
    {URI_ID_0x11, URI_ID_0x11_STRING},
    {URI_ID_0x12, URI_ID_0x12_STRING},
    {URI_ID_0x13, URI_ID_0x13_STRING},
    {URI_ID_0x14, URI_ID_0x14_STRING},
    {URI_ID_0x15, URI_ID_0x15_STRING},
    {URI_ID_0x16, URI_ID_0x16_STRING},
    {URI_ID_0x17, URI_ID_0x17_STRING},
    {URI_ID_0x18, URI_ID_0x18_STRING},
    {URI_ID_0x19, URI_ID_0x19_STRING},
    {URI_ID_0x1A, URI_ID_0x1A_STRING},
    {URI_ID_0x1B, URI_ID_0x1B_STRING},
    {URI_ID_0x1C, URI_ID_0x1C_STRING},
    {URI_ID_0x1D, URI_ID_0x1D_STRING},
    {URI_ID_0x1E, URI_ID_0x1E_STRING},
    {URI_ID_0x1F, URI_ID_0x1F_STRING},
    {URI_ID_0x20, URI_ID_0x20_STRING},
    {URI_ID_0x21, URI_ID_0x21_STRING},
    {URI_ID_0x22, URI_ID_0x22_STRING},
    {URI_ID_0x23, URI_ID_0x23_STRING},
};
 #endif
/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/

/*****************************************************************************
 �� �� ��  : nfc_print_message
 ��������  : NFC ����ģ��LOG��ӡ
 �������  : int print_level
             const char *fmt...
 �������  : NA
 �� �� ֵ  : vprintk����ֵ
 ���ú���  : va_start vprintk va_end
 ��������  : NFC_ERROR NFC_WARNING NFC_NOTICE NFC_INFO HUAWEI_NFC_DEBUG
*****************************************************************************/
asmlinkage int nfc_print_message(int print_level, const char* fmt, ...)
{
    va_list args;
    int rc = 0;

    /*С��g_chg_log_level�����LOG���Ա���ӡ*/
    if ((uint32_t)print_level <= g_nfc_log_level)
    {
        va_start(args, fmt);
        rc = vprintk(fmt, args);
        va_end(args);
    }

    return rc;
}

/*****************************************************************************
 �� �� ��  : huawei_nfc_is_initialized
 ��������  : �ж�NFCоƬ�Ƿ��ʼ�����
 �������  : NA
 �������  : NA
 �� �� ֵ  : NFCоƬ��ʼ��״̬
 ���ú���  : NA
 ��������  : NA
*****************************************************************************/
int32_t huawei_nfc_is_initialized(void)
{
    return g_nfc_init;
}

/*****************************************************************************
 �� �� ��  : huawei_nfc_i2c_read_wifi_url_token
 ��������  : ��NFC��ȡwifi��URL��Ϣ
 �������  : ��
 �������  : sWifiUrlToken *pToken �洢��ȡ����wifi��URL��Ϣ
 �� �� ֵ  : 0     :����ִ��ʧ��
                  ���� :д����ֽ���
 ���ú���  : TT4_ReadWifiURLToken
 ��������  : NA
*****************************************************************************/
static uint16_t huawei_nfc_i2c_read_wifi_url_token(sWifiUrlToken* pToken)
{
    uint16_t ret = ERROR;

    if(NULL == pToken)
    {
        NFC_ERROR("HUAWEI_NFC: invalid params!\n");
        return - EINVAL;
    }

    ret = TT4_ReadWifiURLToken(pToken);

    if ( SUCCESS != ret )
    {
        NFC_ERROR("HUAWEI_NFC: nfc read ndef failed!\n");
    }

    return ret;
}

/*****************************************************************************
 �� �� ��  : huawei_nfc_i2c_write_wifi_url_token
 ��������  : д��WIFI+URL ���ݵ�NDEF File
 �������  : sWifiUrlToken *pToken ����WIFI��SSID+PWD,URL����Ϣ��
 �������  : ��
 �� �� ֵ  : 0     :����ִ��ʧ��
             ����  :д����ֽ���
 ���ú���  : TT4_WriteWifiURLToken
 ��������  : NA
*****************************************************************************/
static uint16_t huawei_nfc_i2c_write_wifi_url_token(sWifiUrlToken* pToken)
{
    uint16_t ret = ERROR;

    if(NULL == pToken)
    {
        NFC_ERROR("HUAWEI_NFC: invalid params!\n");
        return -EINVAL;
    }

    if(TRUE != g_nfc_write_flag)
    {
        ret = TT4_WriteWifiURLToken(pToken);

        if ( SUCCESS != ret )
        {
            NFC_ERROR("HUAWEI_NFC: nfc write ndef failed!\n");
        }
    }
    else
    {
        NFC_ERROR("HUAWEI_NFC: g_nfc_write_flag is true, do not write wifi token!\n");
    }

    return ret;
}
#if (FEATURE_ON == MBB_NFC_PARSE_URL)
/*****************************************************************************
 �� �� ��  : huawei_nfc_parse_url
 ��������  : ��URL�ֽ�Ϊprotocol + message ������
 �������  : char *pURL
 �������  : sURI_Info *pToken
 �� �� ֵ  : NA
 ���ú���  : NA
 ��������  : NA
*****************************************************************************/
static void huawei_nfc_parse_url(char* pURL, sURI_Info* pToken)
{
    uint32_t nID = 0;
    char* pTemp = NULL;

    if(NULL == pToken)
    {
        NFC_ERROR("HUAWEI_NFC: invalid params!\n");
        return;
    }

    for (nID = 0; nID < URI_RFU; nID++)
    {
        if (!strncmp( pURL, URI_Table[nID].aString, strlen(URI_Table[nID].aString)))
        {
            strncpy(pToken->protocol,
                URI_Table[nID].aString, NFC_URL_PROTOCOL_MAX_SIZE);
            pTemp = pURL + strlen(URI_Table[nID].aString);
            strncpy(pToken->URI_Message, pTemp, NFC_URL_MESSAGE_MAX_SIZE);
            return;
        }
    }
    NFC_ERROR("HUAWEI_NFC:ERROR (%s) does not mached any string!\n", pURL);
}
#endif
/*****************************************************************************
 �� �� ��  : huawei_nfc_show
 ��������  : ��ȡNFC����
 �������  : struct device *dev,
		     struct device_attribute *attr
 �������  : char *buf
 �� �� ֵ  : ����   :����ִ��ʧ��
             �Ǹ�   :����ִ�гɹ������ض������ֽ�����
 ���ú���  : huawei_nfc_i2c_read_wifi_url_token
 ��������  : NA
*****************************************************************************/
static ssize_t huawei_nfc_show(struct device* dev,
                                struct device_attribute* attr,
                                char* buf)
{
    uint16_t         ret  = 0;
    sWifiUrlToken    token;
    huawei_nfc_token nfc_token;
    /* get mutex lock*/
    down(&nfc_operate_mutex);
    if(FALSE == huawei_nfc_is_initialized())
    {
        NFC_ERROR("HUAWEI_NFC: nfc chip have not initialized yet!\n");
        up(&nfc_operate_mutex);
        return -EAGAIN;
    }

    memset(&token, '\0', sizeof(sWifiUrlToken));
    memset(&nfc_token, '\0', sizeof(huawei_nfc_token));
   /*��ȡWiFi��URL��Ϣ*/
    ret = huawei_nfc_i2c_read_wifi_url_token(&token);

    if (SUCCESS != ret)
    {
        NFC_ERROR("HUAWEI_NFC:ERROR read ndef (%d)!\n", ret);
        up(&nfc_operate_mutex);
        return -EFAULT;
    }

    strncpy(nfc_token.NetworkSSID, token.WifiToken.NetworkSSID,
        HUAWEI_NFC_MAX_SSID_SIZE);
    strncpy(nfc_token.NetworkKey, token.WifiToken.NetworkKey,
        HUAWEI_NFC_MAX_PWD_SIZE);
    strncpy(nfc_token.NetSecType, token.WifiToken.AuthenticationType,
        HUAWEI_NFC_MAX_SEC_SIZE);
    strncpy(nfc_token.URL, token.URLToken.protocol,
        HUAWEI_NFC_MAX_URL_SIZE);
    strncat(nfc_token.URL, token.URLToken.URI_Message,
        (HUAWEI_NFC_MAX_URL_SIZE - strlen(token.URLToken.protocol)));
    memcpy(buf, (char*)&nfc_token, sizeof(huawei_nfc_token));
    up(&nfc_operate_mutex);
    return sizeof(huawei_nfc_token);
}
/*****************************************************************************
 �� �� ��  : huawei_nfc_store
 ��������  : д��NFC����
 �������  : struct device *dev,
            struct device_attribute *attr
 �������  : char *buf
 �� �� ֵ  : ����     :����ִ��ʧ��
            �Ǹ�     :����ִ�гɹ�������д����ֽ���
 ���ú���  : huawei_nfc_i2c_write_wifi_url_token
 ��������  : NA
*****************************************************************************/
static ssize_t huawei_nfc_store(struct device* dev,
                                struct device_attribute* attr,
                                const char* buf,
                                size_t count)
{
    uint16_t         ret = 0;
    sWifiUrlToken    token ;
    huawei_nfc_token nfc_token ;

    down(&nfc_operate_mutex);
    if(FALSE == huawei_nfc_is_initialized())
    {
        NFC_ERROR("HUAWEI_NFC: nfc chip have not initialized yet!\n");
        up(&nfc_operate_mutex);
        return -EAGAIN;
    }

    if(NULL == buf)
    {
        NFC_ERROR("HUAWEI_NFC: buf is null!\n");
        up(&nfc_operate_mutex);
        return -EINVAL;
    }

    if(count > sizeof(huawei_nfc_token))
    {
        NFC_ERROR("HUAWEI_NFC:invalid input params!\n");
        up(&nfc_operate_mutex);
        return -EINVAL;
    }

    memset(&token, '\0', sizeof(sWifiUrlToken));
    memset(&nfc_token, '\0', sizeof(huawei_nfc_token));
    memcpy(&nfc_token, buf, sizeof(huawei_nfc_token));

    /*������ݳ��ȵ���Ч��*/
    if ( (strlen(nfc_token.NetworkSSID) > HUAWEI_NFC_MAX_SSID_SIZE)
         || (strlen(nfc_token.NetworkKey) >  HUAWEI_NFC_MAX_PWD_SIZE)
         || (strlen(nfc_token.URL) > HUAWEI_NFC_MAX_URL_SIZE)
         || (strlen(nfc_token.NetSecType) > HUAWEI_NFC_MAX_SEC_SIZE))
    {
        NFC_ERROR("HUAWEI_NFC:nfc_token length is error!\n");
        up(&nfc_operate_mutex);
        return -EINVAL;
    }
    else
    {
        strncpy(token.WifiToken.NetworkSSID, nfc_token.NetworkSSID,
            HUAWEI_NFC_MAX_SSID_SIZE);
        strncpy(token.WifiToken.NetworkKey, nfc_token.NetworkKey,
            HUAWEI_NFC_MAX_PWD_SIZE);
        strncpy(token.WifiToken.AuthenticationType, nfc_token.NetSecType,
            HUAWEI_NFC_MAX_SEC_SIZE);
#if (FEATURE_ON == MBB_NFC_PARSE_URL)
        huawei_nfc_parse_url(nfc_token.URL,&token.URLToken);
#else
        /*URLΪ�̶�ֵ����������д��*/
        strncpy(token.URLToken.protocol, URI_ID_0x04_STRING,
            NFC_URL_PROTOCOL_MAX_SIZE);
        strncpy(token.URLToken.URI_Message,
            HUAWEI_NFC_URL_MESSAGE, NFC_URL_MESSAGE_MAX_SIZE);
#endif
    }

    /*дWIFI��URL��Ϣ��NDEF*/
    ret = huawei_nfc_i2c_write_wifi_url_token(&token);

    if (SUCCESS != ret)
    {
        NFC_ERROR("HUAWEI_NFC:ERROR write ndef (%d)!\n", ret);
        up(&nfc_operate_mutex);
        return -EFAULT;
    }

#if(FEATURE_ON == MBB_FACTORY)
    /*�������Ƭ�汾����д�����Ϣ��������*/
    memset(&g_FactoryVerifyInfo, '\0', sizeof(sWifiUrlToken));
    memcpy(&g_FactoryVerifyInfo, &token, sizeof(sWifiUrlToken));
#endif

    NFC_INFO("HUAWEI_NFC:write successfully!\n");
    up(&nfc_operate_mutex);
    return count;
}

static DEVICE_ATTR(nfc_dev, S_IRUGO | S_IWUSR, huawei_nfc_show, huawei_nfc_store);

/*****************************************************************************
 �� �� ��  : huawei_nfc_irq_handler
 ��������  : NFC �жϴ�����
 �������  : int irq     void *handle
 �������  : NA
 �� �� ֵ  : static rqreturn_t �ж���Ӧ���
 ���ú���  :
 ��������  :
*****************************************************************************/

static irqreturn_t huawei_nfc_irq_handler(int irq, void* handle)
{
    /*�ӳ�ʱ��*/
    wake_lock_timeout(&g_huawei_nfc_device->nfc_wake_lock,
                      (long)msecs_to_jiffies(HUAWEI_NFC_INT_WAKE_TIMEOUT));

    disable_irq_nosync(g_huawei_nfc_client->irq);

    schedule_work(&g_huawei_nfc_device->work);

    return IRQ_HANDLED;
}

/*****************************************************************************
 �� �� ��  : huawei_nfc_netlink_report
 ��������  : NFC֪ͨӦ�û���WiFi
 �������  : NA
 �������  : NA
 �� �� ֵ  : void
 ���ú���  : huawei_nfc_i2c_work_func
 ��������  : device_event_report
*****************************************************************************/
static void huawei_nfc_netlink_report(void)
{
    DEVICE_EVENT stusbEvent = {0};
    stusbEvent.device_id = DEVICE_ID_NFC;
    stusbEvent.event_code = NFC_INTR;
    stusbEvent.len = 0;
    (void)device_event_report(&stusbEvent, sizeof(DEVICE_EVENT));
}


/*****************************************************************************
 �� �� ��  : huawei_nfc_write_info
 ��������  : �˽ӿ�����AT�ӿڵ��ã�д��NFC�����Ϣ
 �������  : huawei_nfc_token *nfc_info NFC��Ϣ�ṹ��
 �������  : NA
 �� �� ֵ  : ERROR    :����ִ��ʧ��
             SUCCESS  :����ִ�гɹ�
 ���ú���  : huawei_nfc_i2c_write_wifi_url_token
 ��������  : ATģ��
*****************************************************************************/
unsigned short huawei_nfc_write_info(huawei_nfc_token *nfc_info)
{
    sWifiUrlToken token;
    uint16_t      ret = 0;

    if(NULL == nfc_info)
    {
        NFC_ERROR("HUAWEI_NFC:ERROR nfc_info is null!\n");
        return ERROR;
    }
    down(&nfc_operate_mutex);
    if((strlen(nfc_info->NetworkSSID) > HUAWEI_NFC_MAX_SSID_SIZE) || 
        (strlen(nfc_info->NetworkKey) > HUAWEI_NFC_MAX_PWD_SIZE))
    {
        NFC_ERROR("HUAWEI_NFC:ERROR nfc_info size is error!\n");
        up(&nfc_operate_mutex);
        return ERROR;
    }

    if(FALSE == huawei_nfc_is_initialized())
    {
        NFC_ERROR("HUAWEI_NFC: nfc chip have not initialized yet!\n");
        up(&nfc_operate_mutex);
        return ERROR;
    }

    g_nfc_write_flag = FALSE;    /*д֮ǰ��ΪFALSE*/

    memset(&token, '\0', sizeof(sWifiUrlToken));

    strncpy(token.WifiToken.NetworkSSID, nfc_info->NetworkSSID,
            HUAWEI_NFC_MAX_SSID_SIZE);
    strncpy(token.WifiToken.NetworkKey, nfc_info->NetworkKey,
            HUAWEI_NFC_MAX_PWD_SIZE);

    strncpy(token.WifiToken.AuthenticationType, nfc_info->NetSecType,
            HUAWEI_NFC_MAX_SEC_SIZE);
    /*URLΪ�̶�ֵ����������д��*/
    strncpy(token.URLToken.protocol, URI_ID_0x04_STRING,
        NFC_URL_PROTOCOL_MAX_SIZE);
    strncpy(token.URLToken.URI_Message,
        HUAWEI_NFC_URL_MESSAGE, NFC_URL_MESSAGE_MAX_SIZE);

    /*дWIFI��URL��Ϣ��NDEF*/
    ret = huawei_nfc_i2c_write_wifi_url_token(&token);

    g_nfc_write_flag = TRUE;    /*д֮����ΪTRUE*/

#if(FEATURE_ON == MBB_FACTORY)
    /*�������Ƭ�汾����д�����Ϣ��������*/
    memset(&g_FactoryVerifyInfo, '\0', sizeof(sWifiUrlToken));
    memcpy(&g_FactoryVerifyInfo, &token, sizeof(sWifiUrlToken));
#endif

    if (SUCCESS != ret)
    {
        NFC_ERROR("HUAWEI_NFC:ERROR write ndef file failed!\n");
        up(&nfc_operate_mutex);
        return ERROR;
    }
    up(&nfc_operate_mutex);
    return SUCCESS;
}

/*****************************************************************************
 �� �� ��  : huawei_nfc_read_info
 ��������  : �˽ӿ�����AT�ӿڵ��ã���ȡNFC�����Ϣ
 �������  : huawei_nfc_token *nfc_info NFC��Ϣ�ṹ��
 �������  : NA
 �� �� ֵ  : ERROR    :����ִ��ʧ��
             SUCCESS  :����ִ�гɹ�
 ���ú���  : huawei_nfc_i2c_read_wifi_url_token
 ��������  : ATģ��
*****************************************************************************/
unsigned short huawei_nfc_read_info(huawei_nfc_token *nfc_info)
{
    sWifiUrlToken    token;
    huawei_nfc_token nfc_token;
    uint16_t         ret = 0;

    if(NULL == nfc_info)
    {
        NFC_ERROR("HUAWEI_NFC:ERROR nfc_info is null!\n");
        return ERROR;
    }
    down(&nfc_operate_mutex);
    if(FALSE == huawei_nfc_is_initialized())
    {
        NFC_ERROR("HUAWEI_NFC: nfc chip have not initialized yet!\n");
        up(&nfc_operate_mutex);
        return ERROR;
    }

    memset(&token, '\0', sizeof(sWifiUrlToken));
    memset(&nfc_token, '\0', sizeof(huawei_nfc_token));

    ret = huawei_nfc_i2c_read_wifi_url_token(&token);

    if (SUCCESS != ret)
    {
        NFC_ERROR("HUAWEI_NFC:ERROR read ndef file failed!\n");
        up(&nfc_operate_mutex);
        return ERROR;
    }

    strncpy(nfc_token.NetworkSSID, token.WifiToken.NetworkSSID,
        HUAWEI_NFC_MAX_SSID_SIZE);
    strncpy(nfc_token.NetworkKey, token.WifiToken.NetworkKey,
        HUAWEI_NFC_MAX_PWD_SIZE);
    memcpy((void *)nfc_info, (void *)&nfc_token, sizeof(huawei_nfc_token));
    up(&nfc_operate_mutex);
    return SUCCESS;
}
/*****************************************************************************
 �� �� ��  : huawei_nfc_rf_enable
 ��������  : ����nfc�򿪹ر�
 �������  : rf_enable rf_enableֵ
 �������  : NA
 �� �� ֵ  : ��0    :����ִ��ʧ��
             0      :����ִ�гɹ�
 ���ú���  : M24SR_RFEnable
 ��������  : huawei_nfc_enable_store
*****************************************************************************/
int huawei_nfc_rf_enable(unsigned char rf_enable)
{
    uint16_t ret = 0;

    if((HUAWEI_NFC_DISABLE_VALUE != rf_enable) && (HUAWEI_NFC_ENABLE_VALUE != rf_enable))
    {
        NFC_ERROR("HUAWEI_NFC:huawei_nfc_rf_enable input parameter is error!\n");
        return -1;
    }

    ret = M24SR_RFEnable(rf_enable);

    if((M24SR_ERROR_I2CTIMEOUT == ret) || (M24SR_ERROR_CRC == ret))
    {
        NFC_ERROR("HUAWEI_NFC:M24SR_RFEnable failed!\n");
        return -1;
    }

    ret = M24SR_Deselect();

    if(M24SR_ACTION_COMPLETED != ret)
    {
        NFC_ERROR("HUAWEI_NFC:M24SR_Deselect failed!\n");
        return -1;
    }

    return 0;
}

/*****************************************************************************
 �� �� ��  : huawei_nfc_get_rf_enable_status
 ��������  : ��ȡRF ENABLE״̬
 �������  : NA
 �������  : NA
 �� �� ֵ  : 0-NFC RF READ�رգ�1-NFC RF READ�򿪣�-1-��ȡ״̬ʧ��
 ���ú���  : M24SR_ReadBinary/M24SR_KillSession/M24SR_SelectApplication
 ��������  : huawei_nfc_enable_show
*****************************************************************************/

int huawei_nfc_get_rf_enable_status(void)
{
    uint16_t status = 0;
    unsigned char enable_status = 0;
    int ret = 0;

    status = M24SR_KillSession();

    if(M24SR_ACTION_COMPLETED != status)
    {
        printk(KERN_ERR "[%s : %d] : M24SR_KillSession failed!!!\n",__func__,__LINE__);
        return -1;
    }

    (void)M24SR_SelectApplication();
    (void)M24SR_SelectSystemfile();

    status = M24SR_ReadBinary ( HUAWEI_NFC_RF_DISABLE_OFFSET , HUAWEI_NFC_READ_RF_DISABLE_BYTES , &enable_status );

    if((M24SR_ERROR_I2CTIMEOUT == status) || (M24SR_ERROR_CRC == status))
    {
        printk(KERN_ERR "M24SR_ReadBinary failed!!!\n");
        return -1;
    }

    status = M24SR_Deselect();

    if(M24SR_ACTION_COMPLETED != status)
    {
        printk(KERN_ERR "M24SR_Deselect failed!!!\n");
        return -1;
    }

    if(HUAWEI_NFC_DISABLE_VALUE == enable_status)
    {
        ret = 0;
    }
    else if(HUAWEI_NFC_ENABLE_VALUE == enable_status)
    {
        ret = 1;
    }
    else
    {
        printk(KERN_ERR "enable_status is other value!!!\n");
        ret = -1;
    }

    return ret;
}
/*****************************************************************************
 �� �� ��  : huawei_nfc_enable_show
 ��������  : ��ȡNFCʹ��״̬
 �������  : struct device *dev,
            struct device_attribute *attr
 �������  : char *buf
 �� �� ֵ  : ���ض������ֽ���
 ���ú���  : huawei_nfc_get_rf_enable_status
 ��������  : NA
*****************************************************************************/
static ssize_t huawei_nfc_enable_show(struct device* dev,
                                                    struct device_attribute* attr,
                                                    char* buf)
{
    int nfc_enable_status;

    nfc_enable_status = huawei_nfc_get_rf_enable_status();

    if(-1 == nfc_enable_status)
    {
        printk(KERN_ERR "huawei_nfc_get_rf_enable_status error!!!\n");
        return -1;
    }

    if(NULL == buf)
    {
        printk(KERN_ERR "buf is null!!!\n");
        return -1;
    }

    return snprintf(buf, HUWAEI_NFC_ENABLE_BUFF_SIZE, "%u\n", nfc_enable_status);
}

/*****************************************************************************
 �� �� ��  : huawei_nfc_enable_store
 ��������  : ����NFC�򿪻��߹ر�
 �������  : struct device *dev,
            struct device_attribute *attr
 �������  : char *buf
 �� �� ֵ  : ����д����ֽ���
 ���ú���  : huawei_nfc_rf_enable
 ��������  : NA
*****************************************************************************/
static ssize_t huawei_nfc_enable_store(struct device* dev,
                                                   struct device_attribute* attr,
                                                   const char* buf,
                                                   size_t count)
{
    unsigned long status;
    ssize_t ret = -EINVAL;
    int nfc_enable_result;

    ret = kstrtoul(buf, 10, &status);

    if(ret)
    {
        printk(KERN_ERR "kstrtoul error!\n");
        return ret;
    }

    if(HUAWEI_NFC_ENABLE_OFF == status)
    {
        nfc_enable_result = huawei_nfc_rf_enable(HUAWEI_NFC_DISABLE_VALUE);

        if(-1 == nfc_enable_result)
        {
            printk(KERN_ERR "huawei_nfc_enable_store : enable off error!\n");
            return -1;
        }
    }
    else if(HUAWEI_NFC_ENABLE_ON == status)
    {
        nfc_enable_result = huawei_nfc_rf_enable(HUAWEI_NFC_ENABLE_VALUE);

        if(-1 == nfc_enable_result)
        {
            printk(KERN_ERR "huawei_nfc_enable_store : enable on error!\n");
            return -1;
        }
    }
    else
    {
        printk(KERN_ERR "huawei_nfc_enable_store : input value is error!\n");
    }
    return count;
}

static DEVICE_ATTR(nfc_enable, S_IRUGO | S_IWUSR, huawei_nfc_enable_show, huawei_nfc_enable_store);

/*****************************************************************************
 �� �� ��  : huawei_nfc_i2c_work_func
 ��������  : NFCģ�����Ϣ����work�����ڴ���nfc��д�¼�
 �������  : struct work_struct *w
 �������  : NA
 �� �� ֵ  : NA
 ���ú���  :
 ��������  :
*****************************************************************************/
static void huawei_nfc_i2c_work_func(struct work_struct* w)
{
#if(FEATURE_ON == MBB_FACTORY)
    char* p = NULL;
    uint16_t ret = 0;
    uint16_t count = 0;

    if (FALSE == huawei_nfc_is_initialized())
    {
        NFC_ERROR("HUAWEI_NFC: NFC chip not initialized yet!\n");
        enable_irq(g_huawei_nfc_device->nfc_i2c_client->irq);
        wake_unlock(&g_huawei_nfc_device->nfc_wake_lock);
        return;
    }
    p = g_FactoryVerifyInfo.WifiToken.NetworkKey;
    /*���һ���ֽڼ�1����֤�жϱ�����*/
    *(p + strlen(p) - 1) += 0x01;
    do
    {
        /*The interrupt is triggered by opening RF read session.
        When the token exists and is assigned to one interface (RF or I2C), the
        M24SR02-Y cannot communicate with the other host. So we wait 500ms so that RF could finish its job.*/
        mdelay(HUAWEI_NFC_FACTORY_DELAY);

        ret = TT4_WriteWifiURLToken(&g_FactoryVerifyInfo);

        if (SUCCESS != ret )
        {
            nfc_print_message(HUAWEI_NFC_ERR, "HUAWEI_NFC: nfc write NDEF failed\n");
        }
        /*If failed, write again.To make sure NFC wirte successfully. Limited within 10 times.*/
    }
    while (!ret && ++count < 10);

    enable_irq(g_huawei_nfc_device->nfc_i2c_client->irq);
    wake_unlock(&g_huawei_nfc_device->nfc_wake_lock);
#else
    huawei_nfc_netlink_report();
    enable_irq(g_huawei_nfc_device->nfc_i2c_client->irq);
#endif
    NFC_DEBUG("HUAWEI_NFC: An interrupt occured!\n");
}

/*****************************************************************************
 �� �� ��  : huawei_nfc_get_handle
 ��������  : ��ȡ����NFC�ľ��
 �������  : ��
 �������  : ��
 �� �� ֵ  : �������
 ���ú���  : NA
 ��������  : NA
*****************************************************************************/
struct i2c_client* huawei_nfc_get_handle(void)
{
    return g_huawei_nfc_client;
}

/*****************************************************************************
 �� �� ��  : huawei_nfc_probe
 ��������  : NFC �������غ���
 �������  : struct i2c_client* client
             const struct i2c_device_id* id
 �������  : NA
 �� �� ֵ  : 0-�������سɹ�����0-��������ʧ��
 ���ú���  : NA
 ��������  : NA
*****************************************************************************/
static int huawei_nfc_probe(struct i2c_client* client, const struct i2c_device_id* id)
{
    int32_t ret = 0;
    u16 init_result = ERROR;
    struct i2c_client* nfc_client = NULL;
    struct huawei_nfc_device* nfc_device = NULL;
    struct device_node* node = client->dev.of_node;
    client->irq = gpio_to_irq(NFC_INT_GPIO);

    dev_info(&client->dev, "HUAWEI_NFC:%s:***huawei_nfc_probe***\n", __func__);

    if (!node)
    {
        dev_err(&client->dev,
            "HUAWEI_NFC:%s: device tree information missing\n", __func__);
        return - ENODEV;
    }

    if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE_DATA)) //�ж�����������
    {
        dev_err(&client->dev, "HUAWEI_NFC:%s: SMBUS_BYTE_DATA unsupported\n", __func__);
        return - EIO;
    }

    nfc_device = devm_kzalloc(&client->dev, sizeof(struct huawei_nfc_device), GFP_KERNEL);

    if (!nfc_device)
    {
        dev_err(&client->dev, "HUAWEI_NFC:%s: devm_kzalloc failed\n", __func__);
        return - ENOMEM;
    }

    memset(nfc_device, 0x0, sizeof(struct huawei_nfc_device));

    nfc_device->nfc_i2c_client = client;

    i2c_set_clientdata(client, nfc_device);

    g_huawei_nfc_client = nfc_device->nfc_i2c_client;
    g_huawei_nfc_device = nfc_device;

    /* ��ʼ�����ڻ����жϵĳ�ʱ��*/
    wake_lock_init(&(nfc_device->nfc_wake_lock), WAKE_LOCK_SUSPEND, "HUAWEI_NFC_WORK");
    if (client->irq)
    {
        NFC_ERROR("HUAWEI_NFC: trying to request irq: %s-%d\n", client->name, client->irq);
        ret = request_irq(client->irq,
                          huawei_nfc_irq_handler,
                          IRQF_NO_SUSPEND | IRQF_TRIGGER_FALLING,
                          client->name,
                          nfc_client);

        if (ret < 0)
        {
            NFC_ERROR("HUAWEI_NFC: Can't allocate irq %d, ret %d\n", client->irq, ret);
            ret = - EBUSY;
            goto err_request_irq;
        }
    }
    INIT_WORK(&(nfc_device->work), huawei_nfc_i2c_work_func);

    /*Create nfc device class.--> /sys/class/huawei_nfc.*/
    g_nfc_class = class_create(THIS_MODULE, "huawei_nfc");
    if(IS_ERR(g_nfc_class))
    {
        ret = PTR_ERR(g_nfc_class);
        NFC_ERROR("HUAWEI_NFC: Create class failed!\n");
        goto err_create_class;
    }

    /*Create NFC device --> /sys/class/huawei_nfc/nfc.*/
    nfc_device->nfc_dev = device_create(g_nfc_class, NULL, MKDEV(0, 0), "%s", "nfc");
    if (IS_ERR(nfc_device->nfc_dev))
    {
        ret = PTR_ERR(nfc_device->nfc_dev);
        NFC_ERROR("HUAWEI_NFC: Create device failed!\n");
        goto  err_create_device;
    }

    /*Create NFC sysfs device node --> /sys/class/huawei_nfc/nfc/nfc_dev.*/
    ret = device_create_file(nfc_device->nfc_dev, &dev_attr_nfc_dev);
    if (ret)
    {
        NFC_ERROR("HUAWEI_NFC: Create sysfs attributes failed!\n");
        goto err_create_sysfs_nfc_dev;
    }

    /*Create NFC sysfs device node --> /sys/class/huawei_nfc/nfc/nfc_enable.*/
    ret = device_create_file(nfc_device->nfc_dev, &dev_attr_nfc_enable);
    if (ret)
    {
        NFC_ERROR("HUAWEI_NFC: Create sysfs attributes failed!\n");
        goto err_create_sysfs_nfc_enable;
    }

    NFC_DEBUG("HUAWEI_NFC: Init NFC start!\n");

    /*Init NFC chip.*/
    init_result = TT4_Init();

    if(SUCCESS == init_result)
    {
        g_nfc_init = TRUE;
        NFC_DEBUG("HUAWEI_NFC: Init NFC finish!\n");
    }
    else
    {
        NFC_ERROR("HUAWEI_NFC: Init NFC ERROR!\n");
    }

    return 0;
err_create_sysfs_nfc_enable:
    device_remove_file(nfc_device->nfc_dev, &dev_attr_nfc_dev);
err_create_sysfs_nfc_dev:
    device_destroy(g_nfc_class, MKDEV(0, 0));
err_create_device:
    class_destroy(g_nfc_class);
err_create_class:
err_request_irq:
    devm_kfree(&client->dev, nfc_device);
    nfc_device = NULL;
    return ret;
}

static const struct i2c_device_id nfc_device_id[] =
{
    {
        .name = NFC_IC_DRVNAME,
    },

    {

    },
};

static const struct of_device_id nfc_device_match[] =
{
    {
        .compatible = NFC_COMPATIBLE_NAME, 
    },

    {

    },
};


static struct i2c_driver huawei_nfc_i2c_driver =
{
    .driver    = 
    {
        .name           = NFC_IC_DRVNAME,
        .owner          = THIS_MODULE,
        .of_match_table = nfc_device_match,
    },

    .probe          = huawei_nfc_probe,   /*i2c_client��i2c_driverƥ��ʱ����*/
    .id_table       = nfc_device_id,
};

static int __init nfc_init(void)
{
    return i2c_add_driver(&huawei_nfc_i2c_driver);
}
module_init(nfc_init);

static void __exit nfc_exit(void)
{
    i2c_del_driver(&huawei_nfc_i2c_driver);
    return;
}
module_exit(nfc_exit);

MODULE_DESCRIPTION(NFC_IC_DRVNAME "NFC");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("i2c:" NFC_IC_DRVNAME);
