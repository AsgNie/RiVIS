#include <stdio.h>
#include <getopt.h>
#include <libgen.h> // Supplies basename()
#include <assert.h>
#include "cli.h"

/* defines */
#define DEFAULT_PROGNAME "RiVIS"
#define OPTSTR "vi:o:h"
#define USAGE_FMT  "Usage: %s [-v] [-i <inputfile>] [-o <outputfile>] [-h]\n-v = verbosity\n-i = input\n-o = output\n-h = help/usage\n"

/* external declarations */
extern char *optarg;
extern int optind;  // Index to next argument in *argv[]. Will point to first input that is non-option argument when while(getopt) is done.
extern int opterr, optopt; // Not used, but kept to notify later users of their existence. See getopt documentation for more.

/* function prototypes */
static void usage(char *progname);

cli_return_values_t cliProcessInputs(int argc, char *argv[], cli_options_t* options)
{
    int opt;
    bool bUsage = false;
    bool bInFile = false;
    bool bUnknowArg = false;
    cli_return_values_t retVal = CLI_SUCCESS;

    assert(options != NULL && "options must not be NULL\n");

    while( (opt = getopt(argc, argv, OPTSTR)) != EOF )
    {
        switch(opt)
        {
        case 'i':
            bInFile = true;
            options->inFileName = optarg;
            break;
        case 'o':
            options->outFileName = optarg;
            break;
        case 'v':
            options->verbosity += 1;
            break;
        case 'h':
            bUsage = true;
            break;
        default:
            bUnknowArg = true;
            // getopt will print a descriptive error to stdout
            break;
        }
    }

    // Check for any non-option argument and warn user that they were ignored
    for (int i = optind; i < argc; i++)
    {
        printf ("Non-option argument '%s' ignored\n", argv[i]);
    }

    // Decide output
    if (bUnknowArg)
    {
        usage(basename(argv[0]));
        retVal = CLI_UNKNOWN_ARG;
    }
    else if (bUsage)
    {
        usage(basename(argv[0]));
        retVal = CLI_USAGE;
    }
    else if (bInFile)
    {
        // Empty
    }
    else // No input file to work on. Either no arguments given, or -v -o without a -i.
    {
        usage(basename(argv[0]));
        retVal = CLI_USAGE;
    }

    return retVal;
}

/* usage: prints usage instructions to stderr */
void usage(char *progname)
{
    fprintf(stderr, USAGE_FMT, progname?progname:DEFAULT_PROGNAME);
}
