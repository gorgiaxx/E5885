


#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/gpio.h>
#include <asm-generic/uaccess.h>
#include <mdrv_ipl_kick_wdtime.h>
#include "mbb_process_start.h"
#include <product_config.h>
#include <bsp_sram.h>

/*****************************************************************************
�� �� �� : kick_wdt_acore_wuc
�������� : minidumpʱ����ι��
������� : ��
�� �� ֵ : ret
���ú��� :
�޸����� : �����ɺ���
*****************************************************************************/
void kick_wdt_acore_wuc()
{
    int val = 0;

    /* ��ȡ��ǰwdt gpio�ĵ�ƽ; ��������,�������� */
    val = gpio_get_value(IPL_KICK_WDT_GPIO);
    if (IPL_WDT_GPIO_VAL_HIGH == val)
    {
        gpio_set_value(IPL_KICK_WDT_GPIO, IPL_WDT_GPIO_VAL_LOW);
    }
    else if (IPL_WDT_GPIO_VAL_LOW == val)
    {
        gpio_set_value(IPL_KICK_WDT_GPIO, IPL_WDT_GPIO_VAL_HIGH);
    }

    return;
}

/*****************************************************************************
�� �� �� : stop_wdt_atDload
�������� : һ������ʱֹͣι��
������� : ��
�� �� ֵ : ret
���ú��� : sys_newstat
�޸����� : �����ɺ���
*****************************************************************************/
int stop_wdt_atDload(void)
{
    int ret = -1;
    char *exe_name = "flashwuc";
    char *exe_para = "--watchdog 0";

    /* ִ��flashwuc�����һ������ʱֹͣι�� */
    ret = drv_start_user_process(exe_name, exe_para, NEED_RESULT, WAIT_TIME_5S);
    printk("drv_start_user_process ret = %d  \n"  ,ret);

    return ret ;
}

/*****************************************************************************
 �� �� ��  : ipl_fota_enter_freeze_processes_set
 ��������  : A�˽���˯�߱�־������˯��ʱִ��
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  : ��
*****************************************************************************/
void ipl_fota_enter_freeze_processes_set(void)
{
    smem_huawei_ipl_fota_type *smem_data = NULL;

    /* ��ȡsmem�����ڴ� */
    smem_data = (smem_huawei_ipl_fota_type *)SRAM_MBB_IPL_FOTA_FLAG_ADDR;
    if (NULL == smem_data)
    {
        printk(KERN_ERR"Share mem err %s() line = %d\n", __FUNCTION__, __LINE__);
        return;
    }

    /* ���¹����ڴ��־λ������˯�� */
    smem_data->smem_kernel_suspend_process = ENTER_FREEZE_PROCESSES;
    printk(KERN_INFO"%s()   enter sleep... \n", __FUNCTION__);

    /*
     * ����˯��ʱ�ֶ�ͶƱһ�Σ���֤watchdog gpio�ڻ��Ѻ�
     * ��һ�ν��붨ʱ���ж�ʱ�ܹ�������ת
     * ���ģ��˯�߻���ǰ�������watchdog gpio��ת�����
     * ��Ϊ����ģ��Ԥ��ʱ������⣻
     * ����ͻ����õ�WUC���Ź���ʱʱ�䲻����2����ģ��
     * watchdog gpio��ת���ڣ�������˵���ģ�鱻��λ����
     */
    smem_data->smem_vote_modem_watchdog = IPL_APPCORE_PROCESS_ALIVE;

    return;
}
EXPORT_SYMBOL(ipl_fota_enter_freeze_processes_set);

/*****************************************************************************
 �� �� ��  : ipl_fota_exit_freeze_processes_set
 ��������  : A���˳�˯�߱�־���˳�˯��ʱִ��
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  : ��
*****************************************************************************/
void ipl_fota_exit_freeze_processes_set(void)
{
    smem_huawei_ipl_fota_type *smem_data = NULL;

    /* ��ȡ�����ڴ� */
    smem_data = (smem_huawei_ipl_fota_type *)SRAM_MBB_IPL_FOTA_FLAG_ADDR;
    if (NULL == smem_data)
    {
        printk(KERN_ERR"Share mem err %s() line = %d\n", __FUNCTION__, __LINE__);
        return;
    }

    /* ���¹����ڴ��־λ���˳�˯�� */
    smem_data->smem_kernel_suspend_process = EXIT_FREEZE_PROCESSES;
    printk(KERN_INFO"%s()   exit sleep... \n", __FUNCTION__);

    return;
}
EXPORT_SYMBOL(ipl_fota_exit_freeze_processes_set);

