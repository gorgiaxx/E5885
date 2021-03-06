/* 
 * Copyright (C) 2013 Realtek Semiconductor Corp.  All Rights Reserved.
 * 
 * www.realtek.com 
 * 
 * This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License version 2 and 
 * only version 2 as published by the Free Software Foundation. 
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details. 
 * 
 */ 
#include <rtl8367c_asicdrv_acl.h>

#include <linux/string.h>

#if defined(CONFIG_RTL8367C_ASICDRV_TEST)
rtl8367c_aclrulesmi Rtl8370sVirtualAclRuleTable[RTL8367C_ACLRULENO];
rtl8367c_acl_act_smi_t Rtl8370sVirtualAclActTable[RTL8367C_ACLRULENO];
#endif

void _rtl8367c_aclRuleStSmi2User(rtl8367c_aclrule *pAclUser, rtl8367c_aclrulesmi *pAclSmi);
void _rtl8367c_aclRuleStUser2Smi(rtl8367c_aclrule *pAclUser, rtl8367c_aclrulesmi *pAclSmi);
void _rtl8367c_aclActStSmi2User(rtl8367c_acl_act_t *pAclUser, rtl8367c_acl_act_smi_t *pAclSmi);
void _rtl8367c_aclActStUser2Smi(rtl8367c_acl_act_t *pAclUser, rtl8367c_acl_act_smi_t *pAclSmi);

/* Function Name:
 *      rtl8367c_setAsicAcl
 * Description:
 *      Set port acl function enable/disable
 * Input:
 *      port 	- Physical port number (0~10)
 *      enabled - 1: enabled, 0: disabled
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 		- Success
 *      RT_ERR_SMI  	- SMI access error
 *      RT_ERR_PORT_ID  - Invalid port number
 * Note:
 *      None
 */
ret_t rtl8367c_setAsicAcl(rtk_uint32 port, rtk_uint32 enabled)
{
	if(port > RTL8367C_PORTIDMAX)
		return RT_ERR_PORT_ID;

	return rtl8367c_setAsicRegBit(RTL8367C_ACL_ENABLE_REG, port, enabled);
}
/* Function Name:
 *      rtl8367c_getAsicAcl
 * Description:
 *      Get port acl function enable/disable
 * Input:
 *      port 	- Physical port number (0~10)
 *      enabled - 1: enabled, 0: disabled
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 		- Success
 *      RT_ERR_SMI  	- SMI access error
 *      RT_ERR_PORT_ID  - Invalid port number
 * Note:
 *      None
 */
ret_t rtl8367c_getAsicAcl(rtk_uint32 port, rtk_uint32* pEnabled)
{
	if(port > RTL8367C_PORTIDMAX)
		return RT_ERR_PORT_ID;

	return rtl8367c_getAsicRegBit(RTL8367C_ACL_ENABLE_REG, port, pEnabled);
}
/* Function Name:
 *      rtl8367c_setAsicAclUnmatchedPermit
 * Description:
 *      Set port acl function unmatched permit action
 * Input:
 *      port 	- Physical port number (0~10)
 *      enabled - 1: enabled, 0: disabled
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 		- Success
 *      RT_ERR_SMI  	- SMI access error
 *      RT_ERR_PORT_ID  - Invalid port number
 * Note:
 *      None
 */
ret_t rtl8367c_setAsicAclUnmatchedPermit(rtk_uint32 port, rtk_uint32 enabled)
{
	if(port > RTL8367C_PORTIDMAX)
		return RT_ERR_PORT_ID;

	return rtl8367c_setAsicRegBit(RTL8367C_ACL_UNMATCH_PERMIT_REG, port, enabled);
}
/* Function Name:
 *      rtl8367c_getAsicAclUnmatchedPermit
 * Description:
 *      Get port acl function unmatched permit action
 * Input:
 *      port 	- Physical port number (0~10)
 *      enabled - 1: enabled, 0: disabled
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 		- Success
 *      RT_ERR_SMI  	- SMI access error
 *      RT_ERR_PORT_ID  - Invalid port number
 * Note:
 *      None
 */
ret_t rtl8367c_getAsicAclUnmatchedPermit(rtk_uint32 port, rtk_uint32* pEnabled)
{
	if(port > RTL8367C_PORTIDMAX)
		return RT_ERR_PORT_ID;

	return rtl8367c_getAsicRegBit(RTL8367C_ACL_UNMATCH_PERMIT_REG, port, pEnabled);
}

/*
	Exchange structure type define with MMI and SMI
*/
void _rtl8367c_aclRuleStSmi2User( rtl8367c_aclrule *pAclUser, rtl8367c_aclrulesmi *pAclSmi)
{
    rtk_uint8 *care_ptr, *data_ptr;
    rtk_uint8 care_tmp, data_tmp;
	rtk_uint16 active_portmsk_ext_tmp;
    rtk_uint32 i;

	active_portmsk_ext_tmp = (rtk_uint16)pAclSmi->data_bits_ext.active_portmsk_ext & 0x7;
	pAclUser->data_bits.active_portmsk = ((active_portmsk_ext_tmp & 0x7) << 8) |
											(pAclSmi->data_bits.active_portmsk & 0xff);
	pAclUser->data_bits.type = pAclSmi->data_bits.type;
	pAclUser->data_bits.tag_exist = pAclSmi->data_bits.tag_exist;

    care_ptr = (rtk_uint8*)&pAclSmi->care_bits;
    data_ptr = (rtk_uint8*)&pAclSmi->data_bits;

    for ( i = 0; i < sizeof(struct acl_rule_smi_st); i++)
    {
        care_tmp = *(care_ptr + i) ^ (*(data_ptr + i));
        data_tmp = *(data_ptr + i);

        *(care_ptr + i) = care_tmp;
        *(data_ptr + i) = data_tmp;
    }

    care_ptr = (rtk_uint8*)&pAclSmi->care_bits_ext;
    data_ptr = (rtk_uint8*)&pAclSmi->data_bits_ext;
    care_tmp = (*care_ptr) ^ (*data_ptr);
    data_tmp = (*data_ptr);
    *care_ptr = care_tmp;
    *data_ptr = data_tmp;

	for(i = 0; i < RTL8367C_ACLRULEFIELDNO; i++)
		pAclUser->data_bits.field[i] = pAclSmi->data_bits.field[i];

	pAclUser->valid = pAclSmi->valid;

	active_portmsk_ext_tmp = pAclSmi->care_bits_ext.active_portmsk_ext & 0x7;
	pAclUser->care_bits.active_portmsk = ((active_portmsk_ext_tmp & 0x7) << 8) |
											pAclSmi->care_bits.active_portmsk;
	pAclUser->care_bits.type = pAclSmi->care_bits.type;
	pAclUser->care_bits.tag_exist = pAclSmi->care_bits.tag_exist;

	for(i = 0; i < RTL8367C_ACLRULEFIELDNO; i++)
		pAclUser->care_bits.field[i] = pAclSmi->care_bits.field[i];

}

