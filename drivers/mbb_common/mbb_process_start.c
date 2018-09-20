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
 /* ע��:
  * Ӧ��̬��ȡ�ڵ���ļ���Ϊ:process_start_app.c����Ҫ���ӿ�ִ���ļ��ĵ��ã���
  * ͬ���ڸ��ļ���ı������ӣ�����������������롣
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/jiffies.h>
#include "linux/wakelock.h"
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <osl_sem.h>
#include <linux/kthread.h>
#include "mbb_process_start.h"

#ifdef __cplusplus
extern "C" {
#endif

#define procs_err(fmt, ...)  (printk(KERN_ERR "%s: "fmt, __func__, ##__VA_ARGS__))

#define NAME_MAX_LEN   (64)
#define PARA_MAX_LEN   (128)
#define BUFF_MAX_LEN   (256)

#define PROC_NAME      "process_start"
#define CHAR_CR_LR     "\r\n"

typedef enum
{
    PROCESS_STATE_WAIT    = 0,  /*�ý������ڵȴ���ִ��*/
    PROCESS_STATE_EXECULT = 1,  /*�ý�������ִ��*/
    PROCESS_STATE_DONE    = 2,  /*�ý���ִ�н��������Ի�ȡ���*/
}process_state_e;

typedef struct process_node
{
    char name[NAME_MAX_LEN];    /*��������*/
    char para[PARA_MAX_LEN];    /*������Ҫ�Ĳ�����û�п�Ϊ��*/
    int           result;       /*����Ӧ�÷���ֵ*/
    unsigned int  need_result;  /*�Ƿ���Ҫ����ֵ*/
    unsigned int  state;        /*��ǰ״̬*/
    struct list_head  stlist;   
}process_node_str;

typedef struct process_ctrl
{
    unsigned int inited;         /*�Ƿ��ʼ����־*/
    unsigned int count;          /*�������̼���*/
    spinlock_t   lock;
    osl_sem_id   rw_sem;
    wait_queue_head_t pollwait;  /*poll���������ߵȴ�����*/
    wait_queue_head_t listwait; /*��Ҫִ�н���ĵȴ�����*/
    struct list_head  stlist;    /*list ͷ*/
}process_ctl_str;

static process_ctl_str  g_process_info;  


/*���Խӿڣ����ڲ鿴��ǰ������Ľ��̼���״̬�����ֶ�����*/
void show_process_list_info(void)
{
    struct list_head* me  = NULL;
    process_node_str* cur = NULL;

    list_for_each(me,&g_process_info.stlist)
    {
        cur = list_entry(me, process_node_str, stlist);
        if(NULL != cur)
        {
            procs_err("name:%s, para:%s, state:%d, need_result:%d, result:%d\n",
                cur->name, cur->para, cur->state, cur->need_result, cur->result);
        }
    }
    procs_err("==========list info show end=============\n");
    return;
}
     
/*****************************************************************************
 �� �� ��  : process_read
 ��������  : Ӧ�ö��ڵ�����ӿ�
 �������  : file:  �ļ��������Ӧproc�ڵ㣻
             buf:   Ӧ��̬���ݴ�ŵ�ַ��
             count:Ҫд�ĳ���;
             ppos:  ��ǰд���ļ�λ��
 �������  : ��
 �� �� ֵ  : ��ȡ����
 ˵    ��  : 
*****************************************************************************/
static ssize_t process_read(struct file *file, __user char *buffer, size_t count,loff_t *ppos)
{
    int ret = 0;
    unsigned int  len = 0;
    unsigned long cpy_len = 0;
    unsigned long flag = 0;
    char buf_temp[BUFF_MAX_LEN] = {0};
    struct list_head* me  = NULL;
    process_node_str* cur = NULL;
    
      /*�������*/
    if(NULL == file || NULL == buffer || NULL == ppos)
    {
        procs_err("para is null\n");
        return -1;
    }

    if(0 == g_process_info.count)
    {
        procs_err("there is no process to read.\n");
        return 0;
    }

    osl_sem_down(&g_process_info.rw_sem);
    
    (void)memset(buf_temp, '\0', BUFF_MAX_LEN);
    
    /*�ҵ���Ҫ�����Ľ�������ڵ�*/
    list_for_each(me,&g_process_info.stlist)
    {
        cur = list_entry(me, process_node_str, stlist);
        if(NULL != cur && PROCESS_STATE_WAIT == cur->state)
        {
            break;
        }
    }

    if(&g_process_info.stlist == me)
    {
        procs_err("there is nothing in node to read\n");
        osl_sem_up(&g_process_info.rw_sem);
        return -1;
    }
    
    /*����ʽ��װ����*/
    len  = snprintf(buf_temp, BUFF_MAX_LEN, "%s%s%d%s%s", 
                        cur->name, CHAR_CR_LR, 
                        cur->need_result, CHAR_CR_LR,
                        cur->para);
    
    /*�������ݸ�Ӧ��*/
    cpy_len = count > len ? len : count;
    ret = copy_to_user(buffer, buf_temp, cpy_len);
    if(0 != ret)
    {
        procs_err("msg copy to user failed!\n");
        osl_sem_up(&g_process_info.rw_sem);
        return -1;
    }
    else
    {
        *ppos += cpy_len;
    }
    
    g_process_info.count--;

    if(1 != cur->need_result)
    {
        spin_lock_irqsave(&g_process_info.lock, flag);
        list_del(&cur->stlist);
        kfree(cur);
        cur = NULL;
        spin_unlock_irqrestore(&g_process_info.lock, flag);
    }
    else
    {
        cur->state = PROCESS_STATE_EXECULT;
    }

    osl_sem_up(&g_process_info.rw_sem);
    return cpy_len;
    
}

