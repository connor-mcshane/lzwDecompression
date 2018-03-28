/*
 *  12 fixed width implementation of lzw decompression
 *  Author:Connor McShane
 *  Date: Mar 2018
 *  Sources: http://www.stringology.org/DataCompression/lzw-d/index_en.html and stackexchange
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*
 *  lzw decompression data structure, that contains the lzw dictionary
 */
typedef struct
{
    bool firstCode;
    unsigned char buffer[3];
    size_t dictionarySize;
    char *dictionary[4096];
    size_t dictionaryEntrySize[4096];
} lzwDecompData;

<<<<<<< HEAD
<<<<<<< HEAD
/*
 *  phrase struct to keep track of phrase size
 */
=======
>>>>>>> abb7d7995ea749de18cdfd6312dcd3344d1a85c5
=======
>>>>>>> abb7d7995ea749de18cdfd6312dcd3344d1a85c5
typedef struct
{
    char *buffer;
    size_t size;
} phrase;

/* lzw decompression alglorithm
 *  Args:lzwDecompData structure, FILE source pointer and FILE dest pointer
 *  Returns:0 on success or 1 on failure
 */
int decompress (lzwDecompData* data, FILE* source, FILE* dest);

