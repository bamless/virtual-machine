#ifndef OPCODE_H
#define OPCODE_H

enum Opcode {
	HALT = 0,       // stop program
	ADD_I32 = 1,    // int add
	SUB_I32 = 2,    // int sub
	MUL_I32 = 3,    // int mul
	DIV_I32 = 4,
	MOD_I32 = 5,
	LT_I32 = 6,     // int less than
	LE_I32 = 7,
	GT_I32 = 8,
	GE_I32 = 9,
	EQ_I32 = 10,     // int equal
	NEQ_I32 = 11,
	JMP = 12,        // branch
	JMPT = 13,       // branch if true
	JMPF = 14,       // branch if false
	CONST_I32 = 15,  // push constant integer
	LOAD = 16,       // load from local
	STORE = 17,      // store in local
	PRINT = 18,      // print value on top of the stack
	POP = 19,        // throw away top of the stack
	CALL = 20,       // call procedure
	GETARG = 21,
	RET = 22,        // return from procedure
	DUP = 23,

	CONST_F32 = 24,
	PRINT_FP = 25,
};

#endif
