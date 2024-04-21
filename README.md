# Contiguous Memory Allocation
* When run, this program will prompt the user to input a command. The 4 options include requesting and releasing memory, printing the addresses, and compacting the memory.
* There are 3 options when requesting memory: First Fit (F), Best Fit(B), Worst Fit(W)

## Commands
* RQ <processName> <requestAmount> <allocationAlgorithm> - requests memory
* RL <processName> - releases the process from memory
* C - compacts the processes 
* STAT - prints the addresses

## Examples of Commands:
* allocator>RQ P1 4 F
* allocator>RL P1 
* allocator>C
* allocator>STAT

## Identifying Information
* Name: Kevin Huang
* Student ID: 2402212
* Email: kevhuang@chapman.edu
* Course: cpsc 380-02
* Assignment: Assignment 5 - Contiguous Memory Allocation

## Source Files  
* allocator.c

## References
* https://docs.gtk.org/glib/struct.Array.html
* https://docs.gtk.org/glib/struct.HashTable.html 


## Known Errors
* none

## Build Insructions
* gcc `pkg-config --cflags --libs glib-2.0` -o allocator allocator.c

## Execution Instructions
./allocator <memorySize>

## gcc version
* Apple clang version 14.0.0 (clang-1400.0.29.202)
