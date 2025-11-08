add_library(SimSoft)

target_sources(SimSoft
    PRIVATE
        isaSimSoft.c

    PUBLIC
        FILE_SET HEADERS
        FILES
            isaSimSoft.h
)

target_link_libraries(SimSoft
    PRIVATE
        rv32i
)
