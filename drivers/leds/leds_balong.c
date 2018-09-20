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


#ifdef __cplusplus
extern "C"
{
#endif

#include <linux/err.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/leds.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <hi_dr.h>
#include <mdrv_leds.h>
#include "bsp_leds.h"
#include "bsp_regulator.h"
#include "bsp_dr.h"
#include "bsp_nvim.h"
#include "bsp_icc.h"
#include "bsp_softtimer.h"
#include "leds_balong.h"

#include <product_config.h>

#if (FEATURE_ON == MBB_LED)
#include <linux/gpio.h>

#if (FEATURE_ON == MBB_FEATURE_M2M_LED)
#include "SysNvId.h"
#include "mdrv_version.h"
#endif

static DEFINE_MUTEX(g_led_kernelset_lock);    /*led�ں˿��ƽӿ�������*/
#define long_time    50000    /*dr�����Ĵ���ֵ*/
#define current    3000    /*��λuA��������Ϊ3mA*/

extern int g_dr_brightness[5];     /*��ǰ�����ȱ�ʶ*/
struct regulator *dr_regulator[5];    /*����regulatorȫ�ֱ���*/

static int g_set_flag = 0;    /*�ӿڵ��ñ�ʶλ*/
static int g_ledready_flag = 0;    /*�ڵ������ʶλ*/
#endif

/*lint -e553*/
#if (FEATURE_ON == MBB_COMMON)
#define en_NV_Item_HUAWEI_PCCW_HS_HSPA_BLUE 50032
static int g_leds_balong_init = 0; /* ��ǰ�����Ƿ��ʼ����� */
#endif
/*lint +e553*/
struct softtimer_list led_softtimer;                /* soft timer */
struct nv_led g_nv_led;                             /* store nv */
DRV_MODULE_TEST_STRU g_nv_module_ctrl;
LED_CONTROL_NV_STRU g_led_state_str_lte[LED_LIGHT_STATE_MAX][LED_CONFIG_MAX_LTE] = LED_OM_CONFIG; /* ��ͬ��״̬��Ӧ�����Ʒ�����������led nvһ�� */

/*lint -e553*/
#if (FEATURE_ON == MBB_WPG_LED_FLASH)
LED_CONTROL_NV_STRU g_led_HongKong_StateStr[LED_LIGHT_STATE_MAX][LED_CONFIG_MAX_LTE] = LED_HONGKONG_CONFIG;
LED_CONTROL_NV_STRU g_led_Russia_StateStr[LED_LIGHT_STATE_MAX][LED_CONFIG_MAX_LTE] = LED_RUSSIA_CONFIG;
LED_CONTROL_NV_STRU g_led_Vodafone_StateStr[LED_LIGHT_STATE_MAX][LED_CONFIG_MAX_LTE] = LED_VODAFONE_CONFIG;
LED_CONTROL_NV_STRU g_led_ChinaUnion_StateStr[LED_LIGHT_STATE_MAX][LED_CONFIG_MAX_LTE] = LED_CHINAUNION_CONFIG;
LED_CONTROL_NV_STRU g_led_FranceSFR_StateStr[LED_LIGHT_STATE_MAX][LED_CONFIG_MAX_LTE] = LED_FRANCE_CONFIG;
LED_CONTROL_NV_STRU g_led_IndiaBSNL_StateStr[LED_LIGHT_STATE_MAX][LED_CONFIG_MAX_LTE] = LED_INDIA_CONFIG;
LED_CONTROL_NV_STRU g_led_HolandKPN_StateStr[LED_LIGHT_STATE_MAX][LED_CONFIG_MAX_LTE] = LED_HOLAND_CONFIG;
LED_CONTROL_NV_STRU g_led_SoftBank_StateStr[LED_LIGHT_STATE_MAX][LED_CONFIG_MAX_LTE] = LED_OM_CONFIG;
LED_CONTROL_NV_STRU g_led_Emobile_StateStr[LED_LIGHT_STATE_MAX][LED_CONFIG_MAX_LTE] = LED_OM_CONFIG;
#endif
#if (FEATURE_ON == MBB_FEATURE_M2M_LED)
LED_CONTROL_NV_STRU g_led_M2M_StateStr[LED_LIGHT_STATE_MAX][LED_CONFIG_MAX_LTE] = M2M_LED_DEFAULT_CONFIG;
extern int drv_led_flash(unsigned long state);
struct softtimer_list ce_led_softtimer;
LED_CONTROL_NV_STRU g_led_ce_state_str[DRV_CE_LED_STATE_MAX][LED_CONFIG_MAX_LTE] = CE_LED_DEFAULT_CONFIG;
#endif
/*lint +e553*/
unsigned int led_debug_level;                       /* for LED_TRACE level */

extern struct led_tled_arg g_arg;

EXPORT_SYMBOL_GPL(g_led_state_str_lte);
EXPORT_SYMBOL_GPL(g_nv_led);
EXPORT_SYMBOL_GPL(led_debug_level);

static int balong_led_probe(struct platform_device *dev);
//static int balong_led_remove(struct platform_device *pdev);
int register_led_by_nr(struct balong_led_device *led, int nr, struct led_platform_data *pdata);

/* LED resources */
struct led_info balong_led[] =
{
    [0] = {
        .name = LED1_NAME,
    },
    [1] = {
        .name = LED2_NAME,
    },
    [2] = {
        .name = LED3_NAME,
    },
#ifdef USE_5_DR
    [3] = {
        .name = LED4_NAME,
    },
    [4] = {
        .name = LED5_NAME,
    }
#endif
};

/* LED platform data */
struct led_platform_data led_plat_data =
{
    .num_leds   = ARRAY_SIZE(balong_led),
    .leds       = (struct led_info *)balong_led,
};

/* LED platform device */
struct platform_device balong_led_dev =
{
    .name	= "balong_led",
    .id	    = -1,
    .dev 	= {
        .platform_data	= &led_plat_data,
    }
};
EXPORT_SYMBOL_GPL(balong_led_dev);

unsigned long us2ms(unsigned long us)
{
    return us / 1000;
}

/************************************************************************
 * Name         : brightness_to_limit
 * Function     : brightness to limit
 * Arguments
 *      input   : @led_id         - led id
 *              : @led_brightness - brightness
 *      output  : null
 *
 * Return       : limit
 * Decription   : ������������С��������Ϊ(LED_FULL + 1)��������ݴ˽�led_brightnessת��Ϊlimit,
 *                �ٸ���DR�ɽ��ܵ�8��������λ��ѡ����ӽ���һ����λ��
 ************************************************************************/
int brightness_to_limit(int led_id, int led_brightness)
{
    unsigned i;
    int limit, limit_min, limit_max, limit_bigger, limit_smaller;

    /* compute limit */
    if(LED_OFF == led_brightness)
    {
        return 0;
    }
    else
    {
        limit_min = bsp_dr_list_current(led_id, 0);
        limit_max = bsp_dr_list_current(led_id, DR_CUR_NUMS - 1);

        limit = led_brightness * limit_max / LED_FULL; //DEBUG
        LED_TRACE(LED_DEBUG_LEVEL(DEBUG),"led id %d, brightness %d, limit min %d, limit max %d, limit %d\n",
                  led_id, led_brightness, limit_min,limit_max, limit);
    }

    /* find nearest limit level to use */
    for(i = 0; i < DR_CUR_NUMS; i++)
    {
        limit_bigger = bsp_dr_list_current(led_id, i);
        if(limit < limit_bigger)
        {
            break;
        }
    }
    limit_smaller = (0 == i) ? 0 : bsp_dr_list_current(led_id, i - 1);
    LED_TRACE(LED_DEBUG_LEVEL(DEBUG),"limit smaller %d, limit bigger %d\n", limit_smaller, limit_bigger);

    /* find nearest limit level, if limit is just in the middle of limit_bigger and limit_smaller, chose limit_bigger */
    limit = ((limit - limit_smaller) < (limit_bigger - limit)) ? limit_smaller : limit_bigger;
    LED_TRACE(LED_DEBUG_LEVEL(DEBUG),"limit level %d\n", limit);

    return limit;
}


int name_to_id(struct led_classdev *led_cdev)
{
    /* reject null */
    if(!led_cdev || !led_cdev->name)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"arguments is NULL\n");
        return LED_ERROR;
    }

    LED_TRACE(LED_DEBUG_LEVEL(DEBUG),"led name %s\n", led_cdev->name);

    if(!strcmp(led_cdev->name, LED1_NAME))
    {
        return PMIC_DR01;
    }
    else if(!strcmp(led_cdev->name, LED2_NAME))
    {
        return PMIC_DR02;
    }
