

#ifndef __MBB_COMMON_H__
#define __MBB_COMMON_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <product_config.h>
/*CRADLE类型枚举,后续新增此类型必须要和硬件提供的ID范围一一对应，
  并且要找USB、应用同事确认清楚后方可添加*/
typedef enum _CRADLE_TYPE
{
    CRADLE_TYPE_INVALID = 0,  /*无效类型CRADLE*/
    CRADLE_TYPE_AF31    = 1,  /*AF31 CRADLE*/
    CRADLE_TYPE_AF36    = 2,  /*AF36 CRADLE*/
    CRADLE_TYPE_H3G     = 3,  /*H3G CRADLE*/
    CRADLE_TYPE_V7R22   = 4,  /*USB ID范围[0.25,0.35]*/
    CRADLE_TYPE_AP      = 5,  /*AP CRADLE*/
    CRADLE_TYPE_MAX     = 0xFF
}CRADLE_TYPE;

/*USBID的电压范围门限，识别Cradle类型*/
#define MAX_USBID_VOLTAGE  (350)
#define MIN_USBID_VOLTAGE  (250)
#define USBID_VBUS_HKADC   (1)
/**************************************************************************
  头文件包含                            
**************************************************************************/

/**************************************************************************
  宏定义 
**************************************************************************/
#define    MBB_SUSPEND  (0)
#define    MBB_RESUME   (1)
#if ( FEATURE_ON == MBB_MLOG )
#define    MLOG_MAX_MD_LEN  (16)
#define    MLOG_MAX_LOG_LEN (100)
#endif
/**************************************************************************
  变量、结构定义 
**************************************************************************/
#if ( FEATURE_ON == MBB_MLOG )
#ifndef __KERNEL__ /* C核的情况，M核请勿使用 */
typedef enum _mlog_lv
{
   mlog_lv_none  = 0, /* NONE */
   mlog_lv_fatal = 1, /* FATAL */
   mlog_lv_error = 2, /* ERROR */
   mlog_lv_warn  = 4, /* WARNING */
   mlog_lv_info  = 8 /* INFO */
}mlog_lv_eum;
#endif

typedef struct _tagMbbCommonMlog
{
    char    module[MLOG_MAX_MD_LEN];
    int     loglv;
    int     len;
	char    msg[MLOG_MAX_LOG_LEN];
}stMbbCommonMlog; 
#endif

struct mbb_common_struct{    
	int test; /* 暂时无用 */
};


/**************************************************************************
  函数定义 
**************************************************************************/