/*
	Exchange structure type define with MMI and SMI
*/
void _rtl8367c_aclRuleStUser2Smi(rtl8367c_aclrule *pAclUser, rtl8367c_aclrulesmi *pAclSmi)
{
    rtk_uint8 *care_ptr, *data_ptr;
    rtk_uint8 care_tmp, data_tmp;
    rtk_uint32 i;

	pAclSmi->data_bits_ext.active_portmsk_ext = (pAclUser->data_bits.active_portmsk >> 8) & 0x7;
	pAclSmi->data_bits.active_portmsk = pAclUser->data_bits.active_portmsk & 0xff;
	pAclSmi->data_bits.type = pAclUser->data_bits.type;
	pAclSmi->data_bits.tag_exist = pAclUser->data_bits.tag_exist;

	for(i = 0;i < RTL8367C_ACLRULEFIELDNO; i++)
		pAclSmi->data_bits.field[i] = pAclUser->data_bits.field[i];

	pAclSmi->valid = pAclUser->valid;

	pAclSmi->care_bits_ext.active_portmsk_ext = (pAclUser->care_bits.active_portmsk >> 8) & 0x7;
	pAclSmi->care_bits.active_portmsk = pAclUser->care_bits.active_portmsk & 0xff;
	pAclSmi->care_bits.type = pAclUser->care_bits.type;
	pAclSmi->care_bits.tag_exist = pAclUser->care_bits.tag_exist;

	for(i = 0; i < RTL8367C_ACLRULEFIELDNO; i++)
		pAclSmi->care_bits.field[i] = pAclUser->care_bits.field[i];

    care_ptr = (rtk_uint8*)&pAclSmi->care_bits;
    data_ptr = (rtk_uint8*)&pAclSmi->data_bits;

    for ( i = 0; i < sizeof(struct acl_rule_smi_st); i++)
    {
        care_tmp = *(care_ptr + i) & ~(*(data_ptr + i));
        data_tmp = *(care_ptr + i) & *(data_ptr + i);

        *(care_ptr + i) = care_tmp;
        *(data_ptr + i) = data_tmp;
    }

	care_ptr = (rtk_uint8*)&pAclSmi->care_bits_ext;
    data_ptr = (rtk_uint8*)&pAclSmi->data_bits_ext;
	care_tmp = *care_ptr & ~(*data_ptr);
	data_tmp = *care_ptr & *data_ptr;

	*care_ptr = care_tmp;
	*data_ptr = data_tmp;
}

/*
	Exchange structure type define with MMI and SMI
*/
void _rtl8367c_aclActStSmi2User(rtl8367c_acl_act_t *pAclUser, rtl8367c_acl_act_smi_t *pAclSmi)
{
	rtk_uint16 tmp;

	pAclUser->cact = pAclSmi->cact;
	tmp = (rtk_uint16)pAclSmi->cvidx_cact_ext;
	pAclUser->cvidx_cact = (tmp << 6) | pAclSmi->cvidx_cact;

	pAclUser->sact = pAclSmi->sact;
	tmp = (rtk_uint16)pAclSmi->svidx_sact_ext;
	pAclUser->svidx_sact = (tmp << 6) | pAclSmi->svidx_sact;

	tmp = (rtk_uint16)pAclSmi->aclmeteridx_ext;
	pAclUser->aclmeteridx =(tmp << 6) | pAclSmi->aclmeteridx;

	pAclUser->fwdact = pAclSmi->fwdact;
	tmp = (rtk_uint16)pAclSmi->fwdpmask_ext & 0x7;
	pAclUser->fwdpmask = (tmp << 8) | pAclSmi->fwdpmask;

	pAclUser->priact = pAclSmi->priact;
	tmp = (rtk_uint16)pAclSmi->pridx_ext;
	pAclUser->pridx = (tmp << 6) | pAclSmi->pridx;

	pAclUser->aclint = pAclSmi->aclint;
	pAclUser->gpio_en = pAclSmi->gpio_en;
	pAclUser->gpio_pin = pAclSmi->gpio_pin;

	pAclUser->cact_ext = pAclSmi->cact_ext;
	pAclUser->tag_fmt = pAclSmi->tag_fmt;
	pAclUser->fwdact_ext = pAclSmi->fwdact_ext;
}