#ifdef USE_5_DR
    else if(!strcmp(led_cdev->name, LED3_NAME))
    {
        return PMIC_DR03;
    }
    else if(!strcmp(led_cdev->name, LED4_NAME))
    {
        return PMIC_DR04;
    }
    else
    {
        return PMIC_DR05;
    }
#else
    else
    {
        return PMIC_DR03;
    }
#endif
}

/************************************************************************
 * Name         : set_bre_time_valid
 * Function     : set breath time aviled
 * Arguments
 *      input   : @led_id   - led id
 *                @bre_time_enum - Ҫ���õĲ�������
 *                @p_breathtime_ms - Ҫ���õĲ���ֵ
 *      output  : @p_breathtime_ms - Ҫ���õĲ���ֵ
 *
 * Return       : default breath on time(ms)
 * Decription   : Ĭ��ֵ�˴��ο�Hi6551�û��ֲ��Ӧ�Ĵ�����Ĭ��ֵ
 ************************************************************************/
int set_bre_time_valid(int led_id, dr_bre_time_e bre_time_enum, unsigned long *p_breathtime_ms)
{
    unsigned long value_num, selector;
    unsigned long temp_bigger = 0, temp_smaller, breath_time;
    int ret = LED_ERROR;

    /* ������ */
    if(!p_breathtime_ms)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"arguments is NULL\n");
        goto ERRO;
    }

    value_num = bsp_dr_bre_time_selectors_get(led_id, bre_time_enum);
    if(BSP_DR_PARA_ERROR == value_num)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"%d get %d value num failed\n", led_id, bre_time_enum);
        goto ERRO;
    }

    /* ѡ����õĵ�λֵ */
    breath_time = *p_breathtime_ms;
    for(selector = 0; selector < value_num; selector++)
    {
        temp_bigger = bsp_dr_bre_time_list(led_id, bre_time_enum, selector);
        if(BSP_DR_PARA_ERROR == temp_bigger)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR),"%d get breath time failed\n", led_id);
            goto ERRO;
        }
        if(temp_bigger >= breath_time)
        {
            LED_TRACE(LED_DEBUG_LEVEL(INFO),"%d bigger breath time %d\n", led_id, temp_bigger);
            break;
        }
    }

    if(value_num == selector)   /* �����ֵ�������λ����ʹ�����λ */
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR), "%d breath time is larger than max, breath_time %d, max value %d\n",
                  led_id, breath_time, temp_bigger);
        *p_breathtime_ms = temp_bigger;
    }
    else if(0 == selector)      /* �����ֵС����С��λ����ʹ����С��λ */
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"%d breath time is samller than min, breath_time %d, min value %d\n",
                  led_id, breath_time, temp_bigger);
        *p_breathtime_ms = temp_bigger;
    }
    else                        /* �����ֵ�������λ����С��λ֮�䣬ѡ������ĵ�λ�����ǡλ���������м䣬ʹ�ý�С��λ */
    {
        temp_smaller = bsp_dr_bre_time_list(led_id, bre_time_enum, selector - 1);
        if(BSP_DR_PARA_ERROR == temp_smaller)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR),"%d get breath time failed\n", led_id);
            goto ERRO;
        }
        *p_breathtime_ms = ((temp_bigger - breath_time) > (breath_time - temp_smaller) ? temp_smaller : temp_bigger);
        LED_TRACE(LED_DEBUG_LEVEL(DEBUG),"%d breath time bigger %d, smaller %d\n",
                  led_id, temp_bigger, temp_smaller);
        LED_TRACE(LED_DEBUG_LEVEL(DEBUG),"%d breath time %d, choice %d\n",
                  led_id, breath_time, *p_breathtime_ms);
    }

    return LED_OK;
ERRO:
    return ret;
}

/************************************************************************
 * Name         : set_bre_mode
 * Function     : set breath mode
 * Arguments
 *      input   : @led_id   - led id
 *
 * Return       : 0 - success; else - failed
 * Decription   : DR1/2��DR3/4/5�ĺ���ģʽ���ò�һ��
 ************************************************************************/
int set_bre_mode(int led_id)
{
    int ret = LED_ERROR;

#ifdef USE_5_DR
    switch(led_id)
    {

    case PMIC_DR01:
    case PMIC_DR02:
        ret = bsp_dr_set_mode(led_id, PMU_DRS_MODE_BRE_FLASH);
        break;
    case PMIC_DR03:
    case PMIC_DR04:
    case PMIC_DR05:
        ret = bsp_dr_set_mode(led_id, PMU_DRS_MODE_BRE);
        break;
    default:
        break;
    };
#else
    ret = bsp_dr_set_mode(led_id, PMU_DRS_MODE_BRE);
#endif
    return ret;
}
/************************************************************************
 * Name         : balong_brightness_set
 * Function     : set brightness
 * Arguments
 *      input   : @led_cdev - which led
 *                @brightness - brightness, should between 0~255
 *      output  : null
 *
 * Return       : null
 * Decription   : ������������������ɿ���ʹ���������
 ************************************************************************/
void balong_led_brightness_set(struct led_classdev *led_cdev, enum led_brightness brightness)
{
    struct balong_led_device *led_dev = NULL;
    int led_id, limit, ret;

    if(0 == g_nv_module_ctrl.leds)
        return;

    led_dev = container_of(led_cdev, struct balong_led_device, cdev);

    /* argument check */
    if((!led_cdev) || (!led_dev) || (!led_dev->pdata))
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR), "arguments is NULL\n");
        return;
    }

    LED_TRACE(LED_DEBUG_LEVEL(DEBUG), "%s, brightness %d\n", led_cdev->name, brightness);

    mutex_lock(&led_dev->pdata->mlock);

    /* brightness can't be too big or too small */
    if(brightness >= LED_FULL)
    {
        brightness = LED_FULL;
    }

    /* get id of led */
    led_id = name_to_id(led_cdev);

    /* save brightness */
    led_cdev->brightness = brightness;

    /* set limit */
    limit = brightness_to_limit(led_id, brightness);
    if(0 == limit)
    {
        /* if enabled, disable */
        if(regulator_is_enabled(led_dev->pdata->pregulator))
        {
            ret = regulator_disable(led_dev->pdata->pregulator);
            if(ret)
            {
                LED_TRACE(LED_DEBUG_LEVEL(ERROR),"%s regulator_disable failed, ret = %d\n",
                          led_cdev->name, ret);
                goto EXIT;
            }
        }
        goto EXIT;
    }
    regulator_set_current_limit(led_dev->pdata->pregulator, limit, limit);

    LED_TRACE(LED_DEBUG_LEVEL(DEBUG),"%s set brightness %d limit %d\n",
              led_cdev->name, brightness, limit);

    /* enable led */
    if(!regulator_is_enabled(led_dev->pdata->pregulator))
    {
        ret = regulator_enable(led_dev->pdata->pregulator);
        if(ret)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR), "%s regulator_enable failed, ret = %d\n",
                      led_cdev->name, ret);
            goto EXIT;
        }
    }

