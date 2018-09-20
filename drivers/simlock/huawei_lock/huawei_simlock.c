/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2013-2015. All rights reserved.
 *
 * mobile@huawei.com
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


#include "mdrv.h"
#if (FEATURE_ON == MBB_SIMLOCK_FOUR)
#include <crypto/hash.h>
#include "product_nv_id.h"
#include "product_nv_def.h"
#include "bsp_nvim.h"
#include "bsp_sram.h"
#include "bsp_icc.h"
#include "wb_aes_decrypt.h"
#include "wb_aes_encrypt.h"
#include <bsp_cpufreq.h>
#include <linux/mlog_lib.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/timex.h>
#include <linux/rtc.h>

/*hwlock����У����Ϣ�ṹ�嶨��*/
typedef struct
{
    unsigned char  lock_para[DRV_HWOCK_LEN_MAX];/*�����ַ�������*/
    unsigned int para_len;/*���볤��*/
    unsigned int type;
} drv_hwlock_msg_stru;

struct pbkdf2_param {
    uint8_t     *pwd;     /* �����������ַ��� */
    uint32_t    pwd_len;  /* �������ַ������� */
    uint8_t     *salt;    /* ��ֵ�ַ��� */
    uint32_t    salt_len; /* ��ֵ�ַ������� */
    uint32_t    iterate_cnt; /* ����������� */
};


static void hwlock_get_rtc_time(struct rtc_time *tm)
{
    struct timex  txc;

     if (NULL == tm)
     {
         return;
     }

    memset((void *)&txc, 0, sizeof(txc));
    (void)do_gettimeofday(&(txc.time));
    (void)rtc_time_to_tm(txc.time.tv_sec,tm);
}


static int hmac_sha256(const uint8_t *key, uint32_t ksize, const uint8_t *plaintext,
        uint32_t psize, uint8_t *output)
{
    int ret = 0;
    struct crypto_shash *tfm = NULL;

    if ((NULL == key) || (NULL == plaintext) || (NULL == output))
    {
        printk(KERN_ERR "[HWLOCK] key or plaintext or output is null err!\r\n");
        return -1;
    }

    if (0 == ksize)
    {
        return -EINVAL;
    }

    /*psize ����Ϊunsigned int�ͣ�����У��*/

    tfm = crypto_alloc_shash("hmac(sha256)", 0, 0);
    if (IS_ERR(tfm))
    {
        printk(KERN_ERR "[HWLOCK] alloc ahash failed: err %ld", PTR_ERR(tfm));
        return PTR_ERR(tfm);
    }

    ret = crypto_shash_setkey(tfm, key, ksize);
    if (0 != ret)
    {
        printk(KERN_ERR "[HWLOCK] ahash setkey failed: err %d", ret);
    }
    else
    {
        struct
        {
            struct shash_desc shash;
            char ctx[crypto_shash_descsize(tfm)];
        }desc;

        desc.shash.tfm = tfm;
        desc.shash.flags = CRYPTO_TFM_REQ_MAY_SLEEP;
        ret = crypto_shash_digest(&desc.shash, plaintext, psize, output);
    }

    /* this function will clear and free the buffer pointed by tfm */
    crypto_free_shash(tfm);
    return ret;
}


