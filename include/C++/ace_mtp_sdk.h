/**
 * @copyright:  2023 Ace. All Rights Reserved.
 * @filename:   ace_mtp_sdk.h
 * @version:    1.0.1
 * @brief:      This header file provides the interface of loading and unloading AceSDK.
 * @details:
 */

#ifndef __ACE_MTP_SDK_H__
#define __ACE_MTP_SDK_H__

#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

#if (defined(WIN32) || defined(_WIN64))

#include <tchar.h>

#if _MSC_VER >= 1300
typedef unsigned long long  ACE_UINT64;
typedef long long   ACE_INT64;
#else /* _MSC_VER */
typedef unsigned __int64    ACE_UINT64;
typedef __int64 ACE_INT64;
#endif /* _MSC_VER */

typedef TCHAR ACE_TCHAR;

#else // (defined(WIN32) || defined(_WIN64))

#include <stdint.h>
typedef uint64_t ACE_UINT64;
typedef int64_t ACE_INT64;
typedef char ACE_TCHAR;

#endif

#pragma pack(8)

/* AceSdk general processing results */
typedef enum
{
    ACE_SDK_PROC_OK = 0,                        // Processing Successfully
    ACE_SDK_PROC_INVALID_ARG = 1,               // Invalid parameters
    ACE_SDK_PROC_INTERNAL_ERR = 2,              // AceSdk Internal error
    ACE_SDK_PROC_FAIL = 3,                      // Processing failed
} AceSdkProcResult;

enum
{
    MAX_ACE_AUTH_TOKEN_LEN = 16,
};

/* The struct of AceSdk initialization parameter */
typedef struct
{
    const char *ace_sdk_lib_dir_;              // [IN] the path of AceSdk lib file
    unsigned int game_id_;                      // [IN] the game id assigned by Ace security team
} AceSdkMTPInitInfo;

#pragma pack()

/*
 *    @brief To load and initialize, connect to environment in China
 *
 *    @param [IN] shared_lib_dir   The file path of AceSdk
 *    @return 0                    Load and initialize AceSdk successfully
 *    @return 5043001              Load AceSdk successfully, while the network to AceSdk back-end server is unreachable
 *    @return other value          Fail to load and initialize AceSdk
 */
int ace_sdk_load_and_init(const AceSdkMTPInitInfo *init_info);

/*
 *    @brief To load and initialize, connect to environment in Singapore
 *
 *    @param [IN] shared_lib_dir   The file path of AceSdk
 *    @return 0                    Load and initialize AceSdk successfully
 *    @return 5043001              Load AceSdk successfully, while the network to AceSdk back-end server is unreachable
 *    @return other value          Fail to load and initialize AceSdk
 */
int ace_sdk_load_and_init_asia(const AceSdkMTPInitInfo *init_info);

/*
 *    @brief To unload AceSdk
 *    @return 0                    Unload AceSdk successfully
 *    @return other value          Fail to unload AceSdk
 */
int ace_sdk_unload();

/*
 * To get some AceSdk interface
 * This function should not be called directly, use the specific macro to get corresponding interface instead, such as ACE_SDK_GET_ANTI_STRIP_INTERF
 */
const void *ace_sdk_get_busi_interf(const char *syml_name, const void *data);

#ifdef __cplusplus
} /* end of extern "C" */
#endif

#endif /* end of __ACE_MTP_SDK_H__ */