/*****************************************************************************
 �� �� ��  : smem_vote_modem_watchdog_write
 ��������  : дipl wdt �����ڴ棬��A5ͶƱʹ��
 �������  : file:  �ļ��������Ӧproc�ڵ㣻
             buffer:   Ӧ��̬���ݴ�ŵ�ַ��
             count:Ҫд�ĳ���;
             ppos:  ��ǰд���ļ�λ��
 �������  : �����ڴ��ħ����
 �� �� ֵ  : д����ֵ�ĳ���
 ���ú���  :
*****************************************************************************/
#ifdef CONFIG_PROC_FS
ssize_t smem_vote_modem_watchdog_write
(
    struct file *file
    , const char __user *buffer
    , size_t count
    , loff_t *ppos
)
{
    smem_huawei_ipl_fota_type *smem_data = NULL;
    unsigned char ipl_wdtimer_num[IPL_WDTIMER_FLAG_MAX_LEN + 1] = {0};
    unsigned int val = 0;

    /* ��ȡ�����ڴ� */
    smem_data = (smem_huawei_ipl_fota_type *)SRAM_MBB_IPL_FOTA_FLAG_ADDR;
    if (NULL == smem_data)
    {
        printk(KERN_ERR"Share mem err %s() line = %d\n", __FUNCTION__, __LINE__);
        return -1;
    }

    /* �ж�����Ƿ�Ϸ� */
    if ((count > (IPL_WDTIMER_FLAG_MAX_LEN + 1)) || (NULL == buffer))
    {
        printk(KERN_ERR"Invalid param buf(%p) size(%u)\n", buffer, count);
        return -1;
    }

    if (copy_from_user((void *)ipl_wdtimer_num, buffer, count))
    {
        return -1;
    }

    /* �жϴ��������1��0 */
    if (('0' == ipl_wdtimer_num[0]) || ( '1' == ipl_wdtimer_num[0]))
    {
        val = ipl_wdtimer_num[0] - '0';
    }
    else
    {
        printk("input %d is not available\n", ipl_wdtimer_num[0]);
        return -1;
    }

    /* ��ͶƱ����д�빲���ڴ� */
    if (1 == val)
    {
        smem_data->smem_vote_modem_watchdog = IPL_APPCORE_PROCESS_ALIVE;
    }
    else
    {
        smem_data->smem_vote_modem_watchdog = IPL_APPCORE_PROCESS_NOT_ALIVE;
    }

    return count;
}

/*****************************************************************************
 ���ݽṹ��  : ipl_kick_wdtimer_fops
 ��������    : ���干���ڴ�Ķ�д�ӿ�
*****************************************************************************/
static struct file_operations ipl_kick_wdtimer_fops = {
    .write      = smem_vote_modem_watchdog_write,
};
#endif

/*****************************************************************************
 �� �� ��  : create_smem_vote_modem_watchdog_file
 ��������  : ����proc�ļ����ṩ�����ڴ��д�ӿ�
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  : ��
*****************************************************************************/
static int create_smem_vote_modem_watchdog_file(void)
{
#ifdef CONFIG_PROC_FS
    struct proc_dir_entry *ent = NULL;

    /* ����proc�ڵ� */
    ent = proc_create(SMEM_VOTE_MODEM_WATCHDOG, S_IWUGO, NULL, &ipl_kick_wdtimer_fops);
    if (NULL == ent)
    {
        printk(KERN_ALERT"%s: create proc entry for wdt file failed\n", __FUNCTION__);
        return -1;
    }
#endif

    return 0;
}

/*****************************************************************************
 �� �� ��  : remove_smem_vote_modem_watchdog_file
 ��������  : ɾ��proc�ļ���ж�ع����ڴ��д�ӿ�
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  : ��
*****************************************************************************/
static void remove_smem_vote_modem_watchdog_file(void)
{
#ifdef CONFIG_PROC_FS
    remove_proc_entry(SMEM_VOTE_MODEM_WATCHDOG, NULL);
#endif

    return;
}
/*****************************************************************************
 �� �� ��  : ipl_kick_wdtimer_smem_probe
 ��������  : ������ʼ��
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  : ��
*****************************************************************************/
static int ipl_kick_wdtimer_smem_probe(void)
{
    return create_smem_vote_modem_watchdog_file();
}

/*****************************************************************************
 �� �� ��  : ipl_kick_wdtimer_smem_remove
 ��������  : ����ж��
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  : ��
*****************************************************************************/
static void ipl_kick_wdtimer_smem_remove(void)
{
    remove_smem_vote_modem_watchdog_file();
    return;
}

module_init(ipl_kick_wdtimer_smem_probe);
module_exit(ipl_kick_wdtimer_smem_remove);

MODULE_DESCRIPTION("HUAWEI ipl_kick_wdtimer_smem Driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("HUAIWEI: Ipl_kick_wdtimer_smem Driver");
