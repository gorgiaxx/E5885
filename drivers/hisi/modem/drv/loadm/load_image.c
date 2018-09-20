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

#include "product_config.h"

#include <linux/io.h>
#include <linux/jiffies.h>
#include <linux/slab.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/zlib.h>
#include <linux/dma-mapping.h>
#include <asm/dma-mapping.h>
#include <asm/cacheflush.h>
#include <soc_onchiprom.h>
#include <bsp_shared_ddr.h>
#include <bsp_reset.h>
#include <bsp_sec.h>
#include <bsp_rfile.h>
#include <bsp_version.h>
#include <bsp_ddr.h>
#include <bsp_efuse.h>
#include "load_image.h"
#include "modem_dtb.h"

/* Dalls֮���ֻ���MBB�ںϴ��� */

#define SECBOOT_BUFLEN  (0x100000/8)	/*128KB*/

#define MODEM_IMAGE_PATH	"/modem_fw/"
#define VRL_SIZE					(0x1000)  /*VRL 4K*/

/* ����ȫOS��Ҫ��ȫ���أ�Ԥ�������ڴ棬������ϵͳ��ʱ�����к󣬵�����λʱ�������벻�������ڴ� */
static  u8 SECBOOT_BUFFER[SECBOOT_BUFLEN];

struct image_type_name
{
	enum SVC_SECBOOT_IMG_TYPE etype;
	u32 run_addr;
	u32 ddr_size;
	const char* name;
};

 struct image_type_name modem_images[] =
{
	{MODEM, DDR_MCORE_ADDR,			DDR_MCORE_SIZE,			"balong_modem.bin"},
	{HIFI,	DDR_HIFI_ADDR,			DDR_HIFI_SIZE,			"hifi.img"},/* Ԥ�� */
	{DSP,	DDR_TLPHY_IMAGE_ADDR,	DDR_TLPHY_IMAGE_SIZE,	"lphy.bin"},
	{TAS,	0,						0,						"tas.bin"},
	{WAS,	0,						0,						"was.bin"},
	{CAS,	0,						0,						"cas.bin"},	/* Ԥ�� */
	{MODEM_DTB,	DDR_MCORE_DTS_ADDR,	DDR_MCORE_DTS_SIZE,		"modem_dt.img"},
	{SOC_MAX,		0,				0,						""},
};

/*lint -save -e651 -e708 -e570 -e64 -e785*/
static DEFINE_MUTEX(load_proc_lock);
/*lint -restore */

static int get_image(struct image_type_name** image, enum SVC_SECBOOT_IMG_TYPE etype,u32 run_addr, u32 ddr_size)
{
	int i;
	struct image_type_name* img;

	img = modem_images;
	for(i=0; i<SOC_MAX; i++)
	{
		if(img->etype == etype)
		{
			break;
		}
		img++;
	}
	if(i == SOC_MAX)
	{
		sec_print_err("can not find image of type id %d\n", etype);
		return -ENOENT;
	}
	/*�����tas was����Ļ�Ҫ*/
	if(!img->run_addr)
	{
		img->run_addr = run_addr ;
		img->ddr_size = ddr_size ;
	}
	*image = (struct image_type_name*)img;

	return 0;
}

static int get_file_size(const char *filename)
{
	struct rfile_stat_stru st;
	s32 ret;
	memset(&st,0x00,sizeof (struct rfile_stat_stru));
	ret = bsp_stat((s8*)filename, (void *)&st);
	if(ret)
	{
		sec_print_err("file bsp_stat error .\n");
		return ret;
	}

	return (int)st.size;
}

static int get_file_name(char *file_name, const struct image_type_name *image, bool *is_sec)
{
	/* ������sec_��ͷ�İ�ȫ���� */
	*is_sec = true;
	file_name[0] = '\0';
	strncat(file_name, MODEM_IMAGE_PATH, strlen(MODEM_IMAGE_PATH));
	strncat(file_name, "sec_", strlen("sec_"));
	strncat(file_name, image->name, strlen(image->name));
	sec_print_info("loading %s  image\n", file_name);
	if(bsp_access((s8*) file_name, RFILE_RDONLY))
	{
		sec_print_info("file %s can't access, try unsec image\n", file_name);

		/* �����Էǰ�ȫ���� */
		*is_sec = false;
		file_name[0] = '\0';
		strncat(file_name, MODEM_IMAGE_PATH, strlen(MODEM_IMAGE_PATH));
		strncat(file_name, image->name, strlen(image->name));

		if(bsp_access((s8*) file_name, RFILE_RDONLY))
		{
			sec_print_err("error: file %s can't access, return\n", file_name);
			return -EACCES;
		}
	}

	return 0;
}

