set(RIVIS_PATH ${CMAKE_BINARY_DIR}/src/RiVIS)

add_test(NAME task1_tests
    COMMAND "${CMAKE_CURRENT_LIST_DIR}/DiffTest.sh" "${RIVIS_PATH}" "task1"
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
)

add_test(NAME task2_tests
    COMMAND "${CMAKE_CURRENT_LIST_DIR}/DiffTest.sh" "${RIVIS_PATH}" "task2"
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
)

add_test(NAME task3_tests
    COMMAND "${CMAKE_CURRENT_LIST_DIR}/DiffTest.sh" "${RIVIS_PATH}" "task3"
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
)

add_test(NAME task4_tests
    COMMAND "${CMAKE_CURRENT_LIST_DIR}/DiffTest.sh" "${RIVIS_PATH}" "task4"
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
)

# TheAIBot has a great collection of small binary programs testing each instruction available at
# https://github.com/TheAIBot/RISC-V_Sim/tree/master/RISC-V_Sim/InstructionTests. As the project
# does not provide an open-source license I am unable to include them here, but you can manually
# paste the folder into systemTest and uncomment the test below.
# Note that TheAIBot use a different calling convention on ECALL EXIT which will give alot of warnings,
# but produce correct register file outputs.
# The folder also include a number of unsuported instructions, e.g. mul, which should be removed.

# add_test(NAME InstructionTests
#     COMMAND "${CMAKE_CURRENT_LIST_DIR}/DiffTest.sh" "${RIVIS_PATH}" "InstructionTests"
#     WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
# )
