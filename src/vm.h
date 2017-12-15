#ifndef VM_H
#define VM_H

typedef struct VirtualMachine VirtualMachine;

VirtualMachine* create_vm();
void delete_vm(VirtualMachine *vm);
void exec(VirtualMachine *vm);

#endif
