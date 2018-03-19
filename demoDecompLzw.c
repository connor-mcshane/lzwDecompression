/*
 *  12 fixed width implementation of lzw decompression
 *  Author:Connor McShane
 *  Date: Mar 2018
 *  Sources: http://www.stringology.org/DataCompression/lzw-d/index_en.html and stackexchange
 */

#include "lzw.h"
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char* argv[])
{
    if (argc != 3)
    {
        printf("usage: demoDecompLzw input_file output_file\n");
        return 1;
    }

    /* Open read and write files for binary read and write */
    FILE* source = fopen(argv[1], "rb");
    if (source == NULL)
    {
        printf("Error, couldn't open %s\n", argv[1]);
        return 1;
    }

    FILE* dest = fopen(argv[2], "wb");
    if (dest == NULL)
    {
        printf("Error, couldn't open open %s\n", argv[2]);
        return 1;
    }

    /* decompression data structure */
    lzwDecompData lzw;

    if (decompress(&lzw,source, dest) != 0)
    {
        printf("Decompression Error\n");
        return 1;
    }
    else
    {   printf("Decompression Success\n");
        return 0;
    }

    fclose(source);
    fclose(dest);
}