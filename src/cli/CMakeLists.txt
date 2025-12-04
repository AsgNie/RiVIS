add_library(cli)

target_sources(cli
    PRIVATE
        cli.c

    PUBLIC
        FILE_SET HEADERS
        FILES
            cli.h
)
