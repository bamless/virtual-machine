#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <netinet/in.h>

#include "vm.h"
#include "opcode.h"

static void conver_to_host_byte_order(int32_t *bytecode, size_t length);

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
	size = ftell(program);
	rewind(program);

	unsigned char *bytecode = malloc(sizeof(unsigned char) * size);
	fread(bytecode, size, 1, program);

	fclose(program);

	size /= sizeof(int32_t);
	conver_to_host_byte_order((int32_t *) bytecode, size);

	VirtualMachine *vm = create_vm((bytecode_t *) bytecode, 0, 4096 * 1024);
	exec(vm);
	delete_vm(vm);
}

static void conver_to_host_byte_order(int32_t *bytecode, size_t length) {
	for(size_t i = 0; i < length; i++) {
		bytecode[i] = ntohl(bytecode[i]);
	}
}
