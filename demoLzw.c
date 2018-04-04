/*
 *  12 fixed width implementation of lzw decompression
 *  Author:Connor McShane
 *  Date: Mar 2018
 *  Sources: http://www.stringology.org/DataCompression/lzw-d/index_en.html and stackexchange
 */

#include "lzw_com.h"
#include "lzw_decom.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main (int argc, char* argv[])
{
    if (argc != 4)
    {
        printf("usage: demoLzw function input_file output_file\n function is either 'compress or decompress'");
        return 1;
    }
    

    /* Open read and write files for binary read and write */
    FILE* source = fopen(argv[2], "rb");
    if (source == NULL)
    {
        printf("Error, couldn't open %s\n", argv[1]);
        return 1;
    }

    FILE* dest = fopen(argv[3], "wb");
    if (dest == NULL)
    {
        printf("Error, couldn't open open %s\n", argv[2]);
        return 1;
    }

    if(strcmp(argv[2],"decompress")
    {
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
    }
    else if(strcmp(argv[2],"compress"))
    {
        /* decompression data structure */
        lzwData lzw;
        
        if (compress(&lzw,source, dest) != 0)
        {
            printf("Compression Error\n");
            return 1;
        }
        else
        {
            printf("Compression Success\n");
            return 0;
        }
    }
    else
    {
        printf("Incorrect function call\n");
    }



    fclose(source);
    fclose(dest);
}
