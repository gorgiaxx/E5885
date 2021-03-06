/* Copyright (C) Huawei Technologies Co., Ltd.
 *   2013-05-17 - add for perf tool and acm csp. wangxinli <foss@huawei.com>
 *   The contribution by Huawei Technologies is under GPLv2
 */
#ifndef _ARM_HW_BREAKPOINT_H
#define _ARM_HW_BREAKPOINT_H

#ifdef __KERNEL__

struct task_struct;

#ifdef CONFIG_HAVE_HW_BREAKPOINT

struct arch_hw_breakpoint_ctrl {
#ifdef CONFIG_HISI_WATCHPOINT
		u32 __reserved	: 3,
        addr_range  : 5,
        __reserved1 : 1,
#else
		u32 __reserved	: 9,
#endif
		mismatch	: 1,
#ifdef CONFIG_HISI_WATCHPOINT
                    :6,
        scope       :2,
        __reserved2 : 1,
#else
				: 9,
#endif
		len		: 8,
		type		: 2,
		privilege	: 2,
		enabled		: 1;
};

struct arch_hw_breakpoint {
	u32	address;
	u32	trigger;
	struct	arch_hw_breakpoint_ctrl step_ctrl;
	struct	arch_hw_breakpoint_ctrl ctrl;
#ifdef CONFIG_HISI_WATCHPOINT
    u32 access;             /*watchpoint ddr access type*/
#endif

#ifdef CONFIG_BALONG_PERF_MONITOR
    /* for v7r2 perf watchpoint/breakpoint */
    u32 dummy_watch;                    /* watch point dummy status */
	u32 dummy_break;                    /* break point dummy status */
	u32 watch_value;                    /* watch point match value */
	u32 watch_mask;                     /* watch point match mask */
#endif
};

static inline u32 encode_ctrl_reg(struct arch_hw_breakpoint_ctrl ctrl)
{
#ifdef CONFIG_HISI_WATCHPOINT
	return ctrl.addr_range << 24 |(ctrl.mismatch << 22) | (ctrl.scope << 14) |(ctrl.len << 5) | (ctrl.type << 3) |
		(ctrl.privilege << 1) | ctrl.enabled;
#else
	return (ctrl.mismatch << 22) | (ctrl.len << 5) | (ctrl.type << 3) |
		(ctrl.privilege << 1) | ctrl.enabled;
#endif
}

static inline void decode_ctrl_reg(u32 reg,
				   struct arch_hw_breakpoint_ctrl *ctrl)
{
#ifdef CONFIG_HISI_WATCHPOINT
	ctrl->enabled	= reg & 0x1;
	reg >>= 1;
	ctrl->privilege	= reg & 0x3;
	reg >>= 2;
	ctrl->type	= reg & 0x3;
	reg >>= 2;
	ctrl->len	= reg & 0xff;
    reg >>= 9;
    ctrl->scope = reg &0x3;
	reg >>= 8;
	ctrl->mismatch	= reg & 0x1;
    reg >>= 2;
    ctrl->addr_range = reg & 0x1f;
#else
	ctrl->enabled	= reg & 0x1;
	reg >>= 1;
	ctrl->privilege	= reg & 0x3;
	reg >>= 2;
	ctrl->type	= reg & 0x3;
	reg >>= 2;
	ctrl->len	= reg & 0xff;
	reg >>= 17;
	ctrl->mismatch	= reg & 0x1;
#endif
}

/* Debug architecture numbers. */
#define ARM_DEBUG_ARCH_RESERVED	0	/* In case of ptrace ABI updates. */
#define ARM_DEBUG_ARCH_V6	1
#define ARM_DEBUG_ARCH_V6_1	2
#define ARM_DEBUG_ARCH_V7_ECP14	3
#define ARM_DEBUG_ARCH_V7_MM	4
#define ARM_DEBUG_ARCH_V7_1	5

/* Breakpoint */
#define ARM_BREAKPOINT_EXECUTE	0