/*****************************************************************************
 �� �� ��  : process_write
 ��������  : Ӧ��д�ڵ�����ӿ�
 �������  : file:  �ļ��������Ӧproc�ڵ㣻
             buf:   Ӧ��̬���ݴ�ŵ�ַ��
             count:Ҫд�ĳ���;
             ppos:  ��ǰд���ļ�λ��
 �������  : ��
 �� �� ֵ  : д�볤��
 ˵    ��  : 
*****************************************************************************/
static ssize_t process_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    int ret = 0;
    int result = 0;
    unsigned long cpy_len = 0;
    char name[BUFF_MAX_LEN] = {0};
    char buf_temp[BUFF_MAX_LEN] = {0};
    struct list_head* me  = NULL;
    process_node_str* cur = NULL;
    
     /*�������*/
    if(NULL == file || NULL == buf || NULL == ppos)
    {
        procs_err("para is null\n");
        return -1;
    }
    
    osl_sem_down(&g_process_info.rw_sem);
    
    (void)memset(buf_temp, 0x0, BUFF_MAX_LEN);

    /*�ӽڵ��ȡ����*/
    cpy_len = count > BUFF_MAX_LEN ? BUFF_MAX_LEN - 1 : count;
    ret = copy_from_user(buf_temp, buf, cpy_len);
    if(0 != ret)
    {
        procs_err("msg copy from user failed!\n");
        osl_sem_up(&g_process_info.rw_sem);
        return -1;
    }
    *ppos += cpy_len;

    /*���������ͽ����������*/
    (void)sscanf(buf_temp, "%s\r\n%d", name, &result);

    /*���������ҵ���Ӧ��node*/
    list_for_each(me,&g_process_info.stlist)
    {
        cur = list_entry(me, process_node_str, stlist);
        if(NULL != cur && PROCESS_STATE_EXECULT == cur->state)
        {
            if(0 == strncmp(cur->name, name, strlen(cur->name)))
            {
                cur->result = result;
                cur->state = PROCESS_STATE_DONE;
                wake_up_interruptible(&g_process_info.listwait);
                break;
            }
        }
    }

    if(&g_process_info.stlist == me)
    {
        procs_err("there is no process matched,name:%s!\n",name);
    }
    
    osl_sem_up(&g_process_info.rw_sem);
    return cpy_len;
}

/*****************************************************************************
 �� �� ��  : ant_poll
 ��������  : �ļ�poll�ӿڣ����ﵽһ������(��gpio���ʱ)��Ӧ��֪ͨ�¼�
 �������  : file:�ļ��������Ӧproc�ڵ㣻
             wait:û���¼�����ʱ�����صĵȴ����б�
 �������  : ��
 �� �� ֵ  : �¼�����
 ˵    ��  : 
*****************************************************************************/
static unsigned int process_poll(struct file *file, poll_table *wait)
{
    int ret = 0;
        /*�������*/
    if (NULL == file || NULL == wait)
    {
        procs_err("file or wait is null.");
        return  -1;
    }
   
    poll_wait(file, &g_process_info.pollwait, wait);

    if (0 != g_process_info.count)
    {
        ret = POLLIN | POLLRDNORM;
    }
    return ret;
}
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
            ��ʱ�ýӿڻ������ȴ�ִ�н����ֱ��wait_time��ʱʱ�䵽��
