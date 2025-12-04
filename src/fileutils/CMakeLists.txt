add_library(fileutils)

target_sources(fileutils
    PRIVATE
        fileutils.c

    PUBLIC
        FILE_SET HEADERS
        FILES
            fileutils.h
)
