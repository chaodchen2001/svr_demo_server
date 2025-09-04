/**
 * @copyright:  2019 Ace. All Rights Reserved.
 * @filename:   ace_sdk_account_busi.h
 * @version:    1.0.0
 * @brief:      This header file provides the definition of AceSDK account.
 * @details:
 */
#ifndef __ACE_SDK_ACCOUNT_BUSI_H__
#define __ACE_SDK_ACCOUNT_BUSI_H__

#include <stdio.h>
#include <string.h>

#pragma pack(1)
/* the enumeration of AceSdk account type */
enum AceAccountType
{
    DEFAULT_MBGAME_ACCOUNT_TYPE = 6,                            // If only account_ can ensure that AceAccountId is unique, then account_type_ can fill in this default value
    ACCOUNT_TYPE_USER_DEFINE_START = 100,                       // The beginning of user-defined account_type
    ACCOUNT_TYPE_GUEST = 101,                                   // eg：guest account_type
    ACCOUNT_TYPE_USER_DEFINE_END = 199,                         // The end of user-defined account_type
};

/* the enumeration of AceSdk plat id */
enum AceAccountPlatId
{
    ACEPLAT_ID_IOS = 0,                                     // iOS
    ACEPLAT_ID_ANDROID = 1,                                 // Android
    ACEPLAT_ID_RESERVE = 2,                                 // reserved
    ACEPLAT_ID_PC_CLIENT = 3,                               // PC client
    ACEPLAT_ID_MICRO_WEB = 4,                               // Micro web
    ACEPLAT_ID_MICRO_CLIENT = 5,                            // Micro client
    ACEPLAT_ID_SWITCH = 6,                                  // Switch client
    ACEPLAT_ID_PS_CLIENT = 7,                               // PS client
    ACEPLAT_ID_XBOX_CLIENT = 8,                             // XBOX client
    ACEPLAT_ID_UNKNOWN = 9,
};

enum AceAccountWorldId
{
    DEFAULT_MBGAME_WORLDID = 0,
};

enum
{
    MAX_ACE_ACCOUNT_INFO_ACCOUNT_LEN = 65,
    MAX_ACE_ACCOUNT_INFO_RESERVE_LEN = 11,
};

/* The struct of basic account info */
typedef struct
{
    char account_[MAX_ACE_ACCOUNT_INFO_ACCOUNT_LEN];        // The account of user, and need to ensure that the account is unique in the game
    unsigned short account_type_;                           // The account type of user, bind to AceAccountType
} AceAccountId;

/* The struct of user account info */
typedef struct
{
    AceAccountId account_id_;                               // The basic account info of user
    unsigned short plat_id_;                                // The plat info, bind to AceAccountPlatId
    unsigned int game_id_;                                  // The game id
    unsigned int world_id_;                                 // The world id of user
    unsigned int channel_id_;                               // The channel id of user,lease fill in area_id, WeChat (1), mobile QQ (2), tourist (3)
    unsigned long long role_id_;                            // The role id of user
    char reserve_buf_[MAX_ACE_ACCOUNT_INFO_RESERVE_LEN];    // reserved data
} AceAccountInfo;

#define ACE_SDK_USER_EXT_DATA_MAX_LEN 1024
/* The struct of user extend data */
typedef struct
{
    char user_ext_data_[ACE_SDK_USER_EXT_DATA_MAX_LEN];     // The extend data of user
    unsigned int ext_data_len_;                             // The length of extend data
} AceSdkUserExtData;

#pragma pack()

#endif /* end of __ACE_SDK_ACCOUNT_BUSI_H__ */

