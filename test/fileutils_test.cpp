#include <gtest/gtest.h>
#include "fff.h"
#include <stdio.h>

DEFINE_FFF_GLOBALS;

extern "C" {
    #include <fileutils.h>
}

/* mocked system functions */
FAKE_VALUE_FUNC2(void*,     calloc, size_t, size_t);
FAKE_VALUE_FUNC (int,       fclose, FILE*);
FAKE_VALUE_FUNC2(FILE*,     fopen, const char *, const char *);
FAKE_VALUE_FUNC4(size_t,    fread, void*, size_t, size_t, FILE*);
FAKE_VALUE_FUNC3(int,       fseek, FILE*, long int, int);
FAKE_VALUE_FUNC (long int,  ftell, FILE*);
FAKE_VALUE_FUNC4(size_t,    fwrite, const void *, size_t, size_t, FILE *);
FAKE_VOID_FUNC  (           rewind, FILE*);

/* not mocked system functions
FAKE_VOID_FUNC  (           free, void*);
FAKE_VALUE_FUNC (void*,     malloc, size_t);

free:   Used by fileutils, but if mocked return_val was higher than expected.
        My suspicion is that the mock interfeer with its use in the unit test
        framework. Calls to free are therefore not verified in these unit tests.
malloc: To give credit to the above theory it was also attemted to mock malloc.
        The result was that the test executeable was unable to run, which
        strengthens the theory.
Callock is mocked such that its failure can be tested, but on success it
returns pointers produced by malloc so that the later free(ptr) call is legal.
*/



/********************************************************************
 *********************** fileutilsWriteBinary ***********************
 ********************************************************************/
// WriteBinary call where fopen() fails
TEST(fileutils, WriteBinaryFileOpenFails)
{
    const char* fileName = "testout.bin";
    uint8_t mem[] = "0xdeadbeef";
    size_t memSize = sizeof(mem);
    bool retVal = true;

    fopen_fake.return_val = NULL; // Mock fopen returning NULL

    retVal = fileutilsWriteBinary(fileName, mem, memSize);
    EXPECT_EQ(retVal, false);
}

// WriteBinary call where fopen() succeeds, but fwrite() fails
TEST(fileutils, WriteBinaryFileWriteFails)
{
    RESET_FAKE(fclose);

    const char* fileName = "testout.bin";
    uint8_t mem[] = "0xdeadbeef";
    size_t memSize = sizeof(mem);
    bool retVal = true;
    FILE file;

    fopen_fake.return_val = &file;  // Mock fopen returning valid FILE pointer
    fwrite_fake.return_val = 0;     // Mock fwrite returning 0, i.e. failed to write
    fclose_fake.return_val = 0;     // Mock fclose returning 0, i.e. succeeding

    retVal = fileutilsWriteBinary(fileName, mem, memSize);
    EXPECT_EQ(retVal, false);
    EXPECT_EQ(fclose_fake.call_count, 1);
}

// WriteBinary successfull call
TEST(fileutils, WriteBinarySuccess)
{
    RESET_FAKE(fclose);

    const char* fileName = "testout.bin";
    uint8_t mem[] = "0xdeadbeef";
    size_t memSize = sizeof(mem);
    bool retVal = false;
    FILE file;

    fopen_fake.return_val = &file;      // Mock fopen returning valid FILE pointer
    fwrite_fake.return_val = memSize;   // Mock fwrite success
    fclose_fake.return_val = 0;         // Mock fclose returning 0, i.e. succeeding

    retVal = fileutilsWriteBinary(fileName, mem, memSize);
    EXPECT_EQ(retVal, true);
    EXPECT_EQ(fclose_fake.call_count, 1);
}

/********************************************************************
 *********************** fileutilsReadBinary ************************
 ********************************************************************/

/*
fileutilsReadBinary uses a series of system calls. Some are conditional on the
result of previous calls. The graph below shows the dependencies.

system call order    | fopen -> calloc -> [fseek -> ftell -> rewind -> fread]
conditions           |      \(if success)            /(if any failure in block)
cond. sys calls      |       fclose              free
*/

