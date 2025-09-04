#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

#include "ace_sdk_anti_mtp.h"
#include "ace_mtp_sdk.h"
#include "antimtp_util.h"

// 测试用，游戏侧需替换为自己的game_id、secret_id、secret_key
unsigned int g_gameid_ = 20059;
char g_secret_id_[] = "3805C9C21D515E26";
char g_secret_key_[] = "09627ad140016536e626204bfd50b5ad";

void test1_send_anti_data(const AceSdkMtpAntiInterf* anti_interf);
void test2_send_many_data_trigger_bandwidth_limit(const AceSdkMtpAntiInterf* anti_interf);
void test_send_data4(const AceSdkMtpAntiInterf* anti_interf);
void test_get_light_feature(const AceSdkMtpAntiInterf* anti_interf);

const char *get_lib_path()
{
    const char *lib_path;
#if defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)
#if defined(_WIN64) || defined(WIN64)
    lib_path = "../../../lib/win64";
#else
    lib_path = "../../../lib/win32";
#endif // defined(_WIN64) || defined(WIN64)
#else
    lib_path = "../lib/linux64";
#endif // defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)
    return lib_path;
}


int main()
{
    /* 1.初始化acesdk */
    int ret = 0;
    AceSdkMTPInitInfo init_info;
    // 请设置为libace_sdk.so和libace_strategy.so所在路径，需在同一路径下
    init_info.ace_sdk_lib_dir_ = get_lib_path();
    // 请设置为分配该游戏的 game_id，ace_sdk根据该字段区分不同的游戏
    init_info.game_id_ = g_gameid_;

    ret = ace_sdk_load_and_init(&init_info);  // 连国内环境
    // ret = ace_sdk_load_and_init_asia(&init_info);  // 连国际服环境（新加坡）
    
    if (ret != 0)
    {
        LOG_ERROR("fail to load ace sdk");
        return 0;
    }
    LOG_INFO("succ to load ace sdk");


    /* 2.获取数据上报接口 */
    AceSdkMtpAntiInitInfo mtp_anti_init_info;
    snprintf(mtp_anti_init_info.secret_id, sizeof(mtp_anti_init_info.secret_id), "%s", g_secret_id_);
    snprintf(mtp_anti_init_info.secret_key, sizeof(mtp_anti_init_info.secret_key), "%s", g_secret_key_);
    const AceSdkMtpAntiInterf* anti_interf = ACE_SDK_GET_ANTI_MTP_INTERF(&mtp_anti_init_info);
    if (anti_interf == NULL)
    {
        LOG_ERROR("get anti interface fail");
        return -1;
    }
    LOG_INFO("--------- get interface success --------- ");
    anti_sleep(1 * 1000);
    

    /* 3.设置限流，可以不设，初始化sdk时已设置默认值 */
    ret = anti_interf->set_bandwidth_limit_(1);
    if (ret != 0)
    {
        LOG_ERROR("fail to set_bandwidth_limit_");
        return -1;
    } 
    LOG_INFO("succ to set_bandwidth_limit_");

    /* 4.循环发data1和data2 */
    test1_send_anti_data(anti_interf);

    /* 5.循环发data4 */
    test_send_data4(anti_interf);

    /* 6.循环获取轻特征 */
    test_get_light_feature(anti_interf);

    /* 7.发大量数据触发限流 */
    test2_send_many_data_trigger_bandwidth_limit(anti_interf);

    anti_sleep(1 * 1000);

    ace_sdk_unload();
    LOG_INFO("succ to unload ace sdk");
    
    return 0;
}

