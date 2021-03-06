
#ifndef _AMON_BALONG_H_
#define _AMON_BALONG_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "product_config.h"
#include "osl_types.h"
#include "osl_sem.h"
#include "mdrv_public.h"
#include "hi_amon.h"
#include "soc_interrupts.h"
#include "bsp_softtimer.h"
#include "bsp_amon.h"
#include "drv_amon.h"
#include "bsp_memmap.h"

#include "bsp_edma.h"
#include "bsp_version.h"
#include "amon_balong.h"
#include "drv_nv_def.h"


/**************************************************************************
                                   宏定义
**************************************************************************/
#define AXI_INT_VECTOR                  INT_LVL_AMON                /* 中断向量 */
#define AXI_WAIT_CNT                    1000                        /* 等待软复位或者启停操作完成尝试次数 */
#define AXI_MAX_PORT                    AXI_MAX_PORT_CNT            /* 最大监控端口个数 */
#define AXI_MAX_ID                      AXI_MAX_CONFIG_ID           /* 最大监控ID数目 */
#define AXI_CAPT_THRESHOLD              (50*1024)                   /* EDMA传输长度 */
#define AXI_EDMA_BINDEX                 128                         /* EDMA二维传输步长 */
#define SOC_EDMA_REQUEST                EDMA_AMON_SOC               /* SOC EDMA外设请求号 */
#define CPUFAST_EDMA_REQUEST            EDMA_AMON_CPUFAST           /* CPUFAST EDMA外设请求号 */
#define AXI_SOCP_CHAN_ID                SOCP_CODER_SRC_AXI_MONITOR  /* MONITOR使用的SOCP源通道ID */
#define SOCP_BD_PACKET_SIZE             8                           /* SOCP BD包大小 */
#define SOCP_BD_BUF_SIZE                (SOCP_BD_PACKET_SIZE*1024)  /* SOCP BD buffer大小 */
#define SOCP_RD_PACKET_SIZE             8                           /* SOCP RD包大小 */
#define SOCP_RD_BUF_SIZE                (SOCP_RD_PACKET_SIZE*1024)  /* SOCP RD buffer大小 */
#define AXI_SOCP_PACKET_SIZE            (2*1024)                    /* 每2k进行SOCP组包 */
#define AXI_SOCP_TRAN_WAIT_CNT          5                           /* 采集结束，等待SOCP传输完成最大次数 */
#define AXI_CAPT_TRAN_OVER_TIME         500                         /* 采集传输完成最大等待时间 */
#define AXI_CAPT_CNF_REG_CNT            17                          /* 采集配置命令回复需要上报寄存器的数目 */
#define AXI_MON_CNF_REG_CNT             35                          /* 监控配置命令回复需要上报寄存器的数目 */
#define AXI_CAPT_CYCLE_DEFAULT          2000                        /* 采集周期默认值 */
#define AXI_SOFT_RESET                  0x6                         /* AXI monitor软复位 */
#define AXI_RESET_TO_IDLE               0x1                         /* AXI monitor强制进入IDLE态 */
#define AXI_RUN_STATE_MASK              0xC                         /* AXI monitor运行状态 */
#define AXI_RESET_STATE_MASK            0x10                        /* AXI monitor软复位状态 */
#define AXI_FIFO_DEPTH                  32                          /* Monitor FIFO深度 */
#define AMON_MAIN_VERSION               1
#define AMON_MINOR_VERSION              1

/**************************************************************************
                                 寄存器定义
**************************************************************************/

