/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2012-2015. All rights reserved.
 * foss@huawei.com
 *
 * If distributed as part of the Linux kernel, the following license terms
 * apply:
 *
 * * This program is free software; you can redistribute it and/or modify
 * * it under the terms of the GNU General Public License version 2 and 
 * * only version 2 as published by the Free Software Foundation.
 * *
 * * This program is distributed in the hope that it will be useful,
 * * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * * GNU General Public License for more details.
 * *
 * * You should have received a copy of the GNU General Public License
 * * along with this program; if not, write to the Free Software
 * * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
 *
 * Otherwise, the following license terms apply:
 *
 * * Redistribution and use in source and binary forms, with or without
 * * modification, are permitted provided that the following conditions
 * * are met:
 * * 1) Redistributions of source code must retain the above copyright
 * *    notice, this list of conditions and the following disclaimer.
 * * 2) Redistributions in binary form must reproduce the above copyright
 * *    notice, this list of conditions and the following disclaimer in the
 * *    documentation and/or other materials provided with the distribution.
 * * 3) Neither the name of Huawei nor the names of its contributors may 
 * *    be used to endorse or promote products derived from this software 
 * *    without specific prior written permission.
 * 
 * * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifdef DRV_BUILD_SEPARATE


#include <linux/kernel.h>
#include <linux/semaphore.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/skbuff.h>
#include <linux/if_ether.h>
#include <linux/delay.h>
#include <linux/netdevice.h>
#include <linux/usb/balong_usb_nv.h>

#include "ncm_balong.h"
#include "ncm_balong_test.h"
#include "usb_vendor.h"


#define NCM_TEST_FUNC_ENTER()\
    printk("%s line %d:test case enter.\n",__FUNCTION__,__LINE__)

#define NCM_TEST_FUNC_LEAVE()\
    printk("%s line %d:test case leave.\n",__FUNCTION__,__LINE__)

#define NCM_TEST_FUNC_PASS()\
    printk("%s line %d:test case pass.\n",__FUNCTION__,__LINE__)

#define NCM_TEST_FUNC_FAIL(value)\
    printk("%s line %d:test case fail. value = %d\n",__FUNCTION__,__LINE__,(int)value)

typedef struct
{
    u8  mac_addr[UW_MAC_ADDR_LEN];
    u8  ip_addr[IP_ADDR_LEN];
}net_context_t;

typedef struct tagNCM_DEFAULT_PARAM_S
{
    u32 u32PsUsedMaxBufNum;
    u32 u32DefaultTxMinNum;
    u32 u32DefaultTxTimeOut;
    u32 u32DeafultTxMaxSize;
    u32 u32RxBufSize;
}NCM_DEFAULT_PARAM_S;

typedef struct tagPS_SEND_TASK_PARA
{
    u32 u32DevId;
    s32 nodeNum;
}PS_SEND_TASK_PARA;

PS_SEND_TASK_PARA g_tgNcmPsSendPara;
int g_u32pssendfail = 0;
u32 g_u32NcmDevId = 0;
/* 第一次收到合适buffer标志*/
u32 g_u32FirstRcvFlag[GNET_USED_NUM] = {TRUE, TRUE};
/* NCM ST是否初始化*/
u32 g_u32NCMstInit[GNET_USED_NUM] = {FALSE, FALSE};
/* 是否已经启动NCM发包测试标志*/
u32 g_u32NCMDataStart[GNET_USED_NUM] = {FALSE};
/* NCM流控测试是否初始化标志*/
u32 g_u32NcmFlowCtrlstInit = FALSE;

/* PS下行发包任务ID*/
u32 g_NcmPsSendTaskId = 0;
/* PS是否收到包信号量*/
struct semaphore g_NcmPsRecvSemId[GNET_USED_NUM];
/* PS发送任务taskDelay时间间隔，可以调节发送速率*/
u32 g_u32PsSendDelay = 10;
/* PS发包包头节点*/
struct sk_buff *g_pPsSendHead = NULL;

/* 是否持续发包*/
bool  g_bIsSend = TRUE;
/* 记录最后收到的包地址，用于停止任务时释放资源*/
u32   g_u32PktEncap = 0;

/* 发包使用buffer*/
net_buffer_t *g_tx_loop_buff[GNET_USED_NUM] = {NULL};
void* q_send_buf = NULL;
/* 是否环回测试 TRUE:环回，FALSE:ping */
bool  bIsLoopTest = TRUE;
/* 是否在writedone中释放buffer*/
bool  bIsFreeBuf = TRUE;
/* 记录NCM网卡相关内容*/
net_context_t g_net_ctx[GNET_USED_NUM];
/* NCM默认参数值*/
NCM_DEFAULT_PARAM_S g_stNcmDefaultParam[GNET_USED_NUM];

/* 是否持续发包*/
bool  g_bIsFlowCtl = TRUE;
/* 流控开关时间间隔*/
u32 g_u32FlowDelay = 100;

u32 g_u32PsRcvPktCount = 0;
static unsigned int ncm_dbg_flag = 0;

extern bool ncm_bypass_udi;
extern s32 NCM_IoctlGetDefaultParam(u32 eth_no);
extern s32 NCM_IoctlSetDefaultParam(u32 eth_no);
extern int NCM_TEST_PsSendLLIStart(int nodePktNum,int SendPktLen,int WrongPktType);
extern int NCM_TEST_PsSendLLIStop(void);
extern s32 BSP_UDI_GetPrivate(UDI_HANDLE handle);


void ncm_st_dbg_set(unsigned int enable)
{
    ncm_dbg_flag = enable;
}

unsigned int ncm_st_dbg_get(void)
{
    return ncm_dbg_flag;
}

inline void ncm_print_netctx(net_context_t *net_ctx)
{
    if(ncm_st_dbg_get())
    {
        printk("----------------------------\n");
        printk("net ctx info:\n");
        printk("--mac addr:%02X:%02X:%02X:%02X:%02X:%02X\n",
            net_ctx->mac_addr[0], net_ctx->mac_addr[1], net_ctx->mac_addr[2],
            net_ctx->mac_addr[3], net_ctx->mac_addr[4], net_ctx->mac_addr[5]);
        printk("--ip  addr:%d.%d.%d.%d\n",
            net_ctx->ip_addr[0], net_ctx->ip_addr[1],
            net_ctx->ip_addr[2], net_ctx->ip_addr[3]);
        printk("----------------------------\n");
    }
}
inline void ncm_print_arp(arp_t *arp)
{
    if(ncm_st_dbg_get())
    {
        printk("----------------------------\n");
        printk("arp packet received:\n");
        printk("--opcode      :%04X\n", ua_be16toh(&arp->opcode));
        printk("--src mac addr:%02X:%02X:%02X:%02X:%02X:%02X\n",
            arp->sma[0], arp->sma[1], arp->sma[2],
            arp->sma[3], arp->sma[4], arp->sma[5]);
        printk("--src ip      :%d.%d.%d.%d\n",
            arp->sia[0], arp->sia[1], arp->sia[2], arp->sia[3]);
        printk("--dst mac addr: %02X:%02X:%02X:%02X:%02X:%02X\n",
            arp->dma[0], arp->dma[1], arp->dma[2],
            arp->dma[3], arp->dma[4], arp->dma[5]);
        printk("--dst ip      :%d.%d.%d.%d\n",
            arp->dia[0], arp->dia[1], arp->dia[2], arp->dia[3]);
        printk("----------------------------\n");
    }
}

inline void ncm_print_ip(ip_t *s_ip)
{
    if(ncm_st_dbg_get())
    {
        printk("----------------------------\n");
        printk("ip packet received:\n");
        printk("src: %d.%d.%d.%d dst: %d.%d.%d.%d proto: %02X len: %02X\n",
            s_ip->hdr.sa[0], s_ip->hdr.sa[1], s_ip->hdr.sa[2], s_ip->hdr.sa[3],
            s_ip->hdr.da[0], s_ip->hdr.da[1], s_ip->hdr.da[2], s_ip->hdr.da[3],
            s_ip->hdr.proto, s_ip->hdr.length);
        printk("----------------------------\n");
    }
}

inline void ncm_print_skb(struct sk_buff *skb)
{
    u32 cnt = 0;
    u32 len = skb->len<=64?skb->len:64;

    if(ncm_st_dbg_get())
    {
        printk("----------------------------\n");
        printk("skb data(len %d):\n",skb->len);
        for(cnt=0; cnt<len; cnt++)
        {
            printk("%02x ",skb->data[cnt]);
        }
        printk("\n");
        printk("----------------------------\n");
    }
}

inline void ncm_print_bytes(char *data, u32 len)
{
    u32 cnt = 0;

    printk("----------------------------\n");
    for(cnt=0; cnt<len; cnt++)
    {
        printk("%02x ",data[cnt]);
    }
    printk("\n");
    printk("----------------------------\n");
}

void NCM_AtProcess(u8 * pu8Buf, u32 u32Len)
{
    NCM_TEST_FUNC_ENTER();
    return;
}