#ifndef __KERNEL__ /* C核的情况，M核请勿使用该函数 */
#if ( FEATURE_ON == MBB_MLOG )
extern void mlog_print(char *module, mlog_lv_eum loglv, const char *fmt,...);
extern void NAS_CC_MlogCampInfo(char* strFlg);
#define MLOG_MMC_PRINT_INFO(fmt)                             mlog_print("MMC",mlog_lv_info,fmt)
#define MLOG_MMC_PRINT_INFO_1(fmt, var)                      mlog_print("MMC",mlog_lv_info,fmt,var)
#define MLOG_MMC_PRINT_INFO_2(fmt, var1, var2)               mlog_print("MMC",mlog_lv_info, fmt, var1, var2)
#define MLOG_MMC_PRINT_INFO_3(fmt, var1, var2, var3)         mlog_print("MMC",mlog_lv_info, fmt, var1, var2, var3)
#define MLOG_MMC_PRINT_WARN(fmt)                             mlog_print("MMC",mlog_lv_warn,fmt)
#define MLOG_MMC_PRINT_WARN_1(fmt, var)                      mlog_print("MMC",mlog_lv_warn,fmt,var)
#define MLOG_MMC_PRINT_WARN_2(fmt, var1, var2)               mlog_print("MMC",mlog_lv_warn, fmt, var1, var2)
#define MLOG_MMC_PRINT_WARN_3(fmt, var1, var2, var3)         mlog_print("MMC",mlog_lv_warn, fmt, var1, var2, var3)
#define MLOG_MMC_PRINT_ERR(fmt)                              mlog_print("MMC",mlog_lv_error,fmt)
#define MLOG_MMC_PRINT_ERR_1(fmt, var)                       mlog_print("MMC",mlog_lv_error,fmt,var)
#define MLOG_MMC_PRINT_ERR_2(fmt, var1, var2)                mlog_print("MMC",mlog_lv_error, fmt, var1, var2)
#define MLOG_MMC_PRINT_ERR_3(fmt, var1, var2, var3)          mlog_print("MMC",mlog_lv_error, fmt, var1, var2, var3)
#define MLOG_MCALL_PRINT_INFO(fmt)                           mlog_print("MCALL",mlog_lv_info,fmt)
#define MLOG_MCALL_PRINT_INFO_1(fmt, var)                    mlog_print("MCALL",mlog_lv_info,fmt,var)
#define MLOG_MCALL_PRINT_INFO_2(fmt, var1, var2)             mlog_print("MCALL",mlog_lv_info, fmt, var1, var2)
#define MLOG_MCALL_PRINT_INFO_3(fmt, var1, var2, var3)       mlog_print("MCALL",mlog_lv_info, fmt, var1, var2, var3)
#define MLOG_MCALL_PRINT_WARN(fmt)                           mlog_print("MCALL",mlog_lv_warn,fmt)
#define MLOG_MCALL_PRINT_WARN_1(fmt, var)                    mlog_print("MCALL",mlog_lv_warn,fmt,var)
#define MLOG_MCALL_PRINT_WARN_2(fmt, var1, var2)             mlog_print("MCALL",mlog_lv_warn, fmt, var1, var2)
#define MLOG_MCALL_PRINT_WARN_3(fmt, var1, var2, var3)       mlog_print("MCALL",mlog_lv_warn, fmt, var1, var2, var3)
#define MLOG_MCALL_PRINT_ERR(fmt)                            mlog_print("MCALL",mlog_lv_error,fmt)
#define MLOG_MCALL_PRINT_ERR_1(fmt, var)                     mlog_print("MCALL",mlog_lv_error,fmt,var)
#define MLOG_MCALL_PRINT_ERR_2(fmt, var1, var2)              mlog_print("MCALL",mlog_lv_error, fmt, var1, var2)
#define MLOG_MCALL_PRINT_ERR_3(fmt, var1, var2, var3)        mlog_print("MCALL",mlog_lv_error, fmt, var1, var2, var3)
#define MLOG_UICC_PRINT_INFO(fmt)                            mlog_print("UICC", mlog_lv_info, fmt)
#define MLOG_UICC_PRINT_INFO_1(fmt,var)                      mlog_print("UICC", mlog_lv_info, fmt,var)
#define MLOG_UICC_PRINT_INFO_2(fmt,var1,var2)                mlog_print("UICC", mlog_lv_info, fmt,var1,var2)
#define MLOG_UICC_PRINT_ERR(fmt)                             mlog_print("UICC", mlog_lv_error, fmt)
#define MLOG_UICC_PRINT_ERR_1(fmt,var)                       mlog_print("UICC", mlog_lv_error, fmt,var)
#define MLOG_STK_PRINT_ERR(fmt)                              mlog_print("STK", mlog_lv_error, fmt)
#define MLOG_STK_PRINT_ERR_1(fmt,var)                        mlog_print("STK", mlog_lv_error, fmt,var)
#define MLOG_STK_PRINT_ERR_2(fmt,var1,var2)                  mlog_print("STK", mlog_lv_error, fmt,var1,var2)
#define MLOG_SMS_PRINT_ERR(fmt)                              mlog_print("SMS", mlog_lv_error, fmt)
#define MLOG_SMS_PRINT_WARN(fmt)                             mlog_print("SMS", mlog_lv_warn, fmt)
#define MLOG_VOICE_PRINT_ERR_3(fmt, var1, var2, var3)\
                do{\
                        NAS_CC_MlogCampInfo("VOICE");\
                        mlog_print("VOICE",mlog_lv_error, fmt, var1, var2, var3);/*lint -e717 */\
                }while(0) /*lint +e717 */
#define MLOG_SMS_PRINT_ERR_1(fmt, var1)\
                do{\
                    NAS_CC_MlogCampInfo("SMS");\
                    mlog_print("SMS",mlog_lv_error, fmt, var1);/*lint -e717 */\
                }while(0) /*lint +e717 */
