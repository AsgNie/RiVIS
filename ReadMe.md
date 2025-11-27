# RISC-V ISA Simulator - RiVIS
This repository contains my work on a RISC-V Instruction Set Architeture simulator, i.e. a program that simulates the execution of RISC-V binary instructions in software. The program is developed partly as the final assignment of the course [02155 - Computer Architecture and Engineering](https://kurser.dtu.dk/course/02155) as taught on the Technical University of Denmark autumn 2025, and partly as an exercise in designing, implementing and managing a reasonably sized software project with a build environment, version control, tests, and continious integration.

For the DTU assignment specification see [Assignment specification Github](https://github.com/schoeberl/cae-lab/tree/master/finasgmt).


## Requirements
Here the requirements on the simulator for the assignment specification will summerised. Requirements regarding the report, deadlines, and other practicalities are not presented here.

* Implement the 32 bit RISC-V integer instructionset RV32I (with the exception of the instructions `FENCE`, `FENCE.TSO`, `PAUSE`, `EBREAK` ).
* The simulator must be able to read and execute RISC-V instructions from a binary input file.
* The simulator must stop executing either at the end of the input file, or if the `ecall`-instruction is called with an input of `10`.
* The simulator must write out the 32 registers to a binary file at end of program, from lowest to highest.
* The simulator must implement 1 MB program memory (or possible more). Test programs will set stack pointer to `0x100000` with the stack growing towards lower addresses.
* The simulator must be able to execute on an x64 machine running Ubuntu.

The correct implementation of the simulator will be judged by comparing the simulators binary output of the 32 registers to known solutions for a set of test files.


## Quick start
RiVIS is developped and tested on Linux. I anticipate that the current version can compile and run on Windows, but this has not been tested.

0. Clone

```bash
   $ git clone https://github.com/AsgNie/RiVIS
```

1. Build and compile with CMake
```bash
   $ cmake -B build
   $ cmake --build build
```
2. Copy the executeable to a test location, e.g.
```bash
   $ mkdir misc
   $ cp build/src/RiVIS misc/RiVIS
```

3. Run a RISC-V program
```bash
   $ misc/RiVIS -h
   $ cp test/systemTest/task1/addpos.bin misc/addpos.bin
   $ misc/RiVIS -v -i misc/addpos.bin -o misc/addpos.res
```

## Design
For information on the design see [Design ReadMe](design/ReadMe.md)

## Tests
For information on unit- and system testing see [Test ReadMe](test/ReadMe.md)

## Milestones
Must have componenets (see design for elaboration):

- [x] A command line interface that can takes an input file and an output file.
- [x] Support for reading a binary input file to memory.
- [x] The actual RISC-V ISA simulator.
- [x] Support for writing the registers to a binary file.
- [x] Version control with Git + A remote repositiory
- [x] A make-file with instructions to compile the program.
- [x] Inclusion of test files to verify correct simulator implementation.

Nice to have components:

- [x] CLI support for programe usage.
- [x] Verbosity support; CLI and printing of runtime information
- [x] Debug information on simulator error, e.g. `fwrite` failure, or on reading unknown instruction.
- [x] Automated unit tests.
- [x] Automated system tests of binary programs with known results.
- [ ] Support for Continuous integration
- [ ] Add SingleCycle simulator implementation
- [ ] Add 5 stage pipelined simulator implementation
- [ ] Option to single step simulation.
- [ ] Implement an ASCII UI that displays processor state.


## References
Among the relevant references I would like to highlight **Ripes** as a very well executed graphical RISC-V processor simulator.


* Computer Organization and Design RISC-V Edition, The Hardware Software Interface, by David A. Patterson and John L. Hennessy, 2nd edition, 2021, ISBN: 978-0-12-820331-6.
* [Ripes - A graphical processor simulator and assembly editor for the RISC-V ISA](https://github.com/mortbopet/Ripes/)
* [RISC-V ISA specification](https://riscv.org/specifications/ratified/) - Goto **View Ratified Specs** -> **ISA Volume 1** or **ISA Volume 2**.
* [DTU Course database entry](https://kurser.dtu.dk/course/02155).
* [Course website](https://courses.compute.dtu.dk/02155/).
* [Course Github with labs and final assignments](https://github.com/schoeberl/cae-lab).
* [CMake/CTest introduction](https://cmake.org/getting-started/).
* [GTest introduction](https://google.github.io/googletest/).
