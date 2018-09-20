

/**********************include  file****************************/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/if_ether.h>
#include <linux/rwlock_types.h>
#include <linux/rwlock.h>
#include <linux/netlink.h>
#include <hw_net.h>
/**********************MACRO****************************/

#define MAC_CLONE_OFF "OFF"


/*ʮ����*/
#define DECIMAL   10

#define MAC_CLONE_ENABLE 1
#define MAC_CLONE_DISABLE 0

#define SZ_MAC_LENGTH 20   /*ð�ŷָ���ʽ��MAC��ַ����*/

#define MBB_USBNET_NUMBER_2 2
#define MBB_USBNET_NUMBER_3 3
#define MBB_USBNET_NUMBER_4 4
#define MBB_USBNET_NUMBER_10 10
#define MBB_USBNET_NUMBER_17 17

/**********************variable****************************/
static struct class* lan_class;
static struct device* lan_dev;
static int mac_clone_flag = MAC_CLONE_DISABLE;    /*MAC��ַ��¡���ر�ʶ��1Ϊ�򿪣�0Ϊ�ر�*/
static char  clone_init_addr[ETH_ALEN] = {0};/*eth0�豸��MAC��ַ*/
static char  clone_mac_addr[ETH_ALEN] = {0};    /*Ҫ��¡��MAC��ַ*/
/*�û�������û��ռ䴫���ð�ŷָ���ʽ�����ڿ�¡MAC��ַ*/
static char sz_clone_macaddr[SZ_MAC_LENGTH] = {"00:00:00:00:00:00"}; 
static rwlock_t mac_clone_lock;  /*��д��������ȫ�ֱ�������������*/
static int g_usbnet_net_state = CRADLE_REMOVE; /*��¼��ǰ����״̬*/
static int g_usbnet_net_last_state = CRADLE_REMOVE;/*��¼�ϴ�����״̬*/
static int g_usbnet_usb_state = GPIO_USB_CRADLE_REMOVE; /*��¼��ǰusbnet ״̬*/
static int g_usbnet_speed= CRADLE_SPEED_INVAILD;
static int g_usbnet_cradle_type = CRADLE_TYPE_INVALID;/*��¼��ǰcradle type*/
static int g_usbnet_fty_cradle_state = GPIO_USB_CRADLE_UNPLUG;/*MMI���Լ�¼cradle״̬*/
/**********************function****************************/

static ssize_t get_clone_mac(struct device* dev, struct device_attribute* attr, char* buf, size_t size);
static ssize_t set_clone_mac(struct device* dev, struct device_attribute* attr, char* buf, size_t size);
static ssize_t get_carrier_state(struct device* dev, struct device_attribute* attr, char* buf, size_t size);
static ssize_t get_usb_cradle_state(struct device* dev, struct device_attribute* attr, char* buf, size_t size);
static int translate_mac_address(char* adr_str, char* adr_dst);
static ssize_t get_usb_cradle_type(struct device* dev,struct device_attribute* attr, char* buf, size_t size);
static ssize_t get_usb_fty_cradle_state(struct device* dev,struct device_attribute* attr, char* buf, size_t size);
static DEVICE_ATTR(clone_mac, S_IRUGO | S_IWUSR, get_clone_mac, set_clone_mac);
static DEVICE_ATTR(net_state, S_IRUGO,  get_carrier_state, NULL);
static DEVICE_ATTR(cradle_state, S_IRUGO , get_usb_cradle_state, NULL);
static DEVICE_ATTR(cradle_type, S_IRUGO , get_usb_cradle_type, NULL);
static DEVICE_ATTR(fty_cradle_state, S_IRUGO , get_usb_fty_cradle_state, NULL);
static ssize_t get_usb_cradle_speed(struct device* dev, struct device_attribute* attr, char* buf, size_t size);
static DEVICE_ATTR(speed, S_IRUGO, get_usb_cradle_speed, NULL);

static ssize_t get_clone_mac(struct device* dev,
                            struct device_attribute* attr, char* buf, size_t size)
{
    return snprintf(buf, sizeof(sz_clone_macaddr) + MBB_USBNET_NUMBER_2, "%s\n",
                    sz_clone_macaddr);
}

