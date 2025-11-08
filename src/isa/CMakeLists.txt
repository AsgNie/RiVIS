add_library(rv32i)

target_sources(rv32i
    PRIVATE
        rv32i.c

    PUBLIC
        FILE_SET HEADERS
        FILES
            rv32i.h
)