static int read_file(const char *file_name, unsigned int offset,
		unsigned int length, char *buffer)
{
	int file_fd;
	int read_bytes;
	struct file * file_p;
	int ret;

	file_fd = bsp_open(file_name, RFILE_RDONLY, 0);
	if (file_fd < 0)
	{
		sec_print_err("bsp_open file %s failed!\n", file_name);
		return file_fd;
	}

	ret = bsp_lseek((u32)file_fd, (long)offset, RFILE_SEEK_SET);
	if (ret < 0) {
		sec_print_err("seek ops failed, ret %d", ret);
		return ret;
	}

	read_bytes = ret = bsp_read((u32)file_fd, (s8 *)buffer, length);
	if (ret < 0 || (u32)ret != length) {
			sec_print_err("%s: bsp_read err: ret 0x%x != read_bytes 0x%x\n", __func__, ret, length);
			return -EIO;
	}

	file_p = fget((u32)file_fd);
	if(file_p && file_p->f_inode)
	{
		invalidate_mapping_pages(file_p->f_inode->i_mapping, 0, -1);/*lint !e747 !e570*/
	}
	if(file_p)
	{
		fput(file_p);
	}

	ret = bsp_close(file_fd);
	if(ret)
	{
		return ret;
	}

	return read_bytes;
}

int gzip_header_check(unsigned char* zbuf)
{
	if (zbuf[0] != 0x1f || zbuf[1] != 0x8b || zbuf[2] != 0x08) {
		return 0;
	} else {
		return 1;
	}
}


static TEEC_Session load_session;
static TEEC_Context load_context;

/*
 * Function name:TEEK_init.
 * Discription:Init the TEEC and get the context
 * Parameters:
 *	  @ session: the bridge from unsec world to sec world.
 *	  @ context: context.
 * return value:
 *	  @ TEEC_SUCCESS-->success, others-->failed.
 */
static int TEEK_init(void)
{
	TEEC_Result result;
	TEEC_UUID svc_uuid = TEE_SERVICE_SECBOOT;
	TEEC_Operation operation = {0};
	const char* package_name = "sec_boot";
	u32 root_id = 0;

	result = TEEK_InitializeContext(
			NULL,
			&load_context);

	if(result != TEEC_SUCCESS) {
		sec_print_err("TEEK_InitializeContext failed, result %#x\n", result);
		goto error;
	}

	operation.started = 1;
	operation.cancel_flag = 0;
	operation.paramTypes =
		(u32)TEEC_PARAM_TYPES(TEEC_NONE, TEEC_NONE, TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT);/*lint !e845*/

	operation.params[2].tmpref.buffer = (void *)(&root_id);
	operation.params[2].tmpref.size = sizeof(root_id);
	operation.params[3].tmpref.buffer = (void *)(package_name);
	operation.params[3].tmpref.size = strlen(package_name) + 1;
	result = TEEK_OpenSession(
			&load_context,
			&load_session,
			&svc_uuid,
			TEEC_LOGIN_IDENTIFY,
			NULL,
			&operation,
			NULL);

	if (result != TEEC_SUCCESS)
	{
		sec_print_err("TEEK_OpenSession failed, result %#x\n", result);
		TEEK_FinalizeContext(&load_context);
	}

error:

	return (int)result;
}

static void TEEK_uninit(void)
{
	TEEK_CloseSession(&load_session);
	TEEK_FinalizeContext(&load_context);
}


/*
 * Function name:trans_vrl_to_os.
 * Discription:transfer vrl data to sec_OS
 * Parameters:
 *	  @ session: the bridge from unsec world to sec world.
 *	  @ image: the data of the image to transfer.
 *	  @ buf: the buf in  kernel to transfer
 *	  @ size: the size to transfer.
 * return value:
 *	  @ TEEC_SUCCESS-->success, others--> failed.
 */