/*
	Exchange structure type define with MMI and SMI
*/
void _rtl8367c_aclActStUser2Smi(rtl8367c_acl_act_t *pAclUser, rtl8367c_acl_act_smi_t *pAclSmi)
{

	pAclSmi->cact = pAclUser->cact;
	pAclSmi->cvidx_cact = pAclUser->cvidx_cact & 0x3f;
	pAclSmi->cvidx_cact_ext = (pAclUser->cvidx_cact >> 6) & 0x1;

	pAclSmi->sact = pAclUser->sact;
	pAclSmi->svidx_sact = pAclUser->svidx_sact;
	pAclSmi->svidx_sact_ext = (pAclUser->svidx_sact >> 6) & 0x1;

	pAclSmi->aclmeteridx = pAclUser->aclmeteridx;
	pAclSmi->aclmeteridx_ext = (pAclUser->aclmeteridx >> 6) & 0x1;
	pAclSmi->fwdact = pAclUser->fwdact;
	pAclSmi->fwdpmask = pAclUser->fwdpmask;
	pAclSmi->fwdpmask_ext = (pAclUser->fwdpmask >> 8) & 0x7;

	pAclSmi->priact = pAclUser->priact;
	pAclSmi->pridx = pAclUser->pridx;
	pAclSmi->pridx_ext = (pAclUser->pridx >> 6) & 0x1;

	pAclSmi->aclint = pAclUser->aclint;
	pAclSmi->gpio_en = pAclUser->gpio_en;
	pAclSmi->gpio_pin = pAclUser->gpio_pin;

	pAclSmi->cact_ext = pAclUser->cact_ext;
	pAclSmi->tag_fmt = pAclUser->tag_fmt;
	pAclSmi->fwdact_ext = pAclUser->fwdact_ext;
}
/* Function Name:
 *      rtl8367c_setAsicAclRule
 * Description:
 *      Set acl rule content
 * Input:
 *      index 	- ACL rule index (0-95) of 96 ACL rules
 *      pAclRule - ACL rule stucture for setting
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 *      RT_ERR_OUT_OF_RANGE  	- Invalid ACL rule index (0-95)
 * Note:
 *		System supported 95 shared 289-bit ACL ingress rule. Index was available at range 0-95 only.
 *		If software want to modify ACL rule, the ACL function should be disable at first or unspecify
 *		acl action will be executed.
 *		One ACL rule structure has three parts setting:
 *		Bit 0-147		Data Bits of this Rule
 *		Bit	148		Valid Bit
 *		Bit 149-296	Care Bits of this Rule
 *		There are four kinds of field in Data Bits and Care Bits: Active Portmask, Type, Tag Exist, and 8 fields
 */
ret_t rtl8367c_setAsicAclRule(rtk_uint32 index, rtl8367c_aclrule* pAclRule)
{
	rtl8367c_aclrulesmi aclRuleSmi;
	rtk_uint16* tableAddr;
	rtk_uint32 regAddr;
	rtk_uint32	regData;
	rtk_uint32 i;
	ret_t retVal;

	if(index > RTL8367C_ACLRULEMAX)
		return RT_ERR_OUT_OF_RANGE;

    memset(&aclRuleSmi, 0x00, sizeof(rtl8367c_aclrulesmi));

 	_rtl8367c_aclRuleStUser2Smi(pAclRule, &aclRuleSmi);

    /* Write valid bit = 0 */
    regAddr = RTL8367C_TABLE_ACCESS_ADDR_REG;
    if(index >= 64)
        regData = RTL8367C_ACLRULETBADDR2(DATABITS, index);
    else
        regData = RTL8367C_ACLRULETBADDR(DATABITS, index);
    retVal = rtl8367c_setAsicReg(regAddr,regData);
    if(retVal !=RT_ERR_OK)
        return retVal;

    retVal = rtl8367c_setAsicRegBits(RTL8367C_TABLE_ACCESS_WRDATA_REG(RTL8367C_ACLRULETBLEN), 0x1, 0);
    if(retVal !=RT_ERR_OK)
        return retVal;

    regAddr = RTL8367C_TABLE_ACCESS_CTRL_REG;
    regData = RTL8367C_TABLE_ACCESS_REG_DATA(TB_OP_WRITE, TB_TARGET_ACLRULE);
    retVal = rtl8367c_setAsicReg(regAddr, regData);
    if(retVal !=RT_ERR_OK)
        return retVal;



  	/* Write ACS_ADR register */
	regAddr = RTL8367C_TABLE_ACCESS_ADDR_REG;
    if(index >= 64)
	    regData = RTL8367C_ACLRULETBADDR2(CAREBITS, index);
    else
	    regData = RTL8367C_ACLRULETBADDR(CAREBITS, index);
	retVal = rtl8367c_setAsicReg(regAddr, regData);
	if(retVal != RT_ERR_OK)
		return retVal;

	/* Write Care Bits to ACS_DATA registers */
	 tableAddr = (rtk_uint16*)&aclRuleSmi.care_bits;
	 regAddr = RTL8367C_TABLE_ACCESS_WRDATA_BASE;

	for(i = 0; i < RTL8367C_ACLRULETBLEN; i++)
	{
		regData = *tableAddr;
		retVal = rtl8367c_setAsicReg(regAddr, regData);
		if(retVal != RT_ERR_OK)
			return retVal;

		regAddr++;
		tableAddr++;
	}
	retVal = rtl8367c_setAsicRegBits(RTL8367C_TABLE_ACCESS_WRDATA_REG(RTL8367C_ACLRULETBLEN), 0x7<<1, aclRuleSmi.care_bits_ext.active_portmsk_ext);
	if(retVal != RT_ERR_OK)
		return retVal;

	/* Write ACS_CMD register */
	regAddr = RTL8367C_TABLE_ACCESS_CTRL_REG;
	regData = RTL8367C_TABLE_ACCESS_REG_DATA(TB_OP_WRITE, TB_TARGET_ACLRULE);
	retVal = rtl8367c_setAsicRegBits(regAddr, RTL8367C_TABLE_TYPE_MASK | RTL8367C_COMMAND_TYPE_MASK,regData);
	if(retVal != RT_ERR_OK)
		return retVal;



	/* Write ACS_ADR register for data bits */
	regAddr = RTL8367C_TABLE_ACCESS_ADDR_REG;
    if(index >= 64)
	    regData = RTL8367C_ACLRULETBADDR2(DATABITS, index);
    else
	    regData = RTL8367C_ACLRULETBADDR(DATABITS, index);

	retVal = rtl8367c_setAsicReg(regAddr, regData);
	if(retVal != RT_ERR_OK)
		return retVal;

	/* Write Data Bits to ACS_DATA registers */
	 tableAddr = (rtk_uint16*)&aclRuleSmi.data_bits;
	 regAddr = RTL8367C_TABLE_ACCESS_WRDATA_BASE;

	for(i = 0; i < RTL8367C_ACLRULETBLEN; i++)
	{
		regData = *tableAddr;
		retVal = rtl8367c_setAsicReg(regAddr, regData);
		if(retVal != RT_ERR_OK)
			return retVal;

		regAddr++;
		tableAddr++;
	}

	retVal = rtl8367c_setAsicRegBit(RTL8367C_TABLE_ACCESS_WRDATA_REG(RTL8367C_ACLRULETBLEN), 0, aclRuleSmi.valid);
	if(retVal != RT_ERR_OK)
		return retVal;
	retVal = rtl8367c_setAsicRegBits(RTL8367C_TABLE_ACCESS_WRDATA_REG(RTL8367C_ACLRULETBLEN), 0x7<<1, aclRuleSmi.data_bits_ext.active_portmsk_ext);
	if(retVal != RT_ERR_OK)
		return retVal;

	/* Write ACS_CMD register for care bits*/
	regAddr = RTL8367C_TABLE_ACCESS_CTRL_REG;
	regData = RTL8367C_TABLE_ACCESS_REG_DATA(TB_OP_WRITE, TB_TARGET_ACLRULE);
	retVal = rtl8367c_setAsicRegBits(regAddr, RTL8367C_TABLE_TYPE_MASK | RTL8367C_COMMAND_TYPE_MASK, regData);
	if(retVal != RT_ERR_OK)
		return retVal;

#ifdef CONFIG_RTL8367C_ASICDRV_TEST
	memcpy(&Rtl8370sVirtualAclRuleTable[index], &aclRuleSmi, sizeof(rtl8367c_aclrulesmi));
#endif

	return RT_ERR_OK;
}
/* Function Name:
 *      rtl8367c_getAsicAclRule
 * Description:
 *      Get acl rule content
 * Input:
 *      index 	- ACL rule index (0-63) of 64 ACL rules
 *      pAclRule - ACL rule stucture for setting
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 *      RT_ERR_OUT_OF_RANGE  	- Invalid ACL rule index (0-63)
  * Note:
 *		None
 */