// !!!! 此函数仅为ACE侧提供的data1和data4数据字符串转二进制流参考，与客户端demo二进制流转字符串相对应
// !!!! 若游戏客户端以自定义格式上报data1和data4二进制数据，则忽略此函数，自定义逆向处理即可
int hex_str_to_bytes(const char *data_str, unsigned char *buff, unsigned int &buf_len)
{
    if (data_str == NULL || buff == NULL)
    {
        // 参数不对
        return -1;
    }

    unsigned idx = 0;
    char temp_char = 0;
    int temp = 0;
    unsigned int data_str_len = (unsigned int)strlen(data_str);

    if (data_str_len > (unsigned int)2 * buf_len)
    {
        // buff空间太小存不下
        return -2;
    }

    for (unsigned int i = 0; i < data_str_len; ++ i)
    {
        temp_char = data_str[i];

        if (temp_char <= '9' && temp_char >= '0')
        {
            temp_char -= 0x30;
        }
        else
        {
            if (temp_char >= 'A' && temp_char <= 'F')
            {
                temp_char -= 0x37;
            }
            else
            {
                if (temp_char >= 'a' && temp_char <= 'f')
                {
                    temp_char -= 0x57;
                }
                else
                {
                    temp_char = 0;
                }
            }
        }

        temp += temp_char;

        if ((i % 2) != 0)
        {
            buff[idx] = (unsigned char)temp;
            temp = 0;
            idx ++;
        }
        else
        {
            temp *= 0x10;
        }
    }

    buf_len = idx;
    return 0;
}

void test1_send_anti_data(const AceSdkMtpAntiInterf* anti_interf)
{
    int ret = 0;
    AceSdkMtpAntiData input_data;
    memset(&input_data, 0, sizeof(input_data));
    // [必填(Required)] account_: 用户账号(user account)
    snprintf(input_data.account_info_.account_id_.account_, sizeof(input_data.account_info_.account_id_.account_) - 1, "%s", "ace_sdk_test");
    // [必填(Required)] account_type_
    input_data.account_info_.account_id_.account_type_ = DEFAULT_MBGAME_ACCOUNT_TYPE;
    // [必填(Required)] game_id:  游戏id(game_id)
    input_data.account_info_.game_id_ = g_gameid_;
    // [必填(Required)] plat_id:  平台id(plat_id)
    input_data.account_info_.plat_id_ = ACEPLAT_ID_ANDROID;
    input_data.account_info_.world_id_ = 0;
    input_data.account_info_.channel_id_ = 0;
    input_data.account_info_.role_id_ = 0;
    char anti_data_str[1024] = "0000000100C70112236D0000000000000001000000000000005CDA5AAC0000EB8C00000000000000000001B20700004CDD000000153B696E635F69643A31333B6F62665F69643A31330000000003000000214246303132363235313745364145303242463141334242344243433135354642000000000100000000093A7A79676F74653A000000000700000000000000000000000000000000000000000000000200000000000000020000000000000000000000000000004506030000040400BBD437899A0000";
    unsigned char anti_data_buff[1024];
    unsigned int anti_buff_len = sizeof(anti_data_buff);
    ret = hex_str_to_bytes(anti_data_str, anti_data_buff, anti_buff_len);
    if (ret != 0)
    {
        LOG_ERROR("anti data convert error, ret:%d", ret);
        return;
    }
    
    input_data.anti_data_ = (const unsigned char*)anti_data_buff;
    input_data.anti_data_len_ = anti_buff_len;

    /* 1 发data1 */
    int loop_cyles = 1;
    do
    {
        LOG_DEBUG("before send data1 interf, game_id:%u, account:%s, account_type:%hu, plat_id:%hu, anti_data_len:%u",
                 input_data.account_info_.game_id_,
                 input_data.account_info_.account_id_.account_,
                 input_data.account_info_.account_id_.account_type_,
                 input_data.account_info_.plat_id_,
                 input_data.anti_data_len_);
        ret = anti_interf->send_anti_data1_(&input_data);
        if (ret != ACE_SDK_PROC_OK)
        {
            LOG_ERROR("send data1 failed, ret:%d", ret);
            continue;
        }
        LOG_INFO("send data1 succeed!");

    }while (loop_cyles-- && anti_interf != NULL);

    /* 2 发data2 */
    char anti_data2_str[1024] = "49FF80FB0000A018042B0200111000000059000400510000E1B132FC05014D5341762514000000000043402867008d9700000000000000ff0000010030d52ee6";
    input_data.anti_data_ = (const unsigned char*)anti_data2_str;
    input_data.anti_data_len_ = strlen(anti_data2_str);
    loop_cyles = 1;
    do
    {
        LOG_DEBUG("before send data2 interf, game_id:%u, account:%s, account_type:%hu, plat_id:%hu, anti_data_len:%u",
                 input_data.account_info_.game_id_,
                 input_data.account_info_.account_id_.account_,
                 input_data.account_info_.account_id_.account_type_,
                 input_data.account_info_.plat_id_,
                 input_data.anti_data_len_);
        ret = anti_interf->send_anti_data2_(&input_data);
        if (ret != ACE_SDK_PROC_OK)
        {
            LOG_ERROR("send data2 failed, ret:%d", ret);
            continue;
        }
        LOG_INFO("send data2 succeed!");

    }while (loop_cyles-- && anti_interf != NULL);
}

