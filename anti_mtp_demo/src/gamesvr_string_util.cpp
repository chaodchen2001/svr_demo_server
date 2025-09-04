#include "gamesvr_string_util.h"

int
GamesvrStringUtil::hex_to_str(const unsigned char *buf, unsigned int buf_len, char *dest, unsigned int dest_len)
{
    if (dest_len < buf_len * 2 + 1)
    {
        return -1;
    }

    unsigned short *p_dest = (unsigned short *)dest;

    for (unsigned int i = 0; i < buf_len; ++ i)
    {
        *p_dest = single_hex_to_str(buf[i]);
        p_dest++;
    }

    // 字符串结尾
    *(char *)p_dest = 0;
    return 0;
}

unsigned short
GamesvrStringUtil::single_hex_to_str(unsigned char h)
{
    // 16进制对应的字符
    static const char *tag_8_upper =
        "000102030405060708090A0B0C0D0E0F"
        "101112131415161718191A1B1C1D1E1F"
        "202122232425262728292A2B2C2D2E2F"
        "303132333435363738393A3B3C3D3E3F"
        "404142434445464748494A4B4C4D4E4F"
        "505152535455565758595A5B5C5D5E5F"
        "606162636465666768696A6B6C6D6E6F"
        "707172737475767778797A7B7C7D7E7F"
        "808182838485868788898A8B8C8D8E8F"
        "909192939495969798999A9B9C9D9E9F"
        "A0A1A2A3A4A5A6A7A8A9AAABACADAEAF"
        "B0B1B2B3B4B5B6B7B8B9BABBBCBDBEBF"
        "C0C1C2C3C4C5C6C7C8C9CACBCCCDCECF"
        "D0D1D2D3D4D5D6D7D8D9DADBDCDDDEDF"
        "E0E1E2E3E4E5E6E7E8E9EAEBECEDEEEF"
        "F0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF";
    static const char *tag_8 = tag_8_upper;
    unsigned short *p_src = (unsigned short *)tag_8;
    return p_src[h];
}