ret_t rtl8367c_getAsicAclRule(rtk_uint32 index, rtl8367c_aclrule *pAclRule)
{
	rtl8367c_aclrulesmi aclRuleSmi;
	rtk_uint32 regAddr, regData;
	ret_t retVal;
	rtk_uint16* tableAddr;
	rtk_uint32 i;

	if(index > RTL8367C_ACLRULEMAX)
		return RT_ERR_OUT_OF_RANGE;

	memset(&aclRuleSmi, 0x00, sizeof(rtl8367c_aclrulesmi));

	/* Write ACS_ADR register for data bits */
	regAddr = RTL8367C_TABLE_ACCESS_ADDR_REG;
    if(index >= 64)
        regData = RTL8367C_ACLRULETBADDR2(DATABITS, index);
    else
	    regData = RTL8367C_ACLRULETBADDR(DATABITS, index);

	retVal = rtl8367c_setAsicReg(regAddr, regData);
	if(retVal != RT_ERR_OK)
		return retVal;


	/* Write ACS_CMD register */
	regAddr = RTL8367C_TABLE_ACCESS_CTRL_REG;
	regData = RTL8367C_TABLE_ACCESS_REG_DATA(TB_OP_READ, TB_TARGET_ACLRULE);
	retVal = rtl8367c_setAsicRegBits(regAddr, RTL8367C_TABLE_TYPE_MASK | RTL8367C_COMMAND_TYPE_MASK, regData);
	if(retVal != RT_ERR_OK)
		return retVal;

	/* Read Data Bits */
	regAddr = RTL8367C_TABLE_ACCESS_RDDATA_BASE;
	tableAddr = (rtk_uint16*)&aclRuleSmi.data_bits;
	for(i = 0; i < RTL8367C_ACLRULETBLEN; i++)
	{
		retVal = rtl8367c_getAsicReg(regAddr, &regData);
		if(retVal != RT_ERR_OK)
			return retVal;

		*tableAddr = regData;

		regAddr ++;
		tableAddr ++;
	}

	/* Read Valid Bit */
	retVal = rtl8367c_getAsicRegBit(RTL8367C_TABLE_ACCESS_RDDATA_REG(RTL8367C_ACLRULETBLEN), 0, &regData);
	if(retVal != RT_ERR_OK)
		return retVal;
	aclRuleSmi.valid = regData & 0x1;
	/* Read active_portmsk_ext Bits */
	retVal = rtl8367c_getAsicRegBits(RTL8367C_TABLE_ACCESS_RDDATA_REG(RTL8367C_ACLRULETBLEN), 0x7<<1, &regData);
	if(retVal != RT_ERR_OK)
		return retVal;
	aclRuleSmi.data_bits_ext.active_portmsk_ext = regData & 0x7;


	/* Write ACS_ADR register for carebits*/
	regAddr = RTL8367C_TABLE_ACCESS_ADDR_REG;
    if(index >= 64)
	    regData = RTL8367C_ACLRULETBADDR2(CAREBITS, index);
    else
        regData = RTL8367C_ACLRULETBADDR(CAREBITS, index);

	retVal = rtl8367c_setAsicReg(regAddr, regData);
	if(retVal != RT_ERR_OK)
		return retVal;

	/* Write ACS_CMD register */
	regAddr = RTL8367C_TABLE_ACCESS_CTRL_REG;
	regData = RTL8367C_TABLE_ACCESS_REG_DATA(TB_OP_READ, TB_TARGET_ACLRULE);
	retVal = rtl8367c_setAsicRegBits(regAddr, RTL8367C_TABLE_TYPE_MASK | RTL8367C_COMMAND_TYPE_MASK, regData);
	if(retVal != RT_ERR_OK)
		return retVal;

	/* Read Care Bits */
	regAddr = RTL8367C_TABLE_ACCESS_RDDATA_BASE;
	tableAddr = (rtk_uint16*)&aclRuleSmi.care_bits;
	for(i = 0; i < RTL8367C_ACLRULETBLEN; i++)
	{
		retVal = rtl8367c_getAsicReg(regAddr, &regData);
		if(retVal != RT_ERR_OK)
			return retVal;

		*tableAddr = regData;

		regAddr ++;
		tableAddr ++;
	}
	/* Read active_portmsk_ext care Bits */
	retVal = rtl8367c_getAsicRegBits(RTL8367C_TABLE_ACCESS_RDDATA_REG(RTL8367C_ACLRULETBLEN), 0x7<<1, &regData);
	if(retVal != RT_ERR_OK)
		return retVal;
	aclRuleSmi.care_bits_ext.active_portmsk_ext = regData & 0x7;

#ifdef CONFIG_RTL8367C_ASICDRV_TEST
	memcpy(&aclRuleSmi,&Rtl8370sVirtualAclRuleTable[index], sizeof(rtl8367c_aclrulesmi));
#endif

	 _rtl8367c_aclRuleStSmi2User(pAclRule, &aclRuleSmi);

	return RT_ERR_OK;
}
/* Function Name:
 *      rtl8367c_setAsicAclNot
 * Description:
 *      Set rule comparison result inversion / no inversion
 * Input:
 *      index 	- ACL rule index (0-95) of 96 ACL rules
 *      not 	- 1: inverse, 0: don't inverse
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 *      RT_ERR_OUT_OF_RANGE  	- Invalid ACL rule index (0-95)
 * Note:
 *		None
 */
