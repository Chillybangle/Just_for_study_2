# Processor
            
_What is it?_
-----------

Processor - a set of programs that simulate the
processing of assembly code into processor commands
and their subsequent execution.

*The Latest Version*
------------------

The latest version of the program. The program is 
debugged and works fine.

*Using*
-------------

You can overlook Factorial.txt and Square_equation.txt
files with code examples and make sure that the commands
used are similar to the assembler commands which you can
easy google.

List of available commands:
_push, pop, add, sub, mul, div, dup, jmp, jb, jbe, je,
ja, jae, jne, call, ret, sqrt, floor, sin, cos, hlt_

cmd _in []_ - input the float number in stack
cmd _out []_ - pop and out value from stack
cmd _PRINT_PHRASE [phrase_number]_ - specific cmd for
Square_equation program to output its text

*Installation*
------------

Install program folder, go to the directory. Open 
the command line, print 'make' on the command line
(you need to have g++ compiler installed). Done.

*Launching the program*
---------------------

When you have written the assembly text, save it in
txt format. Then you have to assemble your program.

To do this, go to the build folder and run Asm.exe
with an argument your_file_to_assemble.txt. You will
get a binary file b.bin in the main folder of the
project. It is ready for further launch by the
processor. To run it, run the program CPU.exe with
the argument b.bin.

Below is an example code that you can try
(there are two test programs in the folder: for 
calculating the factorial of a number and finding
the roots of a quadratic equation):
>cd Build; ./ASM.exe ../Factorial.txt; ./CPU.exe ../b.bin

or
>cd Build; ./ASM.exe ../Square_equation.txt; ./CPU.exe ../b.bin

*Easy launch of test programs*
----------------------------

You can type in the command line to launch factorial
program (a program that counts the factorial of a
number):
>gcc mk_factorial.cpp;./a.out

or

>gcc mk_square.cpp;./a.out

to launch a program that counts the roots of a
quadratic equation

*Contacts*
--------

If you want to discuss the project you can write me to
the mail ostrovnoi.ka@phystech.edu 
