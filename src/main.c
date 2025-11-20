/*** Includes ***/
#include <stdio.h>  // Supplies file access + print
#include <stdint.h> // Suplies types, e.g. uint64_t
#include <stdbool.h>
#include <stdlib.h> // Suplies EXIT_FAILURE, EXIT_SUCCESS, exit()
#include <getopt.h> // For parsing command line options. getop() ect.
#include <libgen.h> // Supplies basename() // TODO: Seems unnecessary
#include "isaSimSoft.h"

/*** Defines ***/
#define OPTSTR "vi:o:h"
#define USAGE_FMT  "%s [-v] [-i <inputfile>] [-o <outputfile>] [-h]\n"
#define ERR_FOPEN_OUTPUT "fopen(output, w) failed"
#define ERR_FOPEN_INPUT "fopen(input, r) failed"
#define DEFAULT_PROGNAME "RiVIS"
#define PROGRAM_SIZE_BYTES          ( 1048576 ) // 1 MiB
#define REGISTRY_FILE_SIZE_BYTES    ( 128 )     // 32 32-bit registers

/* external declarations */
extern int errno;
extern char *optarg;
extern int opterr, optind; // TODO: Do I need those?

/* type declerations */
typedef struct
{
    int8_t      verbosity;
    FILE*       inFile;
    FILE*       outFile;
} options_t;

typedef struct
{
    bool inFileOpen;
    bool outFileOpen;
} ressources_t;

/* function prototypes */
void usage(char *progname);
void releaseAndExit(int exitStatus, const options_t* options, ressources_t* ressources, uint8_t* progMem);
int32_t readInputFileToMemory(FILE* file, uint8_t* prog);

int main(int argc, char *argv[])
{
    int opt;
    //opterr = 0;
    options_t options = {0, NULL, NULL};
    ressources_t ressources = {false, false};

    uint8_t* prog = NULL;;
    int32_t inFileSize = -1;
    int32_t regFile[32] = {};


    // Handle command-line arguments
    while( (opt = getopt(argc, argv, OPTSTR)) != EOF)
    {
        switch(opt)
        {
        case 'i':
            if( (options.inFile = fopen(optarg, "rb")) == NULL )
            {
                perror(ERR_FOPEN_INPUT);
            }
            else
            {
                ressources.inFileOpen = true;
            }
            break;
        case 'o':
            if( (options.outFile = fopen(optarg, "wb")) == NULL )
            {
                perror(ERR_FOPEN_OUTPUT);
            }
            else
            {
                ressources.outFileOpen = true;
            }
            break;
        case 'v':
            options.verbosity += 1;
            break;
        case 'h':
            usage(basename(argv[0]));
            releaseAndExit(EXIT_SUCCESS, &options, &ressources, prog);
            break;
        default:
            usage(basename(argv[0]));
            releaseAndExit(EXIT_FAILURE, &options, &ressources, prog);
            break;
        }
    }

    // Check on files
    if (options.inFile == NULL || options.outFile == NULL)
    {
        fprintf(stderr, "Error opening input or outout file(s)\n");
        releaseAndExit(EXIT_FAILURE, &options, &ressources, prog);
    }

    // Allocate and zero initialise program memory
    prog = calloc(PROGRAM_SIZE_BYTES, sizeof(uint8_t));
    if (prog == NULL)
    {
        fprintf(stderr, "Error allocating program memory\n");
        releaseAndExit(EXIT_FAILURE, &options, &ressources, prog);
    }

    inFileSize = readInputFileToMemory(options.inFile, prog);
    if (inFileSize <= 0 )
    {
        releaseAndExit(EXIT_FAILURE, &options, &ressources, prog);
    }
    fclose(options.inFile);
    ressources.inFileOpen = false;
    if (options.verbosity)
    {
        fprintf(stderr, "Input file of size %d B read\n", inFileSize);
    }

    // Run program
    int8_t res = isaSimSoftRun(prog, PROGRAM_SIZE_BYTES, regFile, options.verbosity);

    // Store result to file
    if (fwrite(regFile, sizeof(uint8_t), REGISTRY_FILE_SIZE_BYTES, options.outFile) != REGISTRY_FILE_SIZE_BYTES )
    {
        perror("Output file not written to end");
    }

    //Print info
    releaseAndExit(EXIT_SUCCESS, &options, &ressources, prog);
}

/* usage: prints usage instructions to stderr */
void usage(char *progname)
{
    fprintf(stderr, USAGE_FMT, progname?progname:DEFAULT_PROGNAME);
}

void releaseAndExit(int exitStatus, const options_t* options, ressources_t* ressources, uint8_t* progMem)
{
    if (ressources->inFileOpen)
    {
        fclose(options->inFile);
        ressources->inFileOpen = false;
    }

    if (ressources->outFileOpen)
    {
        fclose(options->outFile);
        ressources->outFileOpen = false;
    }

    if (progMem)
    {
        free(progMem);
    }

    exit(exitStatus);
}

int32_t readInputFileToMemory(FILE *file, uint8_t *prog)
{
    int32_t fileSize = -1;

    if( fseek(file, 0, SEEK_END) != 0 )
    {
        perror("Unable to locate end of input file");
    }
    else if ( (fileSize = ftell(file)) <= 0)
    {
        perror("Unable to read ftell() filesize, or filesize == 0");
    }
    else
    {
        rewind(file); // Rewind to start file
        if (fread(prog, sizeof(uint8_t), fileSize, file) != fileSize )
        {
            perror("Input file not read to end");
            fileSize = -1;
        }
    }

    return fileSize;
}