/* AXI monitor寄存器偏移地址定义 begin */
#define AXI_MON_CNT_RESET               HI_AMON_CNT_RESET_OFFSET                /* AXI monitor统计计数器软复位寄存器  */
#define AXI_MON_CNT_CTRL                HI_AMON_CNT_CTRL_OFFSET                 /* AXI monitor系统配置寄存器 */
#define AXI_MON_CNT_STATE_INT           HI_AMON_CNT_STATE_INT_OFFSET            /* AXI monitor系统状态寄存器 */
#define AXI_MON_WIN_COUNTER             HI_AMON_WIN_COUNTER_OFFSET              /* AXI monitor系统时间窗计数器 */
#define AXI_MON_IDLE_COUNTER            HI_AMON_IDLE_COUNTER_OFFSET             /* AXI monitor系统idle统计计数器 */
#define AXI_MON_PORT_SEL                HI_AMON_PORT_SEL_OFFSET                 /* AXI monitor监控ID的AXI port配置 */
#define AXI_MON_INT_CLR                 HI_AMON_INT_CLR_OFFSET                  /* AXI monitor中断清除寄存器 */
#define AXI_ID_INT_SRC                  HI_AMON_ID_INT_SRC_OFFSET               /* ID读写监控中断原始寄存器 */
#define AXI_ID_INT_MASK                 HI_AMON_ID_INT_MASK_OFFSET              /* ID读写监控中断屏蔽寄存器 */
#define AXI_ID_INT_STAT                 HI_AMON_ID_INT_STAT_OFFSET              /* ID读写监控中断状态寄存器 */

/* n为port，取值范围0-7 */
#define AXI_MON_RD_WAIT_CYCLE_PORTS(n)  (HI_AMON_RD_WAIT_CYCLE_PORTS_0_OFFSET + n*0x10)   /* 监控PORTs为精确定位读等待计数而消耗的时间周期寄存器 */
#define AXI_MON_WR_WAIT_CYCLE_PORTS(n)  (HI_AMON_WR_WAIT_CYCLE_PORTS_0_OFFSET + n*0x10)   /* 监控PORTs为精确定位写等待计数而消耗的时间周期寄存器 */
/* n为ID, 取值范围0-7 */
#define AXI_MON_CNT_TYPE(n)             (HI_AMON_CNT_TYPE_0_OFFSET + n*0x4)         /* 监控ID的监控类型选择 */
#define AXI_MON_CNT_ID(n)               (HI_AMON_CNT_ID_0_OFFSET + n*0x100)         /* AXI monitor系统ID信息寄存器 */
#define AXI_MON_ID_ADDR_DES(n)          (HI_AMON_ID_ADDR_DES_0_OFFSET + n*0x100)    /* 监控地址下边界 */
#define AXI_MON_ID_ADDR_DES_M(n)        (HI_AMON_ID_ADDR_DES_M_0_OFFSET + n*0x100)  /* 监控地址上边界 */

