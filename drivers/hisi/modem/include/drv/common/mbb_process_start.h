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

#ifndef __MBB_PROCESS_START_H__
#define __MBB_PROCESS_START_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define SEM_UP  (1)

enum{
    NO_NEED_RESULT = 0,   /*����Ҫ����ֵ*/
    NEED_RESULT,          /*��Ҫ����ֵ*/
    NEED_MAX
};


enum{
    WAIT_TIME_1S = 1000,   /*�ȴ�ʱ��1s*/
    WAIT_TIME_2S = 2000,
    WAIT_TIME_3S = 3000,
    WAIT_TIME_4S = 4000,
    WAIT_TIME_5S = 5000,
    
    WAIT_TIME_1M = 60000,         /*�ȴ�ʱ��1����*/
    WAIT_TIME_MAX = 300*1000,     /*��ߵȴ�ʱ��300s��5����*/
};

/*****************************************************************************
 �� �� ��  : drv_start_user_process
 ��������  : �ײ����Ӧ��̬����ͨ�ýӿ�
 �������  : name: ��������
             para: ���̵Ĳ�����û�еĻ�Ϊnull
             need_result:  �Ƿ���Ҫ����ֵ
             wait_time:��ʱ�ȴ�ʱ�䣬��λms
 �������  : ��
 �� �� ֵ  : 0--���������ɹ�����ִ�н��Ϊok��
             other--ʧ�ܣ�
 ˵    ��  :���ִ�еĽ��̲����˳���һֱִ�У�need_result�����봫��0��wait_timeΪ0��
            ִ�еĽ��̻��˳���need_result�����봫��1��wait_time���Ϊ5���ӣ���λΪms��
            ��ʱ�ýӿڻ������ȴ�ִ�н����ֱ����ʱʱ�䵽��
*****************************************************************************/
int  drv_start_user_process(char* name, char* para, unsigned int need_result, unsigned int wait_time);


#ifdef __cplusplus
}
#endif

#endif

 