void test2_send_many_data_trigger_bandwidth_limit(const AceSdkMtpAntiInterf* anti_interf)
{
    int ret = 0;
    AceSdkMtpAntiData input_data;
    memset(&input_data, 0, sizeof(input_data));
    // [必填(Required)] account_: 用户账号(user account)
    snprintf(input_data.account_info_.account_id_.account_, sizeof(input_data.account_info_.account_id_.account_) - 1, "%s", "ace_sdk_test");
    // [必填(Required)] account_type_
    input_data.account_info_.account_id_.account_type_ = DEFAULT_MBGAME_ACCOUNT_TYPE;
    // [必填(Required)] game_id:  游戏id(game_id)
    input_data.account_info_.game_id_ = g_gameid_;
    // [必填(Required)] plat_id:  平台id(plat_id)
    input_data.account_info_.plat_id_ = ACEPLAT_ID_ANDROID;
    input_data.account_info_.world_id_ = 0;
    input_data.account_info_.channel_id_ = 0;
    input_data.account_info_.role_id_ = 0;
    
    /*
    *   1 Mbps = 每秒发 131072 Bytes；如下AntiData为17500 Bytes，若限流1Mbps, 发8次就应该触发限流
    *   触发限流时，预期在/tmp/tss_sdk.log文件中出现类似如下ERROR日志
    *       [ERROR][PID:11712][TID:210228992][bool tss_sdk_impl::TssSdkOverLoadProTect::protect(uint32_t, uint64_t, uint32_t)] over_load_protect, gameid:19677, cmd:56000000, stat_bytes:142664, overload_thred_bytes_:131072
    */
    std::string anti_data = "";
    for (size_t i = 0; i < 175; i++)
    {
        anti_data += "49FF80FB0000A018042B0200111000000059000400510000E1B132FC05014D5341762514000000000043402867008d970000";
    }
    
    input_data.anti_data_ = (const unsigned char*)anti_data.c_str();
    input_data.anti_data_len_ = anti_data.size();

    int loop_cyles = 20;
    for (int i = 0; i < loop_cyles && anti_interf != NULL; i++)
    {
        ret = anti_interf->send_anti_data1_(&input_data);
        if (ret != ACE_SDK_PROC_OK)
        {
            LOG_ERROR("send data1 failed, ret:%d", ret);
            continue;
        }
        LOG_INFO("send data1 succeed! anti_data_len:%u, i:%d", input_data.anti_data_len_, i);
        
        if (i == 9)
        {
            LOG_INFO("--------- sleep 6s wait bandwidth limit clear, i:%d ---------", i);
            // sdk限流日志5s打一次
            anti_sleep(6 * 1000);
        }
    }
}