ret_t rtl8367c_setAsicAclNot(rtk_uint32 index, rtk_uint32 not)
{
	if(index > RTL8367C_ACLRULEMAX)
		return RT_ERR_OUT_OF_RANGE;

	if(index < 64)
		return rtl8367c_setAsicRegBit(RTL8367C_ACL_ACTION_CTRL_REG(index), RTL8367C_ACL_OP_NOT_OFFSET(index), not);
	else
		return rtl8367c_setAsicRegBit(RTL8367C_ACL_ACTION_CTRL2_REG(index), RTL8367C_ACL_OP_NOT_OFFSET(index), not);

}
/* Function Name:
 *      rtl8367c_getAsicAcl
 * Description:
 *      Get rule comparison result inversion / no inversion
 * Input:
 *      index 	- ACL rule index (0-95) of 95 ACL rules
 *      pNot 	- 1: inverse, 0: don't inverse
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 *      RT_ERR_OUT_OF_RANGE  	- Invalid ACL rule index (0-95)
 * Note:
 *		None
 */
ret_t rtl8367c_getAsicAclNot(rtk_uint32 index, rtk_uint32* pNot)
{
	if(index > RTL8367C_ACLRULEMAX)
		return RT_ERR_OUT_OF_RANGE;

	if(index < 64)
		return rtl8367c_getAsicRegBit(RTL8367C_ACL_ACTION_CTRL_REG(index), RTL8367C_ACL_OP_NOT_OFFSET(index), pNot);
	else
		return rtl8367c_getAsicRegBit(RTL8367C_ACL_ACTION_CTRL2_REG(index), RTL8367C_ACL_OP_NOT_OFFSET(index), pNot);

}
/* Function Name:
 *      rtl8367c_setAsicAclTemplate
 * Description:
 *      Set fields of a ACL Template
 * Input:
 *      index 	- ACL template index(0~4)
 *      pAclType - ACL type stucture for setting
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 *      RT_ERR_OUT_OF_RANGE  	- Invalid ACL template index(0~4)
 * Note:
 *	    The API can set type field of the 5 ACL rule templates.
 *		Each type has 8 fields. One field means what data in one field of a ACL rule means
 *		8 fields of ACL rule 0~95 is descripted by one type in ACL group
 */
ret_t rtl8367c_setAsicAclTemplate(rtk_uint32 index, rtl8367c_acltemplate_t* pAclType)
{
	ret_t retVal;
	rtk_uint32 i;
	rtk_uint32 regAddr, regData;

	if(index >= RTL8367C_ACLTEMPLATENO)
		return RT_ERR_OUT_OF_RANGE;

	regAddr = RTL8367C_ACL_RULE_TEMPLATE_CTRL_REG(index);

	for(i = 0; i < (RTL8367C_ACLRULEFIELDNO/2); i++)
    {
    	regData = pAclType->field[i*2+1];
		regData = regData << 8 | pAclType->field[i*2];

		retVal = rtl8367c_setAsicReg(regAddr + i, regData);

		if(retVal != RT_ERR_OK)
	        return retVal;
	}

	return retVal;
}
/* Function Name:
 *      rtl8367c_getAsicAclTemplate
 * Description:
 *      Get fields of a ACL Template
 * Input:
 *      index 	- ACL template index(0~4)
 *      pAclType - ACL type stucture for setting
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 *      RT_ERR_OUT_OF_RANGE  	- Invalid ACL template index(0~4)
 * Note:
 *	    None
 */
ret_t rtl8367c_getAsicAclTemplate(rtk_uint32 index, rtl8367c_acltemplate_t *pAclType)
{
	ret_t retVal;
	rtk_uint32 i;
	rtk_uint32 regData, regAddr;

	if(index >= RTL8367C_ACLTEMPLATENO)
		return RT_ERR_OUT_OF_RANGE;

	regAddr = RTL8367C_ACL_RULE_TEMPLATE_CTRL_REG(index);

	for(i = 0; i < (RTL8367C_ACLRULEFIELDNO/2); i++)
	{
		retVal = rtl8367c_getAsicReg(regAddr + i,&regData);
		if(retVal != RT_ERR_OK)
			return retVal;

		pAclType->field[i*2] = regData & 0xFF;
		pAclType->field[i*2 + 1] = (regData >> 8) & 0xFF;
	}

	return RT_ERR_OK;
}
/* Function Name:
 *      rtl8367c_setAsicAclAct
 * Description:
 *      Set ACL rule matched Action
 * Input:
 *      index 	- ACL rule index (0-95) of 96 ACL rules
 *      pAclAct 	- ACL action stucture for setting
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 *      RT_ERR_OUT_OF_RANGE  	- Invalid ACL rule index (0-95)
 * Note:
 *	    None
 */
