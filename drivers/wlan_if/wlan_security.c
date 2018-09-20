
/*===========================================================================
                       linuxϵͳͷ�ļ�
===========================================================================*/
#include <linux/ctype.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/if_addr.h>
#include <linux/if_arp.h>
#include <linux/if_vlan.h>
#include <linux/if_ether.h>
#include <linux/netdevice.h>
#include <linux/inetdevice.h>
#include <../net/bridge/br_private.h>

/*===========================================================================
                       ƽ̨ͷ�ļ�
===========================================================================*/
#include "wlan_utils.h"

/*===========================================================================
                        �ڲ�ʹ�ö�������
===========================================================================*/
#if (!defined(__KERNEL__) || (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,35))) /* Linux�ں˰汾���� */
    #define GET_BR_PORT(netdev) ((struct net_bridge_port *)(((struct net_device *)netdev)->br_port))
#else
    #define GET_BR_PORT(netdev) ((struct net_bridge_port *)rcu_dereference(((struct net_device *)netdev)->rx_handler_data))
#endif

/* MAC��ַ�Ƚ�:d_mac��s_macΪ6�ֽ�MAC��ַ��ָ��,���ȱȶԵ�ַǰ4�ֽڱȶ�,Ȼ��Ե�ַ��2�ֽڱȶ�,����2����2��unsigned short����ռ��4���ֽ� */
#define ARP_SMAC_CMP(d_mac, s_mac)   ( \
    ((unsigned int)(*((unsigned int *)(d_mac))) == (unsigned int)(*((unsigned int *)(s_mac))))  \
    && ((unsigned short)(*(((unsigned short *)(d_mac)) + 2)) == (unsigned short)(*((unsigned short *)(s_mac) + 2))) \
)

/*===========================================================================

                        ����ʵ�ֲ���

===========================================================================*/
/*****************************************************************************
 ��������  : wlan_get_br_by_port
 ��������  : ��ȡ�豸��Ӧ�����豸
 �������  : netdev: �豸����
 �������  : NA
 �� �� ֵ  : ָ�����豸��ָ��
*****************************************************************************/
static inline struct net_device *wlan_get_br_by_port(struct net_device *netdev)
{
    struct net_bridge_port *br_port = NULL;

    if (NULL == netdev)
    {
        return NULL;
    }

    br_port = GET_BR_PORT(netdev);
    if (NULL == br_port)
    {
        return NULL;
    }
    if (NULL == br_port->br)
    {
        return NULL;
    }

    return (br_port->br->dev);
}

/*****************************************************************************
 ��������  : wlan_get_arphdr
 ��������  : ��ȡarpͷ
 �������  : pskb: ��Ҫ�������ݰ�
 �������  : NA
 �� �� ֵ  : ָ��arpͷ��ָ��
*****************************************************************************/
static inline struct arphdr *wlan_get_arphdr(struct sk_buff *pskb)
{
    struct ethhdr *eth = NULL;
    struct arphdr *arp = NULL;
    struct vlan_ethhdr *veth = NULL;

    if (NULL == pskb)
    {
        return NULL;
    }

    eth = (struct ethhdr *)(pskb->data);
    if (NULL == eth)
    {
        return NULL;
    }

    if (__constant_htons(ETH_P_ARP) == eth->h_proto)
    {
        arp = (struct arphdr*)(pskb->data + ETH_HLEN);
    }
    else if (__constant_htons(ETH_P_8021Q) == eth->h_proto)
    {
        veth = (struct vlan_ethhdr *)eth;
        if (__constant_htons(ETH_P_ARP) == veth->h_vlan_encapsulated_proto)
        {
            arp = (struct arphdr *)(veth + 1);
        }
    }
    else
    {
        arp = NULL;
    }

    return (arp);
}
/*****************************************************************************
 ��������  : wlan_check_arp_spoofing
 ��������  : ���wlan��arp��ƭ  ����֧��IPV4Э��
 �������  : netdev: Դnetif�ӿ�, pskb: ��Ҫ�������ݰ�
 �������  : NA
 �� �� ֵ  : 0:����ARP��ƭ���ģ�1: ��ARP��ƭ����
*****************************************************************************/
int wlan_check_arp_spoofing(struct net_device *port_dev, struct sk_buff *pskb)
{
    struct arphdr *arp = NULL;
    struct net_device *br_dev = NULL;
    int is_arp_spoofing_pkt = 0;
    unsigned int arp_src_ip = 0;
    unsigned int select_addr = 0;

    if (NULL == port_dev || NULL == pskb)
    {
        return (0);
    }

    /* ����Ƿ�ARP���� */
    arp = wlan_get_arphdr(pskb);
    if (NULL == arp)
    {
        return (0);
    }

    arp_src_ip = *((unsigned int *)((char *)(arp + 1) + arp->ar_hln)); /* ԴIP */
    if (0 == arp_src_ip)
    {
        return (0);
    }

    /* ��ȡ���豸 */
    br_dev = wlan_get_br_by_port(port_dev);
    if (NULL == br_dev)
    {
        return (0);
    }
    if (NULL == br_dev->dev_addr)
    {
        return (0);
    }

    /* ����Ƿ�����IP ARP RSP ���� */
    select_addr = (unsigned int)inet_select_addr(br_dev, arp_src_ip, RT_SCOPE_LINK);
    if ((arp_src_ip == select_addr) && (!ARP_SMAC_CMP(arp + 1, br_dev->dev_addr)))
    {    /* ARP RSP: IP��ַΪ����IP��ԴMAC��ַΪ����MAC */
         /* �˴�����������ȡIP��ַ��MAC��ַ,��ӡlog,�޷��� */
        WLAN_TRACE_INFO("(%s) arp SrcIP [%d.%d.%d.%d], SrcMAC [XX:XX:XX:%02X:%02X:%02X]",
            port_dev->name,
            ((char *)&arp_src_ip)[0], ((char *)&arp_src_ip)[1],
            ((char *)&arp_src_ip)[2], ((char *)&arp_src_ip)[3],
            ((char *)(arp + 1))[3], ((char *)(arp + 1))[4], ((char *)(arp + 1))[5]);
        is_arp_spoofing_pkt = 1;
    }

    return (is_arp_spoofing_pkt);
}
EXPORT_SYMBOL(wlan_check_arp_spoofing);