EXIT:
    mutex_unlock(&led_dev->pdata->mlock);
    return;
}

/************************************************************************
 * Name         : brightness_get
 * Function     : get brightness
 * Arguments
 *      input   : @led_cdev - which led
 *      output  : null
 *
 * Return       : brightness
 * Decription   : null
 ************************************************************************/
enum led_brightness balong_led_brightness_get(struct led_classdev *led_cdev)
{
    if(0 == g_nv_module_ctrl.leds)
        return (enum led_brightness)LED_OK;

    /* argument check */
    if(!led_cdev)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"arguments is NULL\n");
        return (enum led_brightness)LED_ERROR;
    }

    return (enum led_brightness)led_cdev->brightness;
}

/************************************************************************
 * Name         : balong_blink_set
 * Function     : set blink
 * Arguments
 *      input   : @led_cdev - which led
 *              : @delay_on - delay time when led is on
 *              : @delay_off - delay time when led is off
 *      output  : null
 *
 * Return       : 0 - success
 *              : else - error
 * Decription   : null
 ************************************************************************/
int balong_led_blink_set(struct led_classdev *led_cdev, unsigned long *delay_on, unsigned long *delay_off)
{
    int led_id, ret;
    unsigned long local_delayon, local_delayoff;
    DR_FLA_TIME dr_fla_time;
    struct balong_led_device *led_dev = NULL;

    if(0 == g_nv_module_ctrl.leds)
        return LED_OK;

    led_dev = container_of(led_cdev, struct balong_led_device, cdev);

    /* argument check */
    if((!led_cdev) || (!led_dev) || (!led_dev->pdata))
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"arguments is NULL\n");
        return LED_ERROR;
    }

    mutex_lock(&led_dev->pdata->mlock);

    /* get id of led */
    led_id = name_to_id(led_cdev);

    /* if null, set LED_VALUE_INVALID */
    if(!delay_on)
    {
        local_delayon = LED_VALUE_INVALID;
        delay_on = &local_delayon;
    }

    if(!delay_off)
    {
        local_delayoff = LED_VALUE_INVALID;
        delay_off = &local_delayoff;
    }

    LED_TRACE(LED_DEBUG_LEVEL(DEBUG),"%s blink feriod %d, blink on %d\n",
              led_cdev->name, *delay_on + *delay_off, *delay_on);

    /* set blink */
    dr_fla_time.fla_off_us = *delay_off;
    dr_fla_time.fla_on_us = *delay_on;
    ret = bsp_dr_fla_time_set(led_id, &dr_fla_time);
    if(ret)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"%s ERROR: set flash time failed, ret = %d.\n",
                  led_cdev->name, ret);
        goto EXIT;
    }

    /* set blink mode */
    ret = bsp_dr_set_mode(led_id, PMU_DRS_MODE_FLA_FLASH);
    if(ret)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"%s ERROR: set flash mode failed, ret = %d.\n",
                  led_cdev->name, ret);
        goto EXIT;
    }

    mutex_unlock(&led_dev->pdata->mlock);
    return LED_OK;

EXIT:
    mutex_unlock(&led_dev->pdata->mlock);
    return LED_ERROR;
}

/************************************************************************
 * Name         : balong_led_breath_set
 * Function     : set
 * Arguments
 *      input   : @led_cdev - which led
 *              : @full_on - delay time(us) when led is on
 *              : @full_off - delay time(us) when led is off
 *              : @fade_on - delay time(us) when led is breathing from off to on
 *              : @fade_off - delay time(us) when led is breathing form on to off
 *      output  : null
 *
 * Return       : 0 - success
 *              : else - error
 * Decription   : null
 ************************************************************************/
int balong_led_breath_set(struct led_classdev *led_cdev, unsigned long *full_on, unsigned long *full_off, unsigned long *fade_on, unsigned long *fade_off)
{
    DR_BRE_TIME dr_bre_time;
    int led_id, ret;
    unsigned long local_fullon, local_fulloff, local_breon, local_breoff;
    struct balong_led_device *led_dev = NULL;

    if(0 == g_nv_module_ctrl.leds)
        return LED_OK;

    led_dev = container_of(led_cdev, struct balong_led_device, cdev);

    /* argument check */
    if((!led_cdev) || (!led_dev) || (!led_dev->pdata))
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"arguments is NULL.\n");
        return LED_ERROR;
    }

    mutex_lock(&led_dev->pdata->mlock);

    /* get id of led */
    led_id = name_to_id(led_cdev);

    /* if null, set default value */
    if(!full_on)
    {
        local_fullon = LED_VALUE_INVALID;
    }
    else
    {
        local_fullon = us2ms(*full_on);
        ret = set_bre_time_valid(led_id, PMU_DRS_BRE_ON_MS, &local_fullon);
        if(ret)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR),"%s ERROR: set breath time valid failed, ret = %d.\n",
                      led_cdev->name, ret);
            goto EXIT;
        }
    }

    if(!full_off)
    {
        local_fulloff = LED_VALUE_INVALID;
    }
    else
    {
        local_fulloff = us2ms(*full_off);
        ret = set_bre_time_valid(led_id, PMU_DRS_BRE_OFF_MS, &local_fulloff);
        if(ret)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR),"%s ERROR: set breath time valid failed, ret = %d.\n",
                      led_cdev->name, ret);
            goto EXIT;
        }
    }

    if(!fade_on)
    {
        local_breon = LED_VALUE_INVALID;
    }
    else
    {
        local_breon = us2ms(*fade_on);
        ret = set_bre_time_valid(led_id, PMU_DRS_BRE_RISE_MS, &local_breon);
        if(ret)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR),"%s ERROR: set breath time valid failed, ret = %d.\n",
                      led_cdev->name, ret);
            goto EXIT;
        }
    }

    if(!fade_off)
    {
        local_breoff = LED_VALUE_INVALID;
    }
    else
    {
        local_breoff = us2ms(*fade_off);
        ret = set_bre_time_valid(led_id, PMU_DRS_BRE_FALL_MS, &local_breoff);
        if(ret)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR),"%s ERROR: set breath time valid failed, ret = %d.\n",
                      led_cdev->name, ret);
            goto EXIT;
        }
    }

    /* set time */
    dr_bre_time.bre_fall_ms = local_breoff;
    dr_bre_time.bre_off_ms = local_fulloff;
    dr_bre_time.bre_on_ms = local_fullon;
    dr_bre_time.bre_rise_ms = local_breon;

    LED_TRACE(LED_DEBUG_LEVEL(DEBUG),"%d full_on %d, full_off %d, fade_on %d, fade_off %d\n",
              led_id, dr_bre_time.bre_on_ms, dr_bre_time.bre_off_ms, dr_bre_time.bre_rise_ms, dr_bre_time.bre_fall_ms);

    ret = bsp_dr_bre_time_set(led_id, &dr_bre_time);
    if(ret)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"%s ERROR: set breath time failed, ret = %d.\n",
                  led_cdev->name, ret);
        goto EXIT;
    }

    /* set breath mode */
    ret = set_bre_mode(led_id);
    if(ret)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"%s ERROR: set breath mode, ret = %d.\n",
                  led_cdev->name, ret);
        goto EXIT;
    }

    mutex_unlock(&led_dev->pdata->mlock);
    return LED_OK;

EXIT:
    mutex_unlock(&led_dev->pdata->mlock);
    return LED_ERROR;

}
/************************************************************************
 * Name         : register_led_by_nr
 * Function     : register led by number
 * Arguments
 *      input   : nr - LED id, from 0 to 4
 *      output  : null
 *
 * Return       : 0 - success
 *              : else - error
 * Decription   : null
 ************************************************************************/