ret_t rtl8367c_setAsicAclAct(rtk_uint32 index, rtl8367c_acl_act_t* pAclAct)
{
	rtl8367c_acl_act_smi_t aclActSmi;
	ret_t retVal;
	rtk_uint32 regAddr, regData;
	rtk_uint16* tableAddr;
	rtk_uint32 i;

	if(index > RTL8367C_ACLRULEMAX)
		return RT_ERR_OUT_OF_RANGE;

	memset(&aclActSmi, 0x00, sizeof(rtl8367c_acl_act_smi_t));
	 _rtl8367c_aclActStUser2Smi(pAclAct, &aclActSmi);


	/* Write ACS_ADR register for data bits */
	regAddr = RTL8367C_TABLE_ACCESS_ADDR_REG;
	regData = index;
	retVal = rtl8367c_setAsicReg(regAddr, regData);
	if(retVal != RT_ERR_OK)
		return retVal;

	/* Write Data Bits to ACS_DATA registers */
	 tableAddr = (rtk_uint16*)&aclActSmi;
	 regAddr = RTL8367C_TABLE_ACCESS_WRDATA_BASE;

	for(i = 0; i < RTL8367C_ACLACTTBLEN; i++)
	{
		regData = *tableAddr;
		retVal = rtl8367c_setAsicReg(regAddr, regData);
		if(retVal != RT_ERR_OK)
			return retVal;

		regAddr++;
		tableAddr++;
	}

	/* Write ACS_CMD register for care bits*/
	regAddr = RTL8367C_TABLE_ACCESS_CTRL_REG;
	regData = RTL8367C_TABLE_ACCESS_REG_DATA(TB_OP_WRITE, TB_TARGET_ACLACT);
	retVal = rtl8367c_setAsicRegBits(regAddr, RTL8367C_TABLE_TYPE_MASK | RTL8367C_COMMAND_TYPE_MASK, regData);
	if(retVal != RT_ERR_OK)
		return retVal;

#ifdef CONFIG_RTL8367C_ASICDRV_TEST
	Rtl8370sVirtualAclActTable[index] = aclActSmi;
#endif

	return RT_ERR_OK;
}
/* Function Name:
 *      rtl8367c_getAsicAclAct
 * Description:
 *      Get ACL rule matched Action
 * Input:
 *      index 	- ACL rule index (0-95) of 96 ACL rules
 *      pAclAct 	- ACL action stucture for setting
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 *      RT_ERR_OUT_OF_RANGE  	- Invalid ACL rule index (0-95)
  * Note:
 *	    None
 */
ret_t rtl8367c_getAsicAclAct(rtk_uint32 index, rtl8367c_acl_act_t *pAclAct)
{
	rtl8367c_acl_act_smi_t aclActSmi;
	ret_t retVal;
	rtk_uint32 regAddr, regData;
	rtk_int16* tableAddr;
	rtk_uint32 i;

	if(index > RTL8367C_ACLRULEMAX)
		return RT_ERR_OUT_OF_RANGE;

	memset(&aclActSmi, 0x00, sizeof(rtl8367c_acl_act_smi_t));


	/* Write ACS_ADR register for data bits */
	regAddr = RTL8367C_TABLE_ACCESS_ADDR_REG;
	regData = index;
	retVal = rtl8367c_setAsicReg(regAddr, regData);
	if(retVal != RT_ERR_OK)
		return retVal;


	/* Write ACS_CMD register */
	regAddr = RTL8367C_TABLE_ACCESS_CTRL_REG;
	regData = RTL8367C_TABLE_ACCESS_REG_DATA(TB_OP_READ, TB_TARGET_ACLACT);
	retVal = rtl8367c_setAsicRegBits(regAddr, RTL8367C_TABLE_TYPE_MASK | RTL8367C_COMMAND_TYPE_MASK, regData);
	if(retVal != RT_ERR_OK)
		return retVal;

	/* Read Data Bits */
	regAddr = RTL8367C_TABLE_ACCESS_RDDATA_BASE;
	tableAddr = (rtk_uint16*)&aclActSmi;
	for(i = 0; i < RTL8367C_ACLACTTBLEN; i++)
	{
		retVal = rtl8367c_getAsicReg(regAddr, &regData);
		if(retVal != RT_ERR_OK)
			return retVal;

		*tableAddr = regData;

		regAddr ++;
		tableAddr ++;
	}

#ifdef CONFIG_RTL8367C_ASICDRV_TEST
	aclActSmi = Rtl8370sVirtualAclActTable[index];
#endif

	 _rtl8367c_aclActStSmi2User(pAclAct, &aclActSmi);

	return RT_ERR_OK;
}
/* Function Name:
 *      rtl8367c_setAsicAclActCtrl
 * Description:
 *      Set ACL rule matched Action Control Bits
 * Input:
 *      index 		- ACL rule index (0-95) of 96 ACL rules
 *      aclActCtrl 	- 6 ACL Control Bits
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 *      RT_ERR_OUT_OF_RANGE  	- Invalid ACL rule index (0-95)
 * Note:
 *	    ACL Action Control Bits Indicate which actions will be take when a rule matches
 */
ret_t rtl8367c_setAsicAclActCtrl(rtk_uint32 index, rtk_uint32 aclActCtrl)
{
	ret_t retVal;

	if(index > RTL8367C_ACLRULEMAX)
		return RT_ERR_OUT_OF_RANGE;

    if(index >= 64)
        retVal = rtl8367c_setAsicRegBits(RTL8367C_ACL_ACTION_CTRL2_REG(index), RTL8367C_ACL_OP_ACTION_MASK(index), aclActCtrl);
    else
        retVal = rtl8367c_setAsicRegBits(RTL8367C_ACL_ACTION_CTRL_REG(index), RTL8367C_ACL_OP_ACTION_MASK(index), aclActCtrl);

    return retVal;
}
/* Function Name:
 *      rtl8367c_getAsicAclActCtrl
 * Description:
 *      Get ACL rule matched Action Control Bits
 * Input:
 *      index 		- ACL rule index (0-95) of 96 ACL rules
 *      pAclActCtrl 	- 6 ACL Control Bits
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 *      RT_ERR_OUT_OF_RANGE  	- Invalid ACL rule index (0-95)
 * Note:
 *	    None
 */
ret_t rtl8367c_getAsicAclActCtrl(rtk_uint32 index, rtk_uint32 *pAclActCtrl)
{
	ret_t retVal;
	rtk_uint32 regData;

	if(index > RTL8367C_ACLRULEMAX)
		return RT_ERR_OUT_OF_RANGE;

    if(index >= 64)
        retVal = rtl8367c_getAsicRegBits(RTL8367C_ACL_ACTION_CTRL2_REG(index), RTL8367C_ACL_OP_ACTION_MASK(index), &regData);
    else
        retVal = rtl8367c_getAsicRegBits(RTL8367C_ACL_ACTION_CTRL_REG(index), RTL8367C_ACL_OP_ACTION_MASK(index), &regData);

	if(retVal != RT_ERR_OK)
		return retVal;

	*pAclActCtrl = regData;

	return RT_ERR_OK;
}
/* Function Name:
 *      rtl8367c_setAsicAclPortRange
 * Description:
 *      Set ACL TCP/UDP range check
 * Input:
 *      index 		- TCP/UDP port range check table index
 *      type 		- Range check type
 *      upperPort 	- TCP/UDP port range upper bound
 *      lowerPort 	- TCP/UDP port range lower bound
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 *      RT_ERR_OUT_OF_RANGE  	- Invalid TCP/UDP port range check table index
 * Note:
 *	    None
 */