/*����������ڽ�ð�ż�����ַ�����ʽMAC��ַת��Ϊ6�ֽڸ�ʽ*/
static int translate_mac_address(char* adr_str, char* adr_dst)
{
    int ret = 0;
    int i = 0, j = 0;
    int data;
    unsigned char c = 0;

    printk("%s Enter\n", __FUNCTION__);

    if (!adr_dst)
    { return - EINVAL; }

    if (!adr_str)
    { return - EINVAL; }

    data = 0;
    i = 0;

    while (i < MBB_USBNET_NUMBER_17)
    {
        c = adr_str[i];
        data = data << MBB_USBNET_NUMBER_4;
        j = i % MBB_USBNET_NUMBER_3;
        if (MBB_USBNET_NUMBER_2 == j)
        {
            if (':' == c)
            {
                i++;
                continue;
            }
            else
            {
                ret = -1;
                break;
            }
        }
        if ('0' <= c && '9' >= c)
        { data += c - '0'; }
        else if ('A' <= c && 'F' >= c)
        { data += c - 'A' + MBB_USBNET_NUMBER_10; }
        else if ('a' <= c && 'f' >= c)
        { data += c - 'a' + MBB_USBNET_NUMBER_10; }
        else
        {
            ret = -1;
            break;
        }
        if (1 == j)
        {
            adr_str[i / MBB_USBNET_NUMBER_3] = data;
            data = 0;
        }
        i++;
    }

    if (-1 != ret)
    {
        memcpy(adr_dst, adr_str, ETH_ALEN);
    }
    else
    {
        printk("%s: error mac addr\n", __FUNCTION__);
    }

    return 0;
}
//�ļ�clone_mac״̬�����仯ʱ�����ôκ����ж��Ƿ���MAC��ַ��¡����
static ssize_t set_clone_mac(struct device* dev,
                            struct device_attribute* attr, char* buf, size_t size)
{
    char sz_macaddr[SZ_MAC_LENGTH] = {0};

    strncpy(sz_clone_macaddr, buf, SZ_MAC_LENGTH - 1);
    strncpy(sz_macaddr, sz_clone_macaddr, SZ_MAC_LENGTH - 1);
    write_lock(&mac_clone_lock);
    mac_clone_flag = MAC_CLONE_DISABLE; /*������Ϊ0*/

    if (strncmp(buf, MAC_CLONE_OFF, strlen(MAC_CLONE_OFF)))
    {
        /*��ð�ż�����ַ�����ʽMAC��ַת��Ϊ6�ֽڸ�ʽ*/
        if (0 == translate_mac_address(sz_macaddr, clone_mac_addr))
        {
            mac_clone_flag = MAC_CLONE_ENABLE;
        }
    }

    write_unlock(&mac_clone_lock);

    return size;
}

static ssize_t get_carrier_state(struct device* dev,
                                struct device_attribute* attr, char* buf, size_t size)
{

    return snprintf(buf, sizeof(int), "%0x\n", g_usbnet_net_state);
}

static ssize_t get_usb_cradle_state(struct device* dev,
                                    struct device_attribute* attr, char* buf, size_t size)
{
    int usb_state = 0;

    if ((GPIO_USB_CRADLE_ATTACH == g_usbnet_usb_state) || (GPIO_USB_AF18_ATTACH == g_usbnet_usb_state))
    {
        usb_state = 1;
    }
    return snprintf(buf, sizeof(int), "%0x\n", usb_state);
}

static ssize_t get_usb_cradle_type(struct device* dev,
                                    struct device_attribute* attr, char* buf, size_t size)
{
    int cradle_type = 0;

    cradle_type = g_usbnet_cradle_type;
    if (buf)
    {
        return snprintf(buf, sizeof(int), "%0x\n", cradle_type);
    }
    else
    {
        return 0;
    }
}

static ssize_t get_usb_fty_cradle_state(struct device* dev,
                                    struct device_attribute* attr, char* buf, size_t size)
{
    int fty_cradle_state = 0;

