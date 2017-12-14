#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "vm.h"

static volatile long begin, end;

long current_time_ms() {
	struct timeval t;
	gettimeofday(&t, NULL);
	return (long) ((t.tv_sec + (t.tv_usec / 1000000.0)) * 1000.0);
}

int fib(int n) {
	if(n == 0 || n == 1)
		return n;

	return fib(n - 1) + fib(n - 2);
}

int fact(int n) {
	if(n == 0) return 1;
	return n * fact(n - 1);
}

int main() {
	int32_t p2[] = {
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
	int32_t n = 12;
	int32_t factorial[] = {
		CALL, 6, 0, 0,
		PRINT,
		HALT,
		CONST_I32, n,
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
	int32_t fib_addr = 13;
	int32_t arg = 30;
	int32_t fibonacci[] = {
		CALL, 6, 0, 0,        // 0 - call main
		PRINT,				  // 4
		HALT,                 // 5 - exit
		// entrypoint - main function
	    CONST_I32, arg,       // 6 - put 6
	    CALL, fib_addr, 1, 0, // 7 - call function: fib(arg)
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
	   	CALL, fib_addr, 1, 0, // 37 - call fib function with 1 arg. from the stack
	   	GETARG, 0,            // 41 - load N again
	   	CONST_I32, 2,         // 43 - put 2
	   	SUB_I32,              // 45 - calculate: N-2, result is on the stack
	   	CALL, fib_addr, 1, 0, // 46 - call fib function with 1 arg. from the stack
	   	ADD_I32,              // 50 - since 2 fibs pushed their ret values on the stack, just add them
	   	RET,                  // 51 - return from procedure
	};

	VirtualMachine *vm = create_vm(factorial, 0, 4096 * 1024);
	begin = current_time_ms();
	exec(vm);
	end = current_time_ms();
	delete_vm(vm);

	printf("Done executing bytecode in: %ldms\n", end - begin);

	begin = current_time_ms();
	fact(n);
	end = current_time_ms();

	printf("Done executing native in: %ldms\n", end - begin);

}
