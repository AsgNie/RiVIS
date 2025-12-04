/*** Includes ***/
#include <stdio.h>  // Supplies file access + print
#include <stdint.h> // Suplies types, e.g. uint64_t
#include <stdbool.h>
#include <stdlib.h> // Suplies EXIT_FAILURE, EXIT_SUCCESS, exit()
#include "cli.h"
#include "fileutils.h"
#include "simSoft.h"

/*** Defines ***/
#define PROGRAM_SIZE_BYTES          ( 1048576 ) // 1 MiB
#define REGISTRY_FILE_SIZE_BYTES    ( 128 )     // 32 32-bit registers


int main(int argc, char *argv[])
{
    uint8_t* prog = NULL;
    int32_t regFile[32] = {};
    cli_options_t cliOptions = {0, NULL, NULL};


    // Handle command-line arguments
    switch (cliProcessInputs(argc, argv, &cliOptions))
    {
    case CLI_SUCCESS:
        break; // Continue with main
    case CLI_USAGE:
        exit(EXIT_SUCCESS);     // Printed usage
    case CLI_UNKNOWN_ARG:       // Fallthrough
    default:
        exit(EXIT_FAILURE);     // Unexpected CLI invocation encountered
    }

    // Read binary file to program memory
    prog = fileutilsReadBinary(cliOptions.inFileName, PROGRAM_SIZE_BYTES);
    if ( prog == NULL )
    {
        exit(EXIT_FAILURE);
    }

    // Run program
    int8_t res = simSoftRun(prog, PROGRAM_SIZE_BYTES, regFile, cliOptions.verbosity); // TODO: Evaluate return value
    free(prog);

    // If output file was given save regfile as binary file
    if( cliOptions.outFileName != NULL )
    {
        if ( fileutilsWriteBinary(cliOptions.outFileName, (uint8_t*) regFile, REGISTRY_FILE_SIZE_BYTES) != true )
        {
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}
