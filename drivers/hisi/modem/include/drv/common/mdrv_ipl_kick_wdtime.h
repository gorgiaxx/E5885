
#ifndef __MDRV_CCORE_WDTIME_H__
#define __MDRV_CCORE_WDTIME_H__
#ifdef __cplusplus
extern "C"
{
#endif

#include "mdrv_public.h"

/* APP ͶƱ�ڵ� */
#define SMEM_VOTE_MODEM_WATCHDOG    "SMEM_VOTE_MODEM_WATCHDOG"
#define IPL_WDTIMER_FLAG_MAX_LEN    1

/* ����GPIO��ƽ�ߵ� */
#define IPL_WDT_GPIO_VAL_LOW        0
#define IPL_WDT_GPIO_VAL_HIGH       1

/* A��˯�߱�־λ */
#define ENTER_FREEZE_PROCESSES      0x45465052
#define EXIT_FREEZE_PROCESSES       0x52504645

/* APPͶƱ��־λ */
#define IPL_APPCORE_PROCESS_NOT_ALIVE    0x98761345
#define IPL_APPCORE_PROCESS_ALIVE        0x45137698

#define  WDTIME_OPRT_ERROR    (-1)

/*��ѯ������WDTIME��*/
typedef enum WDTIME_OPRT_ENUM
{
    WDTIME_OPRT_SET = 0,
    WDTIME_OPRT_GET,
    WDTIME_OPRT_BUTT
}WDTIME_OPRT_E;

/*****************************************************************************
�� �� �� : stop_wdt_atDload
�������� : һ������ʱֹͣι��
������� : ��
�� �� ֵ : ret
���ú��� : sys_newstat
�޸����� : �����ɺ���
*****************************************************************************/
int stop_wdt_atDload(void);

/*****************************************************************************
�� �� �� : kick_wdt_acore_wuc
�������� : minidumpʱ����ι��
������� : ��
�� �� ֵ : ret
���ú��� :
�޸����� : �����ɺ���
*****************************************************************************/
void kick_wdt_acore_wuc(void);

/*****************************************************************************
 �� �� ��  : drv_wdtime_oprt
 ��������  : ���úͲ�ѯWDTIME�ĵ�ƽ��
 �������  : ulOp : 0:����/1:��ѯ; wdtimeʱ��
 �������  : ��
 �� �� ֵ  : 0  �����ɹ�
            -1  ����ʧ��
 ���ú���  : ��
 ��������  : ��
*****************************************************************************/

int drv_wdtime_oprt(WDTIME_OPRT_E ulOp, unsigned int *wdtime);

#ifdef __cplusplus
}
#endif

#endif