int register_led_by_nr(struct balong_led_device *led, int nr, struct led_platform_data *pdata)
{
    led[nr].pdata->led_breath_set      = balong_led_breath_set;
    led[nr].cdev.brightness_set        = balong_led_brightness_set;
    led[nr].cdev.brightness_get        = balong_led_brightness_get;
    led[nr].cdev.blink_set             = balong_led_blink_set;
    led[nr].cdev.name                  = pdata->leds[nr].name;

    /* regist regulator device */
    led[nr].pdata->pregulator = regulator_get(NULL, led[nr].cdev.name);

#if (FEATURE_ON == MBB_LED)
    dr_regulator[nr] = led[nr].pdata->pregulator;    /*��ֵregulatorȫ�ֱ���*/
#endif
    
    if (IS_ERR(led[nr].pdata->pregulator))
    {
        pr_err("[%s] led%d get regulator failed\n", __FUNCTION__, nr + 1);
        return LED_ERROR;
    }

    led[nr].cdev.default_trigger       = pdata->leds[nr].default_trigger;
    led[nr].cdev.flags                 = 0;

    /* mutex lock init */
    mutex_init(&led[nr].pdata->mlock);
    return LED_OK;
}

/*lint -e534 -e516*/
#if(FEATURE_ON == MBB_FEATURE_M2M_LED)

BSP_VOID BSP_CombineLedNVCtrlPara(
    NV_LED_SET_PARA_STRU               *pstLedNvStru,
    NV_LED_SET_PARA_STRU_EXPAND        *pstLedNvExStru,
    NV_LED_SET_PARA_STRU_COMBINED      *pstLedNvCombinedStru
)
{
    BSP_U8                              i;

    /* �ϲ�GU\LTE��ʽ���������� */
    pstLedNvCombinedStru->ucLedMode = pstLedNvStru->ucLedMode;

    for (i = 0; i < NV_GU_LED_SERVICE_STATE_NUM; i++)
    {
        memcpy(&pstLedNvCombinedStru->stLedStr[i], &pstLedNvStru->stLedStr[i], sizeof(M2M_LED_CONTROL_STRU));
    }

    for (i = 0; i < NV_LTE_LED_SERVICE_STATE_NUM; i++)
    {
        memcpy(&pstLedNvCombinedStru->stLedStr[NV_GU_LED_SERVICE_STATE_NUM + i], &pstLedNvExStru->stLedStr[i], sizeof(M2M_LED_CONTROL_STRU));
    }
}


BSP_VOID BSP_PartLedNVCtrlPara(
    NV_LED_SET_PARA_STRU               *pstLedNvStru,
    NV_LED_SET_PARA_STRU_EXPAND        *pstLedNvExStru,
    NV_LED_SET_PARA_STRU_COMBINED      *pstLedNvCombinedStru
)
{
    BSP_U8                              i;

    /* ���GU\LTE��ʽ���������� */
    pstLedNvStru->ucLedMode = pstLedNvCombinedStru->ucLedMode;

    for (i = 0; i < NV_GU_LED_SERVICE_STATE_NUM; i++)
    {
        memcpy(&pstLedNvStru->stLedStr[i], &pstLedNvCombinedStru->stLedStr[i], sizeof(M2M_LED_CONTROL_STRU));
    }

    for (i = 0; i < NV_LTE_LED_SERVICE_STATE_NUM; i++)
    {
        memcpy(&pstLedNvExStru->stLedStr[i], &pstLedNvCombinedStru->stLedStr[NV_GU_LED_SERVICE_STATE_NUM + i], sizeof(M2M_LED_CONTROL_STRU));
    }
}

/****************************************************************************************
 �� �� ��  : BSP_LedNVParaPro
 ��������  : 1.����NV����������֤NV������Ч��
             2.���û�δ����ĳЩ����״̬�µ����Ʒ�ʽ����������
             3.�޸�ȫ�ֱ�����Ĭ�ϳ���������ʱ��
 �������  :
 �������  : ��
 �� �� ֵ  : ����NVֵ�Ƿ�Ϸ�
 ע������  : ��

****************************************************************************************/
int  BSP_LedNVParaPro(
    NV_LED_SET_PARA_STRU_COMBINED      *pstLedNvStru
)
{
    BSP_U8                              ucLoop1 = 0;
    BSP_U8                              ucLoop2 = 0;
    BSP_U8                              ucLedColor;
    BSP_U8                              ucTimeLength;
    BSP_U8                              ucLedColor_ON;
    BSP_U8                              ucLedColor_OFF;    
    BSP_U8                              ucTimeLength_ON;
    BSP_U8                              ucTimeLength_OFF;    

    if (NULL == pstLedNvStru)
    {
        return BSP_ERROR;
    }

    switch(pstLedNvStru->ucLedMode)
    {        
        /* ����ģʽΪ�ر� */
        case DRV_M2M_LED_MODE_CLOSED:
            memset(pstLedNvStru, 0x00, sizeof(NV_LED_SET_PARA_STRU_COMBINED));
            return BSP_OK;

        /* ����ģʽΪ��ΪĬ�����Ʒ�ʽ */
        case DRV_M2M_LED_MODE_DEFAULT:
            memset(pstLedNvStru, 0x00, sizeof(NV_LED_SET_PARA_STRU_COMBINED));
            pstLedNvStru->ucLedMode = 1;            
            memcpy(g_nv_led.g_led_state_str_om, g_led_M2M_StateStr, sizeof(g_led_M2M_StateStr));

            return BSP_OK;
            
        /* ����ģʽΪ��Ϊ�û��Զ������Ʒ�ʽ */
        case DRV_M2M_LED_MODE_USER:
            /* ����NV��ÿһ���������ã����NV�����д�����ΪĬ�����Ʒ�ʽ��˸ */
            for (ucLoop1 = 0; ucLoop1 < NV_LED_SERVICE_STATE_NUM; ucLoop1++)
            {
                ucLedColor   = pstLedNvStru->stLedStr[ucLoop1].stLED[0].ucLedColor;
                ucTimeLength = pstLedNvStru->stLedStr[ucLoop1].stLED[0].ucTimeLength;            
                
                /* CASE1:�û�δ����ĳ����״̬�µ����Ʒ�ʽ */
                if (0 == ucTimeLength)
                {
                    memset(pstLedNvStru->stLedStr + ucLoop1, 0x00, sizeof(M2M_LED_CONTROL_STRU));
    
                    /* ��Ĭ�ϳ���ʽ��˸ */
                    g_nv_led.g_led_state_str_om[ucLoop1][0].ucLedColor    = LED_NULL;
                    g_nv_led.g_led_state_str_om[ucLoop1][0].ucTimeLength  = NV_LED_FLICKER_DEFAULT_TIME;
                }
                /* CASE2:�û����ø÷���״̬�µ����Ʒ�ʽΪ���������� */
                else if (0xFF == ucTimeLength)
                {
                    if (ucLedColor <= LED_WHITE)
                    {
                        memset(pstLedNvStru->stLedStr + ucLoop1, 0x00, sizeof(M2M_LED_CONTROL_STRU));
                        pstLedNvStru->stLedStr[ucLoop1].stLED[0].ucLedColor     = ucLedColor;
                        pstLedNvStru->stLedStr[ucLoop1].stLED[0].ucTimeLength   = ucTimeLength;
                        
                        /* ��Ĭ�ϳ��𡢳�����ʽ��˸ */
                        memset(g_nv_led.g_led_state_str_om[ucLoop1], 0x00, sizeof(g_nv_led.g_led_state_str_om[ucLoop1]));
                        g_nv_led.g_led_state_str_om[ucLoop1][0].ucLedColor    = ucLedColor;
                        g_nv_led.g_led_state_str_om[ucLoop1][0].ucTimeLength  = NV_LED_FLICKER_DEFAULT_TIME;
                    }
                    else
                    {
                        return BSP_ERROR;
                    }
                }
                /* CASE3:�û����ø÷���״̬�µ����Ʒ�ʽΪ������˫�� */
                else
                {                       
                    for (ucLoop2 = 0; ucLoop2 < NV_LED_FLICKER_MAX_NUM; ucLoop2++)
                    {
                        ucLedColor_ON    = pstLedNvStru->stLedStr[ucLoop1].stLED[2*ucLoop2].ucLedColor;
                        ucTimeLength_ON  = pstLedNvStru->stLedStr[ucLoop1].stLED[2*ucLoop2].ucTimeLength;
                        ucLedColor_OFF   = pstLedNvStru->stLedStr[ucLoop1].stLED[2*ucLoop2 + 1].ucLedColor;
                        ucTimeLength_OFF = pstLedNvStru->stLedStr[ucLoop1].stLED[2*ucLoop2 + 1].ucTimeLength;
    
                        /* �жϲ�����ȡֵ�Ƿ�Ϸ� */
                        if ((ucLedColor_ON > LED_NULL && ucLedColor_ON <= LED_WHITE) 
                            && (LED_NULL == ucLedColor_OFF) 
                            && (ucTimeLength_ON > 0 && ucTimeLength_ON <= NV_LED_FLICKER_MAX_TIME)
                            && (ucTimeLength_OFF > 0 && ucTimeLength_OFF <= NV_LED_FLICKER_MAX_TIME))
                        {
                            g_nv_led.g_led_state_str_om[ucLoop1][2*ucLoop2].ucLedColor        = ucLedColor_ON;
                            g_nv_led.g_led_state_str_om[ucLoop1][2*ucLoop2].ucTimeLength      = ucTimeLength_ON;
                            g_nv_led.g_led_state_str_om[ucLoop1][2*ucLoop2 + 1].ucLedColor    = ucLedColor_OFF;
                            g_nv_led.g_led_state_str_om[ucLoop1][2*ucLoop2 + 1].ucTimeLength  = ucTimeLength_OFF;
                        }
                        else if ((LED_NULL == ucLedColor_OFF && LED_NULL == ucLedColor_ON) 
                            && (0 == ucTimeLength_ON && 0 == ucTimeLength_OFF))
                        {
                            break;
                        }
                        else
                        {
                            return BSP_ERROR;
                        }
                    }
                }  
            }            
            return BSP_OK;
            
        default:
            return BSP_ERROR;
            
    }
}

