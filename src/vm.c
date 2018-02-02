#include "vm.h"
#include "opcode.h"

#include <stdlib.h>
#include <stdio.h>

#define STACK_SIZE 1024

struct VirtualMachine {
	int32_t loc_size;

	bytecode_t *locals; // local scoped data
	bytecode_t *code;   // array of byte codes to be executed
	bytecode_t *stack;  // oprerator stack

	int32_t opsp;       // stack pointer of operand stack

	int32_t pc;         // program counter (aka. IP - instruction pointer)
	int32_t sp;         // stack pointer (points at the end of local storage)
	int32_t fp;         // frame pointer (points at the start of local storage stack frame)
};

#define PUSH(vm, v)     (vm->stack[++vm->opsp] = (bytecode_t)(v)) // push value on top of the stack
#define POP(vm)         (vm->stack[vm->opsp--])                   // pop value from top of the stack
#define PUSHARG(vm, v)  (vm->locals[vm->sp++] = v)                //push values beyond curr frame as args to the next
#define NEXTCODE(vm)    (vm->code[vm->pc++])                      // get next bytecode

VirtualMachine* create_vm(bytecode_t *code, int32_t pc, int32_t datasize) {
	VirtualMachine *vm = malloc(sizeof(*vm));
	vm->code = code;
	vm->pc = pc;
	vm->opsp = -1;
	vm->fp = 0;
	vm->sp = 0;
	vm->loc_size = datasize;
	vm->locals = malloc(sizeof(bytecode_t) * datasize);
	vm->stack = malloc(sizeof(bytecode_t) * STACK_SIZE);

	return vm;
}

void delete_vm(VirtualMachine *vm) {
	free(vm->stack);
	free(vm->locals);
	free(vm);
}

#define MAX(a,b) (((a)>(b))?(a):(b))

void exec(VirtualMachine *vm) {
	for(;;) {
		bytecode_t a, b, v, addr, offset, argc;

		//fetch
		switch (NEXTCODE(vm).int32) { //decode
		case HALT: return;
		case CONST_I32:
			v = NEXTCODE(vm);
			PUSH(vm, v);
			continue;
		case ADD_I32:
			b = POP(vm);
			a = POP(vm);
			PUSH(vm, a.int32 + b.int32);
			continue;
		case SUB_I32:
			b = POP(vm);
			a = POP(vm);
			PUSH(vm, a.int32 - b.int32);
			continue;
		case MUL_I32:
			b = POP(vm);
			a = POP(vm);
			PUSH(vm, a.int32 * b.int32);
			continue;
		case DIV_I32:
			b = POP(vm);
			a = POP(vm);
			PUSH(vm, a.int32 / b.int32);
			continue;
		case MOD_I32:
			b = POP(vm);
			a = POP(vm);
			PUSH(vm, a.int32 % b.int32);
			continue;
		case LT_I32:
			b = POP(vm);
			a = POP(vm);
			PUSH(vm, (a.int32 < b.int32) ? 1 : 0);
			continue;
		case LE_I32:
			b = POP(vm);
			a = POP(vm);
			PUSH(vm, (a.int32 <= b.int32) ? 1 : 0);
			continue;
		case GT_I32:
			b = POP(vm);
			a = POP(vm);
			PUSH(vm, (a.int32 > b.int32) ? 1 : 0);
			continue;
		case GE_I32:
			b = POP(vm);
			a = POP(vm);
			PUSH(vm, (a.int32 >= b.int32) ? 1 : 0);
			continue;
		case EQ_I32:
			b = POP(vm);
			a = POP(vm);
			PUSH(vm, (a.int32 == b.int32) ? 1 : 0);
			continue;
		case NEQ_I32:
			b = POP(vm);
			a = POP(vm);
			PUSH(vm, (a.int32 != b.int32) ? 1 : 0);
			continue;
		case CONST_F32:
			v = NEXTCODE(vm);
			PUSH(vm, v);
			continue;
		case DUP:
			v = vm->stack[vm->sp];
			PUSH(vm, v);
			continue;
		case JMP:
			v = NEXTCODE(vm);
			vm->pc = v.int32;
			continue;
		case JMPT:
			addr = NEXTCODE(vm);
			if(POP(vm).int32)
				vm->pc = addr.int32;
			continue;
		case JMPF:
			addr = NEXTCODE(vm);
			if(!POP(vm).int32)
				vm->pc = addr.int32;
			continue;
		case LOAD:
			offset = NEXTCODE(vm);
			PUSH(vm, vm->locals[vm->fp + offset.int32]);
			continue;
		case STORE:
			v = POP(vm);
			offset = NEXTCODE(vm);
			vm->locals[vm->fp + offset.int32] = v;
			// if the offset exceeds the stack pointer increment it
			vm->sp = MAX(vm->sp, vm->fp + offset.int32 + 1);
			continue;
		case PUSHARG:
			v = POP(vm);
			PUSHARG(vm, v);
			continue;
		case CALL:
			// we expect all args to be on the local storage stack
			addr = NEXTCODE(vm);  // get next value in the bytecode as address jump
			argc = NEXTCODE(vm);  // next one as number of arguments to load

			PUSH(vm, vm->pc); // save the program counter
			PUSH(vm, vm->fp); // save the stack frame pointer

			// set the new frame pointer to stack pointer minus agrs number.
			// this way the function arguments will be the first n elements in the new frame
			vm->fp  =  vm->sp - argc.int32;
			vm->pc = addr.int32;
			continue;
		case RET:
			v = POP(vm); // pop return value from top of the stack

			vm->sp = vm->fp;        // reset stack pointer to frame pointer
			vm->fp = POP(vm).int32; // restore frame pointer for locals stack
			vm->pc = POP(vm).int32; // restore instruction pointer

			PUSH(vm, v); // re-push the return value on top of the operand stack
			continue;
		case RETVOID:
			vm->sp = vm->fp;
			vm->fp = POP(vm).int32;  // restore frame pointer for locals stack
			vm->pc = POP(vm).int32;  // restore instruction pointer
			continue;
		case POP:
			(void) POP(vm);          // throw away value at top of the stack
			continue;
		case PRINT:
			v = POP(vm);             // pop value from top of the stack...
			printf("%d\n", v.int32); // ...and print it
			continue;
		case PRINT_FP:
			v = POP(vm);             // pop the value from top of the stack...
			printf("%f\n", v.fp32);  // ...and print it as a floating point
			continue;
		default:
			continue;
		}
	}
}

#undef MAX