/* 统计计数器 begin */
#define AXI_MON_INCR1_ID(n)             (HI_AMON_INCR1_ID_0_OFFSET + n*0x100)               /* 监控ID n发起INCR1操作计数器 */
#define AXI_MON_INCR2_ID(n)             (HI_AMON_INCR2_ID_0_OFFSET + n*0x100)               /* 监控ID n发起INCR2操作计数器 */
#define AXI_MON_INCR4_ID(n)             (HI_AMON_INCR4_ID_0_OFFSET + n*0x100)               /* 监控ID n发起INCR4操作计数器 */
#define AXI_MON_INCR8_ID(n)             (HI_AMON_INCR8_ID_0_OFFSET + n*0x100)               /* 监控ID n发起INCR8操作计数器 */
#define AXI_MON_INCR16_ID(n)            (HI_AMON_INCR16_ID_0_OFFSET + n*0x100)              /* 监控ID n发起INCR16操作计数器 */
#define AXI_MON_WRAP_ID(n)              (HI_AMON_WRAP_ID_0_OFFSET + n*0x100)                /* 监控ID n发起WRAP操作计数器 */
#define AXI_MON_BURST_ID(n)             (HI_AMON_BURST_ID_0_OFFSET + n*0x100)               /* 监控ID n发起BURST操作计数器 */
#define AXI_MON_FINISH_ID(n)            (HI_AMON_FINISH_ID_0_OFFSET + n*0x100)              /* 监控ID n完成BURST操作计数器 */
#define AXI_MON_RD_WAIT_ID_LOW(n)       (HI_AMON_READ_WAIT_ID_0_OFFSET + n*0x100)           /* 监控ID n读等待周期计数器，低32位(64位寄存器，0~43有效) */
#define AXI_MON_RD_WAIT_ID_HIGH(n)      (HI_AMON_READ_WAIT_ID_0_OFFSET + 0x4 + n*0x100)     /* 监控ID n读等待周期计数器，高32位(64位寄存器，0~43有效) */
#define AXI_MON_WR_WAIT_ID_LOW(n)       (HI_AMON_WRITE_WAIT_ID_0_OFFSET + n*0x100)          /* 监控ID n写等待周期计数器，低32位(64位寄存器，0~43有效) */
#define AXI_MON_WR_WAIT_ID_HIGH(n)      (HI_AMON_WRITE_WAIT_ID_0_OFFSET + 0x4 + n*0x100)    /* 监控ID n写等待周期计数器，高32位(64位寄存器，0~43有效) */
#define AXI_MON_WR_WAIT_RESP_ID_LOW(n)  (HI_AMON_WRITE_WAIT_RESP_ID_0_OFFSET + n*0x100)     /* 监控ID n写响应等待周期计数器，低32位(64位寄存器，0~43有效) */
#define AXI_MON_WR_WAIT_RESP_ID_HIGH(n) (HI_AMON_WRITE_WAIT_RESP_ID_0_OFFSET + 0x4 + n*0x100)/* 监控ID n写响应等待周期计数器，高32位(64位寄存器，0~43有效) */
#define AXI_MON_RD_MAX_WAIT_ID(n)       (HI_AMON_READ_MAX_WAIT_ID_0_OFFSET + n*0x100)       /* 监控ID n读最大等待周期计数器 */
#define AXI_MON_WR_MAX_WAIT_ID(n)       (HI_AMON_WRITE_MAX_WAIT_ID_0_OFFSET + n*0x100)      /* 监控ID n写最大等待周期计数器 */
#define AXI_MON_WR_MAX_WAIT_RESP_ID(n)  (HI_AMON_WRITE_MAX_WAIT_RESP_ID_0_OFFSET + n*0x100) /* 监控ID n写响应最大等待周期计数器 */
#define AXI_MON_RD_BYTES_ID_LOW(n)      (HI_AMON_READ_BYTES_ID_0_OFFSET + n*0x100)          /* 监控ID n读数据总量，byte为单位，低32位(64位寄存器，0~39有效) */
#define AXI_MON_RD_BYTES_ID_HIGH(n)     (HI_AMON_READ_BYTES_ID_0_OFFSET + 0x4 + n*0x100)    /* 监控ID n读数据总量，byte为单位，高32位(64位寄存器，0~39有效) */
#define AXI_MON_WR_BYTES_ID_LOW(n)      (HI_AMON_WRITE_BYTES_ID_0_OFFSET + n*0x100)         /* 监控ID n写数据总量，byte为单位，低32位(64位寄存器，0~39有效) */
#define AXI_MON_WR_BYTES_ID_HIGH(n)     (HI_AMON_WRITE_BYTES_ID_0_OFFSET + 0x4 + n*0x100)   /* 监控ID n写数据总量，byte为单位，高32位(64位寄存器，0~39有效) */
/* 统计计数器 end */

