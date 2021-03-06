/*
 *  lzw_com.h
 *  12 fixed width implementation of lzw decompression
 *  Author:Connor McShane
 *  Date: Mar 2018
 *  Sources: http://www.stringology.org/DataCompression/lzw-d/index_en.html and stackexchange
 */

#ifndef lzw_com_h
#define lzw_com_h

#include "hash_table.h"
#include <stdbool.h>

#endif /* lzw_com_h */

typedef struct{
    bool firstCode;
    bool bufferEmpty;
    unsigned char buffer[3];
    dictTable* table;
} lzwCompData;

typedef struct
{
    char *buffer;
    size_t size;
    size_t code;
} comPhrase;

int compress(lzwCompData* data, FILE* source, FILE* dest);