#if 0
void NCM_NetStusChange(u32 u32NcmId,NCM_IOCTL_CONNECT_STUS_E enStatus, void * pBuffer)
{
    printk("%s line %d:net status change(ncm_id:%d,status:%d).\n",__FUNCTION__,__LINE__,u32NcmId,enStatus);
    return;
}
#endif

void NCM_PsRcvPkt(UDI_HANDLE handle, struct sk_buff *skb)
{
    g_u32PsRcvPktCount ++;
    kfree_skb(skb);
    return;
}

s32 NCM_TEST_Init(u32 eth_no)
{
    u32 result = 0;

    if (TRUE == g_u32NCMstInit[eth_no])
    {
        return 0;
    }

    NCM_TEST_FUNC_ENTER();

    result = NCM_IoctlGetDefaultParam(eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    result = NCM_IoctlSetDefaultParam(eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 申请环回发包buffer*/
    /* g_tx_loop_buff = (net_buffer_t *)memalign(CACHE_LINE_SIZE, LOOPBACK_SEND_DATA_LEN); */
    /* g_tx_loop_buff = (net_buffer_t *)BSP_SMalloc(LOOPBACK_SEND_DATA_LEN, 1); */
    g_tx_loop_buff[eth_no] = (u8 *)kzalloc(LOOPBACK_SEND_DATA_LEN, GFP_KERNEL);
    if (!g_tx_loop_buff[eth_no])
    {
        NCM_TEST_FUNC_FAIL(g_tx_loop_buff[eth_no]);
        return -1;
    }

    sema_init(&g_NcmPsRecvSemId[eth_no], 0);

    g_u32NCMstInit[eth_no] = TRUE;
    NCM_TEST_FUNC_PASS();
    return 0;
}

int NCM_TEST_UnInit(void)
{
    u32 loop;
    for (loop = 0; loop < GNET_USED_NUM; loop++) {

        if (g_tx_loop_buff[loop])
        {
            kfree((u8 *)g_tx_loop_buff[loop]);
            g_tx_loop_buff[loop]= NULL;
        }

        g_u32NCMstInit[loop] = FALSE;
    }

    return 0;
}

void NCM_Net_Init(u32 eth_no)
{
    u32 eth_idx;

    u8 ip_addr[GNET_USED_NUM][IP_ADDR_LEN] = {0};

    u8 mac_addr[GNET_USED_NUM][UW_MAC_ADDR_LEN] = {0};


    for (eth_idx = 0; eth_idx < GNET_USED_NUM; eth_idx++) {
        ip_addr[eth_idx][0] = 192;
        ip_addr[eth_idx][1] = 168;
        ip_addr[eth_idx][2] = eth_idx;
        ip_addr[eth_idx][3] = 5;

        mac_addr[eth_idx][0] = 0x00;
        mac_addr[eth_idx][1] = 0xBA;
        mac_addr[eth_idx][2] = 0xBA;
        mac_addr[eth_idx][3] = 0xCA;
        mac_addr[eth_idx][4] = 0xFE;
        mac_addr[eth_idx][5] = eth_idx;
    }

    memcpy(g_net_ctx[eth_no].mac_addr, mac_addr[eth_no], UW_MAC_ADDR_LEN);
    memcpy(g_net_ctx[eth_no].ip_addr, ip_addr[eth_no], IP_ADDR_LEN);

}

s32 NCM_DataStart(void *rcvFunc, void *freeFunc, u32 eth_no)
{
    u32 u32Result = 0;
    u32 u32LinkStus = NCM_IOCTL_CONNECTION_LINKDOWN;
    NCM_IOCTL_CONNECTION_SPEED_S stNcmConnectSpeed;

    if (TRUE == g_u32NCMDataStart[eth_no])
    {
        return 0;
    }

    NCM_TEST_Init(eth_no);
    NCM_Net_Init(eth_no);

    u32Result = bsp_ncm_open(NCM_DEV_DATA_TYPE, eth_no);
    if (-1 == u32Result)
    {
        NCM_TEST_FUNC_FAIL(u32Result);
        return -1;
    }

    u32Result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_REG_UPLINK_RX_FUNC, (void *)rcvFunc);
    if(0 != u32Result)
    {
        NCM_TEST_FUNC_FAIL(u32Result);
        return -1;
    }
    u32Result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_REG_FREE_PKT_FUNC, (void *)freeFunc);
    if(0 != u32Result)
    {
        NCM_TEST_FUNC_FAIL(u32Result);
        return -1;
    }
/*
    u32LinkStus = NCM_IOCTL_CONNECTION_LINKDOWN;
    u32Result = bsp_ncm_ioctl(g_u32NcmDevId, NCM_IOCTL_NETWORK_CONNECTION_NOTIF, &u32LinkStus);
    if(0 != u32Result)
    {
        NCM_TEST_FUNC_FAIL(u32Result);
        return -1;
    }
*/
    stNcmConnectSpeed.u32DownBitRate = 13 * 512 * 8 * 1000 * 8;//100 * 1000 * 1000;
    stNcmConnectSpeed.u32UpBitRate   = 13 * 512 * 8 * 1000 * 8;//50 * 1000 * 1000;

    u32Result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_CONNECTION_SPEED_CHANGE_NOTIF, (void *)&stNcmConnectSpeed);
    if(0 != u32Result)
    {
        NCM_TEST_FUNC_FAIL(u32Result);
        return -1;
    }

    u32LinkStus = NCM_IOCTL_CONNECTION_LINKUP;
    u32Result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_NETWORK_CONNECTION_NOTIF, &u32LinkStus);
    if(0 != u32Result)
    {
        NCM_TEST_FUNC_FAIL(u32Result);
        return -1;
    }

    g_u32NCMDataStart[eth_no] = TRUE;
    return 0;
}

void NCM_FreeLoopBuf(void *PktEncap)
{
    struct sk_buff *skb = (struct sk_buff *)PktEncap;
    kfree(skb);
}

void NCM_FreePingBuf(void *PktEncap)
{
    struct sk_buff *skb = (struct sk_buff *)PktEncap;
    kfree(skb->data);
    kfree(skb);
}

void NCM_PsSendFreeBuf(void *PktEncap)
{
    g_u32PktEncap = (u32)PktEncap;
    return;
}

s32 NCM_GetRxData(UDI_HANDLE handle, struct sk_buff *skb)
{
    net_buffer_t *rx_buff = NULL;
    u16 type;
    u8 des_mac = {0xff,0xff,0xff,0xff,0xff,0xff};
    /* 用于区分SSDP协议包 request method:M-SEARCH*/
    u8 u8aSSDP_http_req_method[8] = {0x4d,0x2d,0x53,0x45,0x41,0x52,0x43,0x48};

    /*multi_eth add by wangweichao*/
    u32 dev_idx = 0;

    rx_buff = (net_buffer_t *)skb->data;
    type = ua_be16toh(&rx_buff->frame.hdr.type);

    if (type != PROTO_IP)
    {
        kfree_skb(skb);
        return 0;
    }

    /* 网卡连接后，PC会发出NBNS包和SSDP包，过滤掉*/
    if (0 == memcmp((void*)des_mac, (void*)rx_buff->frame.hdr.da,
        UW_MAC_ADDR_LEN))
    {
        kfree_skb(skb);
        return 0;
    }

    /* 用于区分SSDP协议包 request method:M-SEARCH*/
    if (0 == memcmp((void*)u8aSSDP_http_req_method, (void*)((u32)rx_buff+42), 8))
    {
        kfree_skb(skb);
        return 0;
    }

    if (TRUE == g_u32FirstRcvFlag[dev_idx])
    {
        g_u32FirstRcvFlag[dev_idx] = FALSE;

        memcpy(g_tx_loop_buff[dev_idx], rx_buff, skb->len);

        /* switch between source and dest mac address */
        memcpy(g_tx_loop_buff[dev_idx]->frame.hdr.sa, rx_buff->frame.hdr.da, UW_MAC_ADDR_LEN);
        memcpy(g_tx_loop_buff[dev_idx]->frame.hdr.da, rx_buff->frame.hdr.sa, UW_MAC_ADDR_LEN);

        /* switch between source and dest ip address */
        memcpy(g_tx_loop_buff[dev_idx]->frame.packet.ip.hdr.da, rx_buff->frame.packet.ip.hdr.sa, IP_ADDR_LEN);
        memcpy(g_tx_loop_buff[dev_idx]->frame.packet.ip.hdr.sa, rx_buff->frame.packet.ip.hdr.da, IP_ADDR_LEN);

        //cacheFlush(DATA_CACHE, (void *)(g_tx_loop_buff), LOOPBACK_SEND_DATA_LEN);
        printk(">>NCM_GetRxData g_tx_loop_buff is %p \n",g_tx_loop_buff);
        //os_cache_flush( (void *)(g_tx_loop_buff), LOOPBACK_SEND_DATA_LEN);
        //semGive(g_NcmPsRecvSemId);
        up(&g_NcmPsRecvSemId);
    }

    kfree_skb(skb);

    return 0;
}

