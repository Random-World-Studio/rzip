/**
 * @file huffman.cc
 * @author pointer-to-bios (pointer-to-bios@outlook.com)
 * @brief
 * @version 0.1
 * @date 2022-11-28
 *
 * @copyright Copyright (c) 2022 Random World Studio
 *
 */

#include "huffman.hh"

#include <cstring>

void bits::append(bool bit)
{
    if (this->length * 8 == this->size)
    {
        u8 *tmp = new u8[this->size + 1];
        memcpy(tmp, this->pointer, this->size + 1);
        delete this->pointer;
        this->pointer = tmp;
        this->size++;
    }
    if (bit)
        pointer[length / 8] |= (1 << (length % 8));
    else
        pointer[length / 8] &= ~(1 << (length % 8));
    length++;
}

bool bits::cut()
{
    length--;
    return pointer[length / 8] & (1 << (length % 8));
}

void bits::operator=(bits b)
{
    this->pointer = new u8[b.size];
    memcpy(this->pointer, b.pointer, b.size);
    this->size = b.size;
    this->length = b.length;
}

bitstream::bitstream() : length(0), begin(new bitblock)
{
    end = begin;
    begin->prev = nullptr;
    end->next = nullptr;
}

u64 bitstream::size()
{
    bitblock *p = begin;
    u64 l = 0;
    while (p != nullptr)
    {
        l++;
        p = p->next;
    }
    return l * bitblock::len;
}

void bitstream::operator<<(bool bit)
{
    if (start + length == size())
    {
        bitblock *tmp = new bitblock;
        tmp->prev = end;
        tmp->next = nullptr;
        end->next = tmp;
        end = tmp;
    }
    u64 x = start + length;
    x %= bitblock::len;
    if (bit)
        end->block |= (1 << x);
    else
        end->block &= ~(1 << x);
    length++;
}

void bitstream::operator>>(bool &bit)
{
    if (length == 0)
    {
        bit = 0;
        return;
    }
    bit = begin->block & (1 << start);
    start++;
    length--;
    if (start == bitblock::len)
    {
        begin = begin->next;
        delete begin->prev;
        begin->prev = nullptr;
        start = 0;
    }
}

void bitstream::operator>>(u8 &byte)
{
    bool b;
    for (int i = 0; i < 8; i++)
    {
        *this >> b;
        if (b)
            byte |= (1 << i);
        else
            byte &= ~(1 << i);
    }
}

void bitstream::operator>>(bits &bit)
{
    bool b;
    *this >> b;
    bit.append(b);
}

u64 bitstream::get_length()
{
    return length;
}

void bitstream::input(u8 *bits, u64 count)
{
    for (u8 i = 0; i < count; i++)
    {
        (*this) << (bool)(bits[i] & (1 << (count % 8)));
    }
}