static int trans_vrl_to_os(enum SVC_SECBOOT_IMG_TYPE  image,
		  void * buf,
		  const unsigned int size)
{
	TEEC_Session *session = &load_session;
	TEEC_Result result;
	TEEC_Operation operation;
	u32 origin;

	operation.started = 1;
	operation.cancel_flag = 0;
	operation.paramTypes =
		(u32)TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_MEMREF_TEMP_INPUT, TEEC_NONE, TEEC_NONE);/*lint !e845*/

	operation.params[0].value.a = image;
	operation.params[1].tmpref.buffer = (void *)buf;
	operation.params[1].tmpref.size = size;

	result = TEEK_InvokeCommand(
				session,
				SECBOOT_CMD_ID_COPY_VRL_TYPE,
				&operation,
				&origin);
	if (result != TEEC_SUCCESS) {
		sec_print_err("invoke failed, result %#x\n", result);
	}

	return (int)result;
}

/*
 * Function name:trans_data_to_os.
 * Discription:transfer image data to sec_OS
 * Parameters:
 *	  @ session: the bridge from unsec world to sec world.
 *	  @ image: the data of the image to transfer.
 *	  @ run_addr: the image entry address.
 *	  @ buf: the buf in  kernel to transfer
 *	  @ offset: the offset to run_addr.
 *	  @ size: the size to transfer.
 * return value:
 *	  @ TEEC_SUCCESS-->success, others--> failed.
 */
static int trans_data_to_os(enum SVC_SECBOOT_IMG_TYPE  image,
						u32 run_addr,
						void * buf,
						const unsigned int offset,
						const unsigned int size)
{
	TEEC_Session *session = &load_session;
	TEEC_Result result;
	TEEC_Operation operation;
	u32 origin;
	unsigned long paddr;
	paddr = MDDR_FAMA(run_addr);

	operation.started = 1;
	operation.cancel_flag = 0;
	operation.paramTypes =
		(u32)TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_VALUE_INPUT, TEEC_VALUE_INPUT, TEEC_VALUE_INPUT);/*lint !e845*/

	operation.params[0].value.a = image;
	operation.params[0].value.b = (u32)(paddr & 0xFFFFFFFF);;
	operation.params[1].value.a = (u32)((u64)paddr >> 32);/* �ֻ���MBB ���� */
	operation.params[1].value.b = offset;
	operation.params[2].value.a = (u32)virt_to_phys(buf);/* �ֻ���MBB ���� */
	operation.params[2].value.b = (u64)virt_to_phys(buf) >> 32;/* �ֻ���MBB ���� */
	operation.params[3].value.a = size;
	result = TEEK_InvokeCommand(
				session,
				SECBOOT_CMD_ID_COPY_DATA_TYPE,
				&operation,
				&origin);
	if (result != TEEC_SUCCESS) {
		sec_print_err("invoke failed, result %#x\n", result);
	}

	return (int)result;
}

/*
 * Function name:start_soc_image.
 * Discription:start the image verification, if success, unreset the soc
 * Parameters:
 *	  @ session: the bridge from unsec world to sec world.
 *	  @ image: the image to verification and unreset
 *	  @ run_addr: the image entry address
 * return value:
 *	  @ TEEC_SUCCESS-->success, others-->failed.
 */
static int verify_soc_image(enum SVC_SECBOOT_IMG_TYPE  image,
						u32 run_addr)
{
	TEEC_Session *session = &load_session;
	TEEC_Result result;
	TEEC_Operation operation;
	u32 origin;
	unsigned long paddr;
    int ret;
    paddr = MDDR_FAMA(run_addr);

    ret = bsp_efuse_ops_prepare();
    if(ret)
    {
        return ret;
    }
	operation.started = 1;
	operation.cancel_flag = 0;
	operation.paramTypes =
		(u32)TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE);/*lint !e845*/

	 operation.params[0].value.a = image;
	 operation.params[0].value.b = 0;/*SECBOOT_LOCKSTATE , not used currently*/
	 operation.params[1].value.a = (u32)(paddr & 0xFFFFFFFF);
	 operation.params[1].value.b = (u32)((u64)paddr >> 32);/* �ֻ���MBB ���� */
	 result = TEEK_InvokeCommand(session,
								   SECBOOT_CMD_ID_VERIFY_DATA_TYPE,
									&operation,
									&origin);
	 if (result != TEEC_SUCCESS){
		sec_print_err("start  failed, result is 0x%x!\n", result);
	}
    bsp_efuse_ops_complete();
	return (int)result;
}

