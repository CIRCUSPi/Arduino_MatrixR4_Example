/**
 * @file BitConverter.h
 * @author Zack Huang (zackhuang0513@gmail.com)
 * @brief Matrix Mini Example
 * @version 0.0.1
 * @date 2023-10-26
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BITCONVERTER_H
#define BITCONVERTER_H

#include <stdint.h>

class BitConverter
{
public:
    static uint32_t ToUInt32(uint8_t* value, int startIdx);
    static uint16_t ToUInt16(uint8_t* value, int startIdx);

    static int32_t ToInt32(uint8_t* value, int startIdx);
    static int16_t ToInt16(uint8_t* value, int startIdx);
    static void    GetBytes(uint8_t* buff, uint32_t value);
    static void    GetBytes(uint8_t* buff, int32_t value);
    static void    GetBytes(uint8_t* buff, uint16_t value);
    static void    GetBytes(uint8_t* buff, int16_t value);

private:
};

#endif   // BITCONVERTER_H