void test_send_data4(const AceSdkMtpAntiInterf* anti_interf)
{
    int ret = 0;
    AceSdkMtpAntiSecScanData input_data;
    memset(&input_data, 0, sizeof(input_data));
    // [必填(Required)] account_: 用户账号(user account)
    snprintf(input_data.account_info_.account_id_.account_, sizeof(input_data.account_info_.account_id_.account_) - 1, "%s", "ace_sdk_test");
    // [必填(Required)] account_type_
    input_data.account_info_.account_id_.account_type_ = DEFAULT_MBGAME_ACCOUNT_TYPE;
    // [必填(Required)] game_id:  游戏id(game_id)
    input_data.account_info_.game_id_ = g_gameid_;
    // [必填(Required)] plat_id:  平台id(plat_id)
    input_data.account_info_.plat_id_ = ACEPLAT_ID_ANDROID;
    input_data.account_info_.world_id_ = 0;
    input_data.account_info_.channel_id_ = 0;
    input_data.account_info_.role_id_ = 0;

    char anti_data_str[2048] = "78563412A6C91A578936BF75F1CFF1D5CA3120513FF9A47E2AAF3BB83A091B2E2104174454A6C91A75D177E1ED89C0E74DE005292366F2BBF14DED4775664816056E2A390750A2C9B03B8F379FEBB9D42C50EA1BC3354CAA70E956FB5A5B79571A5D610C915958B31D9F75D1CFE3E3A30F7842F3A2BC727ECD6CFE5259592738482A797D94C90466D66DEC1D87DF96CBD610226D43F2B73A45EE7EC368514E2771173B1A5095C20B2D8E2DFFE5B0CFEB50C00B2929758EFAE648F7666E340D5777770A222560C591B478816F9AA6A485751AB701BB763AAE26E965FA4F7F6B0F6400264F9E5E13C925922D92C7E3E4B40F2A51C3F9BE7B53AE21B4556B1E716A156F4448B3EE2D7EB713E012B9C4C5C5FB5607416F9DF32436F27EC14A7E6F4A442E02486095DA012B850DA8E1A58E925FD506272A67BBC3E362F12866404F6E5967456614619DD99F298D3C9FED89C01362D15ABA3A548E45C365F34E27757306772F4F9E5D41C86FE9799CF0C7E3B42F2450E9F2B37144A72CC93D7A6F2A303D150351B5EF2B13D11CE47DF9F09BA6C353711435FDBF0335D854BD3C0B69285341140017B49F587BA16F97D584819010B056737E7E8BD9D939AE252A3F407B5F753637054183EFBF3CDD68DEC1ABD40B42EC02F97B61A078A370FB45796C52221A6608D03A62B364E871D5D197A9F555392381CF8A46029E158D043C2B1A0978563412F0AC6824E059D148C0B3A2918167452302CF8A46049E158D093C2B1A12785634004FF1037B7C4E798865F221CB459409EE3CC760AEA8CC758B9F38B189831C181036300248BE7DB9CAA8E1ECF3F871A44B3962E6B468DAE7AE0976E738D54819B22108E9B101869669ACB30EF4BE3137757076EEA0C902C03146CFAEBD75FECB98068EA8BD65EC57A32F4304F681DD473BA0799C94F85770F0A72090B5FD87C09D35973865C37907EBB767BC8EDE2831DEDEF39D730AB2A6E75C435AA335133B5E7E9AA74C72726CF04EB54A6E00D95F6F43AB4845408A7C0AFF1B88C0CB0DA7FCBEB2D3C4E167FCD5E2230563371571B6BF2241A805039971A83997AEEDA5869641AF5A8605CF218FADB40C65C83D6492915072706FDAB9491E5AE66EACD3E4E0";
    unsigned char anti_data_buff[2048];
    unsigned int anti_buff_len = sizeof(anti_data_buff);
    ret = hex_str_to_bytes(anti_data_str, anti_data_buff, anti_buff_len);
    if (ret != 0)
    {
        LOG_ERROR("anti data convert error, ret:%d", ret);
        return;
    }
    AceSdkSecScanInfo sec_scan_info;
    sec_scan_info.scan_status_ = 1;
    sec_scan_info.scan_data_len_ = anti_buff_len;
    sec_scan_info.scan_data_ = (const unsigned char*)anti_data_buff;
    input_data.sec_scan_info_ = (const struct AceSdkSecScanInfo *)&sec_scan_info;

    // 用于获取返回字符串的buf，推荐长度4096
    char result_data_buf[4096];
    input_data.result_data_buf_ = result_data_buf;
    input_data.data_buf_len_ = sizeof(result_data_buf);
    input_data.data_type_ = REPORT_DATA4_JSON;

    /* 发data4 */
    int loop_cyles = 1;
    do
    {
        LOG_DEBUG("before send data4 interf, game_id:%u, account:%s, account_type:%hu, plat_id:%hu, scan_data_len:%hu, data_buf_len:%u",
                 input_data.account_info_.game_id_, input_data.account_info_.account_id_.account_, input_data.account_info_.account_id_.account_type_,
                 input_data.account_info_.plat_id_, input_data.sec_scan_info_->scan_data_len_, input_data.data_buf_len_);
        ret = anti_interf->send_anti_data4_(&input_data);
        if (ret != ACE_SDK_PROC_OK)
        {
            LOG_ERROR("send data4 failed, ret:%d", ret);
            continue;
        }
        LOG_INFO("send data4 succeed! data_buf_len=%u, result_data_buf_=%s", input_data.data_buf_len_, input_data.result_data_buf_);

    }while (loop_cyles-- && anti_interf != NULL);
}