s32 NCM_DataStop(u32 eth_no)
{
    u32 u32Result;

    u32Result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_REG_UPLINK_RX_FUNC, NULL);
    if(0 != u32Result)
    {
        NCM_TEST_FUNC_FAIL(u32Result);
        return -1;
    }

    u32Result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_REG_FREE_PKT_FUNC, NULL);
    if(0 != u32Result)
    {
        NCM_TEST_FUNC_FAIL(u32Result);
        return -1;
    }

    u32Result = bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
    if (-1 == u32Result)
    {
        NCM_TEST_FUNC_FAIL(u32Result);
        return -1;
    }

    g_u32NCMDataStart[eth_no] = FALSE;
    g_u32FirstRcvFlag[eth_no] = TRUE;

    return 0;
}

static u16 in_cksum(u16 *addr, int len)
{
    int nleft = len;
    u16 *w = addr;
    int sum = 0;
    u16 answer = 0;

    /*
     * Our algorithm is simple, using a 32 bit accumulator (sum), we add
     * sequential 16 bit words to it, and at the end, fold back all the
     * carry bits from the top 16 bits into the lower 16 bits.
     */

    while (nleft > 1)
    {
        sum += *w++;
        nleft -= 2;
    }

    /* mop up an odd byte, if necessary */

    if (nleft == 1)
    {
        *(uint8_t *)(&answer) = *(uint8_t *)w;
        sum += answer;
    }

    /* add back carry outs from top 16 bits to low 16 bits */

    sum = (sum >> 16) + (sum & 0xffff);     /* add hi 16 to low 16 */
    sum += (sum >> 16);                     /* add carry */
    answer = (u16)(~sum);              /* truncate to 16 bits */

    return answer;
}

u32 NCM_process_icmp(net_context_t *net_ctx, u16 len,
    icmp_t *s_icmp, icmp_t *d_icmp)
{
    u16 chkRet = 0;

    memcpy(d_icmp, s_icmp, len);
    d_icmp->hdr.type = ICMP_TYPE_PING_REPLY;
    memset(&d_icmp->hdr.chk, 0, sizeof(d_icmp->hdr.chk));

    chkRet = in_cksum((u16 *)d_icmp, len);
    memcpy(&d_icmp->hdr.chk, &chkRet, 2);
    return len;
}

u32 NCM_process_ip(net_context_t *net_ctx, ip_t *s_ip, ip_t *d_ip)
{
    u32 o_len = 0;
    u16 length;

    length = ua_be16toh(&s_ip->hdr.length);

    ncm_print_netctx(net_ctx);
    ncm_print_ip(s_ip);

    memcpy((void *)&d_ip->hdr, (void *)&s_ip->hdr, sizeof(ip_header_t));
    memcpy((void *)d_ip->hdr.sa, net_ctx->ip_addr, IP_ADDR_LEN);
    memcpy((void *)d_ip->hdr.da, (void *)s_ip->hdr.sa, IP_ADDR_LEN);

    switch (s_ip->hdr.proto)
    {
    case PROTO_IP_ICMP:
        o_len = NCM_process_icmp(net_ctx, length - sizeof(ip_header_t),
            &s_ip->data.icmp, &d_ip->data.icmp);

        break;

    default:
        break;
    }

    if (o_len)
        o_len += sizeof(ip_header_t);

    return o_len;
}

u32 NCM_process_arp(net_context_t *net_ctx, arp_t *s_arp, arp_t *d_arp)
{
    ncm_print_arp(s_arp);
    ncm_print_netctx(net_ctx);

    if (memcmp(net_ctx->ip_addr, (void *)s_arp->dia, IP_ADDR_LEN))
        return 0;

    memset((void *)d_arp, 0, sizeof(arp_t));

    memcpy(d_arp, s_arp, 6);
    ua_htobe16(&d_arp->opcode, ARP_OPCODE_REPLY);
    memcpy((void *)d_arp->sma, net_ctx->mac_addr, UW_MAC_ADDR_LEN);
    memcpy((void *)d_arp->sia, net_ctx->ip_addr, IP_ADDR_LEN);
    memcpy((void *)d_arp->dma, (void *)s_arp->sma, UW_MAC_ADDR_LEN);
    memcpy((void *)d_arp->dia, (void *)s_arp->sia, IP_ADDR_LEN);

    return sizeof(arp_t);
}

u32 NCM_PING(UDI_HANDLE handle, struct sk_buff *skb)
{
    net_buffer_t *rx_buff = NULL;
    net_buffer_t *tx_buff = NULL;
    struct sk_buff *skb_tx = NULL;
    u16 type;
    u32 u32PacketLen = 0;
    int rc;
    s32 dev_idx;

    dev_idx = handle;

    skb_tx = dev_alloc_skb(LOOPBACK_SEND_DATA_LEN);
    if (!skb_tx)
    {
        printk("alloc skb_tx failed,line %d.\n",__LINE__);
        rc = -ENOMEM;
        goto error;
    }

    rx_buff = (net_buffer_t *)skb->data;
    type = ua_be16toh(&rx_buff->frame.hdr.type);

    tx_buff = (net_buffer_t *)skb_tx->data;

    /* fill mac header */
    memcpy((void *)tx_buff->frame.hdr.da, (void *)rx_buff->frame.hdr.sa, UW_MAC_ADDR_LEN);
    memcpy((void *)tx_buff->frame.hdr.sa, g_net_ctx[dev_idx].mac_addr, UW_MAC_ADDR_LEN);
    memcpy((void *)tx_buff->frame.hdr.type, (void *)rx_buff->frame.hdr.type, 2);

    switch (type)
    {
        case PROTO_ARP:
            u32PacketLen = NCM_process_arp(&g_net_ctx[dev_idx], &rx_buff->frame.packet.arp,
                (arp_t *)&tx_buff->frame.packet.arp);
            if (!u32PacketLen)
            {
                rc = -1;
                goto error;
            }
            break;

        case PROTO_IP:
            u32PacketLen = NCM_process_ip(&g_net_ctx[dev_idx], &rx_buff->frame.packet.ip,
                  (ip_t *)&tx_buff->frame.packet.ip);
            if (!u32PacketLen)
            {
                rc = -1;
                goto error;
            }
            break;

        default:
            printk("unsupported frame type(0x%x),line %d.\n",type,__LINE__);
            rc = -EINVAL;
            goto error;
    }

    u32PacketLen += sizeof(eth_header_t);
    skb_tx->len = u32PacketLen;

    /* send skb_tx */
    rc = bsp_ncm_write(dev_idx, skb_tx, NULL);
    if (rc)
    {
        printk("bsp_ncm_write failed,line %d.\n",__LINE__);
        goto error;
    }

    goto done;

error:
    if (skb_tx)
    {
        dev_kfree_skb(skb_tx);
        skb_tx = NULL;
    }

done:
    if (skb)
    {
        dev_kfree_skb(skb);
        skb = NULL;
    }

    return rc;
}

u32 NCM_LOOP(UDI_HANDLE handle, struct sk_buff *skb)
{
    net_buffer_t *rx_buff = NULL;
    u8 mac_addr[UW_MAC_ADDR_LEN] = {0};
    u8 ip_addr[IP_ADDR_LEN];
    u16 type;
    s32 rc = 0;
    s32 dev_idx;

    dev_idx = handle;
    rx_buff = (net_buffer_t *)skb->data;
    type = ua_be16toh(&rx_buff->frame.hdr.type);

    if (type != PROTO_IP)
    {
        goto error;
    }

    /* switch between source and dest mac address */
    memcpy(mac_addr,rx_buff->frame.hdr.sa,UW_MAC_ADDR_LEN);
    memcpy(rx_buff->frame.hdr.sa, rx_buff->frame.hdr.da, UW_MAC_ADDR_LEN);
    memcpy(rx_buff->frame.hdr.da, mac_addr, UW_MAC_ADDR_LEN);

    /* switch between source and dest ip address */
    memcpy(ip_addr,rx_buff->frame.packet.ip.hdr.sa,IP_ADDR_LEN);
    memcpy(rx_buff->frame.packet.ip.hdr.sa, rx_buff->frame.packet.ip.hdr.da, IP_ADDR_LEN);
    memcpy(rx_buff->frame.packet.ip.hdr.da, ip_addr, IP_ADDR_LEN);

    rc = bsp_ncm_write(dev_idx, skb, NULL);
    if (rc)
    {
        printk("bsp_ncm_write failed,line %d.\n",__LINE__);
        goto error;
    }

    goto done;

error:
    if(skb)
    {
        dev_kfree_skb_any(skb);
        skb = NULL;
    }

done:
    return rc;
}

