#ifndef VM_H
#define VM_H

#include <stdlib.h>

typedef union bytecode_t {
	int32_t int32;
	float   fp32;
} bytecode_t;
typedef struct VirtualMachine VirtualMachine;

VirtualMachine* create_vm(bytecode_t *code, int32_t pc, int32_t datasize);
void delete_vm(VirtualMachine *vm);
void exec(VirtualMachine *vm);

#endif
