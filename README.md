# CptS460

##### Operating Systems and Computer Architecture | KC Wang | Spring 2021 | Washington State University

<!--(https://eecs.wsu.edu/~cs460/) -->
<!-- https://eecs.wsu.edu/~cs460/samples/ -->

## Course Goal
Design and implement a REAL embedded operating system

## Resources

* [Website](https://eecs.wsu.edu/~cs460/ "eecs.wsu.edu/~cs460/")
* [Samples & Base Code](https://eecs.wsu.edu/~cs460/samples/ "eecs.wsu.edu/~cs460/samples/")
* Embedded and Real-time Operating Systems by K.C. Wang
* Design and Implementation of the MTX Operating System by K.C. Wang


## Labs

1. [Booter](./LAB1)
2. [LCD Driver](./LAB2)
3. [Timer and KBD Drivers](./LAB3)
4. [Midterm](./Midterm)
5. [User Mode Commands and Syscalls](./LAB5)
6. [Fork & Exec](./LAB6)
7. [Buffer Management](./LAB7)
8. [Last](./LAST)

## Last Assignment

### Objective
Develop our own User Command Program

### Requirements

| System Call | Function | E & RT OS |
| :------------ |:---------------|:---------------|
| init | fork several login processes on serial ports | pg 321|
| login | handle login processes | pg 322 |
| ls [filename] | list cwd or filename | pg 299 |
| cat [filename] | display file contents or stdin to screen | - |
| more [filename] | display file one page/line at a time | - |
| grep pattern [filename] | prints each line matching the pattern | - |
| l2u [file1 file2] | convert lower to upper case | - |
| cp [file1 file2] | copy files or directories | - |
| sh | executable program, support I/O redirections and pipes | pg 365 |

### Base Code

sdimage:

            /---|
                |----bin/ : All binary executables are in /bin
                |
                |---dev/ : special files tty0  c 4 0 
                |                        fd0   b 2 0 
                |                        sdc   b 3 0 
                |                        ttyS0 c 5 0
                |                        ttyS1 c 5 1
                |                        ttyS2 c 5 2   
                |---etc/ : passwd file 
                |           
                |---boot/: bootable EOS kernels
                |
                |---user/: users HOME directories


mku: script to generate a NEW user mode command from test.c and copy to /bin of sdimage

string.c: bcopy(), memcpy(), memset(), memcmp(), strcmp(), strcpy(), strlen(), strcat(), atoi(), etc

ucode.c: rest of user commands we don't have to write

uio.c: printk(char*); prints(char*), printi(int), printu(u32), printx(u32), printc(char)

## Prerequisites
[Ubuntu](https://ubuntu.com/download/desktop "ubuntu.com/download/desktop") Linux version 20.04

## Installation

1. Install ARM Cross Compiling packages

    ```
    sudo apt-get install gcc-arm-none-eabi
    sudo apt-get install qemu-system-arm
    ```
2. Clone the repository

   ```
   git clone https://github.com/k-radford/CptS460.git
   ```

3. Start Kernel

   ```
   cd CptS460/LAST
   ./mku test
   ```
4. Run a Command 
    ```
    ##################### KATES's HELP MENU #####################
    #  ls       cd     pwd    cat   more    cp     mv   >   >>  #
    #  mkdir    rmdir  creat  rm    chmod   chown  lpr  <   |   #
    #############################################################
    SH 1 (input command): _
    ```

## Demo Commands & Grading

| Command | Expected Results | Observed | Weight |
| :------------ |:---------------|:---------------|:---------------|
| Boot up | P1 (init) create login processes | show 3 logins: console, ttys0, ttys1 | 10 |
| login | login from tty0 | user=root pass=12345, show root logged in and sh2 | 10 |
|  | login from ttyS0, ttyS1 | show can login | 5 |
|  | logout; login again | logout from console and ttys0, show another login | 5 |
| Control-C | sh must not die | on tty0 or console only | 5 |
| `ls` | SAME as ls -l in Linux | ls; ls /bin | 10 |
| `cat` | show input lines from stdin | enter keys from keyboard, cat echos | 5 |
| `cat f` | show contents of file f |   | 5 |
| `cat f1 > f2; ls` | show f1 and f2 SAME size |   | 10 |
| `grep abcd` | grep for abcd from stdin |   | 5 |
| `grep printf f` | show lines with "printf" ONLY |   | 5 |
| `more f` | pause after display ONE screen, Enter: advance by line, Space: advance by screen | MUST pause to let user input keys | 5 |
| `cat f \ more` | must PAUSE for Enter/Space | show can stop to allow user input | 10 |
| `cat f \ grep printf` | show lines with printf | ONE Pipe | 10 |
| `cat f \ l2u \ grep PRINT` | outputs all uppercase | 2 pipes | 20 |
| `cat < f \ cat \ grep print > out; cat out` | 3 PIPEs | out file created with contents | 10 |
| `cp f g; ls` | show f and g SAME size | Test their cp | 10 |

## Course Topics

1. Introduction to Operating Systems:
    Unix/Linux, MTX. computer system and operations, system development 
    software, PC emulators, link C and assembly programs.

2. Booting OS

3. ARM Archicture, ARM programming and embedded systems
     
4. Processes:
    Concept and implementation of processes; process states,
    context switching, process scheduling.  

5. Process management: 
      fork, wait, exit, exec, signals, pipes in Unix/Linux and Wanix. 
   Processes in (microKernel based) OS:
      Micro vs. Monnolithic Kernels, tasks, servers and user processes in Minix; 

6. Process Synchronization:  
    The process model; mutual exclusion and critical regions, 
    Implementation of low-level mutual exclusion primitives.
    Synchronization primitives; events, event queues, semaphores.

7. Process Communication:    
    High-level process synchronization constructs; messages.

8. Process Control:      
    Scheduling algorithms. Dead lock and starvation problems.

9. Memory Management:
    Memory management schemes  
    Paging and Virtual memory

10. I/O device drivers:         
    Interrupts and interrupt processing
    Interraction between interrupt handler and process.
    Design and implementation of I/O drivers;

11. File Systems: 
    Review of EXT2 file system.
    NFS and RFS based on UDP and TCP/IP

11. Real-time OS
