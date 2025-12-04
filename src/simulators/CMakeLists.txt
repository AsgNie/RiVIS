add_library(simSoft)

target_sources(simSoft
    PRIVATE
        simSoft.c

    PUBLIC
        FILE_SET HEADERS
        FILES
            simSoft.h
)

target_link_libraries(simSoft
    PRIVATE
        rv32i
)