/* 采集寄存器 begin */
#define AXI_CAPT_CTRL                   (HI_AMON_CAPT_CTRL_OFFSET)              /* 控制数据采集开始和结束 */
#define AXI_CAPT_ID_EN                  (HI_AMON_CAPT_ID_EN_OFFSET)             /* 采数使能寄存器 */
#define AXI_CAPT_TRAN_CONFIG            (HI_AMON_CAPT_CONFIG_OFFSET)            /* 采集传输配置寄存器 */
#define AXI_CAPT_ID(id)                 (HI_AMON_CAPT_ID_0_OFFSET + id*0x4)     /* ID信息寄存器 */
#define AXI_CAPT_INT_SRC                (HI_AMON_CAPT_INT_SRC_OFFSET)           /* 采集原始中断寄存器 */
#define AXI_CAPT_INT_MASK               (HI_AMON_CAPT_INT_MASK_OFFSET)          /* 采集中断屏蔽寄存器 */
#define AXI_CAPT_INT_STATE              (HI_AMON_CAPT_INT_STATE_OFFSET)         /* 采集中断状态寄存器 */
#define AXI_CAPT_INT_CLR                (HI_AMON_CAPT_INT_CLR_OFFSET)           /* 采集中断清除寄存器 */
#define AXI_CAPT_ERR_STATE              (HI_AMON_AXI_STATE_OFFSET)              /* 读写错误指示状态寄存器 */
#define AXI_CAPT_TRANS_WAIT_TIME        (HI_AMON_CAPT_TRANS_WAIT_TIME_OFFSET)   /* 数据搬移请求中断上报等待时长寄存器 */
#define AXI_CAPT_TRANS_DATA_CNT         (HI_AMON_TRANS_DATA_CNT_OFFSET)         /* 数据搬移量统计寄存器, 存放低11位以及进位标志 */
#define AXI_CAPT_TRANS_DATA_CNT_HIGH    (HI_AMON_TRANS_DATA_CNT_HIGH_OFFSET)    /* 数据搬移量统计寄存器, 存放高20位 */
#define AXI_CAPT_TRANS_DATA_LEVEL       (HI_AMON_TRANS_DATA_LEVEL_OFFSET)       /* 数据搬移阈值配置寄存器 */
#define AXI_CAPT_CMD_PKG_SIZE           (HI_AMON_CMD_PKG_SIZE_OFFSET)           /* 命令包长度寄存器 */
#define AXI_CAPT_DATA_PKG_SIZE          (HI_AMON_DATA1_PKG_SIZE_OFFSET)         /* 数据包长度寄存器 */
#define AXI_CAPT_EXT_PKG_SIZE           (HI_AMON_DATA2_PKG_SIZE_OFFSET)         /* 附加包长度寄存器 */
#define AXI_CAPT_DMA_REQ_TYPE           (HI_AMON_DMA_REQ_EN_OFFSET)             /* DMA外设请求寄存器 */
#define AXI_CAPT_HEAD_FLAG              (0x960)                                 /* 包头标志寄存器 */
#define AXI_CAPT_FIFO_ADDR              (0xa00)                                 /* Monitor FIFO地址 */
/* 采集寄存器 end */

/* AXI monitor寄存器偏移地址定义 end */

/**************************************************************************
                               数据结构定义
**************************************************************************/


/* 时间窗配置 */
enum axi_win_config_enum
{
    AXI_WIN_DISABLE,            /* 时间窗不使能 */
    AXI_WIN_ENABLE,             /* 时间窗使能 */
    AXI_WIN_BUTT
};
typedef unsigned char axi_win_config_enum_uint8;

/* 获取AXI monitor状态请求 */
typedef enum axi_get_state_req_enum
{
    AXI_GET_RUN_STATE_REQ,      /* 获取运行状态 */
    AXI_GET_RESET_STATE_REQ,    /* 获取软复位状态 */
    AXI_GET_STATE_REQ_BUTT
}AXI_REQ_STATE;

/* AXI monitor状态 */
typedef enum axi_state_enum
{
    AXI_IDLE            = 0x0,  /* IDLE状态 */
    AXI_WIN_RUNNING     = 0x4,  /* 带时间窗的监控状态 */
    AXI_UNWIN_RUNNING   = 0x8,  /* 不带时间窗的监控状态 */
    AXI_STOP            = 0xC,  /* STOP状态 */
    AXI_RESET_FINISH    = 0x10, /* 软复位完成 */
    AXI_STATE_BUTT      = 0xFFFFFFFF
}AXI_STATE;

/* 采集状态切换 */
enum axi_capt_full_state_enum
{
    AXI_CAPT_NORMAL             = 0x1,      /* 正常状态 */
    AXI_CAPT_FULL               = 0x2,      /* buffer满 */
    AXI_CAPT_FINISHING          = 0x4,      /* 等待传输完成 */
    AXI_CAPT_STOP_REQ           = 0x8,      /* stop请求 */
    AXI_CAPT_PERIOD_STOP_REQ    = 0x10,     /* 周期采集stop请求 */
    AXI_CAPT_BUTT                           /* 无效值 */
};