static int PBKDF2(struct pbkdf2_param *param, char *out, uint32_t out_len)
{
    uint8_t *pwd = NULL;
    uint8_t *salt = NULL;
    uint32_t pwd_len = 0;
    uint32_t salt_len = 0;
    uint32_t iterate_cnt = 0;
    uint32_t i = 0;
    uint32_t j = 0;
    uint8_t hmac_result[SHA256_OUT_LEN] = {0};
    uint8_t pbkdf2_buf[SHA256_OUT_LEN] = {0};
    uint8_t hmac_salt[PBKDF2_SALT_LEN_MAX] = {0}; /*ʵ��ռ��32+4�ֽ�*/
    uint8_t constant[PBKDF2_CONSTANT_LEN] = {0, 0, 0, 1}; 
    int ret = -1;

    if (NULL == param)
    {
        printk(KERN_ERR "[HWLOCK] pointer is NOT correct!\r\n" );
        return -1;
    }

    pwd = param->pwd;
    salt = param->salt;
    pwd_len = param->pwd_len;
    salt_len = param->salt_len;
    iterate_cnt = param->iterate_cnt;

    if ((NULL == pwd) || (NULL == salt) || (NULL == out))
    {
        printk(KERN_ERR "[HWLOCK] pointer is NOT correct!\r\n" );
        return -1;
    }
    
    if ( out_len != PBKDF2_OUT_LEN ) 
    {
        printk(KERN_ERR "[HWLOCK] out_len is NOT correct!\r\n" );
        return -1;
    }

    if (salt_len > PBKDF2_SALT_LEN_MAX)
    {
        printk(KERN_ERR "[HWLOCK] salt_len NOT correct!\r\n" );
        return -1;
    }

    if (strlen((char *)pwd) != pwd_len)
    {
        printk(KERN_ERR "[HWLOCK] pwd_len NOT correct!\r\n" );
        return -1;
    }

    if (SET_ITERATE_CNT > iterate_cnt)
    {
        printk(KERN_ERR "[HWLOCK] iterate_cnt NOT correct!\r\n" );
        return -1;
    }

    /* ��˼ƽ̨����CPUƵ�ʣ����ټ����ٶ� */
    (void)cpufreq_dfs_set_profile(BALONG_FREQ_MAX);
    
    /*����32�ֽ���ֵ*/
    memcpy((void *)hmac_salt, (void *)salt, SHA256_OUT_LEN );

    /*�ڳ�ʼ��ֵ֮����4�ֽڳ���,�������յ�36�ֽ���ֵ*/
    memcpy((void *)(hmac_salt + SHA256_OUT_LEN), (void *)constant, sizeof(constant));
    ret = hmac_sha256(pwd, pwd_len, hmac_salt, SHA256_OUT_LEN + sizeof(constant), hmac_result);
    if (0 != ret)
    {
        printk(KERN_ERR "[HWLOCK] ret is %d!\r\n", ret);
        return -1;
    }

    memset((void *)hmac_salt, '\0', sizeof(hmac_salt));
    for (j = 0; j < SHA256_OUT_LEN; j++)
    {
        pbkdf2_buf[j] ^= hmac_result[j];
    }

    for (i = 1; i <= iterate_cnt; i++)
    {
        memcpy((void *)hmac_salt, (void *)hmac_result, SHA256_OUT_LEN);
        ret = hmac_sha256(pwd, pwd_len, hmac_salt, SHA256_OUT_LEN, hmac_result);
        if (0 != ret)
        {
            printk(KERN_ERR "[HWLOCK] ret is %d!\r\n", ret);
            return -1;
        }

        for (j = 0; j < SHA256_OUT_LEN; j++)
        {
            pbkdf2_buf[j] ^= hmac_result[j];
        }
    }

    /*���Ƶ����������*/
    memcpy((void *)out, (void *)pbkdf2_buf, out_len );

    return 0;
}


static int check_radom_invalid(const uint8_t *random_buf)
{
    uint32_t  i = 0;

    if (NULL == random_buf)
    {
        printk(KERN_ERR "[HWLOCK]  random_buf is null err!\r\n");
        return -1;
    }

    for (i = 0; i < DRV_RANDOM_LEN; i++)
    {
        if (0 != random_buf[i])
        {
            return 0;
        }
    }
    printk(KERN_ERR "[HWLOCK]  random in random_buf is invalid!\r\n");
    return -1;
}


static int get_random(uint8_t *random_buf)
{
    int ret = 0;

    if (NULL == random_buf)
    {
        return -1;
    }

    get_random_bytes((void *)random_buf, DRV_RANDOM_LEN);
    ret = check_radom_invalid(random_buf);
    if (0 != ret)
    {
        get_random_bytes((void *)random_buf, DRV_RANDOM_LEN);
        ret = check_radom_invalid(random_buf);
    }

    return ret;
}


static uint32_t get_pbkdf2_times_random(void)
{
    uint32_t random_num = 0;

    get_random_bytes((void *)(&random_num), DRV_PBKDF2_TIMES_RANDOM_LEN);
    random_num = (random_num % SET_ITERATE_CNT) + SET_ITERATE_CNT;

    return random_num;
}


static int drv_hwlock_check_digit(char num)
{
    return (('0' <= num) && ('9' >= num)) ? 0 : -1;
}


static int drv_hwlock_check_capital_letter(char num)
{
    char letter_tab[] = {'A', 'B', 'D', 'E', 'F', 'G', 'H', 'J', 'L', 'M', 'N', 'Q', 'R', 'T', 'Y'};
    uint32_t loop = 0;
    uint32_t max_num = sizeof(letter_tab);

    for (loop = 0; loop < max_num; loop++)
    {
        if (num == letter_tab[loop])
        {
            return 0;
        }
    }

    return -1;
}


