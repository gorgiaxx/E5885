
#ifndef __GPS_ADAPTER_H__
#define __GPS_ADAPTER_H__
/******************************************************************************
ͷ�ļ�����
******************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif
/******************************************************************************
��������
******************************************************************************/
#define GPS_PRINT(level, fmt, ...) (printk(level"[*GPS*] %s:%d: "fmt"\n", \
        __func__, __LINE__, ##__VA_ARGS__))
    
#define GPS_VOLTAGE_UV          (1800 * 1000)       //GPS��Դ1.8V
#define GPS_DISABLE_PIN         GPIO_0_4
#define LABEL_GPS_DISABLE       "gps_disable"
#define PM_GPS_MSG_TIME         (1000)
#define PM_GPS_WORK_DELAY_TIME  (50)  /*gps disable pin�ŷ���ʱ��*/

/* GPS IOCTRL�¼� */
#define GPS_CMD_BASE 0X88
    /*GPS��Դ����*/
#define GPS_POWER_CTRL_CMD                _IOWR(GPS_CMD_BASE, 0, unsigned long)
    /*GPS 32kʱ�ӿ���*/
#define GPS_CLOCK_CTRL_CMD                _IOWR(GPS_CMD_BASE, 1, unsigned long)
    /*GPS��ѯDisable�ܽ�״̬*/
#define GPS_DISABLE_QUY_CMD               _IOWR(GPS_CMD_BASE, 2, unsigned long)
    /*GPS��HWCUST���ƿ���*/
#define GPS_HWCUST_PIN_SET_CMD            _IOWR(GPS_CMD_BASE, 3, unsigned long)
    /*GPS��HWCUST״̬��ѯ*/
#define GPS_HWCUST_PIN_QUY_CMD            _IOWR(GPS_CMD_BASE, 4, unsigned long)
    /*GPS��Դ����ͶƱ����*/ 
#define GPS_LOCK_CTRL_CMD                 _IOWR(GPS_CMD_BASE, 5, unsigned long)

typedef enum
{
    GPS_POWER_OFF = 0,
    GPS_POWER_ON
}GPS_POWER_STATE;

typedef enum
{
    GPS_32K_CLK_OFF = 0,
    GPS_32K_CLK_ON
}GPS_32K_CLK_STATE;

typedef enum
{
    GPS_PM_UNLOCK = 0,
    GPS_PM_LOCK
}GPS_LOCK_STATE;
/******************************************************************************
��������
******************************************************************************/

void gps_adapter_exit_handle(void);
static void gps_disablepin_event_isr_handle(struct work_struct* data);
#ifdef __cplusplus
}
#endif
#endif


