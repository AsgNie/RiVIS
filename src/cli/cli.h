#ifndef CLI_H
#define CLI_H

#include <stdint.h>
#include <stdbool.h>

typedef struct cli_options_t
{
    int8_t      verbosity;
    char*       inFileName;
    char*       outFileName;
} cli_options_t;

typedef enum cli_return_values_t
{
    CLI_SUCCESS = 0, CLI_USAGE, CLI_UNKNOWN_ARG,
} cli_return_values_t;

cli_return_values_t cliProcessInputs(int argc, char *argv[], cli_options_t* options);


#endif //CLI_H
