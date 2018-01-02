#include <stdio.h>
#include <stdlib.h>

#ifdef __unix
#include <netinet/in.h>
#elif defined _WIN32
#include <Windows.h>
#endif

#include "vm.h"
#include "opcode.h"

static void convert_to_host_byte_order(bytecode_t *bytecode, size_t length);

int main(int argc, char **argv) {
	if(argc < 2) {
		fprintf(stderr, "%s\n", "error: no program file");
		exit(1);
	}

	long size;
	FILE *program = fopen(argv[1], "rb");
	if(program == NULL) {
		fprintf(stderr, "error: invalid program file path\n");
		exit(1);
	}
	fseek(program, 0, SEEK_END);
	size = ftell(program) / sizeof(bytecode_t);
	rewind(program);

	bytecode_t *bytecode = malloc(sizeof(bytecode_t) * size);
	fread(bytecode, size, sizeof(bytecode_t), program);

	fclose(program);

	convert_to_host_byte_order(bytecode, size);

	VirtualMachine *vm = create_vm(bytecode, 0, 4096 * 1024);
	exec(vm);
	delete_vm(vm);

	free(bytecode);
}

static void convert_to_host_byte_order(bytecode_t *bytecode, size_t length) {
	for(size_t i = 0; i < length; i++) {
		bytecode[i].int32 = ntohl(bytecode[i].int32);
	}
}
