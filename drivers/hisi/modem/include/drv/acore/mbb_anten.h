/******************************************************************************
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
******************************************************************************/
#ifndef __MBB_ANTEN_H__
#define __MBB_ANTEN_H__

#ifdef __cplusplus
extern "C"
{
#endif


/*****************************************************************************
 �� �� ��  : anten_switch_get
 ��������  : �ṩ��ATģ���ȡ�����л�״̬�Ľӿ�
 �������  : ant_type : �������ͣ�0--�����ߣ�1--������
 �������  : ��
 �� �� ֵ  : ��ǰ���ߵ�״̬
 ˵    ��  : �˽ӿ�ֻ��atʹ�ã�Ӧ�û�ȡ״̬ͨ���ڵ����
*****************************************************************************/
int anten_switch_get(unsigned int ant_type);


/*****************************************************************************
 �� �� ��  : anten_switch_set
 ��������  : �ṩ��ATģ����������л��Ľӿ�
 �������  : ant_type : �������ͣ�0--�����ߣ�1--�����ߣ�2--��������
             in_or_out: ʹ�����û����ⲿ����
 �������  : ��
 �� �� ֵ  : 0--�ɹ�������--ʧ��
 ˵    ��  : �˽ӿ�ֻ��atʹ�ã�Ӧ���л�����ͨ���ڵ����
*****************************************************************************/
int anten_switch_set(unsigned int ant_type,unsigned int in_or_out);



#ifdef __cplusplus
}
#endif

#endif /*__MBB_ANTEN_H__*/

