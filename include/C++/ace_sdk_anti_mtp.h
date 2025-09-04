/**
 * @copyright:  2022 Ace. All Rights Reserved.
 * @filename:   ace_sdk_anti_mtp.h
 * @version:    1.0.0
 * @brief:      This header file provides the Mtp-Anti AceSDK interface.
 * @details:
 */

#ifndef __ACE_SDK_ANTI_MTP_H__
#define __ACE_SDK_ANTI_MTP_H__

#include "ace_mtp_sdk.h"
#include "ace_sdk_account_busi.h"

#ifdef __cplusplus
extern "C"
{
#endif /* end of __cplusplus */

#pragma pack(1)

enum
{
    MAX_ACE_SECRET_ID_LEN = 32,
    MAX_ACE_SECRET_KEY_LEN = 64,
};

enum ReportData4ReturnType
{
    REPORT_DATA4_JSON = 1,
};

enum CpuArchForLightFeature
{
    ARCH_ARM32 = 1,
    ARCH_ARM64 = 2,
    ARCH_X86   = 3,
    ARCH_X64   = 4,
    ARCH_TEST  = 88,
    ARCH_OTHER = 99,
};

/* Get buf from gamesvr data1 & data2 */
typedef struct
{
    AceAccountInfo account_info_;                           // The struct of user account info
    const unsigned char *anti_data_;                        // Buf from gamesvr
    unsigned int anti_data_len_;                            // Buf length              
} AceSdkMtpAntiData;

typedef struct
{
    char secret_id[MAX_ACE_SECRET_ID_LEN];                        // Product ID
    char secret_key[MAX_ACE_SECRET_KEY_LEN];                      // Product Key
} AceSdkMtpAntiInitInfo;

/* Get buf from gamesvr data4 */
struct AceSdkSecScanInfo
{
	int scan_status_;
	unsigned short scan_data_len_;
	const unsigned char *scan_data_;
};

typedef struct
{
    AceAccountInfo account_info_;                           // The struct of user account info
    const struct AceSdkSecScanInfo *sec_scan_info_;         // data4 from gamesvr
    char *result_data_buf_;                                 
    unsigned int data_buf_len_;                             
    int data_type_;                                         
} AceSdkMtpAntiSecScanData;

struct AceSdkSecSignatureInfo
{
	char *name_buf;         // buffer for signature name
    unsigned int name_buf_len;    // length of name_buf (>= 32 byte)
	unsigned char *sig_buf; // buffer for signature data
    unsigned int sig_buf_len;     // length of sig_buf (>= 64k)
	unsigned int sig_size;        // size of signature
	unsigned int sig_crc;       // crc of signature
};

typedef struct
{
    AceAccountInfo account_info_;                   // The struct of user account info
    int cpu_arch_;                                    
    struct AceSdkSecSignatureInfo *sec_sign_info_;  
} AceSdkMtpAntiSecSignatureData;

/*
 *    @brief The function to notify AceSdk to send anti data
 *
 *    @param [IN] anti_data   AceSdkMtpAntiData
 *    @return 0               Processing Successfully
 *    @return other value     Processing Failed
 */
typedef AceSdkProcResult(*AceSdkMtpSendAntiData)(AceSdkMtpAntiData *anti_data);

/*
 *    @brief The function to set the bandwidth limit of Mtp AceSdk
 *    @brief If not set, the default bandwidth limit of AceSdk is 80Mbps
 *
 *    @param [IN] bandwidth_limit_mbps    unit:Mbps;
 *                eg: bandwidth_limit_mbps=N, N Mbps means the reportable data size per second is N*1024*1024 bits = N*1024*1024/8 Bytes
 *    @return 0               Processing Successfully
 *    @return other value     Processing Failed
 */
typedef AceSdkProcResult(*AceSdkMtpSetBandwidthLimit)(unsigned int bandwidth_limit_mbps);

/*
 *    @brief The function to send anti sec scan data to AceSdk and return data buf
 *
 *    @param [IN] anti_data   AceSdkMtpAntiSecScanData
 *    @return 0               Processing Successfully
 *    @return other value     Processing Failed
 */
typedef AceSdkProcResult(*AceSdkMtpSendAntiSecScanData)(AceSdkMtpAntiSecScanData *anti_data);

/*
 *    @brief The function to get light feature from AceSdk
 *
 *    @param [IN] anti_data   AceSdkMtpAntiSecSignatureData
 *    @return 0               Processing Successfully
 *    @return other value     Processing Failed
 */
typedef AceSdkProcResult(*AceSdkMtpGetLightFeature)(AceSdkMtpAntiSecSignatureData *anti_data);

/* The interfaces of handling AceSdk anti data */
typedef struct
{
    AceSdkMtpSendAntiData send_anti_data1_;
    AceSdkMtpSendAntiData send_anti_data2_;
    AceSdkMtpSetBandwidthLimit set_bandwidth_limit_;
    AceSdkMtpSendAntiSecScanData send_anti_data4_;
    AceSdkMtpGetLightFeature get_light_feature_;
} AceSdkMtpAntiInterf;

#define ACE_SDK_GET_ANTI_MTP_INTERF(init_data) \
    (const AceSdkMtpAntiInterf*)ace_sdk_get_busi_interf("ace_sdk_get_anti_mtp_interf", (const AceSdkMtpAntiInitInfo *)(init_data))
#pragma pack()

#ifdef __cplusplus
} /* end of extern "C" */
#endif /* end of __cplusplus */

#endif /* __ACE_SDK_ANTI_MTP_H__ */

