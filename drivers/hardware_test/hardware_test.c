/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2013-2015. All rights reserved.
 *
 * mobile@huawei.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */



#include <linux/errno.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include "hardware_test.h"
#define KOBJ_ATTR_RO(_name)  static struct kobj_attribute _name##_attr = \
                    __ATTR_RO(_name)

#define KOBJ_ATTR_RW(_name)  static struct kobj_attribute _name##_attr = \
                    __ATTR(_name, 0644, _name##_show, _name##_store)/*创建节点即属性设置*/

//#define HW_TEST_MAGIC_NUM    (0x33557799)
#define HWTEST_PROC_FILE "hwtest_mode"

#if (FEATURE_ON == MBB_BITFLIP_TEST)
int nandc_reg_value = -1;
#endif

struct kobject  *hw_obj = NULL;
static struct proc_dir_entry *g_hwtest_proc_file = NULL;
char *bitflip_info = NULL;/*ecc bit flip info*/
static unsigned int pre_cw_num = 0; /*记录当前申请的bitflip_info的内存长度*/
unsigned int codeword_num = 0;
hwtest_flash_info g_flash_info;


void init_hardware_flash_info(unsigned int codewore_size, unsigned int spare_size, unsigned int ecc_correct_num)
{
    if(0 == codewore_size || 0 == spare_size || 0 == ecc_correct_num)
    {
        return;
    }
    g_flash_info.codewore_size = codewore_size;
    g_flash_info.spare_size = spare_size;
    g_flash_info.ecc_correct_num = ecc_correct_num;
}

#if (FEATURE_ON == MBB_BITFLIP_TEST)
void nandc_reg_read(int value)
{
    nandc_reg_value = value;
}
#endif

/********************************************************************
*Function    : set_hardware_test_mode
*Description : 设置自检模式
*Input       :
*Output      : none
*Return      : 0 :successed
               other:failed
********************************************************************/
int set_hardware_test_mode(hwtest_smem_info *hwtest_info)
{
    hwtest_smem_info *smem_data = NULL;
    smem_data = (hwtest_smem_info *)SRAM_HWTEST_ADDR;

    if (NULL == smem_data)
    {
        printk(KERN_ERR "\r\n [HARDWARE TEST] set mode:NULL pointer!\r\n");
        return -1;
    }
    /*test*/
    //printk(KERN_ERR "\r\n [HARDWARE TEST] smem_vendor1_size=%d\r\n",sizeof(huawei_smem_vendor1_info));

    if (NULL == hwtest_info)
    {
        printk(KERN_ERR "\r\n [HARDWARE TEST] set mode:NULL pointer!\r\n");
        return -1;
    }
    if (HARDWARE_TEST_OPT_MAX <= hwtest_info->smem_hw_option)
    {
        hwtest_info->smem_hw_option = HARDWARE_TEST_OPT_ALL;
    }
    if (hwtest_info->smem_hw_mode)
    {
        /*设置状态*/
        smem_data->smem_hw_mode = HW_TEST_MAGIC_NUM;
        smem_data->smem_hw_option = hwtest_info->smem_hw_option;
    }
    else
    {
        /*清除状态*/
        smem_data->smem_hw_mode = 0;
        smem_data->smem_hw_option = 0;
    }

    printk(KERN_ERR "\r\n [HARDWARE TEST] set mode: mode=%d, option = %d!\r\n",
           hwtest_info->smem_hw_mode, hwtest_info->smem_hw_option);

    return 0;
}
/********************************************************************
*Function    : get_hardware_test_mode
*Description : 查询当前的自检模式
*Input       :
*Output      : hwtest_info
*Return      : 0 :successed
               other:failed
********************************************************************/
int get_hardware_test_mode(hwtest_smem_info *hwtest_info)
{
    hwtest_smem_info *smem_data = NULL;
    smem_data = (hwtest_smem_info *)SRAM_HWTEST_ADDR;
    if (NULL == smem_data)
    {
        printk(KERN_ERR "\r\n [HARDWARE TEST] get mode:NULL pointer!\r\n");
        return -1;
    }

    if (NULL == hwtest_info)
    {
        printk(KERN_ERR "\r\n [HARDWARE TEST] get mode:NULL pointer!\r\n");
        return -1;
    }
    /*获取当前状态*/
    if (HW_TEST_MAGIC_NUM == smem_data->smem_hw_mode)
    {
        hwtest_info->smem_hw_mode = 1;
    }
    else
    {
        hwtest_info->smem_hw_mode = 0;
    }
    hwtest_info->smem_hw_option = smem_data->smem_hw_option;

    printk(KERN_ERR "\r\n [HARDWARE TEST] get mode: mode=%d, option = %d!\r\n",
           hwtest_info->smem_hw_mode, hwtest_info->smem_hw_option);


    return 0;
}
/********************************************************************
*Function    : bit_flip_info_clear
*Description : 删除记录的bitflip信息
*Input       : none
*Output      : none
*Return      : 0 :successed
               other:failed
********************************************************************/
int bit_flip_info_clear(void)
{
    /*容错*/
    if(0 == codeword_num)
    {
        /*传入的长度为0直接退出*/
        return 0;
    }

    if (NULL == bitflip_info)
    {
        bitflip_info = kmalloc(codeword_num, GFP_KERNEL);
        if (NULL == bitflip_info)
        {
            printk(KERN_ERR "\r\n [HARDWARE TEST]mem is not enough!\r\n");
            return -1;
        }
        else
        {
            pre_cw_num = codeword_num;  /*记录首次申请的长度*/
            memset((void*)bitflip_info, 0, codeword_num);
        }
    }
    else
    {
        if(codeword_num > pre_cw_num)
        {
            /*内存已申请,再执行不能超过第一次申请的长度*/
            return -1;
        }
        memset((void*)bitflip_info, 0, codeword_num);
    }
    return 0;
}
/********************************************************************
*Function    : bit_flip_info_record
*Description : 记录bit flip信息
*Input       : cw_id，corrected
*Output      : none
*Return      : 0 :successed
               other:failed
********************************************************************/
int bit_flip_info_record(unsigned int cw_id, unsigned int corrected)
{
    if (NULL == bitflip_info)
    {
        return -1;
    }

    if (cw_id < codeword_num)
    {
        bitflip_info[cw_id] = corrected;
    }

    return 0;
}
/*bit flip info dump*/
int bit_flip_info_dump(void)
{
    int i;

    if (NULL == bitflip_info)
    {
        printk(KERN_ERR "\r\n [HARDWARE TEST]bit_flip_info_record: bitflip_info is NULL!\r\n");
        return -1;
    }
    for (i = 0;i < codeword_num;i++)
    {
        printk(KERN_ERR "\r\n [HARDWARE TEST] bitflip_info[%d]=%d\r\n", i, bitflip_info[i]);
    }

    return 0;
}

/********************************************************************
*Function    : get_sd_det_status
*Description : 获取sd卡dectect引脚状态
*Input       : nonoe
*Output      : none
*Return      : 0 :low
               1:high
               -1:NA
********************************************************************/
int get_sd_det_status(void)
{
    /*目前产品暂不支持*/
    return -1;
}

/********************************************************************
*Function    : get_sim_det_status
*Description : 获取sim卡dectect引脚状态
*Input       : nonoe
*Output      : none
*Return      : 0 :low
               1:high
               -1:NA
********************************************************************/
int get_sim_det_status(void)
{
    /*目前产品暂不支持*/
    return -1;
}
#if (FEATURE_ON == MBB_CHARGE)
extern int chg_is_chargeIC_communication_normal(void);
#else
int chg_is_chargeIC_communication_normal(void)
{
    return 0;
}
#endif

/********************************************************************
*Function    : hardwaretestIoctl
*Description :
*Input       :
*Output      : none
*Return      : 0 :successed
               other:failed
********************************************************************/
static long hardwaretestIoctl(struct file *file, unsigned int cmd, unsigned long data)
{
    int ret = 0;
    hwtest_smem_info hwtest_info = {0, 0, 0};

    //printk(KERN_INFO "\r\n [HARDWARE TEST] hardwaretestIoctl: cmd=%u, data = %lu!\r\n", cmd, data);

    if (NULL == file )
    {
        printk(KERN_ERR "\r\n [HARDWARE TEST]hardwaretestIoctl: file is NULL!\r\n");
        return -1;
    }

    switch (cmd)
    {
        case HARDWARE_TEST_MODE_SET:
            /*设置当前测试模式*/
            ret = copy_from_user(&hwtest_info, (void __user *)data, sizeof(hwtest_info));
            if (0 != ret)
            {
                printk(KERN_ERR "\r\n [HARDWARE TEST]copy_from_user fail!\r\n");
            }
            set_hardware_test_mode((hwtest_smem_info*)&hwtest_info);
            break;
        case HARDWARE_TEST_MODE_GET:
            /*获取当前的测试状态*/
            get_hardware_test_mode((hwtest_smem_info*)&hwtest_info);
            ret = copy_to_user((void*)data, &hwtest_info, sizeof(hwtest_info));
            if (0 != ret)
            {
                printk(KERN_ERR "\r\n [HARDWARE TEST]copy_to_user fail!\r\n");
            }
            break;
        case HARDWARE_TEST_BITFLIP_CLR:
            /*清除记录的bit flip info*/
            ret = copy_from_user(&codeword_num, (void __user *)data, sizeof(codeword_num));
            if (0 != ret)
            {
                printk(KERN_ERR "\r\n [HARDWARE TEST]copy_from_user fail!\r\n");
            }
            //printk(KERN_ERR "\r\n [HARDWARE TEST]codeword_num=%d\r\n",codeword_num);
            bit_flip_info_clear();
            break;
        case HARDWARE_TEST_BITFLIP_GET:
            if (NULL == bitflip_info)
            {
                return -1;
            }
            ret = copy_to_user((void*)data, bitflip_info, codeword_num);
            if (0 != ret)
            {
                printk(KERN_ERR "\r\n [HARDWARE TEST]copy_to_user fail!\r\n");
            }
            break;
        case HARDWARE_TEST_SD_DET_GET:
            ret = get_sd_det_status();
            ret = copy_to_user((void*)data, &ret, sizeof(ret));
            if (0 != ret)
            {
                printk(KERN_ERR "\r\n [HARDWARE TEST]copy_to_user fail!\r\n");
            }
            break;
        case HARDWARE_TEST_SIM_DET_GET:
            ret = get_sim_det_status();
            ret = copy_to_user((void*)data, &ret, sizeof(ret));
            if (0 != ret)
            {
                printk(KERN_ERR "\r\n [HARDWARE TEST]copy_to_user fail!\r\n");
            }
            break;
        case HARDWARE_TEST_CHG_STATUS_GET:
            ret = chg_is_chargeIC_communication_normal();
            ret = copy_to_user((void*)data, &ret, sizeof(ret));
            if (0 != ret)
            {
                printk(KERN_ERR "\r\n [HARDWARE TEST]copy_to_user fail!\r\n");
            }
            break;
        case HARDWARE_TEST_NAND_INFO_GET:
            if (0 == g_flash_info.codewore_size || 0 == g_flash_info.spare_size || 0 == g_flash_info.ecc_correct_num)
            {
                return -1;
            }
            ret = copy_to_user((void*)data, &g_flash_info, sizeof(g_flash_info));
            if (0 != ret)
            {
                printk(KERN_ERR "\r\n [HARDWARE TEST]copy_to_user fail!\r\n");
            }
            break;
#if (FEATURE_ON == MBB_DRV_M2M_AT)
        case HARDWARE_TEST_BITFLIP_TEST_INFO_SET:
        {
            hwtest_boot_test_info_stype *bitflip_data = NULL;
            bitflip_data = (hwtest_boot_test_info_stype *)SRAM_DEVTEST_ADDR;
            unsigned int bitflipinfo = 0;
            if (NULL == bitflip_data)
            {
                printk(KERN_ERR "\r\n [HARDWARE TEST] ddr get info:NULL pointer!\r\n");
                return -1;
            }
            ret = copy_from_user(&bitflipinfo, (void __user *)data, sizeof(unsigned int));
            if (0 != ret)
            {
                printk(KERN_ERR "\r\n [HARDWARE TEST]copy_from_user fail! %d\r\n",ret);
            }
            bitflip_data->bitflip_status = bitflipinfo;
            printk(KERN_ERR "\r\n [HARDWARE TEST] bitflip_status %d\r\n",bitflipinfo);
            break;
        }

        case HARDWARE_TEST_BOOT_DDR_TEST_INFO_GET:
        {
            hwtest_boot_test_info_stype *smem_data = NULL;
            smem_data = (hwtest_boot_test_info_stype *)SRAM_DEVTEST_ADDR;
            if (NULL == smem_data)
            {
                printk(KERN_ERR "\r\n [HARDWARE TEST] ddr get info:NULL pointer!\r\n");
                return -1;
            }
#if ((FEATURE_ON == MBB_DDR_SCREEN) && (FEATURE_ON == MBB_FACTORY))
            printk(KERN_ERR "\r\n [HARDWARE TEST]copy_to_user ddrstatus %d %d 0x%x 0x%x 0x%x\r\n", smem_data->ddr_status, smem_data->error_type, smem_data->error_addr, smem_data->error_data, smem_data->error_read_data);
#else
            printk(KERN_ERR "\r\n [HARDWARE TEST]copy_to_user ddrstatus %d %d 0x%x 0x%x\r\n",smem_data->ddr_status,smem_data->error_type,smem_data->error_addr,smem_data->error_data);
#endif
            ret = copy_to_user((void*)data, smem_data, sizeof(hwtest_boot_test_info_stype));
            if (0 != ret)
            {
                printk(KERN_ERR "\r\n [HARDWARE TEST]copy_to_user boot ddr test info fail!\r\n");
            }

            break;
        }

#endif

#if (FEATURE_ON == MBB_BITFLIP_TEST)
        case HARDWARE_TEST_NANDC_REG_GET:
                {
                    /*获取nandc寄存器oob_len_sel的值*/
                    if (0 > nandc_reg_value)
                    {
                        printk(KERN_ERR "\r\n get nandc reg fail!\r\n");
                        return -1;
                    }
                    ret = copy_to_user((void*)data, &nandc_reg_value, sizeof(nandc_reg_value));
                    if (0 != ret)
                    {
                        printk(KERN_ERR "\r\n [HARDWARE TEST]copy_to_user fail!\r\n");
                    }
                    break;
                }
#endif
#if ((FEATURE_ON == MBB_DDR_SCREEN) && (FEATURE_ON == MBB_FACTORY))
        case HARDWARE_TEST_DDR_SCREEN_RESULT_PLUS:
        {
            hwtest_boot_test_info_plus_stype *smem_data = NULL;
            /*lint -e124*/
            smem_data = (hwtest_boot_test_info_plus_stype *)SRAM_DEVTEST_PLUS_ADDR;
            /*lint +e124*/
            if (NULL == smem_data)
            {
                printk(KERN_ERR "\r\n [HARDWARE TEST] hwtest_boot_test_info_plus_stype:NULL pointer!\r\n");
                return -1;
            }
            /*lint -e516*/
            ret = copy_to_user((void*)data, smem_data, sizeof(hwtest_boot_test_info_plus_stype));
            /*lint +e516*/
            if (0 != ret)
            {
                printk(KERN_ERR "\r\n [HARDWARE TEST]copy_to_user hwtest_boot_test_info_plus_stype fail!\r\n");
            }
            break;
        }
#endif

        default:
            return - ENOTTY;
    }

    return ret;
}

static const struct file_operations hardwaretestFops =
{
    .owner         = THIS_MODULE,
                     .unlocked_ioctl = hardwaretestIoctl,
};

static struct miscdevice hardwaretestMiscdev =
{
    .minor    = MISC_DYNAMIC_MINOR,
                .name    = "hardwaretest",
                           .fops    = &hardwaretestFops
};

static ssize_t mode_show(struct kobject *kobj,
                        struct kobj_attribute *attr, char *buf)
{
    hwtest_smem_info hwtest_info = {0, 0, 0};

    get_hardware_test_mode((hwtest_smem_info*)&hwtest_info);

    return snprintf( buf, sizeof(hwtest_info.smem_hw_mode), "%lu\n", hwtest_info.smem_hw_mode );
}

static ssize_t option_show(struct kobject *kobj,
                        struct kobj_attribute *attr, char *buf)
{
    hwtest_smem_info hwtest_info = {0, 0, 0};

    get_hardware_test_mode((hwtest_smem_info*)&hwtest_info);

    return snprintf( buf, sizeof(hwtest_info.smem_hw_option), "%lu\n", hwtest_info.smem_hw_option );
}

KOBJ_ATTR_RO(mode);
KOBJ_ATTR_RO(option);

static struct attribute *hardwaretest_attrs[] =
{
    &mode_attr.attr,
    &option_attr.attr,
    NULL
};

static struct attribute_group hardwaretest_group =
{
    .attrs = hardwaretest_attrs,
};
/******************************************************************************
  Function      hwtest_proc_read
  Description   获取hwtest的proc文件节点值
  Input         N/A
  Output        N/A
  Return        N/A
  Others        N/A
******************************************************************************/
static ssize_t hwtest_proc_read(struct file *filp,
    char *buffer, size_t length, loff_t *offset)
{
    int ret = -1,mode = 0;
    hwtest_smem_info hwtest_info = {0, 0, 0};

    get_hardware_test_mode((hwtest_smem_info*)&hwtest_info);
    mode = hwtest_info.smem_hw_mode;
    
    printk(KERN_ERR "\r\n [HARDWARE TEST] mode=%d\r\n", mode);

    ret = copy_to_user(buffer, (void *)&mode, sizeof(int));

    if(0 != ret)
    {
        printk(KERN_ERR "\r\n [HARDWARE TEST]copy_to_user fail!\r\n");
    }

    return ret;
}

static struct file_operations hwtest_proc_ops = {
    .read  = hwtest_proc_read,
};

/******************************************************************************
  Function      create_hwtest_proc_file
  Description   创建hwtest的proc文件节点
  Input         N/A
  Output        N/A
  Return        N/A
  Others        N/A
******************************************************************************/
static void create_hwtest_proc_file(void)
{
    struct proc_dir_entry *p = NULL;
    /*创建文件节点，并设置节点权限为644*/
    g_hwtest_proc_file = proc_create(HWTEST_PROC_FILE, 0644, p, &hwtest_proc_ops);

    if(NULL == g_hwtest_proc_file)
    {
        printk(KERN_ERR"%s: create proc entry for hwtest failed\n", __FUNCTION__);
    }
}

/*************************************************************************
* 函数名     :  remove_hwtest_proc_file
* 功能描述   :  删除节点
* 输入参数   :  void
* 输出参数   :  void
* 返回值     :  void
**************************************************************************/
static void remove_hwtest_proc_file(void)
{
    remove_proc_entry(HWTEST_PROC_FILE, NULL);
}

static int __init hardware_test_init(void)
{
    int ret = 0;

    printk(KERN_ERR "\r\n [HARDWARE TEST] init...\r\n");
    ret = misc_register(&hardwaretestMiscdev);
    if (0 > ret)
    {
        printk(KERN_ERR "\r\n [HARDWARE TEST] misc_register failed.\r\n");
        goto err;
    }
    
    //creat proc
    create_hwtest_proc_file();
    
    //create kobject
    hw_obj = kobject_create_and_add("hardwaretest", NULL);
    if (!hw_obj)
    {
        ret = - ENOMEM;
        goto err;
    }

    ret = sysfs_create_group(hw_obj, &hardwaretest_group);
    if (ret)
    {
        kobject_put(hw_obj);
        goto err;
    }
    return 0;
err:
    printk(KERN_ERR "\r\n [HARDWARE TEST] init failed,ret=%d\r\n", ret);
    return ret;

}

static void __exit hardware_test_exit(void)
{
    int ret = 0;
    remove_hwtest_proc_file();
    ret = misc_deregister(&hardwaretestMiscdev);
    if (0 > ret)
    {
        printk(KERN_ERR "\r\n [HARDWARE TEST] misc_deregister failed.\r\n");
    }
    sysfs_remove_group(hw_obj, &hardwaretest_group);
    kobject_put( hw_obj );
}

module_init(hardware_test_init);
module_exit(hardware_test_exit);

MODULE_AUTHOR("Huawei Device");
MODULE_DESCRIPTION("hardware test");
MODULE_LICENSE("GPL");