static int drv_hwlock_check_lowercase(char num)
{
    char letter_tab[] = {'a', 'b', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'm', 'n', 'q', 'r', 't', 'y'};
    unsigned int loop = 0;
    unsigned int max_num = sizeof(letter_tab);

    for(loop = 0; loop < max_num; loop++)
    {
        if (num == letter_tab[loop])
        {
            return 0;
        }
    }
    return -1;
}

/* ��2�������ַ���Ϊ����Ҫ�� */
#define HWLOCK_STR_VALIDE_CODE_TYPE    (2)

static int drv_check_hwlock_string(const char *pt_data, uint32_t len)
{
    uint32_t loop = 0;
    uint32_t lowercase_count_flag = 0;  /*�ִ��а���Сд��ĸ��־*/
    uint32_t capital_letter_count_flag = 0;  /*�ִ��а�����д��ĸ��־*/
    uint32_t digit_count_flag = 0;  /*�ִ��а������ֱ�־*/
    const char *pt_temp = pt_data;
    int ret = -1;
    int check_ret = -1;

    if ((NULL == pt_data) || (0 == len))
    {
        printk(KERN_ERR "[HWLOCK] drv check hwlock_string para err!\r\n");
        return -1;
    }

    if (strlen((char *)pt_data) != len)
    {
        printk(KERN_ERR "[HWLOCK] drv check hwlock string len is not equal with input err!\r\n");
        return -1;
    }

    for (loop = 0; loop < len; loop++)
    {
        check_ret = drv_hwlock_check_digit(pt_temp[loop]);
        if (0 == check_ret)
        {
            digit_count_flag = 1;
            continue;
        }

        check_ret = drv_hwlock_check_capital_letter(pt_temp[loop]);
        if (0 == check_ret)
        {
            capital_letter_count_flag = 1;
            continue;
        }

        check_ret = drv_hwlock_check_lowercase(pt_temp[loop]);
        if (0 == check_ret)
        {
            lowercase_count_flag = 1;
            continue;
        }

        /*�ַ�Ϊ�����ַǺϷ���Сд�ַ�*/
        printk(KERN_INFO "[HWLOCK]drv check hwlock string CHAR IS INVALID!\r\n");
        return -1;
    }

    ret = digit_count_flag + capital_letter_count_flag + lowercase_count_flag;

    /*��2�������ַ���Ϊ����Ҫ��*/
    if (ret >= HWLOCK_STR_VALIDE_CODE_TYPE)
    {
        return 0;
    }

    return -1;
}