    fty_cradle_state = g_usbnet_fty_cradle_state;

    if (buf)
    {
        return snprintf(buf, sizeof(int), "%0x\n", fty_cradle_state);
    }
    else
    {
        return 0;
    }
}

static ssize_t get_usb_cradle_speed(struct device* dev, struct device_attribute* attr, char* buf, size_t size)
{
    int speed = g_usbnet_speed;

    return snprintf(buf, sizeof(int), "%d\n", speed);
}


/*****************************************************************
Function  name:mbb_usbnet_usb_state_notify
Description   : usb cradle insert event notify to app
Parameters    :  GPIO_USB_EVENT eventcode
Return        :    
*****************************************************************/
void mbb_usbnet_usb_state_notify(GPIO_USB_EVENT eventcode)
{
    DEVICE_EVENT stusbEvent = {0};
    stusbEvent.device_id = DEVICE_ID_USB;
    stusbEvent.event_code = eventcode;
    stusbEvent.len = 0;

    (void)device_event_report(&stusbEvent, sizeof(DEVICE_EVENT));
    
    mbb_usbnet_set_usb_state(eventcode);

}
/*****************************************************************
Function  name:mbb_usbnet_set_net_state
Description   : set current crable state
Parameters    :  CRADLE_EVENT state
Return        :    
*****************************************************************/
void mbb_usbnet_set_net_state(CRADLE_EVENT state)
{
    g_usbnet_net_state = state;
}
/*****************************************************************
Function  name:mbb_usbnet_set_last_net_state
Description   : set lastest crable state
Parameters    :  CRADLE_EVENT state
Return        :    
*****************************************************************/
void mbb_usbnet_set_last_net_state(CRADLE_EVENT state)
{
    g_usbnet_net_last_state = state;
}
/*****************************************************************
Function  name:mbb_usbnet_set_usb_state
Description   : set usb cradle insert state
Parameters    :  GPIO_USB_EVENT state
Return        :    
*****************************************************************/
void mbb_usbnet_set_usb_state(GPIO_USB_EVENT state)
{
    g_usbnet_usb_state = state;
}

/*****************************************************************
Function  name:mbb_usbnet_set_cradle_type
Description   : set usb cradle insert type
Parameters    :  CRADLE_TYPE type
Return        :    
*****************************************************************/
void mbb_usbnet_set_cradle_type(CRADLE_TYPE type)
{
    g_usbnet_cradle_type = type;
}

/*****************************************************************
Function  name:mbb_usbnet_set_fty_cradle_state
Description   : set usb fty_cradle insert state
Parameters    :  GPIO_USB_EVENT state
Return        :    
*****************************************************************/
void mbb_usbnet_set_fty_cradle_state(GPIO_USB_EVENT state)
{
    g_usbnet_fty_cradle_state = state;
}