/* The following tests are setpup with mocks returning correct data for all but one system funcction
at a time. It is then verified that ReadBinary has the expected return value, and that the failed
function call was executed, and that the one after was not. */

// ReadBinary fopen fails
TEST(fileutils, ReadBinaryFileOpenFails)
{
    const char* fileName = "testin.bin";
    int32_t fileSize = 10;
    uint8_t* memRet = NULL;
    size_t memSize = 20;
    uint8_t* dynMem;
    FILE file;

    dynMem = (uint8_t*) malloc(memSize);   // This is necessary in order to allow free() to be called on dynMem

    fopen_fake.return_val = NULL;       // Mock fopen NULL, FAILURE
    calloc_fake.return_val = dynMem;    // Mock calloc ptr to memory assigned via malloc, success
    fseek_fake.return_val = 0;          // Mock fseek 1, success
    ftell_fake.return_val = fileSize;   // Mock ftell fileSize, success
    fread_fake.return_val = fileSize;   // Mock fread filseSize, success
    fclose_fake.return_val = 0;         // Mock fclose 0, success

    memRet = fileutilsReadBinary(fileName, memSize);
    EXPECT_EQ(memRet, nullptr);
    EXPECT_EQ(fopen_fake.call_count, 1);
    EXPECT_EQ(calloc_fake.call_count, 0);
    EXPECT_EQ(fclose_fake.call_count, 0);
}

// ReadBinary calloc fails
TEST(fileutils, ReadBinaryCallocFails)
{
    const char* fileName = "testin.bin";
    int32_t fileSize = 10;
    uint8_t* memRet = NULL;
    size_t memSize = 20;
    uint8_t* dynMem;
    FILE file;

    dynMem = (uint8_t*) malloc(memSize);   // This is necessary in order to allow free() to be called on dynMem

    fopen_fake.return_val = &file;      // Mock fopen FILE ptr, success
    calloc_fake.return_val = NULL;      // Mock calloc NULL, FAILURE
    fseek_fake.return_val = 0;          // Mock fseek 1, success
    ftell_fake.return_val = fileSize;   // Mock ftell fileSize, success
    fread_fake.return_val = fileSize;   // Mock fread filseSize, success
    fclose_fake.return_val = 0;         // Mock fclose 0, success

    memRet = fileutilsReadBinary(fileName, memSize);
    EXPECT_EQ(memRet, nullptr);
    EXPECT_EQ(calloc_fake.call_count, 1);
    EXPECT_EQ(fseek_fake.call_count, 0);
    EXPECT_EQ(fclose_fake.call_count, 1);
}

// ReadBinary fseek fails
TEST(fileutils, ReadBinaryFileSeekFails)
{
    const char* fileName = "testin.bin";
    int32_t fileSize = 10;
    uint8_t* memRet = NULL;
    size_t memSize = 20;
    uint8_t* dynMem;
    FILE file;

    dynMem = (uint8_t*) malloc(memSize);   // This is necessary in order to allow free() to be called on dynMem

    fopen_fake.return_val = &file;      // Mock fopen FILE ptr, success
    calloc_fake.return_val = dynMem;    // Mock calloc ptr to memory assigned via malloc, success
    fseek_fake.return_val = 1;          // Mock fseek 1, FAILURE
    ftell_fake.return_val = fileSize;   // Mock ftell fileSize, success
    fread_fake.return_val = fileSize;   // Mock fread filseSize, success
    fclose_fake.return_val = 0;         // Mock fclose 0, success

    memRet = fileutilsReadBinary(fileName, memSize);
    EXPECT_EQ(memRet, nullptr);
    EXPECT_EQ(fseek_fake.call_count, 1);
    EXPECT_EQ(ftell_fake.call_count, 0);
    EXPECT_EQ(fclose_fake.call_count, 1);
}

