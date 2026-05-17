==================================================
PRODUCER-CONSUMER PROBLEM - OPERATING SYSTEMS ASSIGNMENT

Description:
This program solves the Producer-Consumer (Bounded Buffer) 
problem using semaphores for process synchronization.

Features:
- Supports any buffer size
- Supports any number of producers and consumers
- Uses shared memory for the buffer
- Uses System V semaphores (via given library)

How to Compile:
    g++ -o prodcons main.cpp -Wall

How to Run:
    ./prodcons [buffer_size] [num_producers] [num_consumers]
    
Examples:
    ./prodcons 5 2 2
    ./prodcons 10 3 3
    ./prodcons 8 4 2

Semaphores Usage:
- FIRST  (0)  → Mutex (Mutual Exclusion)
- SECOND (1)  → Empty slots counter
- THIRD  (2)  → Full slots counter

Files Included:
- main.cpp          : Main program logic
- semaphor.cpp      : Semaphore library (provided)
- shared_mf.cpp     : Shared memory buffer library (provided)
- prodcons          : Executable binary

The program appropriately avoids race conditions, deadlock, and 
buffer overflow/underflow.
====================================================================
