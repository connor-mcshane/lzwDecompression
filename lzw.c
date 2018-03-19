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


void addToDictionary(lzwDecompData* data, char *inputStr)
{
    /* Add string to lzwDecompdata structure
    *  Args: lzwDecompData data structure and String inputStr
    *  Returns:void
    */
    int tempIndex = data->dictionarySize;
    data->dictionary[tempIndex] = inputStr;
    data->dictionaryEntrySize[tempIndex] = strlen(inputStr);
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
        data->dictionary[i] = calloc(2, sizeof(unsigned char));
        snprintf(data->dictionary[i],2, "%c", i);
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
    int freadVal = 0;

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
    char* oldPhrase, *newPhrase;

    /* Read in the first code */
    oldCode = readCodeEntry(source, data);
    size_t size =data->dictionaryEntrySize[oldCode];
    oldPhrase = calloc(size + 1, sizeof(char));

    memcpy(oldPhrase, data->dictionary[oldCode], size+1);
    fprintf(dest, "%s", oldPhrase);

    /* While there is data to read */
    while ((newCode = readCodeEntry(source, data)))
    {
        /* If the code is in the dictionary, we write out the string and add a
           new dictionary entry (i.e old string plus the first char of the second)
        */
        if (data->dictionary[newCode] != NULL)
        {
            size = data->dictionaryEntrySize[newCode];
            newPhrase = calloc(size + 1, sizeof(char));
            memcpy(newPhrase, data->dictionary[newCode], size+1);
            fprintf(dest, "%s", newPhrase);
            addToDictionary(data,strncat((char*)oldPhrase, (char*)newPhrase, 1));

        } else /* code is outside the dictionary */
        {
            if (newCode == data->dictionarySize)
            {
                addToDictionary(data,strncat((char*)oldPhrase, (char*)oldPhrase, 1));
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