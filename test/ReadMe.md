# Testing
This folder and its subfolders holds project test files. Testing is conducted using CMake's CTest with GTest.

## Usage
Execute `ctest` in the `build/test` directory, e.g. using the following command from root:
`ctest --test-dir build/test --output-on-failure`

## Test setup design
Unittests are written for GTest in C++ and can be found in this folder as `MODULENAME_test.cpp`.

System tests are executed directly from CTest by writting shell scripts that runs the compiled RiVIS program on test data.
RiVIS runs the binary RISC-V program, outputs the resulting register file at end of program, and compares this register file to a known correct result.
The result of the test is transferred to CTest by having the shell script exit with return value `0`on SUCCESS, and non-zero on FAILURE.

## OS limits on test
Currently the system tests are only available on an OS with access to the `Bash` commandline-shell to run `systemTest/DiffTest.sh`.
This could be expanded to Windows by writing a `Batch` script (or Powershell) with similar functionality, and selecting between the two with a platform dependent CMake define.

## Test files license
Generally all files in the project are licensed under the license presented in the root (when I get around to including it).
The exception to this is the testfiles present in `test/systemTest/task#`. These files are included from the [Assignment specification Github](https://github.com/schoeberl/cae-lab/tree/master/finasgmt) and are made available under a BSD 2-Clause license.
For the detailed license see `testfiles_LICENSE.txt`

## Further system tests
More system tests can be found at [TheAIBot RISC-V](https://github.com/TheAIBot/RISC-V_Sim/tree/master/RISC-V_Sim/InstructionTests).
They can easily be included in the project by following the instructions given in systemTest/CMakeLists.txt and uncommenting the test.
They have not been included directly in the project as they are not avaiable under an open-source license.
