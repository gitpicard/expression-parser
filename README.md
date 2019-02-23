# Expression-Parser
A simple expression parser/evaluator written in C. Has no dependencies other than the C standard library.

# Compiling
The project comes with a makefile for GCC to make compiling easier. Type 'make' to compile the program using GCC in C89 mode, type 'make clean' to remove all the compiled output, and type 'make run' to run the compiled program.

# Embedding
The project can easily be embedded in your own project. You only need expression.h and expression.c for this. Use the function 'expr_add_identifier' to add your own custom functions to the evaluator. See src/main.c for an example on how to use the library.
