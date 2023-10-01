#include <stdio.h>
#include <string.h>
#include "tool.h"
#include "vm.h"

SL_vm *SL_vm_new(SL_bytecode *bc) 
{
	SL_vm *vm = calloc(1, sizeof(SL_vm));
	SL_ALLOC_CHECK(vm);

	vm->stack = calloc(SL_STACK_CAPACITY, sizeof(uint64_t));
	SL_ALLOC_CHECK(vm->stack);

	vm->bytecode = bc;
	vm->var_to_addr = SL_hash_map_new();
	vm->var_data = calloc(64, sizeof(uint64_t));

	return vm;
}

void SL_vm_free(SL_vm **vm)
{
	assert(*vm != NULL);

	SL_bytecode_free(&(*vm)->bytecode);
	SL_hash_map_free(&(*vm)->var_to_addr);

	free((*vm)->var_data);
	(*vm)->var_data = NULL;

	free((*vm)->stack);
	(*vm)->stack = NULL;

	free(*vm);
	*vm = NULL;
}

SL_opcode SL_vm_read_opcode(SL_vm *vm)
{
	assert(vm != NULL);
	assert(vm->bytecode != NULL);
	assert(vm->bytecode->code != NULL);
	assert(vm->bytecode->size - (size_t)vm->ip >= 1);

	SL_opcode opcode = SL_bytecode_read_u8(vm->bytecode, vm->ip);
	vm->ip += 1;
	return opcode;
}

uint64_t SL_vm_read_u64(SL_vm *vm)
{
	assert(vm != NULL);
	assert(vm->bytecode != NULL);
	assert(vm->bytecode->code != NULL);
	assert(vm->bytecode->size - (size_t)vm->ip >= 8);

	uint64_t u = SL_bytecode_read_u64(vm->bytecode, vm->ip);
	vm->ip += 8;
	return u;
}

char *SL_vm_read_str(SL_vm *vm)
{
	char *s = SL_bytecode_read_str(vm->bytecode, vm->ip);
	vm->ip += strlen(s) + 1 + 8; // One for \0, 8 for the len, ONE FOR NEXT IP
	return s;
}


void SL_vm_stack_push(SL_vm *vm, uint64_t u)
{
	assert(vm != NULL);
	assert(vm->stack != NULL);

	if (vm->sp >= SL_STACK_CAPACITY) {
		fprintf(stderr, "[ERROR] Stack overflow. At instruction %llu.\n", vm->ip);
		exit(1);
	}

	vm->stack[vm->sp] = u;
	vm->sp += 1;
}

uint64_t SL_vm_stack_pop(SL_vm *vm)
{
	assert(vm != NULL);
	assert(vm->stack != NULL);

	if (vm->sp <= 0) {
		fprintf(stderr, "[ERROR] Stack underflow. At instruction %llu.\n", vm->ip);
		exit(1);
	}

	uint64_t u = vm->stack[vm->sp - 1];
	vm->sp -= 1;

	return u;
}

void SL_vm_print_stack(SL_vm *vm)
{
	assert(vm != NULL);
	assert(vm->stack != NULL);

	printf("-- Stack Top --\n");
	for (long i = vm->sp - 1; i >= 0 ; --i) {
		fprintf(stdout, "%s   %lu\n", 
			i == vm->sp - 1 
				? ">" 
				: " ",
			vm->stack[i]);
	}
	printf("---------------\n");
}

void SL_vm_print_vars(SL_vm *vm)
{
	(void) vm;
	// assert(vm != NULL);
	// assert(vm->bytecode != NULL);

	// printf("-- Var Data --\n");
	// for (size_t i = 0; i < vm->var_count; ++i) {
	// 	printf("0x%02X  : %llu\n", i, vm->var_data[i]);
	// }

	// printf("---------------\n");
}

void SL_vm_execute(SL_vm *vm)
{
	assert(vm != NULL);
	assert(vm->stack != NULL);
	assert(vm->bytecode != NULL);

	static_assert(7 == __OP_COUNT__, "Not all opcode are implemented"); // If this assertion fail, implement the missing operation and increment it

	while ((size_t)vm->ip < vm->bytecode->size)
	{
		// SL_vm_print_stack(vm);
		SL_opcode opcode = SL_vm_read_opcode(vm);

		if (opcode == OP_PUSH) {
			uint64_t u = SL_vm_read_u64(vm);
			SL_vm_stack_push(vm, u);
			continue;
		}

		if (opcode == OP_ADD) {
			uint64_t u = SL_vm_stack_pop(vm);
			uint64_t v = SL_vm_stack_pop(vm);
			SL_vm_stack_push(vm, u + v);
			continue;
		}

		if (opcode == OP_SUB) {
			uint64_t u = SL_vm_stack_pop(vm);
			uint64_t v = SL_vm_stack_pop(vm);
			SL_vm_stack_push(vm, v - u);
			continue;
		}

		if (opcode == OP_MUL) {
			uint64_t u = SL_vm_stack_pop(vm);
			uint64_t v = SL_vm_stack_pop(vm);
			SL_vm_stack_push(vm, u * v);
			continue;
		}

		if (opcode == OP_DIV) {
			uint64_t u = SL_vm_stack_pop(vm);
			uint64_t v = SL_vm_stack_pop(vm);
			SL_vm_stack_push(vm, v / u);
			continue;
		}

		if (opcode == OP_ASSIGN) {
			uint64_t value = SL_vm_stack_pop(vm);
			char *varn = SL_vm_read_str(vm);

			uint64_t *vaddr = SL_hash_map_get(vm->var_to_addr, varn);

			if (vaddr) {
				*vaddr = value;
				printf("FOUND : varn: %s, vaddr: %llu\n", varn, *vaddr);
			} else {
				SL_hash_map_insert(vm->var_to_addr, varn, value);
				printf("CREAT : varn: %s, vaddr: %llu\n", varn, value);
			}
			continue;
		}

		if (opcode == OP_PUSH_VAR) {
			char *varn = SL_vm_read_str(vm);
			uint64_t *vaddr = SL_hash_map_get(vm->var_to_addr, varn);
			if (vaddr) {
				SL_vm_stack_push(vm, *vaddr);
				printf("PUSHS : varn: %s, vaddr: %llu\n", varn, *vaddr);
			} else {
				fprintf(stderr, "[ERROR] Use of undeclared variable '%s'\n", varn);
				exit(1);
			}
			continue;
		}

		fprintf(stderr, "[ERROR] Unknonwn opcode 0x%02X (%u) at ip %lu\n", opcode, opcode, vm->ip);
		exit(1);
	}
}
