/*
 *  12 fixed width implementation of lzw decompression
 *  Author:Connor McShane
 *  Date: Mar 2018
 *  Sources: http://www.stringology.org/DataCompression/lzw-d/index_en.html and stackexchange
 */

#include "lzw.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define BUFFSIZE 1024


void addToDictionary(lzwDecompData* data, char *inputStr, size_t strSize)
{
    /* Add string to lzwDecompdata structure
     *  Args: lzwDecompData data structure and String inputStr and strSize
     *  Returns:void
     */
    data->dictionary[ data->dictionarySize] = inputStr;
    data->dictionaryEntrySize[ data->dictionarySize] = strSize;
    data->dictionarySize++;
}

void clearDictionary(lzwDecompData* data)
{
    /* Deallocate all dictionary entries
     *  Args: lzwDecompData data structure
     *  Returns:void
     */
    for (int i = 0; i < data->dictionarySize; i++)
    {
        free(data->dictionary[i]);
        data->dictionaryEntrySize[i] = 0;
    }
    data->dictionarySize = 0;
    
}

void initDictionary(lzwDecompData* data)
{
    /*
     *  Initiliases the first 256 Values of the dictionary
     *  Args:lzwDecompData structure
     *  Returns:void
     */
    data->firstCode = true;
    data->dictionarySize = 256;
    
    for (int i = 0; i < 256; i++)
    {
        data->dictionary[i] = calloc(1, sizeof(unsigned char));
        *(data->dictionary[i]) = (char)i;
        //snprintf(data->dictionary[i],2, "%c", i);
        data->dictionaryEntrySize[i] = 1;
    }
}

void resetDictionary(lzwDecompData* data)
{
    /* Reset dictionary entries and free allocated memory stores
     *  Args:lzwDecompData structure
     *  Returns:void
     */
    for (int i = 256; i < data->dictionarySize; i++)
    {
        free(data->dictionary[i]);
        data->dictionaryEntrySize[i] = 0;
    }
    data->dictionarySize = 256;
}

void freeDictionary(lzwDecompData* data)
{
    /* Whipes diciontary for data structure
     *  Args:lzwDecompData structure
     *  Returns:void
     */
    for (int i = 256; i < data->dictionarySize; i++)
    {
        free(data->dictionary[i]);
        data->dictionaryEntrySize[i] = 0;
    }
    data->dictionarySize = 256;
}


int readCodeEntry(FILE* source, lzwDecompData *data)
{
    /* Read lzw compressed byte entries and return a valid code entry
     *  Args:File source pointer and lzwDecompData structure
     *  Returns:Integer value of a lzw Code entry from a file, returns 0 when EOF is reached
     */
    int codeValue;
    size_t freadVal = 0;
    
    /* If we are at the end, don't do any more reads*/
    if(feof(source)) {
        return 0;
    }
    
    /* Read 3 bytes for the first code entry*/
    if (data->firstCode == true)
    {
        freadVal = fread(data->buffer, 1, 3, source);

        /* Read 3 bytes for the first code entry*/
        if (freadVal == 3)
        {
            codeValue = (data->buffer[0] << 4) + ((data->buffer[1] & 0xF0) >> 4);
        }
        /*last entry, so we read the padded value */
        else if(freadVal == 0)
        {
            return 0;
        }
        
        else {
            /* We are at the end of the file so we read the 16bit padded value */
            codeValue = (data->buffer[0] << 8) + data->buffer[1];
        }
        data->firstCode = false;
    }
    /* Second code entry read from the second and third byte */
    else
    {
        codeValue = ((data->buffer[1]& 0x0F) << 8) + data->buffer[2];
        data->firstCode = true;
    }
    return codeValue;
}



int decompress (lzwDecompData* data, FILE* source, FILE* dest)
{
    /* lzw decompression alglorithm
     *  Args:lzwDecompData structure, FILE source pointer and FILE dest pointer
     *  Returns:0 on success or 1 on failure
     */
    initDictionary(data);
    int oldCode, newCode;
    phrase oldPhrase, newPhrase;

    /* Read in the first code */
    oldCode = readCodeEntry(source, data);
    size_t size = data->dictionaryEntrySize[oldCode];
    oldPhrase.size = size;
    oldPhrase.buffer = calloc(oldPhrase.size, sizeof(char)); // one for null byte
    memcpy(oldPhrase.buffer, data->dictionary[oldCode],  oldPhrase.size);
    
    fwrite(oldPhrase.buffer, sizeof(char), oldPhrase.size, dest);
    /* As we need to initialise newPhrase */
    newPhrase = oldPhrase;

    /* While there is data to read */
    while ((newCode = readCodeEntry(source, data)))
    {
        /* If the code is in the dictionary, we write out the string and add a
         new dictionary entry (i.e old string plus the first char of the second)
         */
        if (data->dictionary[newCode] != NULL)
        {
            size = data->dictionaryEntrySize[newCode];
            newPhrase.size = size;
            newPhrase.buffer = calloc(size + 1, sizeof(char)); // For chars + null byte
            memcpy(newPhrase.buffer, data->dictionary[newCode], (size));
            fwrite(newPhrase.buffer, sizeof(char), newPhrase.size, dest);

            /* Extra memory for the new char */
            oldPhrase.buffer = realloc(oldPhrase.buffer, oldPhrase.size + 1);
            memcpy(oldPhrase.buffer + (oldPhrase.size), newPhrase.buffer, 1);
            oldPhrase.size += 1;
            
            addToDictionary(data, oldPhrase.buffer , oldPhrase.size);

        } else /* code is outside the dictionary */
        {
            if (newCode >= data->dictionarySize)
            {
                /* Extra memory for the new char */
                oldPhrase.buffer = realloc(oldPhrase.buffer, oldPhrase.size + 1);
                oldPhrase.size += 1;
                memcpy(oldPhrase.buffer + (oldPhrase.size), oldPhrase.buffer, 1);
                addToDictionary(data,oldPhrase.buffer, oldPhrase.size);
            }
            /*Some error*/
            else
            {
                return 1;
            }
        }
        
        /*For the next iteration*/
        oldCode = newCode;
        oldPhrase = newPhrase;
        
        /* reset the dictionary when it is too large */
        if (data->dictionarySize == 4096)
        {
            resetDictionary(data);
        }
    }
    
    /* deallocate dictionary */
    clearDictionary(data);
    
    return 0;
}

