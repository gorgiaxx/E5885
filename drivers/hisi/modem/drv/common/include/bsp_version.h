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


#ifndef __BSP_VERSION_H__
#define __BSP_VERSION_H__

#ifdef __cplusplus
extern "C" {
#endif

#if (defined(__OS_VXWORKS__) || defined(__OS_RTOSCK__) || defined(__KERNEL__)) ||defined(__OS_RTOSCK_SMP__)
#include <mdrv_version.h>
#endif
#include "product_config.h"
#include <osl_types.h>
#include <drv_nv_def.h>
#include <bsp_trace.h>
#include <bsp_shared_ddr.h>

#ifndef VER_ERROR
#define VER_ERROR (-1)
#endif

#ifndef VER_OK
#define VER_OK 0
#endif

#if (FEATURE_ON == MBB_DLOAD)
#define DLOAD_VERSION      "2.0"                   /* ����Э��汾 */
#define  ver_print_info(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_INFO, BSP_MODU_VERSION, "[version]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))
#endif

#define  ver_print_error(fmt, ...) (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_VERSION, "[version]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))

#define HW_VER_INVALID                          (BSP_U32)0XFFFFFFFF
#define HW_VER_UDP_MASK				(BSP_U32)0XFF000000 	/*UDP��������*/
#define HW_VER_UDP_UNMASK			(BSP_U32)(~HW_VER_UDP_MASK) /*UDP����ȡ��*/

#define HW_VER_V711_UDP				(BSP_U32)0X71000000 /*V711 UDP*/
#define HW_VER_V750_UDP				(BSP_U32)0X75000000 /*V7R5 UDP*/
#define HW_VER_V722_UDP				(BSP_U32)0X72000000 /*V722 UDP*/
#define HW_VER_K3V5_UDP				(BSP_U32)0X35000000 /*K3V5 UDP*/
#define HW_VER_PXXX                                 (BSP_U32)0XFF000000 /*P532*/

#define HW_VER_PRODUCT_E5785Lh_92a           (BSP_U32)0X00000001 /*V722 E5785Lh-92a*/
#define HW_VER_PRODUCT_E5783h_92a            (BSP_U32)0X00000017 /*V722 E5783h-92a*/
#define HW_VER_PRODUCT_B520s_93a           (BSP_U32)0X00040021 /*V722 B520s-93a*/
/*Telematic ME919Bs-127a Ӳ���汾�Ŷ���*/
#define HW_VER_PRODUCT_ME919Bs_127a_A  (BSP_U32)0X00050000 /*V722 ME919Bs-127a A�汾*/
#define HW_VER_PRODUCT_ME919Bs_127a_B  (BSP_U32)0X00050001 /*V722 ME919Bs-127a B�汾*/  
#define HW_VER_PRODUCT_ME919Bs_127a_C  (BSP_U32)0X00050002 /*V722 ME919Bs-127a C�汾*/  
#define HW_VER_PRODUCT_ME919Bs_127a_D  (BSP_U32)0X00050003 /*V722 ME919Bs-127a D�汾*/ 

/*Telematic ME919Bs-567a Ӳ���汾�Ŷ���*/
#define HW_VER_PRODUCT_ME919Bs_567a_A  (BSP_U32)0X00050004 /*V722 ME919Bs-567a A�汾*/
#define HW_VER_PRODUCT_ME919Bs_567a_B  (BSP_U32)0X00050005 /*V722 ME919Bs-567a B�汾*/
#define HW_VER_PRODUCT_ME919Bs_567a_C  (BSP_U32)0X00050006 /*V722 ME919Bs-567a C�汾*/
#define HW_VER_PRODUCT_ME919Bs_567a_D  (BSP_U32)0X00050007 /*V722 ME919Bs-567a D�汾*/

/*Telematic ME919Bs-823a Ӳ���汾�Ŷ���*/
#define HW_VER_PRODUCT_ME919Bs_823a_A  (BSP_U32)0X00050008 /*V722 ME919Bs-823a A�汾*/
#define HW_VER_PRODUCT_ME919Bs_823a_B  (BSP_U32)0X00050009 /*V722 ME919Bs-823a B�汾*/
#define HW_VER_PRODUCT_ME919Bs_823a_C  (BSP_U32)0X00050010 /*V722 ME919Bs-823a C�汾*/
#define HW_VER_PRODUCT_ME919Bs_823a_D  (BSP_U32)0X00050011 /*V722 ME919Bs-823a D�汾*/

/*Telematic ME919Bs-727a Ӳ���汾�Ŷ���*/
#define HW_VER_PRODUCT_ME919Bs_727a_A  (BSP_U32)0X00050012 /*V722 ME919Bs-727a A�汾*/
#define HW_VER_PRODUCT_ME919Bs_727a_B  (BSP_U32)0X00050013 /*V722 ME919Bs-727a B�汾*/
#define HW_VER_PRODUCT_ME919Bs_727a_C  (BSP_U32)0X00050014 /*V722 ME919Bs-727a C�汾*/
#define HW_VER_PRODUCT_ME919Bs_727a_D  (BSP_U32)0X00050015 /*V722 ME919Bs-727a D�汾*/

/*Telematic ME909Bs-566N Ӳ���汾�Ŷ���*/
#define HW_VER_PRODUCT_ME909Bs_566N_A  (BSP_U32)0X00050016 /*V722 ME909Bs-566N A�汾*/
#define HW_VER_PRODUCT_ME909Bs_566N_B  (BSP_U32)0X00050017 /*V722 ME909Bs-566N B�汾*/
#define HW_VER_PRODUCT_ME909Bs_566N_C  (BSP_U32)0X00050018 /*V722 ME909Bs-566N C�汾*/
#define HW_VER_PRODUCT_ME909Bs_566N_D  (BSP_U32)0X00050019 /*V722 ME909Bs-566N D�汾*/

/*Telematic ME909Bs-123N Ӳ���汾�Ŷ���*/
#define HW_VER_PRODUCT_ME909Bs_123N_A  (BSP_U32)0X00050020 /*V722 ME909Bs-123N A�汾*/
#define HW_VER_PRODUCT_ME909Bs_123N_B  (BSP_U32)0X00050021 /*V722 ME909Bs-123N B�汾*/
#define HW_VER_PRODUCT_ME909Bs_123N_C  (BSP_U32)0X00050022 /*V722 ME909Bs-123N C�汾*/
#define HW_VER_PRODUCT_ME909Bs_123N_D  (BSP_U32)0X00050023 /*V722 ME909Bs-123N D�汾*/

/*Telematic ME909Bs-827N Ӳ���汾�Ŷ���*/
#define HW_VER_PRODUCT_ME909Bs_827N_A  (BSP_U32)0X00050024 /*V722 ME909Bs-827N A�汾*/
#define HW_VER_PRODUCT_ME909Bs_827N_B  (BSP_U32)0X00050025 /*V722 ME909Bs-827N B�汾*/
#define HW_VER_PRODUCT_ME909Bs_827N_C  (BSP_U32)0X00050026 /*V722 ME909Bs-827N C�汾*/
#define HW_VER_PRODUCT_ME909Bs_827N_D  (BSP_U32)0X00050027 /*V722 ME909Bs-827N D�汾*/

/*Telematic ME919Bs_127bN Ӳ���汾�Ŷ���*/
#define HW_VER_PRODUCT_ME919Bs_127bN_A  (BSP_U32)0X00050032 /*V722 ME919Bs-127bN A�汾*/
#define HW_VER_PRODUCT_ME919Bs_127bN_B  (BSP_U32)0X00050033 /*V722 ME919Bs-127bN B�汾*/
#define HW_VER_PRODUCT_ME919Bs_127bN_C  (BSP_U32)0X00050034 /*V722 ME919Bs-127bN C�汾*/
#define HW_VER_PRODUCT_ME919Bs_127bN_D  (BSP_U32)0X00050035 /*V722 ME919Bs-127bN D�汾*/

/*Telematic ME919Bs-567bN Ӳ���汾�Ŷ���*/
#define HW_VER_PRODUCT_ME919Bs_567bN_A  (BSP_U32)0X00050036 /*V722 ME919Bs-567bN A�汾*/
#define HW_VER_PRODUCT_ME919Bs_567bN_B  (BSP_U32)0X00050037 /*V722 ME919Bs-567bN B�汾*/
#define HW_VER_PRODUCT_ME919Bs_567bN_C  (BSP_U32)0X00050038 /*V722 ME919Bs-567bN C�汾*/
#define HW_VER_PRODUCT_ME919Bs_567bN_D  (BSP_U32)0X00050039 /*V722 ME919Bs-567bN D�汾*/

/*Telematic ME919Bs-821bN Ӳ���汾�Ŷ���*/
#define HW_VER_PRODUCT_ME919Bs_821bN_A  (BSP_U32)0X00050040 /*V722 ME919Bs-821bN A�汾*/
#define HW_VER_PRODUCT_ME919Bs_821bN_B  (BSP_U32)0X00050041 /*V722 ME919Bs-821bN B�汾*/
#define HW_VER_PRODUCT_ME919Bs_821bN_C  (BSP_U32)0X00050042 /*V722 ME919Bs-821bN C�汾*/
#define HW_VER_PRODUCT_ME919Bs_821bN_D  (BSP_U32)0X00050043 /*V722 ME919Bs-821bN D�汾*/

/*Telematic MU809Bs-2 Ӳ���汾�Ŷ���*/
#define HW_VER_PRODUCT_MU809Bs_2_A     (BSP_U32)0X00050028 /*V722 MU809Bs-2 A�汾*/
#define HW_VER_PRODUCT_MU809Bs_2_B     (BSP_U32)0X00050029 /*V722 MU809Bs-2 B�汾*/
#define HW_VER_PRODUCT_MU809Bs_2_C     (BSP_U32)0X00050030 /*V722 MU809Bs-2 C�汾*/
#define HW_VER_PRODUCT_MU809Bs_2_D     (BSP_U32)0X00050031 /*V722 MU809Bs-2 D�汾*/

/*E5787Ph-92a Ӳ���汾�Ŷ���*/
#define HW_VER_PRODUCT_E5787Ph_92a     (BSP_U32)0X00000005 /*V722 E5787Ph-92a*/
#define HW_VER_PRODUCT_E5785Lh_22c     (BSP_U32)0X00000003 /*V722 E5785Lh-22c*/
#define HW_VER_PRODUCT_E5787Ph_67a     (BSP_U32)0X00000011 /*V722 E5787Ph-67a*/
#define HW_VER_PRODUCT_B525s_23a       (BSP_U32)0X00040024 /*V722 B525s-23a*/
#define HW_VER_PRODUCT_B528s_23a       (BSP_U32)0X00040026 /*V722 B528s-23a*/
#define HW_VER_PRODUCT_E5785Ch_22c     (BSP_U32)0X00000007 /*V722 E5785Ch-22c*/
#define HW_VER_PRODUCT_B525s_95a       (BSP_U32)0X00040022 /*V722 B525s-95a*/
#define HW_VER_PRODUCT_B529s_23a       (BSP_U32)0X00040027 /*V722 B529s-23a*/
#define HW_VER_PRODUCT_E5785Lh_67a     (BSP_U32)0X00000015 /*V722 E5785Lh-67a*/
#define HW_VER_PRODUCT_R227h           (BSP_U32)0X00000023 /*V722 R227*/
#define HW_VER_PRODUCT_E5785Lh_23c     (BSP_U32)0X00000021 /*V722 E5785Lh-23c*/
/* B525S-65A Ӳ���汾�Ŷ��� */
#define HW_VER_PRODUCT_B525s_65a       (BSP_U32)0X00040025 /* V722 B525s-65a */
#define HW_VER_PRODUCT_E5885Ls_93a     (BSP_U32)0X00000013 /*V722 E5885Ls-93a*/
typedef enum
{
	CHIP_P531 = 0x0530,
	CHIP_P532 = 0x0532,
	CHIP_K3V3 = 0x3630,
	CHIP_K3V5 = 0x3650,
	CHIP_K3V6 = 0x3660,
	CHIP_K970 = 0x3670,
	CHIP_V8R5 = 0x6250,
	CHIP_V711 = 0x6921,
	CHIP_V722 = 0x6932,
	CHIP_V750 = 0x6950
}VERSION_CHIP_TYPE_E;

typedef enum
{
	PLAT_ASIC= 0x0,
	PLAT_FPGA = 0xa,
	PLAT_EMU = 0xe
}VERSION_PLAT_TYPE_E;

typedef enum{
	 BSP_BOARD_TYPE_BBIT    = 0,
	 BSP_BOARD_TYPE_SFT,
	 BSP_BOARD_TYPE_ASIC,
	 BSP_BOARD_TYPE_SOC,
	 BSP_BOARD_TYPE_MAX
}VERSION_BOARD_TYPE_E;

typedef enum
{
	DALLAS_BBIT = 0x1,
	V722_BBIT = 0x2,
	CHICAGO_BBIT = 0x3,
	BOSTON_BBIT = 0x4
}VERSION_BBIT_TYPE_E;

typedef enum
{
	PRODUCT_MBB= 0x0,
	PRODUCT_PHONE = 0x1,
	PRODUCT_ERROR = 0x2
}VERSION_PRODUCT_TYPE_E;

typedef enum
{
	PRODUCT_AUSTIN = 0x3650,
	PRODUCT_CHICAGO = 0x3660,
	PRODUCT_BOSTON = 0x3670,
	PRODUCT_DALLAS = 0x6250,
	PRODUCT_722 = 0x6932,
	PRODUCT_750 = 0x6950,
	PRODUCT_NOT_SUPPORT = 0xFFFF
}VERSION_PRODUCT_NAME_E;

typedef struct
{
	u32 board_id;                    /*Ӳ���汾�ţ�ͨ��hkadc��ȡ��NV��dts����*/
	u32 board_id_udp_masked;         /*���ο۰���Ϣ��Ӳ���汾�š�ioshare����*/
    u32 chip_version;                /*оƬ�汾��*/
    u16 chip_type;                   /*оƬ���ͣ���CHIP_V711=0x6921*/
	u8  plat_type;                   /*ƽ̨���ͣ���asic/proting/emu*/
	u8  board_type;                  /*ƽ̨���ͣ���BBIT SOC ASIC SFT*/
	u8  bbit_type;                   /*bbit ƽ̨����dalass bbit/722 BBIT/chicago bbit*/
	u8  product_type;                /*��Ʒ���ͣ���MBB/PHONE*/
    u16 product_name;                /*��Ʒ���ƣ���PRODUCT_722����722 porting/bbit/sft/udpͳһ����Ϊ722*/
/*�ڰ汾����Ϣ�ṹ�����һ����ڱ�ʾ��ǰ�����HKADC�ж�ȡ����ʵ
  Ӳ���Ӱ汾�ţ� NVͨ��board_id�޷�ͬʱ֧�ֶ��Ӳ���Ӱ汾��*/
#if (FEATURE_ON == MBB_COMMON)
    u8  real_hwIdSub;
#endif
    u32 reserved;
}BSP_VERSION_INFO_S;
#define VERSION_ISO_MAX_LEN  128
#define VERSION_WEBUI_MAX_LEN  128



/*****************************************************************************
*                                                                                                                               *
*            �����ṩ��version_balong.c(a/c)                                                          *
*                                                                                                                               *
******************************************************************************/

#if defined(__OS_VXWORKS__) || defined(__OS_RTOSCK__)
#define StrParamType (unsigned int)
#define MemparamType (int)
#else
#define StrParamType (int)
#define MemparamType (unsigned int)
#endif

#define VERSION_MAX_LEN 32



/*****************************************************************************
*                                                                                                                               *
*            �����ṩ��version.c(fastboot)                                                       *
*                                                                                                                               *
******************************************************************************/

typedef struct
{
    u16 vol_low;
    u16 vol_high;
}voltage_range;

#if (FEATURE_ON == MBB_COMMON)
#define HW_VER_PRODUCT_TYPE_MASK	(BSP_U32)0x00FF0000
#define HW_VER_PRODUCT_TYPE_OS		16

/*MBB�Ĳ�Ʒ��ͨ����Ver.A/B/C 2��Ӳ���Ӱ汾��*/
#define HW_MBB_SUB_VER_NR  (2)

/*M2M�Ĳ�Ʒ��ͨ����Ver.A/B/C/D 4��Ӳ���Ӱ汾��*/
#define HW_M2M_SUB_VER_NR  (4)
#endif
/* E5��ƷӲ���汾�Ŷ��� */
#ifdef BSP_CONFIG_BOARD_SOLUTION
/*��Ʒ��̬(������Ʒ����̬)ö��,�����ڴ���ֵ*/
typedef enum{
    PRODUCT_TYPE_M2M       = 0xFAAFFA01,  /*ģ��m2m��Ʒ��̬*/
    PRODUCT_TYPE_CE        = 0xFAAFFA02,  /*ģ��ce��Ʒ��̬*/
    PRODUCT_TYPE_TELEMATIC = 0xFAAFFA03,  /*ģ�鳵�ز�Ʒ��̬*/

    PRODUCT_TYPE_INVALID = 0xFAAFFAAF  /*��Ч��Ʒ��̬ħ��*/
}SOLUTION_PRODUCT_TYPE;
/*��Ʒ��̬��Ӳ��ID���ձ�ṹ��*/
typedef struct _SOLUTION_PRODUCT_INFO_T{
    SOLUTION_PRODUCT_TYPE product_type;
    unsigned int product_id;
}SOLUTION_PRODUCT_INFO_T;
#endif
/*****************************************************************************
*                                                                                                                               *
*            �����ṩ��adp_version.c(a/c)                                                              *
*                                                                                                                               *
******************************************************************************/

#ifndef isdigit
#define isdigit(c)      (((c) >= '0') && ((c) <= '9'))
#endif

#define CHIP_TYPE_MASK 0xffff0000
#define PLAT_TYPE_MASK 0x0000f000

#define MAX_VER_SECTION 8
#define VER_PART_LEN 3
#define VERC_PART_LEN 2

#ifndef VER_MAX_LENGTH
#define VER_MAX_LENGTH                  30
#endif

typedef struct
{
    unsigned char CompId;                                           /* ����ţ��μ�COMP_TYPE */
    unsigned char CompVer[VER_MAX_LENGTH+1];         /* ���汾���� 30 �ַ�+ \0 */
}VERSIONINFO;



/*****************************************************************************
*                                                                                                                               *
*            �����ṩ��virtual boardid����                                                          *
*                                                                                                                               *
******************************************************************************/

#define VIRTUAL_BOARDID_SET_OK      0x12345000
#define VIRTUAL_BOARDID_NO_SET      0x12345001
#define VIRTUAL_BOARDID_CMD_NULL    0x12345002
#define VIRTUAL_BOARDID_ERR_FORMAT  0x12345003
#define VIRTUAL_BOARDID_NV_NOBURN   0x12345004
#define VIRTUAL_BOARDID_SET_FLAG    0x12345005

#define MISC_VERSION_OFFSET 100

typedef struct  {
    unsigned err_code;
    unsigned virtual_boardid;
    unsigned timestamp;
    unsigned set_ok_flag;
} misc_ptn_version_info;

typedef enum {
	VIRTUAL_BOARDID_MISC_OK = 0,
	VIRTUAL_BOARDID_MISC_ERROR,
}virtual_boardid_misc_return_type;



/*****************************************************************************
*                                                                                                                               *
*            ����Ϊ����ͷ�ļ�����                                                              *
*                                                                                                                               *
******************************************************************************/

char * bsp_version_get_hardware(void);
char * bsp_version_get_product_inner_name(void);
char * bsp_version_get_product_out_name(void);

char * bsp_version_get_build_date_time(void);
char * bsp_version_get_chip(void);
char * bsp_version_get_firmware(void);
char * bsp_version_get_release(void);

int bsp_version_acore_init(void);
int bsp_version_ccore_init(void);
void bsp_version_ddr_init(void);
int bsp_version_init(void);
void mdrv_ver_init(void);

VERSION_CHIP_TYPE_E bsp_version_get_chip_type(void);
const BSP_VERSION_INFO_S* bsp_get_version_info(void);

int bsp_version_debug(void);

void update_version_boardid(void);
void set_virtual_boardid(char *virtual_boardid);
void show_virtual_boardid(void);
void clear_virtual_boardid(void);
#ifdef BSP_CONFIG_BOARD_SOLUTION
/*****************************************************************************
* ����  : bsp_get_solution_type
* ����  : ��ȡģ���Ӳ�Ʒ��̬(��������ӿ�)
      fastboot�иýӿ�ͨ���������ձ����
      A/C/M���ں���ʹ��ʱֱ�Ӷ�ȡ�����ڴ�
* ����  : void
* ���  : void
* ����  : MBB_PRODUCT_TYPE
*****************************************************************************/
SOLUTION_PRODUCT_TYPE bsp_get_solution_type(void);

/*****************************************************************************
* ����  : m2m_get_board_status
* ����  : ����ģ��ME919Bs/ME909Bs��Ʒ����Ӳ���Ӱ汾�Ż�ȡVBus�����Ϣ
* ����  : isEc ����ָ�룬ECǰ��ֵΪ0��EC��ֵΪ1
*         isVbus ����ָ�룬��֧��VBus��ֵΪ0��֧��VBus��ֵΪ1
* ���  : huawei_product_info.hwIdSub ����HKADC�Ӱ汾��ˢ��Ӳ���Ӱ汾����Ϣ
*****************************************************************************/
void m2m_get_board_status(int *isEC, int *isVbus);
#endif

#if (FEATURE_ON == MBB_DLOAD)
/*****************************************************************************
* ����  : mbb_version_get_board_type
* ����  : ��Ʒ�߻�ȡ��ƷӲ��IDʹ�ýӿ�
                     ��Ʒ�����������в���ʹ������ĺ�˼�ӿ�
* ����  : void
* ���  : void
* ����  : BOARD_TYPE_E
*****************************************************************************/
u32 mbb_version_get_board_type(void);


/*****************************************************************************
* �� �� ��  : bsp_get_web_version
*
* ��������  : ��ȡwebui_version
*
* �������  : ��
* �������  :
*
 �� �� ֵ  : webui_version�ַ�����ָ��
* �޸ļ�¼  :
*
*****************************************************************************/
void bsp_get_web_version(char *str,int len);


void bsp_get_iso_version(char *str, int len);

/*****************************************************************************
* �� �� ��   : bsp_version_get_hardware_no_subver
*
* ��������   : ��ȡӲ���汾��
*
* �������   : ��
* �������   : Ӳ���汾���ַ���ָ��
*
* �� �� ֵ   : 0��ȡ�ɹ�
             -1��ȡʧ��
*
* �޸ļ�¼  :
*
*****************************************************************************/
char * bsp_version_get_hardware_no_subver(void);

/*****************************************************************************
* ����	: huawei_set_update_info
* ����	: set update info 
* ����	: void
* ���	: void
* ����	: void
*
* ����       : ��
*
*****************************************************************************/
void huawei_set_update_info(char* str);

/*****************************************************************************
* ����	: huawei_get_update_info_times
* ����	: get update info times
* ����	: NA 
* ���	: times: num of upgrade 
* ����	: void 
*
* ����       : ��
*
*****************************************************************************/
void huawei_get_update_info_times(s32* times);

/*****************************************************************************
* ����	: huawei_get_spec_num_upinfo
* ����	: get info of once update 
* ����	: num : index of upgrade
* ���	: void
* ����	: 0��ȡ�ɹ�/-1��ȡʧ��
*
* ����       : ��
*
*****************************************************************************/
s32 huawei_get_spec_num_upinfo(char* str, s32 str_len, s32 num);
#endif
#ifdef __cplusplus
}
#endif

#endif