/* 采集ID配置 */
typedef struct
{
	u32 	id_en;				/* 使能 */
	u32 	id_value;			/* ID */
	u32 	mask;				/* 掩码 */
} axi_capt_id_config_t;

/* 采集配置请求， 正常模式 */
typedef struct
{
    u32 	mode;			    /* 区分Monitor工作模式，0:CPUFAST 1:SOC */
	u32 	port;	    		/* 监控端口 */
	u32 	op_type;			/* 操作类型，0:读操作 1:写操作 */
	u32 	dma_req_level;	    /* DMA请求发出级别 */
	u32 	cmd_pkg_len;		/* 命令包长度 */
	u32 	data_pkg_len;		/* 数据包长度 */
	u32 	ext_pkg_len;		/* 附加包长度 */
	u32 	data_trans_thr;		/* 数据传输阈值 */
	u32 	dma_req_type;		/* DMA请求类型 */
    u32 	capt_cycle;         /* 采集周期 */
	axi_capt_id_config_t id_cfg[AXI_MAX_ID];	/* 采集ID配置 */
} axi_capt_config_t;

/* 寄存器配置 */
typedef struct
{
	u32 	reg_offset;		    /* 寄存器偏移 */
	u32 	reg_value;			/* 寄存器值 */
} reg_config_t;

/* 采集、监控配置请求，寄存器配置模式 */
typedef struct
{
	u32 	        mode;	    /* 区分Monitor工作模式，0:CPUFAST 1:SOC */
    u32             capt_cycle; /* 采集周期 */
	u32 	        reg_cnt;	/* 寄存器配置数目 */
	reg_config_t    reg_cfg[];  /* 寄存器配置 */
} axi_reg_config_t;

/* 控制命令类型 */
enum axi_ctrl_type_enum
{
    AXI_CTRL_START,             /* 启动 */
    AXI_CTRL_STOP,              /* 停止 */
    AXI_CTRL_EXPORT,            /* 导出 */
    AXI_CTRL_BUTT               /* 无效值 */
};
typedef unsigned int   axi_ctrl_type_enum_uint32;



/* 控制命令请求：启动、停止、导出 */
typedef struct
{
	u32 	                    mode;		/* 区分Monitor工作模式，0:CPUFAST 1:SOC */
	axi_ctrl_type_enum_uint32 	ctrl_type;	/* 控制命令类型，0:启动 1:停止 2:导出 */
} axi_ctrl_t;

/* 监控操作类型配置 */
typedef struct
{
    u32    incr_1:1;   /* INCR操作统计类型: \
                          0 - INCR1读操作发出数量\
                          1 - INCR1写操作发出数量*/
    u32    incr_2:1;   /* INCR操作统计类型: \
                          0 - INCR2读操作发出数量\
                          1 - INCR2写操作发出数量*/
    u32    incr_4:1;   /* INCR操作统计类型: \
                          0 - INCR4读操作发出数量\
                          1 - INCR4写操作发出数量*/
    u32    incr_8:1;   /* INCR操作统计类型: \
                          0 - INCR8读操作发出数量\
                          1 - INCR8写操作发出数量*/
    u32    incr_16:1;  /* INCR操作统计类型: \
                          0 - INCR16读操作发出数量\
                          1 - INCR16写操作发出数量*/
    u32    wrap:2;     /* WRAP操作统计类型: \
                          00 - WRAP4读操作发出数量\
                          01 - WRAP4写操作发出数量\
                          10 - WRAP8读操作发出数量\
                          11 - WRAP8写操作发出数量*/
    u32    bur_send:1; /* burst发出统计类型: \
                          0 - 统计burst读操作发出数量\
                          1 - 统计burst写操作发出数量*/
    u32    but_fin:1;  /* burst完成统计类型: \
                          0 - 统计burst读操作完成数量\
                          1 - 统计burst写操作完成数量*/
    u32    wr_wait:1;  /* 写等待操作类型: \
                          0 - 统计写操作到写第一个数据的延迟\
                          1 - 统计写操作到写最后一个数据的延迟*/
    u32    reserved:22;
} axi_opt_type_t;

