/**
 * @file zfile.cc
 * @author pointer-to-bios (pointer-to-bios@outlook.com)
 * @brief
 * @version 0.1
 * @date 2022-11-27
 *
 * @copyright Copyright (c) 2022 Random World Studio
 *
 */

#include <iostream>
#include <fstream>
#include <map>
#include <list>
#include <algorithm>

#include "huffman.hh"

std::fstream file;
std::string file_name;
std::fstream ofile;

//权重表
struct weight
{
    u8 byte;
    u64 weight;
    btnode *tree;
};
typedef std::list<weight> weight_list;

//哈夫曼码表
typedef std::pair<u8, bits> hufftable_pair;
typedef std::list<hufftable_pair> huffman_table;

huffman_table hufftable;

/* 制作码表 */
//递归深搜遍历哈夫曼树
void maketable(btnode *hufftree, bits &code)
{
    if (hufftree->prev == nullptr && hufftree->last == nullptr)
    {
        bits *b = new bits;
        *b = code;
        hufftable.push_back(std::make_pair(hufftree->val, *b));
        return;
    }
    if (hufftree->prev)
    {
        code.append(0);
        maketable(hufftree->prev, code);
        code.cut();
    }
    if (hufftree->last)
    {
        code.append(1);
        maketable(hufftree->last, code);
        code.cut();
    }
}

/* 生成哈夫曼码表 */
void make_huffman_table()
{
    //权重表
    //<一字节值, 计数>
    weight_list weight_table;
    //逐字节读取文件，用字节值出现次数代表权重
    while (!file.eof())
    {
        u8 byte;
        file >> byte;
        bool added = false;
        for (weight_list::iterator it = weight_table.begin();
             it != weight_table.end(); it++)
            if ((*it).byte == byte)
            {
                (*it).weight++;
                added = true;
                break;
            }
        if (!added)
        {
            btnode *p = new btnode;
            p->prev = nullptr;
            p->last = nullptr;
            p->val = byte;
            weight_table.push_back(weight{byte, 1, p});
        }
    }
    /* 编码 */
    //构建哈夫曼树
    while (weight_table.size() > 1)
    {
        //排序
        weight_table.sort([](weight a, weight b)
                          { return a.weight < b.weight; });
        //合并前两个
        weight_list::iterator it = weight_table.begin();
        weight_list::iterator it1 = it;
        it1++;
        btnode *p = (*it1).tree;
        (*it1).tree = new btnode;
        (*it1).tree->last = p;
        (*it1).tree->prev = (*it).tree;
        (*it1).weight += (*it).weight;
        weight_table.pop_front();
    }
    btnode *hufftree = (*weight_table.begin()).tree;
    //哈夫曼树构建完毕
    bits code;
    maketable(hufftree, code);
}

/* 向输出文件写哈夫曼码表 */
// 表项数
// 字节 | 编码位数 | 哈夫曼编码
void write_huffman_table()
{
    //排序，以便解压时快速搜索
    hufftable.sort([](hufftable_pair a, hufftable_pair b)
                   { return a.first < b.first; });
    u64 table_len = hufftable.size();
    ofile.write((const char *)&table_len, sizeof(table_len));
    for (hufftable_pair it : hufftable)
    {
        ofile.write((const char *)&it.first, 1);
        ofile.write((const char *)&it.second.length, sizeof(bits::length));
        ofile.write((const char *)it.second.pointer, it.second.size);
    }
}

/* 写入压缩后的数据 */
const u64 blocksize = 8192;
//数据存于比特流中，每blocksize写入一次
void write_zipped_file()
{
    u8 *buffer = new u8[blocksize];
    file.close();
    file.open(file_name, std::ios::in | std::ios::binary);
    bitstream bs;
    while (!file.eof())
    {
        if (bs.get_length() >= blocksize * 8)
        {
            for (u8 i = 0; i < blocksize; i++)
            {
                u8 buf;
                bs >> buf;
                buffer[i] = buf;
            }
            ofile.write((const char *)buffer, blocksize);
        }
        u8 byte;
        file >> byte;
        bits b;
        for (hufftable_pair it : hufftable)
        {
            if (it.first == byte)
            {
                b = it.second;
                break;
            }
        }
        bs.input(b.pointer, b.length);
    }
    if (bs.get_length())
    {
        u8 i;
        for (i = 0; i < blocksize && bs.get_length(); i++)
        {
            u8 buf;
            bs >> buf;
            buffer[i] = buf;
        }
        ofile.write((const char *)buffer, i);
    }
}

/* 压缩和解压缩文件 */

void comp(int argc, char **argv)
{
    file_name = argv[2];
    file.open(argv[2], std::ios::in | std::ios::binary);
    ofile.open(argv[3], std::ios::out | std::ios::binary);
    make_huffman_table();
    write_huffman_table();
    write_zipped_file();
    file.close();
    ofile.close();
}

void decomp(int argc, char **argv)
{
}

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        std::cout << "fatal: no arguments" << std::endl;
        return -3;
    }
    if (argc == 2)
    {
        std::cout << "fatal: no infut file." << std::endl;
        return -1;
    }
    if (argc == 3)
    {
        std::cout << "fatal: no output file." << std::endl;
        return -2;
    }
    if (std::string(argv[1]) == "-c" || std::string(argv[1]) == "--compress")
    {
        comp(argc, argv);
    }
    else if (std::string(argv[1]) == "-d" || std::string(argv[1]) == "--decompress")
    {
    }
    else
    {
        std::cout << "error: unknown argument." << std::endl;
        return -4;
    }
}