/******************************************************************************
Function:		load_data_to_secos
Description:	��ָ��ƫ�ƿ�ʼ����ָ����С�ľ���
Input:
			part_name   - Ҫ���;��������
			offset	  - ƫ�Ƶ�ַ
			sizeToRead	- ���������Ҫд��ľ����bytes��С

Output:			none
Return:			SEC_OK: OK  SEC_ERROR: ERROR��
******************************************************************************/
static int load_data_to_secos(const char* file_name, u32 offset, u32 size,
			const struct image_type_name* image, bool is_sec)
{
	int ret;
	int read_bytes;
	int readed_bytes;
	int remain_bytes;
	u32 file_offset = 0;
	u32 skip_offset = 0;
	u32 load_position_offset = 0;
	u32 is_compress_check_need = 0;

	/* ��ȡָ��ƫ�Ƶ�ָ����С */
	if(0 != offset)
	{
		skip_offset = offset;
		remain_bytes = (int)size;
	}
	else	/* ��ȡ�����ļ� */
	{
        is_compress_check_need = 1;	/* ֻ�д���ʼλ�ü�����Ҫ����Ƿ���gzip��ͷ */
		remain_bytes = get_file_size(file_name);
		if (remain_bytes <=0)
		{
			sec_print_err("error file_size 0x%x\n", remain_bytes);
			return remain_bytes;
		}

		if(is_sec)
		{
			if(remain_bytes <= VRL_SIZE)
			{
				sec_print_err("error file_size (0x%x) less than VRL_SIZE\n", remain_bytes);
				return -EIO;
			}
			remain_bytes -= VRL_SIZE;
			skip_offset = VRL_SIZE;
		}
	}
	sec_print_info("need read file %s 0x%x to 0x%x though secos from offset 0x%x\n",file_name, image->run_addr, remain_bytes, skip_offset);

	/* ����ȡ�Ĵ�С�Ƿ񳬹�ddr������С */
	if((u32)remain_bytes > image->ddr_size)
	{
		sec_print_err("remain_bytes larger than ddr size:  remain_bytes 0x%x > ddr_size 0x%x!\n", remain_bytes, image->ddr_size);
		return -ENOMEM;
	}

	/*split the size to be read to each SECBOOT_BUFLEN bytes.*/
	while (remain_bytes)
	{
		if (remain_bytes > SECBOOT_BUFLEN)
			read_bytes = SECBOOT_BUFLEN;
		else
			read_bytes = remain_bytes;

		readed_bytes = read_file(file_name, skip_offset + file_offset, (u32)read_bytes, (s8 *)SECBOOT_BUFFER);
		if (readed_bytes < 0 || readed_bytes != read_bytes) {
			sec_print_err("read_file %s err: readed_bytes 0x%x\n", file_name, readed_bytes);
			return -EIO;
		}

		if ((is_compress_check_need) && (readed_bytes >= 10)) {
			is_compress_check_need = 0;

			if (gzip_header_check((unsigned char*)SECBOOT_BUFFER)) {
                /* ������gzip��ʽ��ѹ���������DDR�ռ����λ�� */
				load_position_offset = (u32)(image->ddr_size - remain_bytes);
			}
		}

		ret = trans_data_to_os(image->etype, image->run_addr,  (void *)(SECBOOT_BUFFER), load_position_offset+file_offset, (u32)read_bytes);
		sec_print_info("trans data ot os: etype 0x%x ,run_addr 0x%x, from 0x%p to secos file_offset 0x%x, bytes 0x%x success\n",
			image->etype, image->run_addr, (void *)(SECBOOT_BUFFER), file_offset, read_bytes);

		if (ret)
		{
			sec_print_err("modem image trans to os is failed, error code 0x%x\n", ret);
			return ret;
		}

		remain_bytes -= read_bytes;
		file_offset += (u32)read_bytes;
	}

	return SEC_OK;
}

static int ccpu_reset(enum SVC_SECBOOT_IMG_TYPE  image)
{
	TEEC_Session *session = &load_session;
	TEEC_Result result;
	TEEC_Operation operation;
	u32 origin;

	operation.started = 1;
	operation.cancel_flag = 0;

	operation.paramTypes =
		(u32)TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);/*lint !e845*/

	operation.params[0].value.a = image;
	result = TEEK_InvokeCommand(
				session,
				SECBOOT_CMD_ID_RESET_IMAGE,
				&operation,
				&origin);
	if (result != TEEC_SUCCESS) {
		sec_print_err("invoke failed, result %#x\n", result);
	}

	return (int)result;
}

