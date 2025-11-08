add_executable(RiVIS)

target_sources(RiVIS
    PRIVATE
        main.c
)

target_link_libraries(RiVIS
    PRIVATE
        SimSoft
)

add_subdirectory(isa)
add_subdirectory(simulators)
