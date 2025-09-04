#ifndef __GAMESVR_STRING_UTIL_H__
#define __GAMESVR_STRING_UTIL_H__

class GamesvrStringUtil
{
public:
    // 将一段二进制数据以16进制打印转成字符串
    static int hex_to_str(const unsigned char *buf, unsigned int buf_len, char *dest, unsigned int dest_len);

private:
    static unsigned short single_hex_to_str(unsigned char h);
};

#endif