void test_get_light_feature(const AceSdkMtpAntiInterf* anti_interf)
{
    int ret = 0;
    AceSdkMtpAntiSecSignatureData input_data;
    memset(&input_data, 0, sizeof(input_data));
    // [必填(Required)] account_: 用户账号(user account)
    snprintf(input_data.account_info_.account_id_.account_, sizeof(input_data.account_info_.account_id_.account_) - 1, "%s", "ace_sdk_test");
    // [必填(Required)] account_type_
    input_data.account_info_.account_id_.account_type_ = DEFAULT_MBGAME_ACCOUNT_TYPE;
    // [必填(Required)] game_id:  游戏id(game_id)
    input_data.account_info_.game_id_ = g_gameid_;
    // [必填(Required)] plat_id:  平台id(plat_id)
    input_data.account_info_.plat_id_ = ACEPLAT_ID_ANDROID;
    input_data.account_info_.world_id_ = 0;
    input_data.account_info_.channel_id_ = 0;
    input_data.account_info_.role_id_ = 0;

    input_data.cpu_arch_ = ARCH_ARM64;

    AceSdkSecSignatureInfo sec_sign_info;
    memset(&sec_sign_info, 0, sizeof(sec_sign_info));
    char name_buf[64] = {0};
    // 用于获取特征的buf，长度≥64KByte
    static const int SIG_BUF_LENGTH = 64 * 1024;
    unsigned char *sig_buf = new unsigned char[SIG_BUF_LENGTH];
    if (sig_buf == NULL)
    {
        LOG_ERROR("malloc sig_buf fail");
        return;
    }
    sec_sign_info.name_buf = name_buf;
    sec_sign_info.name_buf_len = sizeof(name_buf);
    sec_sign_info.sig_buf = sig_buf;
    sec_sign_info.sig_buf_len = SIG_BUF_LENGTH;
    sec_sign_info.sig_size = 0;
    sec_sign_info.sig_crc = 0;
    input_data.sec_sign_info_ = &sec_sign_info;

    /* 获取轻特征 */
    int loop_cyles = 1;
    do
    {
        LOG_DEBUG("before get light feature interf, game_id:%u, account:%s, account_type:%hu, plat_id:%hu, name_buf_len:%u, sig_buf_len:%u",
                 input_data.account_info_.game_id_, input_data.account_info_.account_id_.account_, input_data.account_info_.account_id_.account_type_,
                 input_data.account_info_.plat_id_, input_data.sec_sign_info_->name_buf_len, input_data.sec_sign_info_->sig_buf_len);
        ret = anti_interf->get_light_feature_(&input_data);
        if (ret != ACE_SDK_PROC_OK)
        {
            LOG_ERROR("get light feature failed, ret:%d", ret);
            continue;
        }
        LOG_INFO("get light feature succeed! name_buf=%s, sig_size=%u, sig_crc=%u",
                 input_data.sec_sign_info_->name_buf, input_data.sec_sign_info_->sig_size, input_data.sec_sign_info_->sig_crc);
        // 首次调用时会触发拉取轻特征到本地，完成拉取之前获取不到特征
        if (input_data.sec_sign_info_->sig_size == 0)
        {
            LOG_INFO("get light feature succeed! But there is no light feature currently");
        }
        
        input_data.sec_sign_info_->sig_size = 0;
        input_data.sec_sign_info_->sig_crc = 0;
        anti_sleep(1 * 1000);
    }while (loop_cyles-- && anti_interf != NULL);
}