*****************************************************************************/
int  drv_start_user_process(char* name, char* para, unsigned int need_result, unsigned int wait_time)
{
    int  result   = -1;
    int  timeleft =  0;
    unsigned int  wtime = 0;
    unsigned long flag = 0;
    struct list_head* me  = NULL;
    process_node_str* cur = NULL;

    if (1 != g_process_info.inited)
    {
        procs_err("this module is not inited,pls call it later.\n");
        return -1;
    }

    if(NULL == name)
    {
        procs_err("name is null.\n");
        return -1;
    }

    /*check �����Ƿ��Ѿ��������еȴ������������ڱ�ִ��*/
    list_for_each(me,&g_process_info.stlist)
    {
        cur = list_entry(me, process_node_str, stlist);
        if(NULL != cur && 0 == strncmp(cur->name, name, strlen(cur->name)))
        {
            procs_err("the process:%s is exist,pls wait and retry\n",name);
            return -1;
        }
    }

    /*���������У���������*/
    cur = (process_node_str *)kmalloc(sizeof(process_node_str), GFP_KERNEL);
    if(NULL == cur)
    {
        procs_err("malloc list node:%s failed\n",name);
        return -1;
    }
    (void)memset(cur, '\0', sizeof(process_node_str));

    /*�������������ڵ�*/
    (void)strncpy(cur->name , name, NAME_MAX_LEN -1);
    if(NULL != para)
    {
        (void)strncpy(cur->para , para, PARA_MAX_LEN - 1);
    }
    cur->need_result = need_result;
    cur->state = PROCESS_STATE_WAIT;
    cur->result = -1;

    spin_lock_irqsave(&g_process_info.lock, flag);
    /*������������poll*/
    list_add(&cur->stlist, &g_process_info.stlist);
    g_process_info.count++;
    wake_up_interruptible(&g_process_info.pollwait);
    
    spin_unlock_irqrestore(&g_process_info.lock, flag);

    if(NEED_RESULT == cur->need_result)
    {
        set_current_state(TASK_INTERRUPTIBLE);
        wtime = wait_time > WAIT_TIME_MAX ? WAIT_TIME_MAX : wait_time;
        timeleft = wait_event_interruptible_timeout(g_process_info.listwait, 
                    PROCESS_STATE_DONE == cur->state, msecs_to_jiffies(wtime));

        if(0 == timeleft)
        {
            procs_err("timeout,the app run process:%s may be failed\n",name);
        }

        result = cur->result;
        procs_err("process:%s ,result is :%d\n",cur->name, result);
        
        spin_lock_irqsave(&g_process_info.lock, flag);
        list_del(&cur->stlist);
        kfree(cur);
        cur = NULL;
        spin_unlock_irqrestore(&g_process_info.lock, flag);

        return result;
    }

    return 0;

}
EXPORT_SYMBOL_GPL(drv_start_user_process);


static struct file_operations process_start_ops = {
    .read  = process_read,
    .write = process_write,
    .poll  = process_poll,
};

/*****************************************************************************
 �� �� ��  : process_proc_init
 ��������  : ������Ӧ��proc�ڵ�
 �������  : ��
 �������  : ��
 �� �� ֵ  : 0--�ɹ���-1--ʧ��
 ˵    ��  : 
*****************************************************************************/
static int process_proc_init(void)
{
    struct proc_dir_entry *proc_entry = NULL;
   
    proc_entry = proc_create(PROC_NAME, 0600, NULL, &process_start_ops);
    if(NULL == proc_entry)
    {
        procs_err("can't create /proc/%s \n", PROC_NAME);
        return -EFAULT;
    }
    spin_lock_init(&g_process_info.lock);
    osl_sem_init(SEM_UP, &g_process_info.rw_sem);

    init_waitqueue_head(&g_process_info.pollwait);
    init_waitqueue_head(&g_process_info.listwait);
    
    INIT_LIST_HEAD(&g_process_info.stlist);
    g_process_info.count = 0;
    g_process_info.inited = 1;

    return 0;
}

rootfs_initcall(process_proc_init);
MODULE_AUTHOR("MBB.Huawei Device");
MODULE_DESCRIPTION("Mbb Common Driver");

#ifdef __cplusplus
}
#endif



