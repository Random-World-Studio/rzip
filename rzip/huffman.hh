/**
 * @file huffman.hh
 * @author pointer-to-bios (pointer-to-bios@outlook.com)
 * @brief
 * @version 0.1
 * @date 2022-11-28
 *
 * @copyright Copyright (c) 2022 Random World Studio
 *
 */

#ifndef huffman_hh
#define huffman_hh 1

#include <iostream>

typedef unsigned char u8;
typedef __uint64_t u64;

struct btnode
{
    unsigned char val; //字符值
    btnode *prev, *last;
}; //只有叶节点的val代表值

struct bits
{
    u8 *pointer;
    u64 length; //比特位数量
    u64 size;   //占用空间

    bits() : pointer(new u8), length(0), size(1) {}

    void append(bool bit); //在结尾添加比特位
    bool cut();            //删除结尾的比特位并返回此位的值

    void operator=(bits b);
};

struct bitblock
{
    u64 block;
    bitblock *prev, *next;
    static const u8 len = 64;
};

//比特流
class bitstream
{
private:
    bitblock *begin, *end;
    u64 start, length;

    u64 size();

public:

    bitstream();

    u64 get_length();

    void operator<<(bool bit);
    void operator>>(bool &bit);
    void operator>>(u8 &byte);
    void operator>>(bits &bit);

    void input(u8 *bits, u64 count);
};

#endif