u32 NCM_Connect(u32 u32DevId, u32 u32DownBitRate, u32 u32UpBitRate)
{
    u32 u32Result = 0;
    NCM_IOCTL_CONNECTION_SPEED_S stNcmConnectSpeed;
    u32 u32LinkStus = NCM_IOCTL_CONNECTION_LINKDOWN;

    u32Result = bsp_ncm_ioctl(u32DevId, NCM_IOCTL_NETWORK_CONNECTION_NOTIF, &u32LinkStus);
    if(0 != u32Result)
    {
        NCM_TEST_FUNC_FAIL(u32Result);
        return u32Result;
    }

    stNcmConnectSpeed.u32DownBitRate = u32DownBitRate;
    stNcmConnectSpeed.u32UpBitRate   = u32UpBitRate;

    u32Result = bsp_ncm_ioctl(u32DevId, NCM_IOCTL_CONNECTION_SPEED_CHANGE_NOTIF, (void *)&stNcmConnectSpeed);
    if(0 != u32Result)
    {
        NCM_TEST_FUNC_FAIL(u32Result);
        return u32Result;
    }

    u32LinkStus = NCM_IOCTL_CONNECTION_LINKUP;
    u32Result = bsp_ncm_ioctl(u32DevId, NCM_IOCTL_NETWORK_CONNECTION_NOTIF, &u32LinkStus);
    if(0 != u32Result)
    {
        NCM_TEST_FUNC_FAIL(u32Result);
        return u32Result;
    }

    return 0;
}

