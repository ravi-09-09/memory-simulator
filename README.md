# Memory Simulator Project
### A simulation of how memory is managed inside a computer

## Introduction
This project is an implementation of a memory management simulator. It models how an operating system manages physical and virtual memory. It is not a real OS — it is only a simulation. The goal is not to fully replicate an operating system, but to provide a clear and educational way to understand memory management.

## Features
1. Dynamic memory allocation  
2. Multi-level CPU cache simulation  
3. Virtual memory and paging

## Project Structure
- **include** – header files (class and function declarations)  
- **src** – main implementation code  
- **tests** – programs used to test the simulator  
- **docs** – detailed documentation and explanations  
- **Makefile** – build script for compiling the project  

## How to Build
This project uses a Makefile so that everything can be compiled with one command.

Open a terminal in the project directory and run:

make


### Main simulator

simulator — main program that demonstrates the system working

### Test programs

vm_normal — basic virtual memory test

cache_hard — heavy cache workload

vm_hard — heavy virtual memory stress test

To clean all compiled files, run:

make clean


## How to Run
After building, the executables are created in the project directory.

You can run them from the terminal, for example:

./vm_normal
./cache_hard
./vm_hard


Each program prints its results to the console.

## Conclusion
This project is mainly focused on learning how memory management works.  
It simplifies many real-world details, but it shows the core ideas clearly.  

For more detailed explanations about the design and implementation, please refer to the **docs** folder.