/*****************************************************************
Function  name:mbb_usbnet_net_state_notify
Description   : crable insert event notify to app
Parameters    :  CRADLE_EVENT status
Return        :    
*****************************************************************/
void mbb_usbnet_net_state_notify(CRADLE_EVENT status)
{
    DEVICE_EVENT stusbEvent;
    g_usbnet_net_state = status;
    if (g_usbnet_net_last_state != g_usbnet_net_state)
    {
        stusbEvent.device_id = DEVICE_ID_CRADLE;
        stusbEvent.event_code = g_usbnet_net_state;
        stusbEvent.len = 0;

        switch (g_usbnet_net_state)
        {
            case CRADLE_INSERT:
            {
                printk("%s: send NET LINE INSERTED\n", __FUNCTION__);
                break;
            }
            case CRADLE_REMOVE:
            {
                printk("%s: send NET LINE PLUG OUT\n", __FUNCTION__);
                break;
            }
        }
        (void)device_event_report(&stusbEvent, sizeof(DEVICE_EVENT));
        g_usbnet_net_last_state = g_usbnet_net_state;
    }
}
/*****************************************************************
Function  name:mbb_usbnet_tx_set_mac_clone
Description   : tx skb set mac addr
Parameters    :  struct sk_buff* skb
Return        :    
*****************************************************************/
void mbb_usbnet_tx_set_mac_clone(struct sk_buff* skb)
{
    char* pskb_tmp = skb->data;

    read_lock(&mac_clone_lock);

    if (MAC_CLONE_ENABLE == mac_clone_flag)
    {
        if (skb->protocol == cpu_to_be16(ETH_P_PPP_DISC)
            || skb->protocol == cpu_to_be16(ETH_P_PPP_SES))
        {
            memcpy(clone_init_addr, pskb_tmp + ETH_ALEN, ETH_ALEN);
            memcpy(pskb_tmp + ETH_ALEN, clone_mac_addr, ETH_ALEN);
        }
    }

    read_unlock(&mac_clone_lock);
}
/*****************************************************************
Function  name:mbb_usbnet_rx_set_mac_clone
Description   : rx skb set mac addr
Parameters    :  struct sk_buff* skb
Return        :    
*****************************************************************/
void mbb_usbnet_rx_set_mac_clone(struct sk_buff* skb)
{
    char* pskb_tmp = NULL;
    if (MAC_CLONE_ENABLE == mac_clone_flag)
    {
        pskb_tmp = skb->data;

        if ((*(unsigned short*)(skb->data + ETH_ALEN + ETH_ALEN) == cpu_to_be16(ETH_P_PPP_DISC)
             || *(unsigned short*)(skb->data + ETH_ALEN + ETH_ALEN) == cpu_to_be16(ETH_P_PPP_SES))
            && (0 == strncmp(pskb_tmp, clone_mac_addr, ETH_ALEN)))
        {
            memcpy(pskb_tmp, clone_init_addr, ETH_ALEN);
        }
    }
}

/*****************************************************************
Function  name:mbb_usbnet_set_speed
Description   : set current crable speed
Parameters    :  speed state
Return        :    
*****************************************************************/
void mbb_usbnet_set_speed(CRADLE_SPEED speed)
{
    g_usbnet_speed = speed;
}

static int __init mbb_usbnet_init(void)
{
    int rc = 0;
    static int usbnet_init = 0;

    /*��ֹ��γ�ʼ��*/
    if (usbnet_init > 0)
    {
        return 0;
    }

    lan_class = class_create(THIS_MODULE, "lan_usb");
    lan_dev = device_create(lan_class, NULL, MKDEV(0, 0), NULL, "lan");

    rc = device_create_file(lan_dev, &dev_attr_net_state);
    if (rc)
    {    
        printk(KERN_ERR "failed to create file net_state\n");
    }

    rc = device_create_file(lan_dev, &dev_attr_cradle_state);
    if (rc)
    {    
        printk(KERN_ERR "failed to create file cradle_state\n");
    }

    rc = device_create_file(lan_dev, &dev_attr_cradle_type);
    if (rc)
    {    
        printk(KERN_ERR "failed to create file cradle_type\n");
    }

    rc = device_create_file(lan_dev, &dev_attr_fty_cradle_state);
    if (rc)
    {    
        printk(KERN_ERR "failed to create file fty_cradle_state\n");
    }

    rc = device_create_file(lan_dev, &dev_attr_clone_mac);
    if (rc)
    {    
        printk(KERN_ERR "failed to create file clone_mac\n");
    }

    rc = device_create_file(lan_dev, &dev_attr_speed);
    if (rc)
    {    
        printk(KERN_ERR "failed to create file speed\n");
    }

    rwlock_init(&mac_clone_lock);
    usbnet_init++;

    return 0;
}
static void __exit mbb_usbnet_cleanup(void)
{
    device_remove_file(lan_dev, &dev_attr_net_state);

    device_remove_file(lan_dev, &dev_attr_cradle_state);

    device_remove_file(lan_dev, &dev_attr_clone_mac);

    device_remove_file(lan_dev, &dev_attr_speed);

    device_destroy(lan_class, MKDEV(0, 0));

    class_destroy(lan_class);
    return;
}

module_init(mbb_usbnet_init);
module_exit(mbb_usbnet_cleanup);
