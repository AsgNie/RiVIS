add_library(SimSoft)

target_sources(SimSoft
    PRIVATE
        simSoft.c

    PUBLIC
        FILE_SET HEADERS
        FILES
            simSoft.h
)

target_link_libraries(SimSoft
    PRIVATE
        rv32i
)
