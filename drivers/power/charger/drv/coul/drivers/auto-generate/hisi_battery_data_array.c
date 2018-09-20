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

#include <linux/slab.h>
#include <bsp_version.h>
#include "hisi_battery_data_default.c"
#include "hisi_battery_data_feimaotui_1780.c"
#include "hisi_battery_data_feimaotui_1500.c"
#include "hisi_battery_data_xinwangda_3000.c"
#include "hisi_battery_data_feimaotui_4800.c"
#include "hisi_battery_data_feimaotui_3000.c"
#include "hisi_battery_data_xinwangda_4800.c"
#include "hisi_battery_data_xinwangda_1500.c"

/*----------------------------------------------*
 * ȫ�ֱ���˵��                                 *
 *----------------------------------------------*/
/*
 *�˴���Ų�Ʒ��֧�ֵĵ�ؽ�ģ����,��������Ҫ��default��صĽ�ģ����,
 *����������ʽΪ battery_data_array_product name.
 */
/*Ĭ�Ͽ��ؼ�֧�ֵ�ص���������*/
static struct hisi_smartstar_coul_battery_data *battery_data_array_default[] = {
    &default_battery_data,
    &feimaotui_1780_battery_data,
    &xinwangda_3000_battery_data,
    &feimaotui_4800_battery_data,
    &feimaotui_3000_battery_data,
    &xinwangda_4800_battery_data,
};
/*E5783h_92a���ؼ�֧�ֵ�ص���������*/
static struct hisi_smartstar_coul_battery_data *batt_data_array_E5783h_92a[] = {
    &default_battery_data,
    &feimaotui_1500_battery_data,
    &xinwangda_1500_battery_data,
};

/*
 *�˴��ǲ�Ʒ֧�ֵĵ��������Ӳ���汾�Ž���ƥ��,���ڼ��ؽ�ģ����ʱ
 *ͨ��ƥ��Ӳ��ID�����ز�Ʒ֧�ֵĵ�ؽ�ģ����;֧�ֵ�ظ�������default����,
 *������ʽ coul_battery_data_diff_product name.
 */
/*Ĭ�ϲ�Ʒ֧�ֵĿ��ؼƵ������*/
static coul_battery_data_diff_st coul_battery_data_diff_default =
{
    .hardware_version_id = 0xFFFFFFFF,
    .support_batt_num = ARRAY_SIZE(battery_data_array_default),
    .coul_batt_data_array = battery_data_array_default,
};
/*E5783h_92a֧�ֵĿ��ؼƵ������*/
static coul_battery_data_diff_st coul_battery_data_diff_E5783h_92a =
{
    .hardware_version_id = HW_VER_PRODUCT_E5783h_92a,
    .support_batt_num = ARRAY_SIZE(batt_data_array_E5783h_92a),
    .coul_batt_data_array = batt_data_array_E5783h_92a,
};
/*
 *�˴��Ǵ�����в�Ʒ���ؼ�֧�ֵĵ��ȫ������,���ڼ��ؽ�ģ����ʱ
 *�˱��н��б���,���Ҷ�Ӧ��Ʒ֧�ֵĵ������.
 */
static coul_battery_data_diff_st *coul_battery_data_diff_array[] =
{
    &coul_battery_data_diff_default,
    &coul_battery_data_diff_E5783h_92a,
};