/* 监控ID配置 */
typedef struct
{
	u32             id_en;			/* ID使能 */
	u32 	        port;			/* 监控端口 */
    u32             id_mon_en;      /* 监控master ID使能 */
	u32 	        id_value;		/* ID */
	u32 	        mask;			/* 掩码 */
	u32 	        addr_en;		/* 地址监控使能 */
	u32 	        addr_start;	    /* 开始地址 */
	u32 	        addr_end;		/* 结束地址 */
	axi_opt_type_t	opt_type;		/* 操作类型 */
} axi_mon_id_config_t;

/* 监控配置请求， 正常模式 */
typedef struct
{
    u32 			    mode;			        /* 区分Monitor工作模式，0:CPUFAST 1:SOC */
	u32 			    win_en;			        /* 时间窗使能 0:不使能 1:使能 */
	axi_mon_id_config_t id_config[AXI_MAX_ID];  /* 采集ID配置 */
} axi_mon_config_t;

/* 中断统计 */
typedef struct
{
    u32     win_over;               /* 时间窗结束中断 */
    u32     tran_thresh;            /* 传输阈值中断 */
    u32     tran_req;               /* 传输请求中断 */
    u32     tran_over;              /* 传输结束中断 */
    u32     buff_over;              /* 缓存buffer满中断 */
    u32     edma_tran_over;         /* edma传输完成中断 */
} axi_int_stat_t;

/* 时钟控制 */
typedef struct
{
    u32                     is_clk_enable[AXI_CONFIG_BUTT]; /* 时钟是否使能 */
    struct clk *            sc_clk[AXI_CONFIG_BUTT];        /* Monitor系统时钟 */
} axi_clk_ctrl_t;

/* 循环buffer */
typedef struct
{
    u8 *    start;     /* 循环buffer头指针 */
    u8 *    end;       /* 循环buffer尾指针 */
    u8 *    read;      /* 循环buffer读指针 */
    u8 *    write;     /* 循环buffer写指针 */
} ring_buf_info_t;

/* 循环buffer空闲区域信息 */
typedef struct
{
   u8 *     p_buf_1;    /* 空闲buffer指针 */
   u32      size_1;     /* 空闲buffer大小 */
   u8 *     p_buf_2;    /* 回卷空闲buffer指针 */
   u32      size_2;     /* 回卷空闲buffer大小 */
} ring_idle_buf_t;

/* 循环buffer数据区域信息 */
typedef ring_idle_buf_t ring_data_buf_t;

/******************amon_addr_balong.c********************************/



typedef struct
{
    u32 soc_rd_cnt[AXI_MAX_CONFIG_ID];
    u32 soc_wr_cnt[AXI_MAX_CONFIG_ID];
} amon_stat_t;

typedef struct
{
    char *  buff;           /* buffer地址 */
    u32     buff_size;      /* buffer大小 */
    u32     write_offset;   /* 写指针位置 */
} amon_buff_info_t;

