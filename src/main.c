#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <netinet/in.h>

#include "vm.h"
#include "opcode.h"

static long begin, end;

static 	int32_t p2[] = {
	CALL, 5, 0, 3,
	HALT,
	CONST_I32, 1,
	CONST_I32, 2,
	CONST_I32, 3,
	STORE, 2,
	STORE, 1,
	STORE, 0,
	LOAD, 0,
	PRINT,
	CALL, 25, 0, 0,//20
	RET,
	CONST_I32, 4,
	STORE, 0,
	CALL, 37, 0, 1,
	LOAD, 0,
	PRINT,
	RET,
	CONST_I32, 9,
	STORE, 0,
	RET,
};

// bytecode factorial function
#define N 12
static int32_t factorial[] = {
	CALL, 6, 0, 0,
	PRINT,
	HALT,
	CONST_I32, N,
	CALL, 13, 1, 0,//8
	RET,
	//int fact(n) {
	//		if(n == 0) return 1
	GETARG, 0,
	CONST_I32, 0,
	EQ_I32,
	JMPF, 23,
	CONST_I32, 1,
	RET,
	//else
	GETARG, 0,
	CONST_I32, 1,
	SUB_I32,
	CALL, 13, 1, 0,
	GETARG, 0,
	MUL_I32,
	RET,
};

//bytecode recursive fibonacci function
#define FIB_ADDR 13
#define ARG 30
static int32_t fibonacci[] = {
	CALL, 6, 0, 0,        // 0 - call main
	PRINT,				  // 4
	HALT,                 // 5 - exit
	// entrypoint - main function
	CONST_I32, ARG,       // 6 - put 6
	CALL, FIB_ADDR, 1, 0, // 7 - call function: fib(arg)
	RET,                  // 11 - return
	//int fib(n) {
	//		if(n == 0) return 0
	GETARG, 0,            // 12 - load last function argument N
	CONST_I32, 0,         // 14 - put 0
	EQ_I32,               // 16 - check equality: N == 0
	JMPF, 23,             // 17 - if they are NOT equal, goto 10
	CONST_I32, 0,         // 19 - otherwise put 0
	RET,                  // 21 - and return it
	//     if(n < 3) return 1;
	GETARG, 0,            // 22 - load last function argument N
	CONST_I32, 3,         // 24 - put 3
	LT_I32,               // 26 - check if 3 is less than N
	JMPF, 33,             // 27 - if 3 is NOT less than N, goto 20
	CONST_I32, 1,         // 29 - otherwise put 1
	RET,                  // 31 - and return it
	//     else return fib(n-1) + fib(n-2);
	GETARG, 0,            // 32 - load last function argument N
	CONST_I32, 1,         // 34 - put 1
	SUB_I32,              // 36 - calculate: N-1, result is on the stack
	CALL, FIB_ADDR, 1, 0, // 37 - call fib function with 1 arg. from the stack
	GETARG, 0,            // 41 - load N again
	CONST_I32, 2,         // 43 - put 2
	SUB_I32,              // 45 - calculate: N-2, result is on the stack
	CALL, FIB_ADDR, 1, 0, // 46 - call fib function with 1 arg. from the stack
	ADD_I32,              // 50 - since 2 fibs pushed their ret values on the stack, just add them
	RET,                  // 51 - return from procedure
};

static void conver_to_host_byte_order(int32_t *bytecode, size_t length);

int main(int argc, char **argv) {
	if(argc < 2) {
		fprintf(stderr, "%s\n", "error: no program file");
		exit(1);
	}

	long size;
	FILE *program = fopen(argv[1], "rb");
	if(program == NULL) {
		fprintf(stderr, "error: invalid program file path\n");
		exit(1);
	}
	fseek(program, 0, SEEK_END);
	size = ftell(program);
	rewind(program);

	unsigned char *bytecode = malloc(sizeof(unsigned char) * size);
	fread(bytecode, size, 1, program);

	fclose(program);

	size /= sizeof(int32_t);
	conver_to_host_byte_order((int32_t *) bytecode, size);
	
	VirtualMachine *vm = create_vm((int32_t*) bytecode, 0, 4096 * 1024);
	exec(vm);
}

static void conver_to_host_byte_order(int32_t *bytecode, size_t length) {
	for(size_t i = 0; i < length; i++) {
		bytecode[i] = ntohl(bytecode[i]);
	}
}
