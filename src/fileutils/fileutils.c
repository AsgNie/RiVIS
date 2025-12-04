#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "fileutils.h"

extern int errno; // Used for reporting errors by stdio.h, e.g. perror()

/* function prototypes */
static int32_t copyFileToMem(FILE *file, uint8_t *mem, size_t memSize);

uint8_t* fileutilsReadBinary(const char* fileName, size_t memSize)
{
    int fileSize;
    FILE* file = NULL;
    uint8_t* mem = NULL;

    // Check if file can be opened
    if ( (file = fopen(fileName, "rb") ) != NULL)
    {
        // Check if memory can be allocated
        if ( (mem = calloc(memSize, sizeof(uint8_t) )) != NULL )
        {
            // Check that copying is ok
            if ( (fileSize = copyFileToMem(file, mem, memSize)) <= 0 )
            {
                free(mem);
                mem = NULL;
            }
        }
        else
        {
            fprintf(stderr, "fileutils error: Failed to allocate memory for reading binary\n");
        }
        fclose(file);
    }
    else
    {
        perror("fileutils error: Failed opening inputfile");
    }

    return mem;
}

bool fileutilsWriteBinary(const char *fileName, const uint8_t *mem, size_t memSize)
{
    FILE* file = NULL;
    bool retVal = false;

    assert(mem != NULL && "mem must not be NULL\n");

    // Open file for writing
    if ( (file = fopen(fileName, "wb")) != NULL )
    {
        // Write memory to file
        if ( fwrite(mem, sizeof(uint8_t), memSize, file) == memSize )
        {
            retVal = true;
        }
        else
        {
            perror("fileutils error: Output file not written to end");
        }
        fclose(file);
    }
    else
    {
        perror("fileutils error: Failed opening outputfile");
    }

    return retVal;
}

int32_t copyFileToMem(FILE *file, uint8_t *mem, size_t memSize)
{
    int32_t fileSize = -1;

    if( fseek(file, 0, SEEK_END) != 0 )
    {
        perror("fileutils error: Unable to locate end of input file");
    }
    else if ( (fileSize = ftell(file)) <= 0)
    {
        perror("fileutils error: Unable to read ftell() filesize, or filesize == 0");
    }
    else if( (size_t) fileSize > memSize)
    {
        fprintf(stderr, "fileutils error: filesize of %d B larger than maximal program size of %ld B\n", fileSize, memSize);
        fileSize = -1;
    }
    else
    {
        rewind(file); // Rewind to start of file
        if (fread(mem, sizeof(uint8_t), fileSize, file) != (size_t) fileSize )
        {
            perror("fileutils error: Input file not read to end");
            fileSize = -1;
        }
    }

    return fileSize;
}
