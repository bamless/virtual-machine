#include "vm.h"
#include "opcode.h"

#include <stdlib.h>
#include <stdio.h>

#define STACK_SIZE 4096 * 1024

struct VirtualMachine {
	int32_t loc_size;

	bytecode_t *locals; // local scoped data
	bytecode_t *code;   // array of byte codes to be executed
	bytecode_t *stack;  // oprerator stack

	int32_t pc;         // program counter (aka. IP - instruction pointer)
	int32_t sp;         // stack pointer
	int32_t fp;         // frame pointer (for local scope)

	int32_t lfp;        // local data frame pointer
	int32_t lsp;        // local data stack pointer
};

#define PUSH(vm, v)     vm->stack[++vm->sp] = v       // push value on top of the stack
#define PUSH_I32(vm, v) vm->stack[++vm->sp].int32 = v // push 32bit integer on top of the stack
#define PUSH_F32(vm, v) vm->stack[++vm->sp].fp32  = v // push 32bit floating point on top of stack
#define POP(vm)         vm->stack[vm->sp--]           // pop value from top of the stack
#define NEXTCODE(vm)    vm->code[vm->pc++]            // get next bytecode

VirtualMachine* create_vm(bytecode_t *code, int pc, int datasize) {
	VirtualMachine *vm = malloc(sizeof(*vm));
	vm->code = code;
	vm->pc = pc;
	vm->fp = 0;
	vm->sp = -1;
	vm->lfp = 0;
	vm->lsp = 0;
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

void exec(VirtualMachine *vm) {
	for(;;) {
		bytecode_t a, b, v, addr, offset, argc, fsize;

		//fetch
		bytecode_t opcode = NEXTCODE(vm);
		switch (opcode.int32) { //decode
		case HALT: return;
		case CONST_I32:
			v = NEXTCODE(vm);
			PUSH(vm, v);
			break;
		case ADD_I32:
			b = POP(vm);
			a = POP(vm);
			PUSH_I32(vm, a.int32 + b.int32);
			break;
		case SUB_I32:
			b = POP(vm);
			a = POP(vm);
			PUSH_I32(vm, a.int32 - b.int32);
			break;
		case MUL_I32:
			b = POP(vm);
			a = POP(vm);
			PUSH_I32(vm, a.int32 * b.int32);
			break;
		case DIV_I32:
			b = POP(vm);
			a = POP(vm);
			PUSH_I32(vm, a.int32 / b.int32);
			break;
		case MOD_I32:
			b = POP(vm);
			a = POP(vm);
			PUSH_I32(vm, a.int32 % b.int32);
			break;
		case LT_I32:
			b = POP(vm);
			a = POP(vm);
			PUSH_I32(vm, (a.int32 < b.int32) ? 1 : 0);
			break;
		case LE_I32:
			b = POP(vm);
			a = POP(vm);
			PUSH_I32(vm, (a.int32 <= b.int32) ? 1 : 0);
			break;
		case GT_I32:
			b = POP(vm);
			a = POP(vm);
			PUSH_I32(vm, (a.int32 > b.int32) ? 1 : 0);
			break;
		case GE_I32:
			b = POP(vm);
			a = POP(vm);
			PUSH_I32(vm, (a.int32 >= b.int32) ? 1 : 0);
			break;
		case EQ_I32:
			b = POP(vm);
			a = POP(vm);
			PUSH_I32(vm, (a.int32 == b.int32) ? 1 : 0);
			break;
		case NEQ_I32:
			b = POP(vm);
			a = POP(vm);
			PUSH_I32(vm, (a.int32 != b.int32) ? 1 : 0);
			break;
		case CONST_F32:
			v = NEXTCODE(vm);
			PUSH(vm, v);
			break;
		case DUP:
			v = vm->stack[vm->sp];
			PUSH(vm, v);
			break;
		case JMP:
			v = NEXTCODE(vm);
			vm->pc = v.int32;
			break;
		case JMPT:
			addr = NEXTCODE(vm);
			if(POP(vm).int32)
				vm->pc = addr.int32;
			break;
		case JMPF:
			addr = NEXTCODE(vm);
			if(!POP(vm).int32)
				vm->pc = addr.int32;
			break;
		case LOAD:
			offset = NEXTCODE(vm);
			PUSH(vm, vm->locals[vm->lfp + offset.int32]);
			break;
		case STORE:
			v = POP(vm);
			offset = NEXTCODE(vm);
			vm->locals[vm->lfp + offset.int32] = v;
			break;
		case CALL:
			// we expect all args to be on the stack
			addr = NEXTCODE(vm); // get next instruction as an address of procedure jump ...
			argc = NEXTCODE(vm); // ... and next one as number of arguments to load ...
			fsize = NEXTCODE(vm);

			PUSH(vm, argc);       // ... save num args ...
			PUSH_I32(vm, vm->fp); // ... save frame pointer ...
			PUSH_I32(vm, vm->lfp);
			PUSH_I32(vm, vm->pc); // ... save instruction pointer ...

			vm->fp  =  vm->sp;  // ... set new frame pointer ...
			vm->lfp =  vm->lsp;
			vm->pc  =  addr.int32;    // ... move instruction pointer to target procedure address
			vm->lsp += fsize.int32;
			break;
		case GETARG:
			offset = NEXTCODE(vm);
			argc = vm->stack[vm->fp - 3];
			PUSH(vm, vm->stack[vm->fp + (offset.int32 - 4 - (argc.int32 - 1))]);
			break;
		case RET:
			v = POP(vm);      // pop return value from top of the stack
			vm->lsp = vm->lfp;
			vm->sp = vm->fp;  // ... return from procedure address ...
			vm->pc = POP(vm).int32; // ... restore instruction pointer ...
			vm->lfp = POP(vm).int32;
			vm->fp = POP(vm).int32; // ... restore framepointer ...
			argc = POP(vm);   // ... hom many args procedure has ...
			vm->sp -= argc.int32;   // ... discard all of the args left ...
			PUSH(vm, v);      // ... leave return value on top of the stack
			break;
		case POP:
		    (void) POP(vm);    // throw away value at top of the stack
		    break;
		case PRINT:
		    v = POP(vm);        // pop value from top of the stack break;
		    printf("%d\n", v.int32);  // break; and print it
		    break;
		case PRINT_FP:
			v = POP(vm);
			printf("%f\n", v.fp32);
			break;
		default:
		    break;
		}
	}
}
