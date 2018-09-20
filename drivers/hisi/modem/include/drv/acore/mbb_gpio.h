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

#ifndef __MBB_GPIO_H__
#define __MBB_GPIO_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <product_config.h>

#ifdef BSP_CONFIG_BOARD_TELEMATIC
#if ((FEATURE_ON == MBB_FACTORY) || (FEATURE_ON == MBB_AGING_TEST)) 
#define PRODUCT_GPIO_RANGE         "000000000000000000000000000-111111111111111111111111111"
#define GPIO_CONTROL_NUMBER           (27)
#else
#if (FEATURE_ON == MBB_IOCFG_HERMES)
#define PRODUCT_GPIO_RANGE         "000000000000000-111111111111111"
#define GPIO_CONTROL_NUMBER           (15)
#elif (FEATURE_ON == MBB_IOCFG_AUDI)
#define PRODUCT_GPIO_RANGE         "00000000000-11111111111"
#define GPIO_CONTROL_NUMBER           (11)
#else
#define PRODUCT_GPIO_RANGE           "0-1"
#define GPIO_CONTROL_NUMBER           (1)
#endif
#endif  /*MBB_FACTORY, MBB_AGING_TEST*/
#else
#define PRODUCT_GPIO_RANGE         "0000000-1111111"
#define GPIO_CONTROL_NUMBER           (7)
#endif  /*BSP_CONFIG_BOARD_TELEMATIC*/

typedef enum{
    GPIO_CMD_SET = 0,
    GPIO_CMD_GET = 1,
    GPIO_CMD_MAX,
}GPIO_CMD_E;

/*���ز��Խ��ö�ٶ���*/
typedef enum
{
    GPIOLOOP_SUCCESS = 0,
    GPIOLOOP_FAIL,
}GPIOLOOP_RESULT_ENUM;

/*֧�ֻ��ز��Ե�GPIO������*/
#define GPIO_TEST_NUM_MAX    (32)

/*���ز��Խ�����ݽṹ����*/
typedef struct
{
    GPIOLOOP_RESULT_ENUM test_result;
    unsigned int fail_num;
    unsigned char fail_list[GPIO_TEST_NUM_MAX];
}GPIOLOOP_RST_STRU;

/*****************************************************************************
 �� �� ��  : drv_gpio_ioctrl_cmd
 ��������  : ��AT^IOCTRL���ã����û��߻�ȡ�ṩ���ͻ���gpio����͵�ƽ
 �������  : cmd:0--SET,1--GET;
             *gpio:��Ҫ���õ�gpio����"0110111",��cmdΪ1ʱ�˲����ɺ��ԣ�
             *direction:��Ҫ���û��߻�ȡ����gpio����1Ϊ�����0Ϊ���룻
             *value: ��Ҫ���û��ȡ����gpio��ƽ��1Ϊ�ߵ�ƽ��0Ϊ�͵�ƽ��
 �������  : 
 �� �� ֵ  :  0--�ɹ���1--ʧ��
 ˵    ��  : ��������Ҫ��֤����Ĳ�����Ч���߽��ܵĵ�ַ��Χ�㹻��
*****************************************************************************/
int drv_gpio_ioctrl_cmd(GPIO_CMD_E cmd, unsigned char *gpio, unsigned char *direction, unsigned char *value);


extern int drv_gpioloop_test_process(GPIOLOOP_RST_STRU *p_ret);


extern int drv_get_support_gpioloop_info(unsigned int *pt_support, unsigned char *pt_num);


extern void drv_gpioloop_set_function_to_gpio(void);


extern void drv_gpioloop_set_gpio_to_default(void);
#ifdef __cplusplus
}
#endif

#endif

