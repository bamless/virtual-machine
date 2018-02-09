#include <stdio.h>
#include <stdlib.h>

#include "vm.h"
#include "opcode.h"
#include "endian.h"

static void convert_to_host_byte_order(bytecode_t *bytecode, size_t length);

int main(int argc, char **argv) {
	if(argc < 2) {
		fprintf(stderr, "%s\n", "error: no program file");
		exit(1);
	}

	// open bytecode file and get its size
	long size;
	FILE *program = fopen(argv[1], "r+b");
	if(program == NULL) {
		fprintf(stderr, "error: invalid program file path\n");
		exit(1);
	}
	fseek(program, 0, SEEK_END);
	size = ftell(program) / sizeof(bytecode_t);
	rewind(program);

	// read bytecode from file
	bytecode_t *bytecode = malloc(sizeof(bytecode_t) * size);
	fread(bytecode, size, sizeof(bytecode_t), program);

	fclose(program);

	// convert the byecode to host byte order
	convert_to_host_byte_order(bytecode, size);

	//execute the code
	VirtualMachine *vm = create_vm(bytecode, 0, 4096 * 1024);
	exec(vm);
	delete_vm(vm);

	free(bytecode);
}

static void convert_to_host_byte_order(bytecode_t *bytecode, size_t length) {
	for(size_t i = 0; i < length; i++)
		bytecode[i].int32 = be32toh(bytecode[i].int32);
}
