
#ifndef _WLAN_SECURITY_H_
#define _WLAN_SECURITY_H_

/*===========================================================================
                       linuxϵͳͷ�ļ�
===========================================================================*/
#include <linux/skbuff.h>
#include <linux/netdevice.h>

/*****************************************************************************
 ��������  : wlan_check_arp_spoofing
 ��������  : ���wlan��arp��ƭ
 �������  : port_dev: ���¹��豸, pskb: ��Ҫ�������ݰ�
 �������  : NA
 �� �� ֵ  : 0:����ARP��ƭ���ģ�1: ��ARP��ƭ����
*****************************************************************************/
int wlan_check_arp_spoofing(struct net_device *port_dev, struct sk_buff *pskb);

#endif /* _WLAN_SECURITY_H_ */
