# RISC-V ISA Simulator - RiVIS
This repository contains my work on a RISC-V Instruction Set Architeture simulator, i.e. a program that simulates the execution of RISC-V binary instructions in software. The program is developed as the final assignment of the course [02155 - Computer Architecture and Engineering](https://kurser.dtu.dk/course/02155) as taught on the Technical University of Denmark autumn 2025.
For the assignment specification see [Assignment specification Github](https://github.com/schoeberl/cae-lab/tree/master/finasgmt).


## Requirements
Here the requirements on the simulator for the assignment specification will summerised. Requirements regarding the report, deadlines, and other practicalities are not presented here.

* Implement the 32 bit RISC-V integer instructionset RV32I (with the exception of the instructions `FENCE`, `FENCE.TSO`, `PAUSE`, `EBREAK` ).
* The simulator must be able to read and execute RISC-V instructions from a binary input file.
* The simulator must stop executing either at the end of the input file, or if the `ecall`-instruction is called with an input of `10`.
* The simulator must write out the 32 registers to a binary file at end of program, from lowest to highest.
* The simulator must implement 1 MB program memory (or possible more). Test programs will set stack pointer to `0x100000` with the stack growing towards lower addresses.
* The simulator must be able to execute on an x64 machine running Ubuntu.

The correct implementation of the simulator will be judged by comparing the simulators binary output of the 32 registers to known solutions for a set of test files.

## Design
Here I will present a rough design for an implementation of the simulator program. First I will present a minimum viable solution that has the necessary steps to solve the assignment. Following that, I will present a number of nice-to have additions that would enhance the value of the simulator. These, or more likely a subset of them, will be implemented as time allows.
Lastly I will present a simple design for the acutal RISC-V ISA simulator part of the program.

### Minimum viable solution
Presented as overall component blocks a minimal solution simulator must have:

- [x] A command line interface that can takes an input file and an output file.
- [x] Support for reading a binary input file to memory.
- [x] The actual RISC-V ISA simulator.
- [x] Support for writing the registers to a binary file.

Furthermore the project must support the following features for convenience:

- [x] A make-file with instructions to compile the program.
- [ ] Inclusion of test files to verify correct simulator implementation.

### Desireable improvements
The following represents the "nice-to-have" additions I would like to implement in my simulator. They are loosely ranked from simple to most advanced.

- [x] CLI support for progame usage.
- [x] CLI support for verbosity + debug prints to console while executing simulator,
- [x] Debug information on simulator error, e.g. `fwrite` failure, or on reading unknown instruction.
- [x] CLI support for a "copy" argument, that overwrites program behaviour to copy input file to output file. Useable for testing file read/write implementation. 
- [x] Automated unit tests.
- [ ] Automated system tests of binary programs with known results.
- [ ] Refactor simulator implementation to emulate a datapath.
- [ ] Expand simulator implementation to have realistic control path.
- [ ] Expand simulator implementation to a 5 stage pipelined processor.
- [ ] Add CLI option to single step simulation.
- [ ] Expose processor states to be available in main().
- [ ] Implement an ASCII UI that displays processor state.

### ISA simulator design

At its core, the simulator needs to support the following parts:

* Memory to hold program instructions and stack
* Register file
* Decoder for translating binary instructions opcode/funct3/funct7/funct12 to an operation.
* Immediate generator.
* Switch case over the operation to execute instruction, e.g. add, load, branch ect.
* A while-loop that executes untill end of instructions array, or `ecall`-instruction with exit value, whichever comes first.

It has the following inputs from the caller (main):
* A pointer to program
* Program size
* A pointer to the register file (for output)

It has the following output to the caller (main)
* A pointer to (possible modified) program
* A pointer to the (modified) register file
* Return status

The sequence diagram below how they can be put together to form a minimal ISA simulator:

![ISA simulator sequence diagram.](/fig/isa.svg)


## References
Among the relevant references I would like to highlight **Ripes** as a very well executed graphical RISC-V processor simulator.


* Computer Organization and Design RISC-V Edition, The Hardware Software Interface, by David A. Patterson and John L. Hennessy, 2nd edition, 2021, ISBN: 978-0-12-820331-6.
* [Ripes - A graphical processor simulator and assembly editor for the RISC-V ISA](https://github.com/mortbopet/Ripes/)
* [RISC-V ISA specification](https://riscv.org/specifications/ratified/) - Goto **View Ratified Specs** -> **ISA Volume 1** or **ISA Volume 2**.
* [DTU Course database entry](https://kurser.dtu.dk/course/02155).
* [Course website](https://courses.compute.dtu.dk/02155/).
* [Course Github with labs and final assignments](https://github.com/schoeberl/cae-lab).