/****************************************************************************************
 �� �� ��  : BSP_M2M_LedUpdate
 ��������  : ģ���������ø��²���
 �������  :
 �������  : ��
 �� �� ֵ  : ��
 ע������  : ��

****************************************************************************************/
BSP_VOID BSP_M2M_LedUpdate(BSP_VOID *pstLedNvStru)
{
    NV_LED_SET_PARA_STRU                stLedNvStru;
    NV_LED_SET_PARA_STRU_EXPAND         stLedNvExStru;
    NV_LED_SET_PARA_STRU_COMBINED       stLedNvCombinedStru;
    
    memset(&stLedNvStru, 0x00, sizeof(NV_LED_SET_PARA_STRU));
    memset(&stLedNvExStru, 0x00, sizeof(NV_LED_SET_PARA_STRU_EXPAND));
    memset(&stLedNvCombinedStru, 0x00, sizeof(NV_LED_SET_PARA_STRU_COMBINED)); 
    memset(g_nv_led.g_led_state_str_om, 0x00, sizeof(g_nv_led.g_led_state_str_om)); 

    /* NV��ȡʧ�ܣ����û�ΪĬ�����Ʒ��� */    
    if((NV_OK != bsp_nvm_read(en_NV_Item_M2M_LED_CONTROL, &stLedNvStru, sizeof(NV_LED_SET_PARA_STRU)))
        || (NV_OK != bsp_nvm_read(en_NV_Item_M2M_LED_CONTROL_EX, &stLedNvExStru, sizeof(NV_LED_SET_PARA_STRU_EXPAND))))
    {
        /* �޸�NV����Ϊ��ΪĬ�����Ʒ��� */
        stLedNvCombinedStru.ucLedMode = 1;

        memcpy(g_nv_led.g_led_state_str_om, g_led_M2M_StateStr, sizeof(g_led_M2M_StateStr));
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] read M2M LED NV fail\n", __FUNCTION__);
    }
    /* NV��ȡ�ɹ�����NV���÷�ʽ���� */
    else
    {
        /* ��ȡ�ϲ��������NV���� */
        BSP_CombineLedNVCtrlPara(&stLedNvStru, &stLedNvExStru, &stLedNvCombinedStru);
    
        /* �ж�����NV���õĺϷ��� */
        if (BSP_ERROR == BSP_LedNVParaPro(&stLedNvCombinedStru))
        {
            /* ���ж�����NV������Ч�����û�ΪĬ�����Ʒ��� */
            memset(&stLedNvCombinedStru, 0x00, sizeof(NV_LED_SET_PARA_STRU_COMBINED));
            stLedNvCombinedStru.ucLedMode = 1;
            
            memcpy(g_nv_led.g_led_state_str_om, g_led_M2M_StateStr, sizeof(g_led_M2M_StateStr));

            LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] LED NV CONFIG WRONG\n", __FUNCTION__);
        }        
    }

    if (BSP_NULL != pstLedNvStru)
    {
        memcpy(pstLedNvStru, &stLedNvCombinedStru, sizeof(NV_LED_SET_PARA_STRU_COMBINED));
    }

    /* ��������״̬������ģʽΪ�̵�Ƶ�� */
    g_nv_led.g_led_state_str_om[DRV_M2M_LED_NORMAL_UPDATING][0].ucLedColor    = LED_RED;
    g_nv_led.g_led_state_str_om[DRV_M2M_LED_NORMAL_UPDATING][0].ucTimeLength  = NV_LED_FLICKER_DEFAULT_TIME;
    g_nv_led.g_led_state_str_om[DRV_M2M_LED_NORMAL_UPDATING][1].ucLedColor    = LED_NULL;
    g_nv_led.g_led_state_str_om[DRV_M2M_LED_NORMAL_UPDATING][1].ucTimeLength  = NV_LED_FLICKER_DEFAULT_TIME;
}