#define MLOG_SMS_PRINT_ERR_2(fmt, var1, var2)\
                do{\
                    NAS_CC_MlogCampInfo("SMS");\
                    mlog_print("SMS",mlog_lv_error, fmt, var1, var2);/*lint -e717 */\
                }while(0) /*lint +e717 */
#define MLOG_USSD_PRINT_ERR(fmt)\
                do{\
                    NAS_CC_MlogCampInfo("USSD");\
                    mlog_print("USSD",mlog_lv_error,fmt);/*lint -e717 */\
                }while(0) /*lint +e717 */
#define MLOG_USSD_PRINT_ERR_1(fmt, var)\
                do{\
                    NAS_CC_MlogCampInfo("USSD");\
                    mlog_print("USSD",mlog_lv_error,fmt,var);/*lint -e717 */\
                }while(0) /*lint +e717 */

#else
#define mlog_print
#define MLOG_MMC_PRINT_INFO(fmt) 
#define MLOG_MMC_PRINT_INFO_1(fmt, var)   
#define MLOG_MMC_PRINT_INFO_2(fmt, var1, var2) 
#define MLOG_MMC_PRINT_INFO_3(fmt, var1, var2, var3)
#define MLOG_MMC_PRINT_WARN(fmt)                  
#define MLOG_MMC_PRINT_WARN_1(fmt, var)      
#define MLOG_MMC_PRINT_WARN_2(fmt, var1, var2)          
#define MLOG_MMC_PRINT_WARN_3(fmt, var1, var2, var3)   
#define MLOG_MMC_PRINT_ERR(fmt)                       
#define MLOG_MMC_PRINT_ERR_1(fmt, var)              
#define MLOG_MMC_PRINT_ERR_2(fmt, var1, var2)                  
#define MLOG_MMC_PRINT_ERR_3(fmt, var1, var2, var3)           
#define MLOG_MCALL_PRINT_INFO(fmt)      
#define MLOG_MCALL_PRINT_INFO_1(fmt, var)              
#define MLOG_MCALL_PRINT_INFO_2(fmt, var1, var2)                  
#define MLOG_MCALL_PRINT_INFO_3(fmt, var1, var2, var3)         
#define MLOG_MCALL_PRINT_WARN(fmt)  
#define MLOG_MCALL_PRINT_WARN_1(fmt, var)                
#define MLOG_MCALL_PRINT_WARN_2(fmt, var1, var2)      
#define MLOG_MCALL_PRINT_WARN_3(fmt, var1, var2, var3)         
#define MLOG_MCALL_PRINT_ERR(fmt)  
#define MLOG_MCALL_PRINT_ERR_1(fmt, var)                       
#define MLOG_MCALL_PRINT_ERR_2(fmt, var1, var2)             
#define MLOG_MCALL_PRINT_ERR_3(fmt, var1, var2, var3)  
#define MLOG_UICC_PRINT_INFO(fmt)                                     
#define MLOG_UICC_PRINT_INFO_1(fmt,var)                                     
#define MLOG_UICC_PRINT_INFO_2(fmt,var1,var2)                                     
#define MLOG_UICC_PRINT_ERR(fmt)                                     
#define MLOG_UICC_PRINT_ERR_1(fmt,var)                               
#define MLOG_STK_PRINT_ERR(fmt)                               
#define MLOG_STK_PRINT_ERR_1(fmt,var)                               
#define MLOG_STK_PRINT_ERR_2(fmt,var1,var2)                               
#define MLOG_SMS_PRINT_ERR(fmt)                               
#define MLOG_SMS_PRINT_WARN(fmt)                               
#define MLOG_VOICE_PRINT_ERR_3(fmt, var1, var2, var3)
#define MLOG_SMS_PRINT_ERR_1(fmt, var1)
#define MLOG_SMS_PRINT_ERR_2(fmt, var1, var2)
#define MLOG_USSD_PRINT_ERR(fmt)
#define MLOG_USSD_PRINT_ERR_1(fmt, var)    
#endif
#endif


CRADLE_TYPE chg_get_cradletype_from_usbid_voltage(void);

#ifdef __cplusplus
}
#endif

#endif