ret_t rtl8367c_setAsicAclPortRange(rtk_uint32 index, rtk_uint32 type, rtk_uint32 upperPort, rtk_uint32 lowerPort)
{
	ret_t retVal;

	if(index > RTL8367C_ACLRANGEMAX)
		return RT_ERR_OUT_OF_RANGE;

	retVal = rtl8367c_setAsicRegBits(RTL8367C_REG_ACL_SDPORT_RANGE_ENTRY0_CTRL2 + index*3, RTL8367C_ACL_SDPORT_RANGE_ENTRY0_CTRL2_MASK, type);
	if(retVal != RT_ERR_OK)
		return retVal;

	retVal = rtl8367c_setAsicReg(RTL8367C_REG_ACL_SDPORT_RANGE_ENTRY0_CTRL1 + index*3, upperPort);
	if(retVal != RT_ERR_OK)
		return retVal;

	retVal = rtl8367c_setAsicReg(RTL8367C_REG_ACL_SDPORT_RANGE_ENTRY0_CTRL0 + index*3, lowerPort);
	if(retVal != RT_ERR_OK)
		return retVal;

	return RT_ERR_OK;
}
/* Function Name:
 *      rtl8367c_getAsicAclPortRange
 * Description:
 *      Get ACL TCP/UDP range check
 * Input:
 *      index 		- TCP/UDP port range check table index
 *      pType 		- Range check type
 *      pUpperPort 	- TCP/UDP port range upper bound
 *      pLowerPort 	- TCP/UDP port range lower bound
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 *      RT_ERR_OUT_OF_RANGE  	- Invalid TCP/UDP port range check table index
 * Note:
 *	    None
 */
ret_t rtl8367c_getAsicAclPortRange(rtk_uint32 index, rtk_uint32* pType, rtk_uint32* pUpperPort, rtk_uint32* pLowerPort)
{
	ret_t retVal;

	if(index > RTL8367C_ACLRANGEMAX)
		return RT_ERR_OUT_OF_RANGE;

	retVal = rtl8367c_getAsicRegBits(RTL8367C_REG_ACL_SDPORT_RANGE_ENTRY0_CTRL2 + index*3, RTL8367C_ACL_SDPORT_RANGE_ENTRY0_CTRL2_MASK, pType);
	if(retVal != RT_ERR_OK)
		return retVal;

	retVal = rtl8367c_getAsicReg(RTL8367C_REG_ACL_SDPORT_RANGE_ENTRY0_CTRL1 + index*3, pUpperPort);
	if(retVal != RT_ERR_OK)
		return retVal;

	retVal = rtl8367c_getAsicReg(RTL8367C_REG_ACL_SDPORT_RANGE_ENTRY0_CTRL0 + index*3, pLowerPort);
	if(retVal != RT_ERR_OK)
		return retVal;

	return RT_ERR_OK;
}
/* Function Name:
 *      rtl8367c_setAsicAclVidRange
 * Description:
 *      Set ACL VID range check
 * Input:
 *      index 		- ACL VID range check index(0~15)
 *      type 		- Range check type
 *      upperVid 	- VID range upper bound
 *      lowerVid 	- VID range lower bound
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 *      RT_ERR_OUT_OF_RANGE  	- Invalid ACL  VID range check index(0~15)
 * Note:
 *	    None
 */
ret_t rtl8367c_setAsicAclVidRange(rtk_uint32 index, rtk_uint32 type, rtk_uint32 upperVid, rtk_uint32 lowerVid)
{
	ret_t retVal;
	rtk_uint32 regData;

	if(index > RTL8367C_ACLRANGEMAX)
		return RT_ERR_OUT_OF_RANGE;

	regData = ((type << RTL8367C_ACL_VID_RANGE_ENTRY0_CTRL1_CHECK0_TYPE_OFFSET) & RTL8367C_ACL_VID_RANGE_ENTRY0_CTRL1_CHECK0_TYPE_MASK) |
				(upperVid & RTL8367C_ACL_VID_RANGE_ENTRY0_CTRL1_CHECK0_HIGH_MASK);

	retVal = rtl8367c_setAsicReg(RTL8367C_REG_ACL_VID_RANGE_ENTRY0_CTRL1 + index*2, regData);
	if(retVal != RT_ERR_OK)
		return retVal;

	retVal = rtl8367c_setAsicReg(RTL8367C_REG_ACL_VID_RANGE_ENTRY0_CTRL0 + index*2, lowerVid);
	if(retVal != RT_ERR_OK)
		return retVal;

	return RT_ERR_OK;
}
/* Function Name:
 *      rtl8367c_getAsicAclVidRange
 * Description:
 *      Get ACL VID range check
 * Input:
 *      index 		- ACL VID range check index(0~15)
 *      pType 		- Range check type
 *      pUpperVid 	- VID range upper bound
 *      pLowerVid 	- VID range lower bound
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 *      RT_ERR_OUT_OF_RANGE  	- Invalid ACL VID range check index(0~15)
 * Note:
 *	    None
 */