/****************************************************************************************
 �� �� ��  : BSP_M2M_LedInit
 ��������  : ģ�����Ƴ�ʼ������
 �������  :
 �������  : ��
 �� �� ֵ  : ��
 ע������  : ��

****************************************************************************************/
BSP_VOID BSP_M2M_LedInit(void)
{
    NV_LED_SET_PARA_STRU                stLedNvStru;
    NV_LED_SET_PARA_STRU_EXPAND         stLedNvExStru;
    NV_LED_SET_PARA_STRU_COMBINED       stComBinedLedNvStru;
    
#if 0 //temp mark
    /* ��MU609����һ�£�����ΪDR1��DR2��������Ϊ12mA */
    pmu_hi6558_reg_write(0, &g_stCurrentSetArg[0], 0);
    pmu_hi6558_reg_write(0, &g_stCurrentSetArg[1], 1);
    pmu_hi6558_reg_write(0, &g_stCurrentSetArg[2], 1);
    pmu_hi6558_reg_write(0, &g_stCurrentSetArg[3], 0);
    pmu_hi6558_reg_write(0, &g_stCurrentSetArg[4], 0);
    pmu_hi6558_reg_write(0, &g_stCurrentSetArg[5], 1);
    pmu_hi6558_reg_write(0, &g_stCurrentSetArg[6], 1);
    pmu_hi6558_reg_write(0, &g_stCurrentSetArg[7], 0);

    /*��ʼ����ɫ��״̬�ṹ��*/
    g_ledPara.ucLedStateID  = DRV_M2M_LED_SYSTEM_INITIAL;
    g_ledPara.ucLedConfigID = 0;
    g_ledPara.ucLedColorID  = g_led_M2M_StateStr[DRV_M2M_LED_SYSTEM_INITIAL][0].ucLedColor;
    g_ledPara.ucLedTime     = g_led_M2M_StateStr[DRV_M2M_LED_SYSTEM_INITIAL][0].ucTimeLength;
    g_ledStateChange        = 1;
#endif
    memset(&stLedNvStru, 0x00, sizeof(NV_LED_SET_PARA_STRU));
    memset(&stLedNvExStru, 0x00, sizeof(NV_LED_SET_PARA_STRU_EXPAND));
    memset(&stComBinedLedNvStru, 0x00, sizeof(NV_LED_SET_PARA_STRU_COMBINED));

    /* ��ȡ�ϲ��������NV���� */
    BSP_M2M_LedUpdate(&stComBinedLedNvStru);

    /* ��ȡ��ֺ������NV���� */
    BSP_PartLedNVCtrlPara(&stLedNvStru, &stLedNvExStru, &stComBinedLedNvStru);

    /* ���ּ�����޸ĺ������NV���� */
    if ((NV_OK != bsp_nvm_write(en_NV_Item_M2M_LED_CONTROL, &stLedNvStru, sizeof(stLedNvStru)))
        || (NV_OK != bsp_nvm_write(en_NV_Item_M2M_LED_CONTROL_EX, &stLedNvExStru, sizeof(stLedNvExStru))))
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] BSP_M2M_LedInit: write LED CONTROL NV fail\n", __FUNCTION__);
    }

    drv_led_flash(DRV_M2M_LED_SYSTEM_INITIAL);
}

EXPORT_SYMBOL(BSP_M2M_LedUpdate);
EXPORT_SYMBOL(BSP_CombineLedNVCtrlPara);
EXPORT_SYMBOL(BSP_PartLedNVCtrlPara);


BSP_VOID BSP_CE_LedInit(void)
{
   memcpy(g_nv_led.g_led_state_str_om,g_led_ce_state_str,sizeof(g_led_ce_state_str));
    ce_led_softtimer.func = (softtimer_func)do_led_status;
    ce_led_softtimer.para = 0;                              
    ce_led_softtimer.timeout = 500;                        /* ��ʱ���ȣ���λms */
    ce_led_softtimer.wake_type = SOFTTIMER_NOWAKE;
    if (bsp_softtimer_create(&ce_led_softtimer))
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s]ERROR: softtimer create failed\n", __FUNCTION__);
        return;
    }
    bsp_softtimer_add(&ce_led_softtimer);
    return;
}
#endif


/************************************************************************
 * Name         : balong_led_probe
 * Function     : probe when init
 * Arguments
 *      input   : @pdev - platform device
 *      output  : null
 *
 * Return       : 0 - success
 *              : else - error
 * Decription   : null
 ************************************************************************/
static int balong_led_probe(struct platform_device *dev)
{
    struct led_platform_data *pdata = dev->dev.platform_data;
    struct balong_led_device *led   = NULL;
    int ret = LED_OK;
    unsigned int i;
#if (FEATURE_ON == MBB_FEATURE_M2M_LED)
    MDRV_VER_SOLUTION_TYPE module_type = MDRV_VER_TYPE_CE;
    module_type = mdrv_ver_get_solution_type(); 
#endif

    ret = (s32)bsp_nvm_read(NV_ID_DRV_TEST_SUPPORT, (u8 *)&g_nv_module_ctrl, sizeof(DRV_MODULE_TEST_STRU));
    if(ret)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"error: read nv failed, ret = %d, enable default\n", ret);
        g_nv_module_ctrl.leds = 1;
    }

    if(0 == g_nv_module_ctrl.leds)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"leds stub\n");
        return LED_OK;
    }

    led = (struct balong_led_device *)kzalloc(sizeof(struct balong_led_device) * ARRAY_SIZE(balong_led), GFP_KERNEL);
    if(!led)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"get buffer failed\n");
        goto ERRO;
    }

    /* get pdata, register led and register classdev for every led device */
    for(i = (unsigned int)PMU_DR_MIN; i <= (unsigned int)PMU_DR_MAX; i++)
    {
        led[i].pdata = (struct balong_led_platdata *)kzalloc(sizeof(struct balong_led_platdata), GFP_KERNEL);
        if(!led[i].pdata)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR),"get pdata buffer failed\n");
            goto ERRO;
        }

        /* regulator register failed is possible, as not all DRs are for LED */
        ret = register_led_by_nr(led, (int)i, pdata);
        if(!ret)
        {
            ret = led_classdev_register(&dev->dev, &(led[i].cdev));
            if(ret)
            {
                LED_TRACE(LED_DEBUG_LEVEL(ERROR),"led%d sysfs register failed\n", i + 1);
            }
        }

        /* coverity[unsigned_compare] */
        if((unsigned int)PMU_TLED_DR_MIN <= i)
        {
            /* set start delay */
            ret = bsp_dr_start_delay_set(i, 0);
            if(ret)
            {
                LED_TRACE(LED_DEBUG_LEVEL(ERROR),"set led%d start delay ERROR, ret = %d.\n",
                          i + 1, ret);
                goto ERRO;
            }
        }


    }

    /* save "led" as private data */
    platform_set_drvdata(dev, led);

    /* timer create for three-color-led */
    led_softtimer.func = (softtimer_func)do_led_threecolor_flush;
    led_softtimer.para = 0;
    led_softtimer.timeout = 0;                        /* ��ʱ���ȣ���λms */
    led_softtimer.wake_type = SOFTTIMER_WAKE;

    if (bsp_softtimer_create(&led_softtimer))
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"ERROR: softtimer create failed\n");
        goto ERRO;
    }

    /* read NV and store for three-color-led*/
    if(g_nv_led.g_already_read != LED_NV_ALREADY_READ)
    {
        ret = (int)bsp_nvm_read(NV_ID_DRV_LED_CONTROL,(u8 *)g_nv_led.g_led_state_str_om, LED_LIGHT_STATE_MAX * LED_CONFIG_MAX_LTE * sizeof(LED_CONTROL_NV_STRU));
        if(NV_OK != ret)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR), "WARNING: read nv failed, use default, ret = %d!\n", ret);

            /* ��nvʧ�ܣ�����Ĭ��ֵ */
            memcpy(g_nv_led.g_led_state_str_om, g_led_state_str_lte,
                   LED_LIGHT_STATE_MAX * LED_CONFIG_MAX_LTE * sizeof(LED_CONTROL_NV_STRU));
        }
    }

/*lint -e553*/
#if (FEATURE_ON == MBB_FEATURE_M2M_LED)
    if (MDRV_VER_TYPE_CE == module_type)
    {
         BSP_CE_LedInit();
    }
    else
    {
        BSP_M2M_LedInit();
    }