int hw_lock_set_proc(char *lock_para, uint32_t para_len, uint32_t type)
{
    struct rtc_time tm;
    NV_AUHT_SIMLOCK_STWICH_STRU hw_lock;
    struct pbkdf2_param param;
    uint32_t hw_lock_id = 0;
    int result = -1;
    uint8_t result_hash[DRV_HWLOCK_HASH_LEN] = {0};/*����������hashֵ + �����hashֵ����hashֵ */
    uint8_t random_buf[DRV_RANDOM_LEN] = {0};
    uint32_t random_encrypt_len = 0;
    confidential_nv_opr_info *smem_data = NULL;
    uint32_t iterate_cnt = 0;    /*����PBKDF2����*/

    smem_data = (confidential_nv_opr_info *)SRAM_CONFIDENTIAL_NV_OPR_ADDR;
    memset((void *)&hw_lock, 0, sizeof(NV_AUHT_SIMLOCK_STWICH_STRU));
    memset((void *)&tm, 0, sizeof(struct rtc_time));

    if (NULL == lock_para)
    {
        result = -1;
        printk(KERN_ERR "[HWLOCK] set hwlock lock_para is err!\r\n");
        goto hw_lock_set_exit;
    }

    if ((strlen((char *)lock_para) != para_len) || (DRV_HWOCK_LEN_MAX < para_len))
    {
        result = -1;
        printk(KERN_ERR "[HWLOCK] set hwlock para len err!\r\n");
        goto hw_lock_set_exit;
    }

    result = drv_check_hwlock_string(lock_para, para_len);
    if (0 != result)
    {
        result = -1;
        printk(KERN_ERR "[HWLOCK] set hwlock str check invalid!\r\n");
        goto hw_lock_set_exit;
    }

    if (HW_LOCK_OEM_TYPE == type)
    {
        hw_lock_id = NV_HUAWEI_OEMLOCK_I;
    }
    else if (HW_LOCK_SIMLOCK_TYPE == type)
    {
        hw_lock_id = NV_HUAWEI_SIMLOCK_I;
    }
    else
    {
        result = -1;
        printk(KERN_ERR "[HWLOCK] set hwlock HW_LOCK_OEM_TYPE err!\r\n");
        goto hw_lock_set_exit;
    }

    /*��ȡʱ���������hashֵ�����浽random_buf*/
    result = get_random(random_buf);
    if (0 != result)
    {
        result = -1;
        printk(KERN_ERR "[HWLOCK] set hwlock get_random is error\r\n");
        goto hw_lock_set_exit;
    }

    iterate_cnt = get_pbkdf2_times_random();
    if (SET_ITERATE_CNT > iterate_cnt)
    {
        iterate_cnt = SET_ITERATE_CNT;
        printk(KERN_ERR "[HWLOCK] set hwlock get times random error\r\n");
    }

    /*��������������浽nv buf*/
    *(unsigned int *)(&hw_lock.reserved[4]) = iterate_cnt;
    /*��������ı��浽hw_lock.nv_lock21*/
    result = wb_aes_encrypt_cbc(SALT_IV, random_buf, DRV_RANDOM_LEN, (unsigned char *)hw_lock.nv_lock21, &random_encrypt_len);
    if (0 != result)
    {
        result = -1;
        printk(KERN_ERR "[HWLOCK] aes encrypt result = %d err!\r\n", result);
        goto hw_lock_set_exit;
    }

    if (DRV_ASE_LEN != random_encrypt_len)
    {
        result = -1;
        printk(KERN_ERR "[HWLOCK] aes encrypt return len = %u err!\r\n", random_encrypt_len);
        goto hw_lock_set_exit;
    }

    param.pwd = (int8_t *)lock_para;
    param.pwd_len = para_len;
    param.salt = random_buf;
    param.salt_len = DRV_RANDOM_LEN;
    param.iterate_cnt = iterate_cnt;

    result = PBKDF2(&param, result_hash, DRV_HWLOCK_HASH_LEN);
    if (0 != result)
    {
        printk(KERN_ERR "[HWLOCK] set hwlock algorithm  process error,result = %d!\r\n", result);
        result = -1;
        goto hw_lock_set_exit;
    }

    hw_lock.reserved[HWLOCK_ENABLE_FLAG] = DRV_OEM_SIMLOCK_ENABLE;
    memcpy((void *)hw_lock.nv_lock30, (void *)result_hash, DRV_HWLOCK_HASH_LEN);

    if (NULL == smem_data)
    {
        result = -1;
        printk(KERN_ERR "[HWLOCK] set hwlock smem flag malloc fail!\n");
        goto hw_lock_set_exit;
    }

    /*���û�Ҫnv��Ȩ��ǣ���Ȩ��ȡnv*/
    smem_data->smem_confidential_nv_opr_flag = SMEM_CONFIDENTIAL_NV_OPR_FLAG_NUM;

    result = bsp_nvm_write(hw_lock_id, (uint8_t *)&hw_lock, sizeof(hw_lock));
    
    if (NV_OK != result)
    {
        result = -1;
        printk(KERN_ERR "[HWLOCK] set hwlock:ERROR:write OEMLOCK nv Fail,result = %d!\r\n", result);
        goto hw_lock_set_exit;
    }
    else
    {
        result = 0;
    }
    
hw_lock_set_exit:
    memset((void *)&hw_lock, 0, sizeof(NV_AUHT_SIMLOCK_STWICH_STRU));
    memset((void *)result_hash, 0, sizeof(result_hash));
    memset((void *)random_buf, 0, sizeof(random_buf));

    (void)hwlock_get_rtc_time(&tm);
#if (FEATURE_ON == MBB_MLOG)

#if (FEATURE_ON == MBB_FACTORY)
    mlog_print(MLOG_SIMLOCK, mlog_lv_fatal, "UTC time :%d-%d-%d %d:%d:%d ", \
        tm.tm_year + YEAR_BASE,tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    mlog_print(MLOG_SIMLOCK, mlog_lv_fatal, "factory set hwlock ret is %d.\r\n", result);
#else
    mlog_print(MLOG_SIMLOCK, mlog_lv_fatal, "UTC time :%d-%d-%d %d:%d:%d ", \
        tm.tm_year + YEAR_BASE,tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    mlog_print(MLOG_SIMLOCK, mlog_lv_fatal, "release set hwlock ret is %d.\r\n", result);
#endif

#endif
    return result;
    
}


int hw_lock_verify_proc(uint8_t *lock_para, uint32_t para_len, uint32_t type)
{
    struct rtc_time tm;
    struct pbkdf2_param param;
    int result = -1;
    uint32_t hw_lock_id = 0;
    NV_AUHT_SIMLOCK_STWICH_STRU hw_lock;
    uint8_t result_hash[DRV_HWLOCK_HASH_LEN] = {0};/*����PBKDF2������*/
    uint8_t random_buf[DRV_RANDOM_LEN] = {0};
    uint32_t random_decrypt_len = 0;
    confidential_nv_opr_info *smem_data = NULL;
    uint32_t iterate_cnt = 0;    /*����PBKDF2����*/

    smem_data = (confidential_nv_opr_info *)SRAM_CONFIDENTIAL_NV_OPR_ADDR;
    memset((void *)&hw_lock, 0, sizeof(NV_AUHT_SIMLOCK_STWICH_STRU));
    memset((void *)&tm, 0, sizeof(struct rtc_time));

    if (NULL == lock_para)
    {
        result = -1;
        printk(KERN_ERR "[HWLOCK] verify hwlock para err!\r\n");
        goto hw_lock_verify_exit;
    }

    if ((strlen((char *)lock_para) != para_len) || (DRV_HWOCK_LEN_MAX < para_len))
    {
        result = -1;
        printk(KERN_ERR "[HWLOCK] verify hwlock para len err!\r\n");
        goto hw_lock_verify_exit;
    }

    if (HW_LOCK_OEM_TYPE == type)
    {
        hw_lock_id = NV_HUAWEI_OEMLOCK_I;
    }
    else if (HW_LOCK_SIMLOCK_TYPE == type)
    {
        hw_lock_id = NV_HUAWEI_SIMLOCK_I;
    }
    else
    {
        result = -1;
        printk(KERN_ERR "[HWLOCK] verify hwlock type err!\r\n");
        goto hw_lock_verify_exit;
    }

    if (NULL == smem_data)
    {
        printk(KERN_ERR "[HWLOCK] verify hwlock smem nv malloc fail!\n");
        result = -1;
        goto hw_lock_verify_exit;
    }
    /*���ü�Ҫnv��Ȩ��ǣ���Ȩ��ȡnv*/
    smem_data->smem_confidential_nv_opr_flag = SMEM_CONFIDENTIAL_NV_OPR_FLAG_NUM;

    result = bsp_nvm_read(hw_lock_id, (uint8_t *)&hw_lock, sizeof(hw_lock));
    if (NV_OK != result)
    {
        result = -1;
        printk(KERN_ERR "[HWLOCK] verify hwlock :ERROR:read nv hw_lock Fail.r\n");
        goto hw_lock_verify_exit;
    }

    iterate_cnt = *(uint32_t *)(&hw_lock.reserved[4]);
    if (SET_ITERATE_CNT > iterate_cnt)
    {
        printk(KERN_ERR "[HWLOCK] verify hwlock get times random invalid\r\n");
        iterate_cnt = SET_ITERATE_CNT;
    }

    /*�����������ֵ*/
    result = wb_aes_decrypt_cbc(SALT_IV, hw_lock.nv_lock21, DRV_ASE_LEN, random_buf, &random_decrypt_len);
    if (0 != result)
    {
        result = -1;
        printk(KERN_ERR "[HWLOCK] aes decrypt result = %d err!\r\n", result);
        goto hw_lock_verify_exit;
    }

    if (DRV_RANDOM_LEN != random_decrypt_len)
    {
        result = -1;
        printk(KERN_ERR "[HWLOCK] aes encrypt return len = %u err!\r\n", random_decrypt_len);
        goto hw_lock_verify_exit;
    }

    param.pwd = (uint8_t *)lock_para;
    param.pwd_len = para_len;
    param.salt = random_buf;
    param.salt_len = DRV_RANDOM_LEN;
    param.iterate_cnt = iterate_cnt;

    /*hwlock ����У�����*/
    result = PBKDF2(&param, result_hash, DRV_HWLOCK_HASH_LEN);
    if (0 != result)
    {
        result = -1;
        printk(KERN_ERR "[HWLOCK] verify hwlock algorithm process error!\r\n");
        goto hw_lock_verify_exit;
    }

    /*��hash���ֵ�Ͳ���д��NV��������бȽϣ����һ��������ɹ�������ʧ��*/
    result = memcmp((void *)result_hash, (void *)hw_lock.nv_lock30, DRV_HWLOCK_HASH_LEN);

hw_lock_verify_exit:
    memset((void *)&hw_lock, 0, sizeof(NV_AUHT_SIMLOCK_STWICH_STRU));
    memset((void *)result_hash, 0, sizeof(result_hash));
    memset((void *)random_buf, 0, sizeof(random_buf));

    (void)hwlock_get_rtc_time(&tm);
#if (FEATURE_ON == MBB_MLOG)

#if (FEATURE_ON == MBB_FACTORY)
    mlog_print(MLOG_SIMLOCK, mlog_lv_fatal, "UTC time :%d-%d-%d %d:%d:%d ",
        tm.tm_year + YEAR_BASE,tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    mlog_print(MLOG_SIMLOCK, mlog_lv_fatal, "factory verify hwlock ret is %d.\r\n", result);
#else
    mlog_print(MLOG_SIMLOCK, mlog_lv_fatal, "UTC time :%d-%d-%d %d:%d:%d ",
        tm.tm_year + YEAR_BASE,tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    mlog_print(MLOG_SIMLOCK, mlog_lv_fatal, "release verify hwlock ret is %d.\r\n", result);
#endif

#endif

    return result;
}


static int drv_hwlock_verify_ack(int result)
{
    /*result--ȡֵΪ���ͣ�����Ҫ���в����Ϸ���У��*/

    uint32_t icc_channel_id = (ICC_CHN_IFC << 16) | IFC_VERIFY_HWLOCK_RSP;
    uint32_t ret_val;
    int msg;

    /*���У����*/
    msg = result;

    ret_val = bsp_icc_send(ICC_CPU_MODEM, icc_channel_id, (uint8_t *)&msg, sizeof(msg));
    if (ret_val != (uint32_t)sizeof(msg))
    {
        printk(KERN_ERR "[HWLOCK] simlock : send verify ack fail !!!\n");
        return -1;
    }

    return 0;
}


static int drv_icc_hwlock_verify(uint32_t chanid, uint32_t len, void* pdata)
{
    uint32_t icc_channel_id = ICC_CHN_IFC << 16 | IFC_VERIFY_FUNC_HWLOCK;
    uint32_t key_len = 0;
    drv_hwlock_msg_stru hwlock_msg;

    int ret_val = BSP_ERROR;
    int ret_result =  BSP_ERROR;

    memset((void*)(&hwlock_msg), 0, sizeof(drv_hwlock_msg_stru));

    if ( icc_channel_id != chanid )
    {
        printk(KERN_ERR "[HWLOCK] simlock: channel_id doesn't match\n");
        return -1;
    }

    if (len > sizeof(drv_hwlock_msg_stru))
    {
        /*�����̹淶�����ж�*/
        printk(KERN_INFO "[HWLOCK] %s :  len is longer than hwlock_msg len\r\n", __func__);
    }

    if (NULL == pdata)
    {
        /*�����̹淶�����ж�*/
        printk(KERN_INFO "[HWLOCK]%s :  pdata is null!\r\n", __func__);
    }

    key_len = bsp_icc_read(icc_channel_id, (unsigned char*)(&hwlock_msg), sizeof(hwlock_msg));
    if (key_len != sizeof(hwlock_msg))
    {
        printk(KERN_ERR "[HWLOCK]read len(%d) != expected len(%d) !\n", key_len, sizeof(hwlock_msg));
        return -1;
    }

    ret_result = hw_lock_verify_proc(hwlock_msg.lock_para, hwlock_msg.para_len , hwlock_msg.type);

    /*��A�˵Ļظ���Ϣ*/
    ret_val = drv_hwlock_verify_ack(ret_result);

    return ret_val;
}


static int __init drv_verify_simlock_acore_init(void)
{
    int ret_val = 0;
    uint32_t icc_channel_id = (ICC_CHN_IFC << 16) | IFC_VERIFY_FUNC_HWLOCK;

    ret_val = bsp_icc_event_register(icc_channel_id,
            (read_cb_func)drv_icc_hwlock_verify, NULL, NULL, NULL);

    if (0 != ret_val)
    { 
        printk(KERN_ERR "[HWLOCK] drv verify simlock acore init fail:%d\n", ret_val);
        return -1;
    }

    printk(KERN_INFO "[HWLOCK] drv verify simlock init ok!\r\n");
    return 0;
}

module_init(drv_verify_simlock_acore_init);


#endif/*end for MBB_SIMLOCK_FOUR*/
