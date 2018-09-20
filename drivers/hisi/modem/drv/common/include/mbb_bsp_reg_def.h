/*��˼Ǩ����ɾ��bsp_reg_def.h�ļ�����������ģ���޷�����
���Ӹ��ļ�����Ϊ��ԭbsp_reg_def.hһ�£�����GPIO���ù�ϵ��Ҫ���ø��ļ�
*/




#ifndef __MBB_BSP_REG_DEF_H__
#define __MBB_BSP_REG_DEF_H__

/*lint -e760 -e547*/
#define INREG32(x)          readl(x)
#define OUTREG32(x, y)      writel((UINT32)(y), (x))
#define SETREG32(x, y)      OUTREG32((x), INREG32(x) | (y))
#define CLRREG32(x, y)      OUTREG32((x), INREG32(x) & ~(y))
#define SETBITVALUE32(addr, mask, value)  OUTREG32((addr), (INREG32(addr) & (~(mask))) | ((value) & (mask)))
/*lint +e760 +e547*/

#endif