ret_t rtl8367c_getAsicAclVidRange(rtk_uint32 index, rtk_uint32* pType, rtk_uint32* pUpperVid, rtk_uint32* pLowerVid)
{
	ret_t retVal;
	rtk_uint32 regData;

	if(index > RTL8367C_ACLRANGEMAX)
		return RT_ERR_OUT_OF_RANGE;

	retVal = rtl8367c_getAsicReg(RTL8367C_REG_ACL_VID_RANGE_ENTRY0_CTRL1 + index*2, &regData);
	if(retVal != RT_ERR_OK)
		return retVal;

	*pType = (regData & RTL8367C_ACL_VID_RANGE_ENTRY0_CTRL1_CHECK0_TYPE_MASK) >> RTL8367C_ACL_VID_RANGE_ENTRY0_CTRL1_CHECK0_TYPE_OFFSET;
	*pUpperVid = regData & RTL8367C_ACL_VID_RANGE_ENTRY0_CTRL1_CHECK0_HIGH_MASK;

	retVal = rtl8367c_getAsicReg(RTL8367C_REG_ACL_VID_RANGE_ENTRY0_CTRL0 + index*2, pLowerVid);
	if(retVal != RT_ERR_OK)
		return retVal;

	return RT_ERR_OK;
}
/* Function Name:
 *      rtl8367c_setAsicAclIpRange
 * Description:
 *      Set ACL IP range check
 * Input:
 *      index 		- ACL IP range check index(0~15)
 *      type 		- Range check type
 *      upperIp 	- IP range upper bound
 *      lowerIp 	- IP range lower bound
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 *      RT_ERR_OUT_OF_RANGE  	- Invalid ACL IP range check index(0~15)
 * Note:
 *	    None
 */
ret_t rtl8367c_setAsicAclIpRange(rtk_uint32 index, rtk_uint32 type, ipaddr_t upperIp, ipaddr_t lowerIp)
{
	ret_t retVal;
	rtk_uint32 regData;
	ipaddr_t ipData;

	if(index > RTL8367C_ACLRANGEMAX)
		return RT_ERR_OUT_OF_RANGE;

	retVal = rtl8367c_setAsicRegBits(RTL8367C_REG_ACL_IP_RANGE_ENTRY0_CTRL4 + index*5, RTL8367C_ACL_IP_RANGE_ENTRY0_CTRL4_MASK, type);
	if(retVal != RT_ERR_OK)
		return retVal;

	ipData = upperIp;

	regData = ipData & 0xFFFF;
	retVal = rtl8367c_setAsicReg(RTL8367C_REG_ACL_IP_RANGE_ENTRY0_CTRL2 + index*5, regData);
	if(retVal != RT_ERR_OK)
		return retVal;

	regData = (ipData>>16) & 0xFFFF;
	retVal = rtl8367c_setAsicReg(RTL8367C_REG_ACL_IP_RANGE_ENTRY0_CTRL3 + index*5, regData);
	if(retVal != RT_ERR_OK)
		return retVal;

	ipData = lowerIp;

	regData = ipData & 0xFFFF;
	retVal = rtl8367c_setAsicReg(RTL8367C_REG_ACL_IP_RANGE_ENTRY0_CTRL0 + index*5, regData);
	if(retVal != RT_ERR_OK)
		return retVal;

	regData = (ipData>>16) & 0xFFFF;
	retVal = rtl8367c_setAsicReg(RTL8367C_REG_ACL_IP_RANGE_ENTRY0_CTRL1 + index*5, regData);
	if(retVal != RT_ERR_OK)
		return retVal;

	return RT_ERR_OK;
}
/* Function Name:
 *      rtl8367c_getAsicAclIpRange
 * Description:
 *      Get ACL IP range check
 * Input:
 *      index 		- ACL IP range check index(0~15)
 *      pType 		- Range check type
 *      pUpperIp 	- IP range upper bound
 *      pLowerIp 	- IP range lower bound
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 *      RT_ERR_OUT_OF_RANGE  	- Invalid ACL IP range check index(0~15)
 * Note:
 *	    None
 */
ret_t rtl8367c_getAsicAclIpRange(rtk_uint32 index, rtk_uint32* pType, ipaddr_t* pUpperIp, ipaddr_t* pLowerIp)
{
	ret_t retVal;
	rtk_uint32 regData;
	ipaddr_t ipData;

	if(index > RTL8367C_ACLRANGEMAX)
		return RT_ERR_OUT_OF_RANGE;

	retVal = rtl8367c_getAsicRegBits(RTL8367C_REG_ACL_IP_RANGE_ENTRY0_CTRL4 + index*5, RTL8367C_ACL_IP_RANGE_ENTRY0_CTRL4_MASK, pType);
	if(retVal != RT_ERR_OK)
		return retVal;

	retVal = rtl8367c_getAsicReg(RTL8367C_REG_ACL_IP_RANGE_ENTRY0_CTRL2 + index*5, &regData);
	if(retVal != RT_ERR_OK)
		return retVal;
	ipData = regData;


	retVal = rtl8367c_getAsicReg(RTL8367C_REG_ACL_IP_RANGE_ENTRY0_CTRL3 + index*5, &regData);
	if(retVal != RT_ERR_OK)
		return retVal;

	ipData = (regData <<16) | ipData;
	*pUpperIp = ipData;


	retVal = rtl8367c_getAsicReg(RTL8367C_REG_ACL_IP_RANGE_ENTRY0_CTRL0 + index*5, &regData);
	if(retVal != RT_ERR_OK)
		return retVal;
	ipData = regData;


	retVal = rtl8367c_getAsicReg(RTL8367C_REG_ACL_IP_RANGE_ENTRY0_CTRL1 + index*5, &regData);
	if(retVal != RT_ERR_OK)
		return retVal;

	ipData = (regData << 16) | ipData;
	*pLowerIp = ipData;

	return RT_ERR_OK;
}

/* Function Name:
 *      rtl8367c_setAsicAclGpioPolarity
 * Description:
 *      Set ACL Goip control palarity
 * Input:
 *      polarity - 1: High, 0: Low
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 		- Success
 *      RT_ERR_SMI  	- SMI access error
 * Note:
 *      none
 */
ret_t rtl8367c_setAsicAclGpioPolarity(rtk_uint32 polarity)
{
    return rtl8367c_setAsicRegBit(RTL8367C_REG_ACL_GPIO_POLARITY, RTL8367C_ACL_GPIO_POLARITY_OFFSET, polarity);
}
/* Function Name:
 *      rtl8367c_getAsicAclGpioPolarity
 * Description:
 *      Get ACL Goip control palarity
 * Input:
 *      pPolarity - 1: High, 0: Low
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 		- Success
 *      RT_ERR_SMI  	- SMI access error
 * Note:
 *      none
 */
ret_t rtl8367c_getAsicAclGpioPolarity(rtk_uint32* pPolarity)
{
    return rtl8367c_getAsicRegBit(RTL8367C_REG_ACL_GPIO_POLARITY, RTL8367C_ACL_GPIO_POLARITY_OFFSET, pPolarity);
}