#else /*FEATURE_ON == MBB_FEATURE_M2M_LED*/
#if (FEATURE_ON == MBB_WPG_LED_FLASH)
    /*��NV50032�ӻ�ȡ���Ƶķ���*/
    unsigned long ulLEDStatus = DEF;
    if(NV_OK != bsp_nvm_read(en_NV_Item_HUAWEI_PCCW_HS_HSPA_BLUE,&ulLEDStatus,sizeof(unsigned long)))
    {
        /*NV��ȡʧ�ܣ���ʹ��Ĭ�����Ʒ���*/
        ulLEDStatus = DEF;
    }

    /*����NV��ȡֵ��ʹ�ò�ͬ���������÷���*/
    switch(ulLEDStatus)
    {
        case(DEF):/*Ĭ������*/
        {
            break;
        }

        case(HONGKONG):/*���PCCW*/
        {
            memcpy(g_nv_led.g_led_state_str_om,g_led_HongKong_StateStr,sizeof(g_led_state_str_lte));
            break;
        }

        case(RUSSIA):/*����˹MTS*/
        {
            memcpy(g_nv_led.g_led_state_str_om,g_led_Russia_StateStr,sizeof(g_led_state_str_lte));
            break;
        }

        case(VODAFONE):/*Vodafone*/
        {
            memcpy(g_nv_led.g_led_state_str_om,g_led_Vodafone_StateStr,sizeof(g_led_state_str_lte));
            break;
        }

        case(CHINAUNION):/*�й���ͨ*/
        {
            memcpy(g_nv_led.g_led_state_str_om,g_led_ChinaUnion_StateStr,sizeof(g_led_state_str_lte));
            break;
        }

        case(SFR):/*����SFR*/
        {
            memcpy(g_nv_led.g_led_state_str_om,g_led_FranceSFR_StateStr,sizeof(g_led_state_str_lte));
            break;
        }

        case(BSNL):/*ӡ��BSNL*/
        {
            memcpy(g_nv_led.g_led_state_str_om,g_led_IndiaBSNL_StateStr,sizeof(g_led_state_str_lte));
            break;
        }

        case(KPN):/*����KPN*/
        {
            memcpy(g_nv_led.g_led_state_str_om,g_led_HolandKPN_StateStr,sizeof(g_led_state_str_lte));
            break;
        }

        case(SOFTBANK):/*�ձ�����*/
        {
            memcpy(g_nv_led.g_led_state_str_om,g_led_SoftBank_StateStr,sizeof(g_led_state_str_lte));
            break;
        }

        case(EMOBILE):/*�ձ�EM*/
        {
            memcpy(g_nv_led.g_led_state_str_om,g_led_Emobile_StateStr,sizeof(g_led_state_str_lte));
            break;
        }

        default:
        {
            break;
        }
    }
#endif
#endif /*FEATURE_ON == MBB_FEATURE_M2M_LED*/
/*lint +e553*/ 


    /* icc register for three-color-led*/
    ret = bsp_icc_event_register(LED_ICC_CHN_ID, led_threecolor_flush, NULL, NULL, NULL);
    if(ret != LED_OK)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"register icc event failed, ret = 0x%x\n", ret);
        goto ERRO;
	}
    
    LED_TRACE(LED_DEBUG_LEVEL(DEBUG),"succeed!!!!!register icc callback\n");
#if (FEATURE_ON == MBB_COMMON)
    g_leds_balong_init = 1; /* ��ʾ�������Ƿ��Ѿ���ʼ�� */
#endif

    return ret;

ERRO:
    for(i = 0; i < ARRAY_SIZE(balong_led); i++)
    {
        if(!led)
        {
            continue;
        }

        if(led[i].pdata->pregulator)
        {
            kfree(led[i].pdata->pregulator);
            led[i].pdata->pregulator = NULL;
        }
        if(led[i].pdata)
        {
            kfree(led[i].pdata);
            led[i].pdata = NULL;
        }
    }
    if(led)
    {
        kfree(led);
        led = NULL;
    }
    LED_TRACE(LED_DEBUG_LEVEL(ERROR),"LED init failed\n");
    return LED_ERROR;
}
#if 0
/************************************************************************
 * Name         : balong_led_remove
 * Function     : remove
 * Arguments
 *      input   : @pdev - platform device
 *      output  : null
 *
 * Return       : 0 - success
 *              : else - error
 * Decription   : null
 ************************************************************************/
static int balong_led_remove(struct platform_device *pdev)
{
    int i;
    struct balong_led_device *led = platform_get_drvdata(pdev);
    if(!led)
    {
        return LED_ERROR;
    }

    if(&led->cdev)
    {
        led_classdev_unregister(&led->cdev);
    }

    for(i = 0; i < (int)(ARRAY_SIZE(balong_led)); i++)
    {
        if(!led[i].pdata)

        {
            continue;
        }

        regulator_put(led[i].pdata->pregulator);
        led_classdev_unregister(&(led[i].cdev));
        if(led[i].pdata->pregulator)
        {
            kfree(led[i].pdata->pregulator);
            led[i].pdata->pregulator = NULL;
        }
        if(led[i].pdata)
        {
            kfree(led[i].pdata);
            led[i].pdata = NULL;
        }
    }

    kfree(led);
    led = NULL;

    return LED_OK; /*lint !e438*/
}
#endif

/*****************************************************************************
 �� �� ��  : drv_led_flash
 ��������  : ��ɫ�����ã����ϲ��ṩ�Ľӿ�
 �������  : status����ɫ�Ƶ�״̬
 �������  : �ޡ�
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
*****************************************************************************/
int drv_led_flash(unsigned long state) 
{
    int ret = LED_ERROR;
	struct balong_led_device *led = NULL;
    led = (struct balong_led_device *)platform_get_drvdata(&balong_led_dev);
    if(!led)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"get balong led device failed.\n");
        return LED_ERROR;
    }    

    g_arg.new_state = (unsigned char)state;
    g_arg.ctl = MNTN_LED_STATUS_FLUSH;    

    ret = do_led_threecolor_flush();
    if(ret)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR), "[%s]three color led flush failed, ret = %d!\n", __FUNCTION__, ret);
        return LED_ERROR;
    }

    return LED_OK;
}


/*************************************************************************/
/* for power manager */

/* LED platform driver */
struct platform_driver balong_led_drv =
{
    .probe		= balong_led_probe,
    //.remove		= balong_led_remove,
    .driver		= {
        .name		= "balong_led",
        .owner		= THIS_MODULE,
        .bus        = &platform_bus_type,
    },
};

#if (FEATURE_ON == MBB_LED_DR)
/************************************************************************
 * Name         : led_regulator_dr_enable
 * Function     :
 * Arguments
 *      input   : led_id:0~4
 *      output  : null
 *
 * Return       : 0 - ok
 *                else - fail
 * Decription   : enable dr regulator
 ************************************************************************/
int led_dr_regulator_enable(int led_id)
{
    int ret = LED_ERROR;
    struct balong_led_device *led = NULL;

    led_id = led_id - 1;
    if(0 > led_id || led_id > 4)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] led_id = %d, dr index error.\n", __FUNCTION__, led_id);
        return LED_ERROR;
    }
    
    led = (struct balong_led_device *)platform_get_drvdata(&balong_led_dev);
    if(!led)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"get balong led device failed.\n");
        return LED_ERROR;
    }

    /* enable led */
    if(regulator_is_enabled(led[led_id].pdata->pregulator))
    {
        ret = regulator_enable(led[led_id].pdata->pregulator);
        if(ret)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] regulator_enable failed, ret = %d\n", __FUNCTION__, ret);
            return LED_ERROR;
        }
    }
    
    return LED_OK;
}
EXPORT_SYMBOL(led_dr_regulator_enable);
#endif

/*******************************************************************************
 * FUNC NAME:
 * balong_led_init() - register dirver and device for led driver
 *
 * PARAMETER:
 * none
 *
 * DESCRIPTION:
 * Linux standard driver initialization interface.
 *
 * CALL FUNC:
 *
 ********************************************************************************/