// ReadBinary ftell fails
TEST(fileutils, ReadBinaryFileTellFails)
{
    const char* fileName = "testin.bin";
    int32_t fileSize = 10;
    uint8_t* memRet = NULL;
    size_t memSize = 20;
    uint8_t* dynMem;
    FILE file;

    dynMem = (uint8_t*) malloc(memSize);   // This is necessary in order to allow free() to be called on dynMem

    fopen_fake.return_val = &file;      // Mock fopen FILE ptr, success
    calloc_fake.return_val = dynMem;    // Mock calloc ptr to memory assigned via malloc, success
    fseek_fake.return_val = 0;          // Mock fseek 0, success
    ftell_fake.return_val = 0;          // Mock ftell 0, FAILURE
    fread_fake.return_val = fileSize;   // Mock fread filseSize, success
    fclose_fake.return_val = 0;         // Mock fclose 0, success

    memRet = fileutilsReadBinary(fileName, memSize);
    EXPECT_EQ(memRet, nullptr);
    EXPECT_EQ(ftell_fake.call_count, 1);
    EXPECT_EQ(fread_fake.call_count, 0);
    EXPECT_EQ(fclose_fake.call_count, 1);
}

// ReadBinary filesize larger than memory
TEST(fileutils, ReadBinaryFileLargerThanMemory)
{
    const char* fileName = "testin.bin";
    int32_t fileSize = 25;
    uint8_t* memRet = NULL;
    size_t memSize = 20;
    uint8_t* dynMem;
    FILE file;

    dynMem = (uint8_t*) malloc(memSize);   // This is necessary in order to allow free() to be called on dynMem

    fopen_fake.return_val = &file;      // Mock fopen FILE ptr, success
    calloc_fake.return_val = dynMem;    // Mock calloc ptr to memory assigned via malloc, success
    fseek_fake.return_val = 0;          // Mock fseek 0, success
    ftell_fake.return_val = fileSize;   // Mock ftell filseSize, success
    fread_fake.return_val = fileSize;   // Mock fread filseSize, success
    fclose_fake.return_val = 0;         // Mock fclose 0, success

    memRet = fileutilsReadBinary(fileName, memSize);
    EXPECT_EQ(memRet, nullptr);
    EXPECT_EQ(ftell_fake.call_count, 1);
    EXPECT_EQ(fread_fake.call_count, 0);
    EXPECT_EQ(fclose_fake.call_count, 1);
}

// ReadBinary fread fails
TEST(fileutils, ReadBinaryFileReadFails)
{
    const char* fileName = "testin.bin";
    int32_t fileSize = 10;
    uint8_t* memRet = NULL;
    size_t memSize = 20;
    uint8_t* dynMem;
    FILE file;

    dynMem = (uint8_t*) malloc(memSize);   // This is necessary in order to allow free() to be called on dynMem

    fopen_fake.return_val = &file;      // Mock fopen FILE ptr, success
    calloc_fake.return_val = dynMem;    // Mock calloc ptr to memory assigned via malloc, success
    fseek_fake.return_val = 0;          // Mock fseek 0, success
    ftell_fake.return_val = fileSize;   // Mock ftell filseSize, success
    fread_fake.return_val = 0;          // Mock fread 0, FAILURE
    fclose_fake.return_val = 0;         // Mock fclose 0, success

    memRet = fileutilsReadBinary(fileName, memSize);
    EXPECT_EQ(memRet, nullptr);
    EXPECT_EQ(fread_fake.call_count, 1);
    EXPECT_EQ(fclose_fake.call_count, 1);
}

// ReadBinary success
TEST(fileutils, ReadBinarySuccess)
{
    const char* fileName = "testin.bin";
    int32_t fileSize = 10;
    uint8_t* memRet = NULL;
    size_t memSize = 20;
    uint8_t* dynMem;
    FILE file;

    dynMem = (uint8_t*) malloc(memSize);   // This is necessary in order to allow free() to be called on dynMem

    fopen_fake.return_val = &file;      // Mock fopen FILE ptr, success
    calloc_fake.return_val = dynMem;    // Mock calloc ptr to memory assigned via malloc, success
    fseek_fake.return_val = 0;          // Mock fseek 0, success
    ftell_fake.return_val = fileSize;   // Mock ftell filseSize, success
    fread_fake.return_val = fileSize;   // Mock fread filseSize, success
    fclose_fake.return_val = 0;         // Mock fclose 0, success

    memRet = fileutilsReadBinary(fileName, memSize);
    EXPECT_EQ(memRet, dynMem);
    EXPECT_EQ(fclose_fake.call_count, 1);
}