s32 load_image(enum SVC_SECBOOT_IMG_TYPE ecoretype, u32 run_addr, u32 ddr_size)
{
	s32 ret;
	bool is_sec;
	char file_name[256] = {0};
	int readed_bytes;
	struct image_type_name *image;

	ret = get_image(&image, ecoretype, run_addr, ddr_size);
	if(ret)
	{
		sec_print_err("can't find image\n");
		return ret;
	}

	if(!run_addr)
	{
		run_addr = image->run_addr;
	}

	ret = get_file_name(file_name, image, &is_sec);
	if(ret)
	{
		sec_print_err("can't find image\n");
		return ret;
	}
	sec_print_info("find file %s, is_sec: %d\n", file_name, is_sec);

	/*load vrl data to sec os*/
	if(is_sec)
	{
		readed_bytes = read_file(file_name, 0, VRL_SIZE, (char*)SECBOOT_BUFFER);
		if(readed_bytes < 0 || readed_bytes != VRL_SIZE)
		{
			sec_print_err("read_file %s error, readed_bytes 0x%x!\n", file_name, readed_bytes);
			ret = -EIO;
			goto error;
		}

		ret = trans_vrl_to_os(image->etype, (void *)(SECBOOT_BUFFER), VRL_SIZE);
		if(ret)
		{
			sec_print_err("trans_vrl_to_os error, ret 0x%x!\n", ret);
			goto error;
		}
		sec_print_err("trans vrl to secos success, from buf 0x%p\n", (void *)(SECBOOT_BUFFER));
	}

	/* load image data to sec os */
	ret = load_data_to_secos(file_name, 0, 0, image, is_sec);
	if(ret)
	{
		sec_print_err("load image %s to secos failed, ret = 0x%x\n", file_name, ret);
		goto error;
	}
	sec_print_err("load image %s to secos success\n", file_name);

	/*end of trans all data, start verify*/
	ret = verify_soc_image(ecoretype, run_addr);
	if(ret)
	{
		sec_print_err("verify image %s fail, ret = 0x%x\n", file_name, ret);
		goto error;
	}
	sec_print_err("verify image %s success\n", file_name);

error:

	return ret;
}



/*****************************************************************************
 �� �� ��  : Modem��ؾ�����ؽӿ�
 ��������  : Modem��ؾ�����ؽӿ�
 �������  : ��
 �������  : ��
 �� �� ֵ  : �ɹ�����OK,ʧ�ܷ���ERROR
*****************************************************************************/
int bsp_load_modem_images(void)
{
	int ret;

	mutex_lock(&load_proc_lock);

	ret = TEEK_init();
	if(ret)
	{
		mutex_unlock(&load_proc_lock);
		sec_print_err("TEEK_init failed! ret %#x\n" ,ret);
		return ret;
	}

	ret = ccpu_reset(MODEM);
    if(ret)
	{
		mutex_unlock(&load_proc_lock);
		sec_print_err("ccpu_reset failed, ret %#x\n", ret);
		return ret;
	}

	ret = load_image(DSP, 0, 0);
	if(ret)
	{
		goto error;
	}



	ret = load_image(MODEM, 0, 0);
	if(ret)
	{
		goto error;
	}

error:
	TEEK_uninit();

	mutex_unlock(&load_proc_lock);

	return ret;
}

/*****************************************************************************
 �� �� ��  : was.img��tas.img�ȶ�̬���ؾ���ӿ�
 ��������  : Modem��ؾ�����ؽӿ�
 �������  : ��
 �������  : ��
 �� �� ֵ  : �ɹ�����OK,ʧ�ܷ���ERROR
*****************************************************************************/
int bsp_load_modem_single_image(enum SVC_SECBOOT_IMG_TYPE ecoretype, u32 run_addr, u32 ddr_size)
{
	int ret;

	mutex_lock(&load_proc_lock);

	ret = TEEK_init();
	if(ret)
	{
		mutex_unlock(&load_proc_lock);
		sec_print_err("TEEK_InitializeContext failed!\n");
		return ret;
	}

	ret = load_image(ecoretype, run_addr, ddr_size);
	if(ret)
	{
		goto error;
	}

error:
	TEEK_uninit();

	mutex_unlock(&load_proc_lock);

	return ret;
}


