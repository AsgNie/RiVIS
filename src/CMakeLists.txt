add_executable(RiVIS)

target_sources(RiVIS
    PRIVATE
        main.c
)

target_link_libraries(RiVIS
    PRIVATE
        cli
        fileutils
        simSoft
)

add_subdirectory(isa)
add_subdirectory(cli)
add_subdirectory(fileutils)
add_subdirectory(simulators)
