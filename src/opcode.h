#ifndef OPCODE_H
#define OPCODE_H

enum Opcode {
	HALT = 0,       // stop program
	ADD_I32 = 1,    // int add
	SUB_I32 = 2,    // int sub
	MUL_I32 = 3,    // int mul
	DIV_I32 = 4,    // int div
	MOD_I32 = 5,    // int mod
	LT_I32 = 6,     // int less than
	LE_I32 = 7,     // int less or equal
	GT_I32 = 8,     // int greater than
	GE_I32 = 9,     // int greateror equal
	EQ_I32 = 10,    // int equal
	NEQ_I32 = 11,   // int cot equal
	JMP = 12,       // branch
	JMPT = 13,      // branch if true
	JMPF = 14,      // branch if false
	CONST_I32 = 15, // push int const
	LOAD = 16,      // load from local (i.e. load from current frame)
	STORE = 17,     // store in local (i.e. store in current frame)
	PRINT = 18,     // print value on top of the stack
	POP = 19,       // throw away top of the stack
	CALL = 20,      // call procedure
	GETARG = 21,    // get argument of current procedure call
	RET = 22,       // return from procedure
	DUP = 23,       // duplicate the value on top of the stackm

	CONST_F32 = 24, // push floating poiunt const
	PRINT_FP = 25,  // print value on top of stack as floating point
};

#endif
