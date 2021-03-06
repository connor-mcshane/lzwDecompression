//
//  lzw_com.c
//  LZW
//
//  Created by Connor McShane
//  Copyright © 2018 Connor McShane. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "lzw_com.h"
#define BUFFSIZE 12

char inBuffer[BUFFSIZE]; // we could also just read in one char at a time

void freeDictionary(lzwCompData* data)
{
    resetDictTable(data->table);
}

void resetDictionary(lzwCompData* data)
{
    char temp;
    for(int i = 0; i < 256; i++)
    {
        temp = i;
        insertItem(data->table, &temp, 1, i);
    }
}

void initDictionary(lzwCompData* data)
{
    data->table = newDictTable(20000);
    data->bufferEmpty = true;
    data->firstCode = true;
    /* Insert first 256 items from the ASCII table
     *
     */
    char temp;
    for(int i = 0; i < 256; i++)
    {
        temp = i;
        insertItem(data->table, &temp, 1, i);
    }
}

void outputCode(lzwCompData* data, FILE* dest, size_t code)
{

    /* we write two codes between two bytes */
    if(data->firstCode)
    { /* msb first  for the 12 bit word */
        data->buffer[0] = (code & 0xFF0) >> 4;
        data->buffer[1] = (code & 0x00F) << 4;   // as this byte is shared 1111 XXXX
        data->firstCode = false;
    }
    else
    {
        data->buffer[1] = data->buffer[1] + ((code & 0xF00) >> 8);  // XXXX 1111
        data->buffer[2] = (code & 0x0FF); // 1111 1111
        data->firstCode = true;
        fwrite(data->buffer, sizeof(unsigned char), 3, dest);
    }
}

void output16BitWord(lzwCompData* data, FILE* dest)
{
    unsigned char lastBuff[2];
    /* write out the first */
    lastBuff[0] &= 0; // clear to zero
    lastBuff[0] |= (0xF0 & data->buffer[0]) >> 4; // 0000 1111
    lastBuff[1] |= (0x0F & data->buffer[0]) << 4;
    lastBuff[1] |= (0xF0 & data->buffer[1]) >> 4;
    fwrite(lastBuff, sizeof(unsigned char), 2, dest);
}


int compress(lzwCompData* data, FILE* source, FILE* dest)
{
    /* lzw compression alglorithm
     *  Args:lzwCompData structure, FILE source pointer and FILE dest pointer
     *  Returns:0 on success
     */
    initDictionary(data);
    size_t oldCode, newCode;
    char newChar;
    comPhrase oldPhrase, newPhrase;
    
    /* Read the first char */
    fread(&newChar, sizeof(char), 1, source);
    /* allocate room for the buffer first */
    oldPhrase.size = 1;
    oldPhrase.buffer = (char*)calloc(oldPhrase.size, sizeof(char));
    oldCode = newChar; // must be an ascii value
    memcpy(oldPhrase.buffer, &newChar, sizeof(char)); // one for the first char
    
    
    /* Read the entire file */
    while (!feof(source))
    {
        /* read in the new char */
        fread(&newChar, sizeof(char), 1, source);
        
        /* copy the old phrase plus the new character into new phrase */
        newPhrase.size = oldPhrase.size + 1;
        newPhrase.buffer = (char*)calloc((oldPhrase.size + 1),sizeof(char));
        memcpy(newPhrase.buffer, oldPhrase.buffer, oldPhrase.size);
        memcpy(newPhrase.buffer + oldPhrase.size, &newChar, 1);
        

        /* new phrase is in the dictinoary, so we continue building the phrase by passing it to the oldPhrase*/
        if((newCode=searchItem(data->table, newPhrase.buffer, newPhrase.size)) != -1)
        {
            oldPhrase = newPhrase;
            oldCode = newCode;
            
        }else
        {
            /*if the new phrase is not in the dictionary, we output the prefix  and then we
             add the new phrase to the dictionary
             */

            insertItem(data->table, newPhrase.buffer, newPhrase.size, data->table->count);
            outputCode(data, dest, searchItem(data->table, oldPhrase.buffer, oldPhrase.size));

            printf("%i\n", searchItem(data->table, oldPhrase.buffer, oldPhrase.size));
            
            oldPhrase.buffer = calloc(1,sizeof(char));
            oldPhrase.size = 1;
            oldPhrase.buffer[0] = newChar;
        }
        
        if(data->table->count == 4096)
        {
            freeDictionary(data);
            resetDictionary(data);
            data->table->count = 256;
        }
        
    }

    if(!data->bufferEmpty)
    {
        output16BitWord(data, dest);
    }
    freeDictTable(data->table);
    
    return 0;
}



