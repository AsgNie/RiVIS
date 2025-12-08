# Testing
This folder and its subfolders holds project test files. Testing is conducted using CMake's CTest with GTest, using the [Fake Function Framework](https://github.com/meekrosoft/fff) for creating mocks.

## Usage
Execute `ctest` in the `build/test` directory, e.g. using the following command from root:
`ctest --test-dir build/test --output-on-failure`

## Test setup design
Unittests are written for GTest in C++ and can be found in this folder as `MODULENAME_test.cpp`.

System tests are executed directly from CTest by writting shell scripts that runs the compiled RiVIS program on test data.
RiVIS runs the binary RISC-V program, outputs the resulting register file at end of program, and compares this register file to a known correct result.
The result of the test is transferred to CTest by having the shell script exit with return value `0`on SUCCESS, and non-zero on FAILURE.

## On the choice of test framework
In choosing a testing framework the criterias were:

- It should be easy to read/write tests.
- It should be easy to integrate into the build environment.
- It should be a framework that supports mocking functions.
- It should have solid documentation.
- It should be a well used framework such that there is a user base to draw upon when meeting dificulties.
- *It should be a framework that works for both C and C++.*

The last criteria was added for pure personal reasons. I would like the experienced gained in this project to translate to my C++ projects.

With the above criterias in mind I originaly decided to use GTest with gMock. However, arriving at the implementation I discovered that gMock only has support for mocking objects and does not mock *free functions* making it a inadecuate for use in a C based project. To remedy this the Fake Function Framework was integrated into the testing system. For future projects I would revisit this decision and make a survey of the alternatives. The following framework combinations looked promising at a glance:

- GTest/FFF (C/C++)
- CppUTest/CppUMock[^1] (C/C++)
- Unity/CMock (C)

[^1]: Used in the book *Test Driven Development for Embedded C* by James W. Grenning which looked like an interesting read.

## OS limits on test
Currently the system tests are only available on an OS with access to the `Bash` commandline-shell to run `systemTest/DiffTest.sh`.
This could be expanded to Windows by writing a `Batch` script (or Powershell) with similar functionality, and selecting between the two with a platform dependent CMake define.

## Test files licenses
Generally all files in the project are licensed under the license presented in the root (when I get around to including it).
The exceptions to this are the testfiles present in `test/systemTest/task#` and `fff.h`.

The test files are included from the [Assignment specification Github](https://github.com/schoeberl/cae-lab/tree/master/finasgmt) are contributions by the Technical University of Denmark, and are made available under a BSD 2-Clause license. For the detailed license see `testfiles_LICENSE.txt`.

`fff.h` is included from the [Fake Function Framework](https://github.com/meekrosoft/fff), and made available under a MIT license. See the header file itself for licensing details.

## Further system tests
More system tests can be found at [TheAIBot RISC-V](https://github.com/TheAIBot/RISC-V_Sim/tree/master/RISC-V_Sim/InstructionTests).
They can easily be included in the project by following the instructions given in systemTest/CMakeLists.txt and uncommenting the test.
They have not been included directly in the project as they are not avaiable under an open-source license.
