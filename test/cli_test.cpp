#include <gtest/gtest.h>
#include <stdint.h>
extern "C" {
    #include <cli.h>
}

// Typical cpi call in expected order
TEST(cli, InputOutput)
{
    cli_options_t cliOptions = {0, NULL, NULL};
    char arg0[] = "RiVIS";
    char arg1[] = "-i";
    char arg2[] = "inTest.bin";
    char arg3[] = "-o";
    char arg4[] = "outTest.res";
    char* argv[] = {arg0, arg1, arg2, arg3, arg4};
    int argc = sizeof(argv)/sizeof(char*);

    EXPECT_EQ(cliProcessInputs(argc, argv, &cliOptions), CLI_SUCCESS);
    EXPECT_EQ(cliOptions.verbosity, 0);
    EXPECT_STREQ(cliOptions.inFileName, arg2);
    EXPECT_STREQ(cliOptions.outFileName, arg4);
}

// Typical cpi call with reversed order of input and output
TEST(cli, OutputInput)
{
    cli_options_t cliOptions = {0, NULL, NULL};
    char arg0[] = "RiVIS";
    char arg1[] = "-i";
    char arg2[] = "inTest.bin";
    char arg3[] = "-o";
    char arg4[] = "outTest.res";
    char* argv[] = {arg0, arg3, arg4, arg1, arg2}; // <---- Changed order
    int argc = sizeof(argv)/sizeof(char*);

    EXPECT_EQ(cliProcessInputs(argc, argv, &cliOptions), CLI_SUCCESS);
    EXPECT_EQ(cliOptions.verbosity, 0);
    EXPECT_STREQ(cliOptions.inFileName, arg2);
    EXPECT_STREQ(cliOptions.outFileName, arg4);
}

TEST(cli, Verbosity)
{
    cli_options_t cliOptions = {0, NULL, NULL};
    char arg0[] = "RiVIS";
    char arg1[] = "-i";
    char arg2[] = "inTest.bin";
    char arg3[] = "-o";
    char arg4[] = "outTest.res";
    char arg5[] = "-v";
    char* argv[] = {arg0, arg1, arg2, arg3, arg4, arg5};
    int argc = sizeof(argv)/sizeof(char*);

    EXPECT_EQ(cliProcessInputs(argc, argv, &cliOptions), CLI_SUCCESS);
    EXPECT_EQ(cliOptions.verbosity, 1);
    EXPECT_STREQ(cliOptions.inFileName, arg2);
    EXPECT_STREQ(cliOptions.outFileName, arg4);
}

TEST(cli, MultipleVerbosity)
{
    cli_options_t cliOptions = {0, NULL, NULL};
    char arg0[] = "RiVIS";
    char arg1[] = "-i";
    char arg2[] = "inTest.bin";
    char arg3[] = "-o";
    char arg4[] = "outTest.res";
    char arg5[] = "-vvv";
    char* argv[] = {arg0, arg1, arg2, arg3, arg4, arg5};
    int argc = sizeof(argv)/sizeof(char*);

    EXPECT_EQ(cliProcessInputs(argc, argv, &cliOptions), CLI_SUCCESS);
    EXPECT_EQ(cliOptions.verbosity, 3);
    EXPECT_STREQ(cliOptions.inFileName, arg2);
    EXPECT_STREQ(cliOptions.outFileName, arg4);
}

TEST(cli, OnlyVerbosity)
{
    cli_options_t cliOptions = {0, NULL, NULL};
    char arg0[] = "RiVIS";
    char arg1[] = "-v";
    char* argv[] = {arg0, arg1};
    int argc = sizeof(argv)/sizeof(char*);

    EXPECT_EQ(cliProcessInputs(argc, argv, &cliOptions), CLI_USAGE);
}

TEST(cli, UnexpectedArgs0)
{
    cli_options_t cliOptions = {0, NULL, NULL};
    char arg0[] = "RiVIS";
    char arg1[] = "-k";
    char arg2[] = "inTest.bin";
    char arg3[] = "-o";
    char arg4[] = "outTest.res";
    char arg5[] = "-v";
    char* argv[] = {arg0, arg1, arg2, arg3, arg4, arg5};
    int argc = sizeof(argv)/sizeof(char*);

    EXPECT_EQ(cliProcessInputs(argc, argv, &cliOptions), CLI_UNKNOWN_ARG);
}

TEST(cli, UnexpectedArgs1)
{
    cli_options_t cliOptions = {0, NULL, NULL};
    char arg0[] = "RiVIS";
    char arg1[] = "inTest.bin";
    char arg2[] = "outTest.res";
    char* argv[] = {arg0, arg1, arg2};
    int argc = sizeof(argv)/sizeof(char*);

    EXPECT_EQ(cliProcessInputs(argc, argv, &cliOptions), CLI_USAGE);
}

TEST(cli, NoArgs)
{
    cli_options_t cliOptions = {0, NULL, NULL};
    char arg0[] = "RiVIS";
    char* argv[] = {arg0};
    int argc = sizeof(argv)/sizeof(char*);

    EXPECT_EQ(cliProcessInputs(argc, argv, &cliOptions), CLI_USAGE);
}

TEST(cli, ArgHelp)
{
    cli_options_t cliOptions = {0, NULL, NULL};
    char arg0[] = "RiVIS";
    char arg1[] = "-h";
    char* argv[] = {arg0, arg1};
    int argc = sizeof(argv)/sizeof(char*);

    EXPECT_EQ(cliProcessInputs(argc, argv, &cliOptions), CLI_USAGE);
}

// Indicate an input without giving a name
TEST(cli, NoInputName)
{
    cli_options_t cliOptions = {0, NULL, NULL};
    char arg0[] = "RiVIS";
    char arg1[] = "-i";
    char* argv[] = {arg0, arg1};
    int argc = sizeof(argv)/sizeof(char*);

    EXPECT_EQ(cliProcessInputs(argc, argv, &cliOptions), CLI_UNKNOWN_ARG);
}

// Call without input option
TEST(cli, NoInputOption)
{
    cli_options_t cliOptions = {0, NULL, NULL};
    char arg0[] = "RiVIS";
    char arg1[] = "-v";
    char arg2[] = "-o";
    char arg3[] = "outTest.res";
    char* argv[] = {arg0, arg1, arg2, arg3};
    int argc = sizeof(argv)/sizeof(char*);

    EXPECT_EQ(cliProcessInputs(argc, argv, &cliOptions), CLI_USAGE);
}
