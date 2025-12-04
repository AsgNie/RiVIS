#ifndef FILEUTILS_H
#define FILEUTILS_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h> // For size_t

uint8_t* fileutilsReadBinary (const char* fileName, size_t memSize);
bool     fileutilsWriteBinary(const char* fileName, const uint8_t* mem, size_t memSize);

#endif // FILEUTILS_H