/*****************************************************************************
* 函 数 名  : tNcmPsSendFunc
*
* 功能描述  : ps发包任务
*
* 输入参数  : u32 u32DevId
*             int nodeNum
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
int tNcmPsSendFunc(void* pPara)
{
    int i = 0;
    int rc = 0;
    PS_SEND_TASK_PARA* pstTaskPara = (PS_SEND_TASK_PARA*)pPara;
    u32 u32DevId;
    u32 nodeNum;

    u32DevId = pstTaskPara->u32DevId;
    nodeNum = pstTaskPara->nodeNum;
    while(g_bIsSend)
    {
        mdelay(10*g_u32PsSendDelay);
        for(i = 0; i < nodeNum; i++)
        {
            rc = bsp_ncm_write(u32DevId, g_pPsSendHead, NULL);
            if (rc)
            {
                g_u32pssendfail++;
                //printk("\r tNcmPsSendFunc bsp_ncm_write fail rc = 0x%x\n",rc);
                break;
            }
        }
    }
    return 0;
}

/*****************************************************************************
* 函 数 名  : tNcmFlowCtlFunc
*
* 功能描述  : 流控任务
*
* 输入参数  : u32 u32DevId
*             int packetNum
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
int tNcmFlowCtlFunc(void* pPara)
{
    int result = 0;
    u32 u32FlowCtl = NCM_IOCTL_FLOW_CTRL_ENABLE;
    u32 u32DevId = (u32)pPara;

    while(g_bIsFlowCtl)
    {
        mdelay(10*g_u32FlowDelay);
        u32FlowCtl = NCM_IOCTL_FLOW_CTRL_ENABLE;
        result = bsp_ncm_ioctl(u32DevId, NCM_IOCTL_FLOW_CTRL_NOTIF, &u32FlowCtl);
        if(0 != result)
        {
            NCM_TEST_FUNC_FAIL(result);
            return -1;
        }
        mdelay(10*g_u32FlowDelay);
        u32FlowCtl = NCM_IOCTL_FLOW_CTRL_DISABLE;
        result = bsp_ncm_ioctl(u32DevId, NCM_IOCTL_FLOW_CTRL_NOTIF, &u32FlowCtl);
        if(0 != result)
        {
            NCM_TEST_FUNC_FAIL(result);
            return -1;
        }
    }
    return 0;
}

int NCM_TEST_SetPsSendDelay(u32 u32Value)
{
    g_u32PsSendDelay = u32Value;

    return 0;
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_DATACHANNEL_001
*
* 功能描述  : 测试打开关闭NCM数据通道
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  : 无
*
*****************************************************************************/
s32 NCM_TEST_DATACHANNEL_001(void)
{
    u32 i;
    u32 result;

    NCM_TEST_FUNC_ENTER();

    for(i=0; i<GNET_USED_NUM; i++)
    {
        result = bsp_ncm_open(NCM_DEV_DATA_TYPE, i);
        if (0 != result)
        {
            NCM_TEST_FUNC_FAIL(result);
            return -1;
        }

        result = bsp_ncm_close(NCM_DEV_DATA_TYPE, i);
        if (0 != result)
        {
            NCM_TEST_FUNC_FAIL(result);
            return -1;
        }
    }

    NCM_TEST_FUNC_PASS();

    return 0;
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_DATACHANNEL_002
*
* 功能描述  : 获取NCM数据设备ID超过设备最大支持数目
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  : 无
*
*****************************************************************************/
s32 NCM_TEST_DATACHANNEL_002(void)
{
    u32 result = 0;
    u32 i;
    u32 u32Flag = 0;

    NCM_TEST_FUNC_ENTER();

    for (i = 0; i < (GNET_USED_NUM + 1); i++)
    {
        result = bsp_ncm_open(NCM_DEV_DATA_TYPE, i);
        if (0 != result)
        {
            break;
        }
    }

    if (GNET_USED_NUM == i)
    {
        u32Flag = 1;
    }

    for (i = 0; i < GNET_USED_NUM; i++)
    {
        result = bsp_ncm_close(NCM_DEV_DATA_TYPE, i);
        if (0 != result)
        {
            NCM_TEST_FUNC_FAIL(result);
            return -1;
        }
    }

    if (1 == u32Flag)
    {
        NCM_TEST_FUNC_PASS();
        return 0;
    }
    else
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_CTRLCHANNEL_001
*
* 功能描述  : 测试打开关闭NCM控制通道
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  :
*
*****************************************************************************/
s32 NCM_TEST_CTRLCHANNEL_001(void)
{
    u32 i;
    u32 result;

    NCM_TEST_FUNC_ENTER();

    for(i=0; i<GNET_USED_NUM; i++)
    {
        result = bsp_ncm_open(NCM_DEV_CTRL_TYPE, i);
        if (0 != result)
        {
            NCM_TEST_FUNC_FAIL(result);
            return -1;
        }

        result = bsp_ncm_close(NCM_DEV_CTRL_TYPE, i);
        if (0 != result)
        {
            NCM_TEST_FUNC_FAIL(result);
            return -1;
        }
    }

    NCM_TEST_FUNC_PASS();

    return 0;
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_CTRLCHANNEL_002
*
* 功能描述  : 获取NCM控制设备ID超过设备最大支持数目
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  : 无
*
*****************************************************************************/
s32 NCM_TEST_CTRLCHANNEL_002(void)
{
    u32 result = 0;
    u32 i;
    u32 u32Flag = 0;

    NCM_TEST_FUNC_ENTER();

    for (i = 0; i < (GNET_USED_NUM + 1); i++)
    {
        result = bsp_ncm_open(NCM_DEV_CTRL_TYPE, i);
        if (0 != result)
        {
            break;
        }
    }

    if (GNET_USED_NUM == i)
    {
        u32Flag = 1;
    }

    for (i = 0; i < GNET_USED_NUM; i++)
    {
        result = bsp_ncm_close(NCM_DEV_CTRL_TYPE, i);
        if (0 != result)
        {
            NCM_TEST_FUNC_FAIL(result);
            return -1;
        }
    }

    if (1 == u32Flag)
    {
        NCM_TEST_FUNC_PASS();
        return 0;
    }
    else
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_INVALPARAM_001
*
* 功能描述  : 测试open函数的错误参数
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  :
*
*****************************************************************************/
s32 NCM_TEST_INVALPARAM_001(void)
{
    u32 pu32DevId = GNET_USED_NUM + 1;
    u32 result;

    NCM_TEST_FUNC_ENTER();

    result = bsp_ncm_open(NCM_DEV_CTRL_TYPE, pu32DevId);
    if (0 != result)
    {
        NCM_TEST_FUNC_PASS();
        return 0;
    }
    else
    {
        NCM_TEST_FUNC_FAIL(-1);
        return -1;
    }
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_INVALPARAM_002
*
* 功能描述  : 测试write函数的错误参数
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  :
*
*****************************************************************************/
s32 NCM_TEST_INVALPARAM_002(u32 eth_no)
{
    struct sk_buff *skb = NULL;
    u32 u32DevIdTmp;
    u32 result;

    NCM_TEST_FUNC_ENTER();

    result = bsp_ncm_open(NCM_DEV_DATA_TYPE, eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    skb = alloc_skb(2*1024, GFP_KERNEL);
    if (!skb)
    {
        NCM_TEST_FUNC_FAIL(skb);
        result = -1;
        goto error;
    }

    /* 输入错误设备ID号*/
    u32DevIdTmp = 100;
    result = bsp_ncm_write(u32DevIdTmp, skb, NULL);
    if (0 == result)
    {
        NCM_TEST_FUNC_FAIL(result);
        result = -1;
        goto error;
    }

    /* 输入空数据指针*/
    result = bsp_ncm_write(eth_no, NULL, NULL);
    if (0 == result)
    {
        NCM_TEST_FUNC_FAIL(result);
        result = -1;
        goto error;
    }

    (void)bsp_ncm_close(NCM_DEV_DATA_TYPE,eth_no);

    /* 输入没有open的设备ID号*/
    result = bsp_ncm_write(eth_no, skb, NULL);
    if (0 == result)
    {
        NCM_TEST_FUNC_FAIL(result);
        result = -1;
        return result;
    }

    kfree_skb(skb);
    skb = NULL;

    NCM_TEST_FUNC_PASS();

    return 0;

error:
    if(skb)
    {
        kfree_skb(skb);
        skb = NULL;
    }

    bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
    return result;
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_INVALPARAM_003
*
* 功能描述  : 测试Iotcl错误参数，未打开数据设备，配置与数据设备相关参数
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  : 无
*
*****************************************************************************/
s32 NCM_TEST_INVALPARAM_003(u32 eth_no)
{
    NCM_AT_RSP_S stAtResponse;
    u32 u32DevId = 0;
    u32 result = 0;
    u32 u32LinkStus = NCM_IOCTL_CONNECTION_LINKDOWN;

    NCM_TEST_FUNC_ENTER();

    memset(&stAtResponse,0x0,sizeof(NCM_AT_RSP_S));

    /* 输入错误的设备ID号*/
    u32DevId = 100;
    result = bsp_ncm_ioctl(u32DevId, NCM_IOCTL_NETWORK_CONNECTION_NOTIF, &u32LinkStus);
    if ((u32)-EINVAL != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 数据通道设备ID未打开,配置与数据通道相关的参数，网卡连接状态*/
    u32DevId = 0;
    result = bsp_ncm_ioctl(u32DevId, NCM_IOCTL_NETWORK_CONNECTION_NOTIF, &u32LinkStus);
    if ((u32)-ENODEV != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 控制通道设备ID未打开,发送AT命令回应*/
    result = bsp_ncm_ioctl(u32DevId, NCM_IOCTL_AT_RESPONSE, &stAtResponse);
    if ((u32)-ENODEV != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 打开数据通道设备*/
    result = bsp_ncm_open(NCM_DEV_DATA_TYPE, eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 输入空参数*/
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_FREE_BUFF, NULL);
    if (-EIO != result)
    {
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);

    NCM_TEST_FUNC_PASS();
    return 0;
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_INVALPARAM_004
*
* 功能描述  : 测试Ioctl错误参数，打开数据设备ID，配置与控制设备相关参数
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  : 无
*
*****************************************************************************/
s32 NCM_TEST_INVALPARAM_004(u32 eth_no)
{
    NCM_AT_RSP_S stAtResponse;
    NCM_IOCTL_CONNECTION_SPEED_S speed;
    u32 result = 0;
    u32 u32LinkStus = NCM_IOCTL_CONNECTION_LINKUP;

    NCM_TEST_FUNC_ENTER();

    memset(&stAtResponse,0x0,sizeof(NCM_AT_RSP_S));

    #if 0
    result = bsp_ncm_open(NCM_DEV_DATA_TYPE, eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }
    #endif

    /*数据通道ID未打开,配置网卡连接状态 */
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_NETWORK_CONNECTION_NOTIF, &u32LinkStus);
    if (0 == result)
    {
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* /*数据通道ID未打开,配置网卡连接速度 */
    speed.u32DownBitRate = 100*1000*1000;
    speed.u32UpBitRate = 100*1000*1000;
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_CONNECTION_SPEED_CHANGE_NOTIF, &speed);
    if (0 == result)
    {
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 控制通道设备ID未打开,注册AT处理回调函数*/
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_REG_AT_PROCESS_FUNC, NCM_AtProcess);
    if (0 == result)
    {
        bsp_ncm_close(NCM_DEV_CTRL_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 控制通道设备ID未打开,发送AT命令回应*/
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_AT_RESPONSE, &stAtResponse);
    if (0 == result)
    {
        bsp_ncm_close(NCM_DEV_CTRL_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }
    #if 0
    bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
    #endif
    NCM_TEST_FUNC_PASS();
    return 0;
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_INVALPARAM_005
*
* 功能描述  : 测试错误参数
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  : 无
*
*****************************************************************************/
s32 NCM_TEST_INVALPARAM_005(u32 eth_no)
{
    NCM_AT_RSP_S stAtResponse;
    u32 result;
    u32 u32LinkStus = NCM_IOCTL_CONNECTION_LINKUP;

    NCM_TEST_FUNC_ENTER();

    stAtResponse.u32Length = 100;
    stAtResponse.pu8AtAnswer = kzalloc(128, GFP_KERNEL);
    if (!stAtResponse.pu8AtAnswer)
    {
        NCM_TEST_FUNC_FAIL(-1);
        return -1;
    }

    /* 打开控制通道 */
    result = bsp_ncm_open(NCM_DEV_CTRL_TYPE, eth_no);
    if (0 != result)
    {
        kfree(stAtResponse.pu8AtAnswer);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 数据通道设备ID未打开,配置网卡连接状态*/
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_NETWORK_CONNECTION_NOTIF, &u32LinkStus);
    if (0 == result)
    {
        kfree(stAtResponse.pu8AtAnswer);
        bsp_ncm_close(NCM_DEV_CTRL_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 注册AT处理回调函数*/
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_REG_AT_PROCESS_FUNC, NCM_AtProcess);
    if (0 != result)
    {
        kfree(stAtResponse.pu8AtAnswer);
        bsp_ncm_close(NCM_DEV_CTRL_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    kfree(stAtResponse.pu8AtAnswer);
    bsp_ncm_close(NCM_DEV_CTRL_TYPE, eth_no);

    NCM_TEST_FUNC_PASS();
    return 0;
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_IoctlWrongParam_006
*
* 功能描述  : 测试Iotcl错误参数，未打开数据设备，配置与数据设备相关参数
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  : 无
*
*****************************************************************************/
s32 NCM_TEST_INVALPARAM_006(u32 eth_no)
{
    NCM_AT_RSP_S stAtResponse;
    u32 result;
    u32 u32LinkStus = NCM_IOCTL_CONNECTION_LINKDOWN;
    u32 u32FlowCtrl = NCM_IOCTL_FLOW_CTRL_ENABLE;

    NCM_TEST_FUNC_ENTER();

    memset(&stAtResponse,0x0,sizeof(NCM_AT_RSP_S));

    /* 打开数据通道设备*/
    result = bsp_ncm_open(NCM_DEV_DATA_TYPE, eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 输入错误的连接状态号*/
    u32LinkStus = 3;
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_NETWORK_CONNECTION_NOTIF, &u32LinkStus);
    if (0 == result)
    {
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 输入错误的流控状态*/
    u32FlowCtrl = 3;
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_FLOW_CTRL_NOTIF, &u32FlowCtrl);
    if (0 == result)
    {
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 输入错误的命令字*/
    result = bsp_ncm_ioctl(eth_no, 100, &u32FlowCtrl);
    if (0 == result)
    {
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
    NCM_TEST_FUNC_PASS();
    return 0;
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_INVALPARAM_007
*
* 功能描述  : 测试错误参数
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  : 无
*
*****************************************************************************/
s32 NCM_TEST_INVALPARAM_007(u32 eth_no)
{
    u32 u32DevId;
    u32 result;
    NCM_AT_RSP_S stAtResponse;
    u8 aAtAnswer[NCM_AT_ANSWER_LEN];

    NCM_TEST_FUNC_ENTER();

	if(!bsp_usb_is_ncm_bypass_mode())
	{
		printk("run this func need enable stick modem\n");
		NCM_TEST_FUNC_FAIL(result);
        return -1;
	}

    memset(aAtAnswer,0x0,MAX_ENCAP_CMD_SIZE);
    memset(&stAtResponse,0x0,sizeof(NCM_AT_RSP_S));

    /* 控制通道设备未打开，发送AT命令应答*/
    stAtResponse.pu8AtAnswer = aAtAnswer;
    stAtResponse.u32Length = NCM_AT_ANSWER_LEN;
    result = bsp_ncm_ioctl(u32DevId, NCM_IOCTL_AT_RESPONSE, &stAtResponse);
    if (0 == result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 打开控制通道 */
    result = bsp_ncm_open(NCM_DEV_CTRL_TYPE, eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 数据通道设备未打开，注册AT处理回调函数*/
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_REG_AT_PROCESS_FUNC, NCM_AtProcess);
    if (0 != result)
    {
        bsp_ncm_close(NCM_DEV_CTRL_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 数据通道设备未打开，发送AT命令应答，buffer为空*/
    stAtResponse.pu8AtAnswer = NULL;
    stAtResponse.u32Length = NCM_AT_ANSWER_LEN;
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_AT_RESPONSE, &stAtResponse);
    if (0 == result)
    {
        bsp_ncm_close(NCM_DEV_CTRL_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 数据通道设备未打开，发送AT命令应答，长度为0*/
    stAtResponse.pu8AtAnswer = aAtAnswer;
    stAtResponse.u32Length = 0;
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_AT_RESPONSE, &stAtResponse);
    if (0 == result)
    {
        bsp_ncm_close(NCM_DEV_CTRL_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    bsp_ncm_close(NCM_DEV_CTRL_TYPE, eth_no);
    NCM_TEST_FUNC_PASS();
    return 0;
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_NETDISABLE_001
*
* 功能描述  : 网卡禁用或者拔掉USB后，测试需要与PC交互的参数配置应该报错
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  :
*
*****************************************************************************/
s32 NCM_TEST_NETDISABLE_001(u32 eth_no)
{

    NCM_IOCTL_CONNECTION_SPEED_S stNcmConnectSpeed;
    NCM_AT_RSP_S  stAtResponse;
    u32 result;
    u32 u32LinkStus = NCM_IOCTL_CONNECTION_LINKUP;
    u32 u32FlowCtrl = NCM_IOCTL_FLOW_CTRL_ENABLE;
    u32 u32TimeOut = 0;

    NCM_TEST_FUNC_ENTER();

    stAtResponse.u32Length = 100;
    stAtResponse.pu8AtAnswer  = kzalloc(128, GFP_KERNEL);
    if (!stAtResponse.pu8AtAnswer)
    {
        NCM_TEST_FUNC_FAIL(-1);
        return -1;
    }

    /* 打开数据通道 */
    result = bsp_ncm_open(NCM_DEV_DATA_TYPE, eth_no);
    if (0 != result)
    {
        kfree(stAtResponse.pu8AtAnswer);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 打开控制通道 */
    result = bsp_ncm_open(NCM_DEV_CTRL_TYPE, eth_no);
    if (0 != result)
    {
        kfree(stAtResponse.pu8AtAnswer);
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 配置网卡连接状态*/
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_NETWORK_CONNECTION_NOTIF, &u32LinkStus);
    if (0 == result)
    {
        kfree(stAtResponse.pu8AtAnswer);
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        bsp_ncm_close(NCM_DEV_CTRL_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 配置连接速度100M/50M*/
    stNcmConnectSpeed.u32DownBitRate= 100*1000*1000;
    stNcmConnectSpeed.u32UpBitRate = 50*1000*1000;
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_CONNECTION_SPEED_CHANGE_NOTIF, (void *)&stNcmConnectSpeed);
    if(0 == result)
    {
        kfree(stAtResponse.pu8AtAnswer);
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        bsp_ncm_close(NCM_DEV_CTRL_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 配置流控*/
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_FLOW_CTRL_NOTIF, (void *)&u32FlowCtrl);
    if(0 == result)
    {
        kfree(stAtResponse.pu8AtAnswer);
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        bsp_ncm_close(NCM_DEV_CTRL_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 发送AT命令回应*/
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_AT_RESPONSE, &stAtResponse);
    if (0 == result)
    {
        kfree(stAtResponse.pu8AtAnswer);
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        bsp_ncm_close(NCM_DEV_CTRL_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 获取超时时间*/
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_GET_DEFAULT_TX_TIMEOUT, &u32TimeOut);
    if (0 != result)
    {
        kfree(stAtResponse.pu8AtAnswer);
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        bsp_ncm_close(NCM_DEV_CTRL_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    kfree(stAtResponse.pu8AtAnswer);
    bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
    bsp_ncm_close(NCM_DEV_CTRL_TYPE, eth_no);
    NCM_TEST_FUNC_PASS();
    return 0;
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_LINKSTATUS_001
*
* 功能描述  : 断开NCM虚拟网卡设备连接，PC网卡图标显示X
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  :
*
*****************************************************************************/
s32 NCM_TEST_LINKSTATUS_001(u32 eth_no)
{
    u32 result;
    u32 u32LinkStus = NCM_IOCTL_CONNECTION_LINKDOWN;

    NCM_TEST_FUNC_ENTER();

    result = bsp_ncm_open(NCM_DEV_DATA_TYPE, eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_NETWORK_CONNECTION_NOTIF, &u32LinkStus);
    if(0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        return -1;
    }

    bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);

    NCM_TEST_FUNC_PASS();

    return 0;
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_LINKSTATUS_002
*
* 功能描述  : 虚拟网卡连接状态及速度测试,配置连接速度1G/1G,PC端显示1G
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  :
*
*****************************************************************************/
s32 NCM_TEST_LINKSTATUS_002(u32 eth_no)
{
    u32 result;
    u32 u32DownBitRate;
    u32 u32UpBitRate;

    NCM_TEST_FUNC_ENTER();

    result = bsp_ncm_open(NCM_DEV_DATA_TYPE, eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* bsp_ncm_ioctl配置连接速度480M/480M */
    u32DownBitRate = 1000*1000*1000;
    u32UpBitRate = 1000*1000*1000;
    result = NCM_Connect(eth_no, u32DownBitRate, u32UpBitRate);
    if (0 != result)
    {
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);

    NCM_TEST_FUNC_PASS();

    return 0;
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_LINKSTATUS_003
*
* 功能描述  : 虚拟网卡连接状态及速度测试，配置连接速度1k/1k，PC显示速度1k
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  :
*
*****************************************************************************/
s32 NCM_TEST_LINKSTATUS_003(u32 eth_no)
{
    u32 result;
    u32 u32DownBitRate;
    u32 u32UpBitRate;

    NCM_TEST_FUNC_ENTER();

    result = bsp_ncm_open(NCM_DEV_DATA_TYPE, eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* bsp_ncm_ioctl配置连接速度1k/1k*/
    u32DownBitRate = 1000;
    u32UpBitRate = 1000;
    result = NCM_Connect(eth_no, u32DownBitRate, u32UpBitRate);
    if (0 != result)
    {
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);

    NCM_TEST_FUNC_PASS();

    return 0;
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_LINKSTATUS_004
*
* 功能描述  : 虚拟网卡连接状态及速度测试,配置连接速度0bit/300M,PC端显示300M
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  :
*
*****************************************************************************/
s32 NCM_TEST_LINKSTATUS_004(u32 eth_no)
{
    u32 result;
    u32 u32DownBitRate;
    u32 u32UpBitRate;

    NCM_TEST_FUNC_ENTER();

    result = bsp_ncm_open(NCM_DEV_DATA_TYPE, eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* bsp_ncm_ioctl配置连接速度1bit/512k*/
    u32DownBitRate = 0;
    u32UpBitRate = 300*1000*1000;
    result = NCM_Connect(eth_no, u32DownBitRate, u32UpBitRate);
    if (0 != result)
    {
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);

    NCM_TEST_FUNC_PASS();
    return 0;
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_LINKSTATUS_005
*
* 功能描述  : 虚拟网卡连接状态及速度测试，配置连接速度500Mbit/0bit，PC端显示500M
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  :
*
*****************************************************************************/
s32 NCM_TEST_LINKSTATUS_005(u32 eth_no)
{
    u32 result;
    u32 u32DownBitRate;
    u32 u32UpBitRate;

    NCM_TEST_FUNC_ENTER();

    result = bsp_ncm_open(NCM_DEV_DATA_TYPE, eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* bsp_ncm_ioctl配置连接速度500Mbit/0bit*/
    u32DownBitRate = 500*1000*1000;
    u32UpBitRate = 0;
    result = NCM_Connect(eth_no, u32DownBitRate, u32UpBitRate);
    if (0 != result)
    {
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);

    NCM_TEST_FUNC_PASS();

    return 0;
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_LINKSTATUS_006
*
* 功能描述  : 虚拟网卡连接状态及速度测试，配置连接速2000M/512bit，报错
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  :
*
*****************************************************************************/
s32 NCM_TEST_LINKSTATUS_006(u32 eth_no)
{
    u32 result;
    u32 u32DownBitRate;
    u32 u32UpBitRate;

    NCM_TEST_FUNC_ENTER();

    result = bsp_ncm_open(NCM_DEV_DATA_TYPE, eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* bsp_ncm_ioctl配置连接速500M/512bit*/
    u32DownBitRate = 2000*1000*1000;
    u32UpBitRate = 512;
    result = NCM_Connect(eth_no, u32DownBitRate, u32UpBitRate);
    if (0 == result)
    {
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);

    NCM_TEST_FUNC_PASS();

    return 0;
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_LINKSTATUS_007
*
* 功能描述  : 虚拟网卡速度显示测试，配置连接速512/512，报错
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  :
*
*****************************************************************************/
s32 NCM_TEST_LINKSTATUS_007(u32 eth_no)
{
    u32 result;
    u32 u32DownBitRate;
    u32 u32UpBitRate;

    NCM_TEST_FUNC_ENTER();

    result = bsp_ncm_open(NCM_DEV_DATA_TYPE, eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* bsp_ncm_ioctl配置连接速0/1000M*/
    u32DownBitRate = 512;
    u32UpBitRate = 512;
    result = NCM_Connect(eth_no, u32DownBitRate, u32UpBitRate);
    if (0 == result)
    {
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);

    NCM_TEST_FUNC_PASS();
    return 0;
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_LINKSTATUS_008
*
* 功能描述  : 网卡状态变化回调测试，注册后，禁用网卡，启用网卡，看板端打印是否正确
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  :
*
*****************************************************************************/
s32 NCM_TEST_LINKSTATUS_008(u32 eth_no)
{
    u32 result;

    NCM_TEST_FUNC_ENTER();

    NCM_TEST_Init(eth_no);

    result = bsp_ncm_open(NCM_DEV_DATA_TYPE, eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 注册网卡状态变化回调函数*/
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_REG_CONNECT_STUS_CHG_FUNC, NCM_NetStusChange);
    if(0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        return -1;
    }

    result = bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    NCM_TEST_FUNC_PASS();
    return 0;
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_IOCTLCMD_001
*
* 功能描述  : 测试IOCTL其他命令
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  :
*
*****************************************************************************/
s32 NCM_TEST_IOCTLCMD_001(u32 eth_no)
{
    u32 result;
    u32 u32Value;
    NCM_IOCTL_PKT_STATISTICS_S stPktStatcs;

    NCM_TEST_FUNC_ENTER();

	if(!bsp_usb_is_ncm_bypass_mode())
	{
		printk("run this func need enable stick modem\n");
		NCM_TEST_FUNC_FAIL(result);
        return -1;
	}

    memset(&stPktStatcs, 0x0, sizeof(NCM_IOCTL_PKT_STATISTICS_S));
    result = bsp_ncm_open(NCM_DEV_DATA_TYPE, eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 获取NCM默认发包门限个数*/
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_GET_DEFAULT_TX_MIN_NUM, &u32Value);
    if(0 != result)
    {
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    if (u32Value != MIN_NCM_TX)
    {
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(u32Value);
        return -1;
    }

    /* 获取NCM默认发包超时时间*/
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_GET_DEFAULT_TX_TIMEOUT, &u32Value);
    if(0 != result)
    {
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    if (u32Value != NCM_TX_TIMEOUT)
    {
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(u32Value);
        return -1;
    }

    /* 获取NCM默认发包字节阈值*/
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_GET_DEFAULT_TX_MAX_SIZE, &u32Value);
    if(0 != result)
    {
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 获取PS可用底软收包buffer个数最大值*/
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_GET_USED_MAX_BUFF_NUM, &u32Value);
    if(0 != result)
    {
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    if (u32Value != (NCM_RCV_BUFFER_NUM - 100))
    {
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 注册网卡状态变化回调函数*/
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_REG_CONNECT_STUS_CHG_FUNC, NCM_NetStusChange);
    if(0 != result)
    {
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 配置统计信息*/
    stPktStatcs.u32CurrentRx = 100*1000*1000;
    stPktStatcs.u32CurrentTx = 100*1000*1000;
    stPktStatcs.u32RxErrCount = 150;
    stPktStatcs.u32RxOKCount = 2000;
    stPktStatcs.u32RxOverFlowCount = 5;
    stPktStatcs.u32TxErrCount = 80;
    stPktStatcs.u32TxOKCount = 3000;
    stPktStatcs.u32TxOverFlowCount = 60;

    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_SET_PKT_STATICS, &stPktStatcs);
    if(0 != result)
    {
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
    NCM_TEST_FUNC_PASS();
    return 0;
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_IOCTLCMD_002
*
* 功能描述  : 设置NCM最小发包个数
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  :
*
*****************************************************************************/
s32 NCM_TEST_IOCTLCMD_002(u32 u32Value, u32 eth_no)
{
    u32 result;

    NCM_TEST_FUNC_ENTER();

    NCM_TEST_Init(eth_no);

    result = bsp_ncm_open(NCM_DEV_DATA_TYPE, eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 设置NCM发包门限个数*/
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_SET_TX_MIN_NUM, &u32Value);
    if(0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    result = bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    NCM_TEST_FUNC_PASS();
    return 0;
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_IOCTLCMD_003
*
* 功能描述  : 设置发包超时时间阈值
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  :
*
*****************************************************************************/
s32 NCM_TEST_IOCTLCMD_003(u32 u32Value, u32 eth_no)
{
    u32 result;

    NCM_TEST_FUNC_ENTER();

    NCM_TEST_Init(eth_no);

    result = bsp_ncm_open(NCM_DEV_DATA_TYPE, eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 设置NCM发包超时时间阈值*/
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_SET_TX_TIMEOUT, &u32Value);
    if(0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    result = bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    NCM_TEST_FUNC_PASS();
    return 0;
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_IOCTLCMD_004
*
* 功能描述  : 设置发包字节数阈值
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  :
*
*****************************************************************************/
s32 NCM_TEST_IOCTLCMD_004(u32 u32Value, u32 eth_no)
{
    u32 result;

    NCM_TEST_FUNC_ENTER();

    NCM_TEST_Init(eth_no);

    result = bsp_ncm_open(NCM_DEV_DATA_TYPE, eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 设置NCM发包长度门限个数*/
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_SET_TX_MAX_SIZE, &u32Value);
    if(0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    result = bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    NCM_TEST_FUNC_PASS();
    return 0;
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_IOCTLCMD_005
*
* 功能描述  : 获取NCM默认参数
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  :
*
*****************************************************************************/
s32 NCM_TEST_IOCTLCMD_005(u32 eth_no)
{
    return NCM_IoctlGetDefaultParam(eth_no);
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_IOCTLCMD_006
*
* 功能描述  : 读取当前配置NCM的发包阈值和超时时间，看和配置值是否相同
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  :
*
*****************************************************************************/
s32 NCM_TEST_IOCTLCMD_006(u32 eth_no)
{
    u32 result;
    u32 u32Value;
    u32 u32GetValue;

    NCM_TEST_FUNC_ENTER();

    NCM_TEST_Init(eth_no);
    NCM_IoctlGetDefaultParam(eth_no);

    result = bsp_ncm_open(NCM_DEV_DATA_TYPE, eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 配置发包门限为1525*/
    u32Value = 1525;
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_SET_TX_MIN_NUM,&u32Value) ;
    if(0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        return -1;
    }

    /* 读取配置值看是否为之前配置值*/
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_GET_CUR_TX_MIN_NUM,&u32GetValue) ;
    if(0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        return -1;
    }

    if(u32GetValue != u32Value)
    {
        NCM_TEST_FUNC_FAIL(result);
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        return -1;
    }


    /* 配置发包超时时间为1525*/
    u32Value = 35;
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_SET_TX_TIMEOUT,&u32Value) ;
    if(0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        return -1;
    }

    /* 读取配置值看是否为之前配置值*/
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_GET_CUR_TX_TIMEOUT,&u32GetValue) ;
    if(0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        return -1;
    }

    if(u32GetValue != u32Value)
    {
        NCM_TEST_FUNC_FAIL(result);
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        return -1;
    }

    result = bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    NCM_IoctlSetDefaultParam(eth_no);

    NCM_TEST_FUNC_PASS();
    return 0;
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_FLOWCTRL_001
*
* 功能描述  : 获取NCM流控状态
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  :
*
*****************************************************************************/
s32 NCM_TEST_FLOWCTRL_001(u32 eth_no)
{
    u32 result;
    u32 u32FlowCtrl = 0;
    u32 u32FlowStus = 0xffffffff;

    NCM_TEST_FUNC_ENTER();

    result = bsp_ncm_open(NCM_DEV_DATA_TYPE, eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 配置流控状态 */
    u32FlowCtrl = NCM_IOCTL_FLOW_CTRL_ENABLE;
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_FLOW_CTRL_NOTIF, &u32FlowCtrl);
    if(0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 获取流控状态*/
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_GET_FLOWCTRL_STATUS, &u32FlowStus);
    if(0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    if (NCM_IOCTL_FLOW_CTRL_ENABLE != u32FlowStus)
    {
        NCM_TEST_FUNC_FAIL(u32FlowStus);
        return -1;
    }

    /* 配置流控状态 */
    u32FlowCtrl = NCM_IOCTL_FLOW_CTRL_DISABLE;
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_FLOW_CTRL_NOTIF, &u32FlowCtrl);
    if(0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 获取流控状态*/
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_GET_FLOWCTRL_STATUS, &u32FlowStus);
    if(0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    if (NCM_IOCTL_FLOW_CTRL_DISABLE != u32FlowStus)
    {
        NCM_TEST_FUNC_FAIL(u32FlowStus);
        return -1;
    }

    result = bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    NCM_TEST_FUNC_PASS();
    return 0;
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_FLOWCTRL_002
*
* 功能描述  : 单板启动后获取NCM流控状态
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  :
*
*****************************************************************************/
s32 NCM_TEST_FLOWCTRL_002(u32 eth_no)
{
    u32 result;
    u32 u32FlowStus = 0xffffffff;

    NCM_TEST_FUNC_ENTER();

    result = bsp_ncm_open(NCM_DEV_DATA_TYPE, eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 获取流控状态*/
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_GET_FLOWCTRL_STATUS, &u32FlowStus);
    if(0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    if (NCM_IOCTL_FLOW_CTRL_DISABLE != u32FlowStus)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    result = bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    NCM_TEST_FUNC_PASS();
    return 0;
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_FLOWCTRL_003
*
* 功能描述  : 配置流控后配置网卡状态断开再连接，查询流控状态
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  :
*
*****************************************************************************/
s32 NCM_TEST_FLOWCTRL_003(u32 eth_no)
{
    u32 result;
    u32 u32FlowCtrl = 0;
    u32 u32FlowStus = 0xffffffff;
    u32 u32LinkStus = NCM_IOCTL_CONNECTION_LINKDOWN;

    NCM_TEST_FUNC_ENTER();

    result = bsp_ncm_open(NCM_DEV_DATA_TYPE, eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 配置流控状态 */
    u32FlowCtrl = NCM_IOCTL_FLOW_CTRL_ENABLE;
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_FLOW_CTRL_NOTIF, &u32FlowCtrl);
    if(0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    u32LinkStus = NCM_IOCTL_CONNECTION_LINKDOWN;
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_NETWORK_CONNECTION_NOTIF, &u32LinkStus);
    if(0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        return -1;
    }

    u32LinkStus = NCM_IOCTL_CONNECTION_LINKUP;
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_NETWORK_CONNECTION_NOTIF, &u32LinkStus);
    if(0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        return -1;
    }

    /* 获取流控状态*/
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_GET_FLOWCTRL_STATUS, &u32FlowStus);
    if(0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    if (NCM_IOCTL_FLOW_CTRL_ENABLE != u32FlowStus)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    /* 配置流控状态 */
    u32FlowCtrl = NCM_IOCTL_FLOW_CTRL_DISABLE;
    result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_FLOW_CTRL_NOTIF, &u32FlowCtrl);
    if(0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    result = bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
    if (0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    NCM_TEST_FUNC_PASS();
    return 0;
}


/*****************************************************************************
* 函 数 名  : NCM_TEST_PING_START
*
* 功能描述  : ping测试，只支持从PC ping UE，UE ip默认为192.168.0.3
*             测试完后需执行NCM_LOOPOrPingStop停止测试，回收资源
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
s32 NCM_TEST_PING_START(void)
{
    u32 loop;
    s32 ret;


	if(!bsp_usb_is_ncm_bypass_mode())
	{
		printk("run this func need enable stick modem\n");
		NCM_TEST_FUNC_FAIL(result);
        return -1;
	}
	
    ncm_bypass_udi = true;

    for(loop=0; loop<GNET_USED_NUM; loop++)
    {
        ret = NCM_DataStart((void *)NCM_PING, (void *)NCM_FreePingBuf, loop);
        if(ret)
        {
            printk("%s line %d:NCM_DataStart failed\n",__FUNCTION__,__LINE__);
            break;
        }
    }
    return ret;
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_PING_STOP
*
* 功能描述  : ping和环回测试停止函数，需先停止从pc ping包和发包，再执行此函数
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
s32 NCM_TEST_PING_STOP(void)
{
    u32 loop;
    s32 ret;

	if(!bsp_usb_is_ncm_bypass_mode())
	{
		printk("run this func need enable stick modem\n");
		NCM_TEST_FUNC_FAIL(result);
        return -1;
	}

    for (loop=0; loop<GNET_USED_NUM; loop++) {

        ret = NCM_DataStop(loop);
        if(ret){
            printk("%s line %d:NCM_DataStopfailed\n",__FUNCTION__,__LINE__);
            break;
        }
    }

    ncm_bypass_udi = false;

    return ret;
}

/*****************************************************************************
* 函 数 名  : NCM_TEST_LOOP_START
*
* 功能描述  : 环回测试，先执行此函数，然后执行PC端路由绑定脚本，然后使用iperf发包
*             测试完后需执行NCM_TEST_LOOP_STOP停止测试，回收资源
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
s32 NCM_TEST_LOOP_START(void)
{
    u32 loop;
    s32 ret;

	if(!bsp_usb_is_ncm_bypass_mode())
	{
		printk("run this func need enable stick modem\n");
		NCM_TEST_FUNC_FAIL(result);
        return -1;
	}

    ncm_bypass_udi = true;

    for(loop=0; loop<GNET_USED_NUM; loop++)
    {
        ret = NCM_DataStart((void *)NCM_LOOP, (void *)NCM_FreeLoopBuf, loop);
        if(ret)
        {
            printk("%s line %d:NCM_DataStart failed\n",__FUNCTION__,__LINE__);
            break;
        }
    }
    return ret;

}
/*****************************************************************************
* 函 数 名  : NCM_TEST_LOOP_STOP
*
* 功能描述  : 环回测试停止函数，需先停止从pc ping包和发包，再执行此函数
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
s32 NCM_TEST_LOOP_STOP(void)
{
    u32 loop;
    s32 ret;

	if(!bsp_usb_is_ncm_bypass_mode())
	{
		printk("run this func need enable stick modem\n");
		NCM_TEST_FUNC_FAIL(result);
        return -1;
	}

    for (loop=0; loop<GNET_USED_NUM; loop++) {

        ret = NCM_DataStop(loop);
        if(ret){
            printk("%s line %d:NCM_DataStopfailed\n",__FUNCTION__,__LINE__);
            break;
        }
    }

    ncm_bypass_udi = false;

    return ret;
}
/*****************************************************************************
* 函 数 名  : NCM_IoctlGetDefaultParam
*
* 功能描述  : 获取NCM默认参数
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  :
*
*****************************************************************************/
s32 NCM_IoctlGetDefaultParam(u32 eth_no)
{
    u32 result;

    memset(g_stNcmDefaultParam, 0x0,sizeof(g_stNcmDefaultParam));

        result = bsp_ncm_open(NCM_DEV_DATA_TYPE, eth_no);
        if (0 != result)
        {
            NCM_TEST_FUNC_FAIL(result);
            return -1;
        }

        result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_GET_USED_MAX_BUFF_NUM,
            &g_stNcmDefaultParam[eth_no].u32PsUsedMaxBufNum);
        if(0 != result)
        {
            NCM_TEST_FUNC_FAIL(result);
            bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
            return -1;
        }


        result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_GET_DEFAULT_TX_MIN_NUM,
            &g_stNcmDefaultParam[eth_no].u32DefaultTxMinNum);
        if(0 != result)
        {
            bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
            NCM_TEST_FUNC_FAIL(result);
            return -1;
        }

        result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_GET_DEFAULT_TX_TIMEOUT,
            &g_stNcmDefaultParam[eth_no].u32DefaultTxTimeOut);
        if(0 != result)
        {
            NCM_TEST_FUNC_FAIL(result);
            bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
            return -1;
        }

        result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_GET_DEFAULT_TX_MAX_SIZE,
            &g_stNcmDefaultParam[eth_no].u32DeafultTxMaxSize);
        if(0 != result)
        {
            bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
            NCM_TEST_FUNC_FAIL(result);
            return -1;
        }

        result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_GET_RX_BUF_SIZE,
            &g_stNcmDefaultParam[eth_no].u32RxBufSize);
        if(0 != result)
        {
            bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
            NCM_TEST_FUNC_FAIL(result);
            return -1;
        }

        result = bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        if(0 != result)
        {
            NCM_TEST_FUNC_FAIL(result);
            return -1;
        }

    return 0;
}

/*****************************************************************************
* 函 数 名  : NCM_IoctlSetDefaultParam
*
* 功能描述  : 配置发包默认参数
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  :
*
*****************************************************************************/
s32 NCM_IoctlSetDefaultParam(u32 eth_no)
{
    u32 result;

        result = bsp_ncm_open(NCM_DEV_DATA_TYPE, eth_no);
        if (0 != result)
        {
            NCM_TEST_FUNC_FAIL(result);
            return -1;
        }

        result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_SET_TX_MIN_NUM,
            &g_stNcmDefaultParam[eth_no].u32DefaultTxMinNum);
        if(0 != result)
        {
            bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
            NCM_TEST_FUNC_FAIL(result);
            return -1;
        }

        result = bsp_ncm_ioctl(eth_no, NCM_IOCTL_SET_TX_TIMEOUT,
            &g_stNcmDefaultParam[eth_no].u32DefaultTxTimeOut);
        if(0 != result)
        {
            bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
            NCM_TEST_FUNC_FAIL(result);
            return -1;
        }

        result = bsp_ncm_close(NCM_DEV_DATA_TYPE, eth_no);
        if(0 != result)
        {
            NCM_TEST_FUNC_FAIL(result);
            return -1;
        }

    return 0;
}

/*****************************************************************************
* 函 数 名  : NCM_IoctlSetTxMinNum
*
* 功能描述  : 配置发包默认参数
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 0&-1
*
* 其它说明  :
*
*****************************************************************************/
s32 NCM_IoctlSetTxMinNum(u32 u32Value)
{
    u32 result;

    result = bsp_ncm_ioctl(g_u32NcmDevId, NCM_IOCTL_SET_TX_MIN_NUM,
        &u32Value);
    if(0 != result)
    {
        NCM_TEST_FUNC_FAIL(result);
        return -1;
    }

    return 0;
}

#endif /* DRV_BUILD_SEPARATE */

