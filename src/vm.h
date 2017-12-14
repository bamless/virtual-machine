#ifndef VM_H
#define VM_H

typedef struct VirtualMachine VirtualMachine;

enum {
	ADD_I32,    // int add
	SUB_I32,    // int sub
	MUL_I32,    // int mul
	LT_I32,     // int less than
	EQ_I32,     // int equal
	JMP,        // branch
	JMPT,       // branch if true
	JMPF,       // branch if false
	CONST_I32,  // push constant integer
	LOAD,       // load from local
	GLOAD,      // load from global
	STORE,      // store in local
	GSTORE,     // store in global memory
	PRINT,      // print value on top of the stack
	POP,        // throw away top of the stack
	HALT,       // stop program
	CALL,       // call procedure
	GETARG,
	RET         // return from procedure
};


VirtualMachine* create_vm();
void delete_vm(VirtualMachine *vm);
void exec(VirtualMachine *vm);

#endif