/* Watchpoints */
#define ARM_BREAKPOINT_LOAD	1
#define ARM_BREAKPOINT_STORE	2
#define ARM_FSR_ACCESS_MASK	(1 << 11)

/* Privilege Levels */
#define ARM_BREAKPOINT_PRIV	1
#define ARM_BREAKPOINT_USER	2

/* Lengths */
#define ARM_BREAKPOINT_LEN_1	0x1
#define ARM_BREAKPOINT_LEN_2	0x3
#define ARM_BREAKPOINT_LEN_4	0xf
#define ARM_BREAKPOINT_LEN_8	0xff

/* Limits */
#define ARM_MAX_BRP		16
#define ARM_MAX_WRP		16
#define ARM_MAX_HBP_SLOTS	(ARM_MAX_BRP + ARM_MAX_WRP)

/* DSCR method of entry bits. */
#define ARM_DSCR_MOE(x)			((x >> 2) & 0xf)
#define ARM_ENTRY_BREAKPOINT		0x1
#define ARM_ENTRY_ASYNC_WATCHPOINT	0x2
#define ARM_ENTRY_SYNC_WATCHPOINT	0xa

/* DSCR monitor/halting bits. */
#define ARM_DSCR_HDBGEN		(1 << 14)
#define ARM_DSCR_MDBGEN		(1 << 15)

/* OSLSR os lock model bits */
#define ARM_OSLSR_OSLM0		(1 << 0)

/* opcode2 numbers for the co-processor instructions. */
#define ARM_OP2_BVR		4
#define ARM_OP2_BCR		5
#define ARM_OP2_WVR		6
#define ARM_OP2_WCR		7

/* Base register numbers for the debug registers. */
#define ARM_BASE_BVR		64
#define ARM_BASE_BCR		80
#define ARM_BASE_WVR		96
#define ARM_BASE_WCR		112

/* Accessor macros for the debug registers. */
#define ARM_DBG_READ(N, M, OP2, VAL) do {\
	asm volatile("mrc p14, 0, %0, " #N "," #M ", " #OP2 : "=r" (VAL));\
} while (0)

#define ARM_DBG_WRITE(N, M, OP2, VAL) do {\
	asm volatile("mcr p14, 0, %0, " #N "," #M ", " #OP2 : : "r" (VAL));\
} while (0)

struct notifier_block;
struct perf_event;
struct pmu;

extern struct pmu perf_ops_bp;
extern int arch_bp_generic_fields(struct arch_hw_breakpoint_ctrl ctrl,
				  int *gen_len, int *gen_type);
extern int arch_check_bp_in_kernelspace(struct perf_event *bp);
extern int arch_validate_hwbkpt_settings(struct perf_event *bp);
extern int hw_breakpoint_exceptions_notify(struct notifier_block *unused,
					   unsigned long val, void *data);

extern u8 arch_get_debug_arch(void);
extern u8 arch_get_max_wp_len(void);
extern void clear_ptrace_hw_breakpoint(struct task_struct *tsk);

int arch_install_hw_breakpoint(struct perf_event *bp);
void arch_uninstall_hw_breakpoint(struct perf_event *bp);
void hw_breakpoint_pmu_read(struct perf_event *bp);
int hw_breakpoint_slots(int type);

#ifdef CONFIG_BALONG_PERF_MONITOR
/*
 * perf hw breakpoint/watchpoint handler for V7R2 w65225
 */
int arch_breakpoint_vendor_handler(struct perf_event *bp, struct pt_regs *regs);
void arch_watchpoint_vendor_handler(struct pt_regs *regs);
int arch_watchpoint_set_mask(u32 value, u32 mask, u32 wp_idx);
#endif

#else
static inline void clear_ptrace_hw_breakpoint(struct task_struct *tsk) {}

#endif	/* CONFIG_HAVE_HW_BREAKPOINT */
#endif	/* __KERNEL__ */
#endif	/* _ARM_HW_BREAKPOINT_H */
