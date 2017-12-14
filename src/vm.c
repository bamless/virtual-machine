#include "vm.h"

#include <stdlib.h>
#include <stdio.h>

#define STACK_SIZE 4096 * 1024

struct VirtualMachine {
	int32_t loc_size;
	int32_t *locals;    // local scoped data
	int32_t *code;      // array of byte codes to be executed
	int32_t *stack;     // oprerator stack

	int32_t pc;         // program counter (aka. IP - instruction pointer)
	int32_t sp;         // stack pointer
	int32_t fp;         // frame pointer (for local scope)

	int32_t lfp;
	int32_t lsp;
};

#define PUSH(vm, v)   vm->stack[++vm->sp] = v // push value on top of the stack
#define POP(vm)       vm->stack[vm->sp--]     // pop value from top of the stack
#define NEXTCODE(vm)  vm->code[vm->pc++]      // get next bytecode

VirtualMachine* create_vm(int32_t *code, int pc, int datasize) {
	VirtualMachine *vm = malloc(sizeof(*vm));
	vm->code = code;
	vm->pc = pc;
	vm->fp = 0;
	vm->sp = -1;
	vm->lfp = 0;
	vm->lsp = 0;
	vm->loc_size = datasize;
	vm->locals = malloc(sizeof(int32_t) * datasize);
	vm->stack = malloc(sizeof(int32_t) * STACK_SIZE);

	return vm;
}

void delete_vm(VirtualMachine *vm) {
	free(vm->stack);
	free(vm->locals);
	free(vm);
}

void exec(VirtualMachine *vm) {
	for(;;) {
		int32_t a, b, v, addr, offset, argc, fsize;

		//fetch
		int32_t opcode = NEXTCODE(vm);
		switch (opcode) { //decode
		case HALT: return;
		case CONST_I32:
			v = NEXTCODE(vm);
			PUSH(vm, v);
			break;
		case ADD_I32:
			b = POP(vm);
			a = POP(vm);
			PUSH(vm, a + b);
			break;
		case SUB_I32:
			b = POP(vm);
			a = POP(vm);
			PUSH(vm, a - b);
			break;
		case MUL_I32:
			b = POP(vm);
			a = POP(vm);
			PUSH(vm, a * b);
			break;
		case LT_I32:
			b = POP(vm);
			a = POP(vm);
			PUSH(vm, (a < b) ? 1 : 0);
			break;
		case EQ_I32:
			b = POP(vm);
			a = POP(vm);
			PUSH(vm, (a == b) ? 1 : 0);
			break;
		case JMP:
			v = NEXTCODE(vm);
			vm->pc = v;
			break;
		case JMPT:
			addr = NEXTCODE(vm);
			if(POP(vm))
				vm->pc = addr;
			break;
		case JMPF:
			addr = NEXTCODE(vm);
			if(!POP(vm))
				vm->pc = addr;
			break;
		case LOAD:
			offset = NEXTCODE(vm);
			PUSH(vm, vm->locals[vm->lfp + offset]);
			break;
		case STORE:
			v = POP(vm);
			offset = NEXTCODE(vm);
			vm->locals[vm->lfp + offset] = v;
			break;
		case GLOAD:
			addr = NEXTCODE(vm);
			v = vm->locals[addr];
			PUSH(vm, v);
			break;
		case GSTORE:
			v = POP(vm);
			addr = NEXTCODE(vm);
			vm->locals[addr] = v;
			break;
		case CALL:
			// we expect all args to be on the stack
			addr = NEXTCODE(vm); // get next instruction as an address of procedure jump ...
			argc = NEXTCODE(vm); // ... and next one as number of arguments to load ...
			fsize = NEXTCODE(vm);

			PUSH(vm, argc);   // ... save num args ...
			PUSH(vm, vm->fp); // ... save frame pointer ...
			PUSH(vm, vm->lfp);
			PUSH(vm, vm->pc); // ... save instruction pointer ...

			vm->fp = vm->sp;  // ... set new frame pointer ...
			vm->lfp = vm->lsp;
			vm->pc = addr;    // ... move instruction pointer to target procedure address
			vm->lsp += fsize;
			break;
		case GETARG:
			offset = NEXTCODE(vm);
			argc = vm->stack[vm->fp - 3];
			PUSH(vm, vm->stack[vm->fp + (offset - 4 - (argc - 1))]);
			break;
		case RET:
			v = POP(vm);      // pop return value from top of the stack
			vm->lsp = vm->lfp;
			vm->sp = vm->fp;  // ... return from procedure address ...
			vm->pc = POP(vm); // ... restore instruction pointer ...
			vm->lfp = POP(vm);
			vm->fp = POP(vm); // ... restore framepointer ...
			argc = POP(vm);   // ... hom many args procedure has ...
			vm->sp -= argc;   // ... discard all of the args left ...
			PUSH(vm, v);      // ... leave return value on top of the stack
			break;
		case POP:
		    (void) POP(vm);    // throw away value at top of the stack
		    break;
		case PRINT:
		    v = POP(vm);        // pop value from top of the stack break;
		    printf("%d\n", v);  // break; and print it
		    break;
		default:
		    break;
		}
	}
}