int __init bsp_led_init(void)
{
    int result;

    result = platform_driver_register(&balong_led_drv);
    if (result < 0)
    {
        return result;
    }

    result = platform_device_register(&balong_led_dev);
    if (result < 0)
    {
        platform_driver_unregister(&balong_led_drv);
        return result;
    }

    LED_TRACE(LED_DEBUG_LEVEL(ERROR),"LED init OK\n");

    return result;
}
#if 0
/*******************************************************************************
 * FUNC NAME:
 * led_module_exit() - unregister dirver for led driver
 *
 * PARAMETER:
 * none
 *
 * DESCRIPTION:
 * Linux standard driver de_initialization interface.
 *
 * CALL FUNC:
 * () -
 *
 ********************************************************************************/
static void __exit bsp_led_exit (void)
{
    platform_driver_unregister(&balong_led_drv);
    platform_device_unregister(&balong_led_dev);
}
#endif
/*****************************************************************************/

module_init(bsp_led_init);   /*lint !e19 */



#if(FEATURE_ON == MBB_LED_DR)

int dr_led_switch(int numb, int onoff)
{
    int ret = -1;
    DR_BRE_TIME dr_bre_time_st;
    
    if((0 != onoff) && (1 != onoff))
    {
        printk(KERN_ERR "[%s] input onoff = [%d] wrong. \n", __func__, onoff);
        return -1;
    }
    else if(PMU_DR_MIN > numb || PMU_DR_MAX < numb )
    {
        printk(KERN_ERR "[%s] input numb = [%d] wrong. \n", __func__, onoff);
        return -1;
    }
    
    g_dr_brightness[numb] = -1;        /*����ֵ��λ*/
    /* ���ú���ģʽ */
    ret = set_bre_mode(numb);
    if(ret)
    {
        printk(KERN_ERR "[%s] ERROR: set breath mode, ret = %d, dr = %d.\n", __FUNCTION__, ret, numb);
        return -1;
    }

    /* ���ú�����ʱ�� �˴�ֻ���ó���ʱ�䣬���ɿ��ؿ���*/
    dr_bre_time_st.bre_fall_ms = 0;
    dr_bre_time_st.bre_off_ms = 0;
    dr_bre_time_st.bre_on_ms = (unsigned int)(long_time);
    dr_bre_time_st.bre_rise_ms = 0;

    ret = set_bre_time_valid(numb, PMU_DRS_BRE_FALL_MS, &dr_bre_time_st.bre_fall_ms);
    if(ret)
    {
        printk(KERN_ERR "[%s] ERROR: set bre_fall_ms  valid failed, ret = %d, dr = %d.\n", __FUNCTION__, ret, numb);
        return -1;
    }
    ret = set_bre_time_valid(numb, PMU_DRS_BRE_OFF_MS, &dr_bre_time_st.bre_off_ms);
    if(ret)
    {
        printk(KERN_ERR "[%s] ERROR: set bre_off_ms  valid failed, ret = %d, dr = %d.\n", __FUNCTION__, ret, numb);
        return -1;
    }
    ret = set_bre_time_valid(numb, PMU_DRS_BRE_ON_MS, &dr_bre_time_st.bre_on_ms);
    if(ret)
    {
        printk(KERN_ERR "[%s] ERROR: set bre_on_ms  valid failed, ret = %d, dr = %d.\n", __FUNCTION__, ret, numb);
        return -1;
    }
    ret = set_bre_time_valid(numb, PMU_DRS_BRE_RISE_MS, &dr_bre_time_st.bre_rise_ms);
    if(ret)
    {
        printk(KERN_ERR "[%s] ERROR: set bre_rise_ms  valid failed, ret = %d, dr = %d.\n", __FUNCTION__, ret, numb);
        return -1;
    }

    ret = bsp_dr_bre_time_set((dr_id_e)(numb), &dr_bre_time_st);
    if(ret)
    {
        printk(KERN_ERR "[%s] ERROR: set breath time failed, ret = %d, dr = %d.\n", __FUNCTION__, ret, numb);
        return -1;
    }

    /*�ر�*/
    if(0 == onoff)
    {
        /* if enabled, disable */
        if(regulator_is_enabled(dr_regulator[numb]))
        {
            ret = regulator_disable(dr_regulator[numb]);
            if(ret)
            {
                printk(KERN_ERR "[%s] regulator_disable failed, ret = %d, dr = %d.\n", __FUNCTION__, ret, numb);
                return -1;
            }
        }
        
        return 0;
    }
    
    /*���õ�������Ϊ3mA*/
    regulator_set_current_limit(dr_regulator[numb], current, current);

    /* ���� */
    if(!regulator_is_enabled(dr_regulator[numb]))
    {
        ret = regulator_enable(dr_regulator[numb]);
        if(ret)
        {
            printk(KERN_ERR "[%s] regulator_enable failed, ret = %d, dr = %d\n", __FUNCTION__, ret, numb);
            return -1;
        }
    }

    return 0;
}
#endif

#if(FEATURE_ON == MBB_LED_GPIO)

static void gpio_led_switch(int numb, int onoff)
{    
    (void)gpio_direction_output(numb, onoff);   
 
    return;
}
#endif

#if (FEATURE_ON == MBB_LED)
/************************************************************************
 *����ԭ�� �� int led_kernel_init_status(void)
 *����     �� �ж�df��gpio���Ƶ�LED�Ƿ��ʼ���ɹ�
 *����     �� NA
 *���     �� NA
 *����ֵ   �� ��ʼ���ɹ� 0����ʼ��ʧ��-1
*************************************************************************/
int led_kernel_init_status(void)
{
    int ret = -1;
    if(0 == g_leds_balong_init)
    {
        return ret;
    }
#if(FEATURE_ON == MBB_LED_GPIO)
    ret = gpio_led_init_status();
    if(ret)
    {
        return ret;
    }
#endif

#if(FEATURE_ON == MBB_LED_DR)
    ret = dr_led_init_status();
#endif
    return ret;
}


int led_kernel_status_set(char* led_name, int onoff)
{
    int ret = -1;

    printk(KERN_INFO "[%s] input led_name is %s ,onoff = %d ,g_set_flag = %d. \n", __func__, led_name, onoff,g_set_flag);
    
    if(NULL == led_name)
    {
        printk(KERN_ERR "[%s] input led_name is NULL. \n",  __func__);
        return ret;
    }
    if((0 != onoff) && (1 != onoff))
    {
        printk(KERN_ERR "[%s] input onoff = [%d] wrong. \n", __func__, onoff);
        return ret;
    }
    
    mutex_lock(&g_led_kernelset_lock);

#if(FEATURE_ON == MBB_LED_GPIO)
    ret = gpio_led_name2gpio(led_name);
    if(LEDS_NOT_READY == ret)
    {
        return ret;
    }
    else if(0 <= ret)
    {
        /*���ҳɹ�gpio���Ƶ���*/
        gpio_led_switch( ret , onoff );
        ret = 0;
        goto out;  
    }
    else
    {
        /* �������£�ʲô������ */
    }
#endif

#if(FEATURE_ON == MBB_LED_DR)
    ret = dr_led_name2dr(led_name);
    if(LEDS_NOT_READY == ret)
    {
        return ret;
    }
    else if(0 <= ret)
    {
        /*���ҳɹ�dr���Ƶ���, DR ID��0��ʼ���豸���д�1��ʼ����Ҫת��*/
        dr_led_switch( ret - 1, onoff );      
        ret = 0;
        goto out;  
    }
    else
    {
        /* �������£�ʲô������ */
    }
#endif

    printk(KERN_ERR "[%s] input led_name no found. \n", __func__);  /*����ʧ��*/
out:
    mutex_unlock(&g_led_kernelset_lock);
    return  ret;
}


#endif

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hisilicon Tech.Co.,Ltd.<lusuo@hisilicon.com>");
MODULE_DESCRIPTION("BalongV700R200 Hisilicon LED driver");

#ifdef __cplusplus
}
#endif