/* 低功耗: 寄存器备份、恢复 */
typedef struct
{
    u32     ctrl_reg;                           /* 控制寄存器 begin */
    u32     port_reg;
    u32     int_mask;
    u32     id_reg[AXI_MAX_CONFIG_ID];
    u32     addr_start_reg[AXI_MAX_CONFIG_ID];
    u32     addr_end_reg[AXI_MAX_CONFIG_ID];   /* 控制寄存器 end */
    u32     incr1_reg[AXI_MAX_CONFIG_ID];      /* 统计寄存器 begin */
    u32     incr2_reg[AXI_MAX_CONFIG_ID];
    u32     incr4_reg[AXI_MAX_CONFIG_ID];
    u32     incr8_reg[AXI_MAX_CONFIG_ID];
    u32     incr16_reg[AXI_MAX_CONFIG_ID];
    u32     wrap_reg[AXI_MAX_CONFIG_ID];
    u32     burst_send_reg[AXI_MAX_CONFIG_ID];
    u32     burst_fin_reg[AXI_MAX_CONFIG_ID];
    u32     rd_cnt_low_reg[AXI_MAX_CONFIG_ID];
    u32     rd_cnt_high_reg[AXI_MAX_CONFIG_ID];
    u32     wr_cnt_low_reg[AXI_MAX_CONFIG_ID];
    u32     wr_cnt_high_reg[AXI_MAX_CONFIG_ID];/* 统计寄存器 end */
} axi_reg_bak_t;


typedef enum{
    AMON_SOC_SRST_EN       = 0,
    AMON_SOC_SRST_DIS      = 1,
    AMON_SOC_MONITOR_START = 2,
    AMON_SOC_DTS_BUTT
}AMON_SOC_DTS_SYSCTRL;
typedef enum{
    AMON_BASE_ADDR = 0,
    AMON_START_BIT,
    AMON_END_BIT,
    AMON_ADDR_BUTT
}AMON_DTS_SYSCTRL_ADDR;

struct amon_sysctrl{
    void* base_addr;
    u32 start_bit;
    u32 end_bit;
};

typedef enum{
    AMON_BUS_STRESS_START_TIME = 0,
    AMON_BUS_STRESS_END_TIME   = 1,
    AMON_BUS_STRESS_TIME_BUTT
}AMON_BUS_STRESS_TIME;

typedef struct amon_cfg
{
    void*               base_addr;
    struct clk*         soc_clk;
    u32                 irq_num;
    struct amon_sysctrl amon_sysctrl[AMON_SOC_DTS_BUTT];
    u32                 init_flag;
}AMON_CFG_STRU;



/*bus streess start*/
#define STRESS_TIME_FREQ     (1920000)/*1.92M时钟*/

#ifdef BSP_CONFIG_PHONE_TYPE
#define MODEM_BUS_FREQ      (50)/*总线频率,50MHZ*/
#define MODEM_PORT_WITH     (MODEM_BUS_FREQ * 8 * 1024 * 1024)/*每个port的总带宽，单位bytes/s*/
#else
/*每个port的总带宽*/
#define MODEM_BUS_FREQ      (50)/*总线频率,50MHZ*/
#define MODEM_PORT_WITH     (MODEM_BUS_FREQ * 8 * 1024 * 1024)/*bytes/s*/
#endif

/*bus streess end*/




#define AMON_IS_VALID_ADDR(addr) ((addr != NULL) ? true:false)


#define amon_error(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR,   BSP_MODU_AMON, "[amon] %s:"fmt, __FUNCTION__, ##__VA_ARGS__))
#define amon_debug(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_DEBUG,   BSP_MODU_AMON, "[amon] %s:"fmt, __FUNCTION__, ##__VA_ARGS__))


/**************************************************************************
                                函数声明
**************************************************************************/
/* AXI寄存器读操作 */
void axi_reg_read(u32 reg, u32 * value);
#define AXI_REG_READ(reg, value)                axi_reg_read(  reg, value)
/* AXI寄存器写操作 */

void axi_reg_write(u32 reg, u32 value);
#define AXI_REG_WRITE(reg, value)               axi_reg_write(  reg, value)
/* AXI寄存器读位操作 */
void axi_reg_getbits(  u32 reg, u32 pos, u32 bits, u32 * value);
#define AXI_REG_GETBITS(reg, pos, bits, value)  axi_reg_getbits(  reg, pos, bits, value)
/* AXI寄存器写位操作 */
void axi_reg_setbits(u32 reg, u32 pos, u32 bits, u32 value);
#define AXI_REG_SETBITS(  reg, pos, bits, value)  axi_reg_setbits(  reg, pos, bits, value)




#ifdef __cplusplus
}
#endif

#endif /* _AMON_BALONG_H_ */
