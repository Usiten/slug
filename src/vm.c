#include <stdio.h>
#include "tool.h"
#include "vm.h"

SL_vm *SL_vm_new(SL_bytecode *bc) 
{
	SL_vm *vm = calloc(1, sizeof(SL_vm));
	SL_ALLOC_CHECK(vm);

	vm->stack = calloc(SL_STACK_CAPACITY, sizeof(uint64_t));
	SL_ALLOC_CHECK(vm->stack);

	vm->bytecode = bc;

	return vm;
}

void SL_vm_free(SL_vm **vm)
{
	assert(*vm != NULL);

	SL_bytecode_free(&(*vm)->bytecode);

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

void SL_vm_push(SL_vm *vm, uint64_t u)
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

uint64_t SL_vm_pop(SL_vm *vm)
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
	assert(vm != NULL);
	assert(vm->bytecode != NULL);

	printf("-- Var Data --\n");
	for (size_t i = 0; i < vm->bytecode->var_count; ++i) {
		printf("0x%02X  : %llu\n", i, vm->bytecode->var_data[i]);
	}

	printf("---------------\n");
}

void SL_vm_execute(SL_vm *vm)
{
	assert(vm != NULL);
	assert(vm->stack != NULL);
	assert(vm->bytecode != NULL);

	static_assert(6 == __OP_COUNT__, "Not all opcode are implemented"); // If this assertion fail, implement the missing operation and increment it

	while ((size_t)vm->ip < vm->bytecode->size)
	{
		SL_opcode opcode = SL_vm_read_opcode(vm);

		if (opcode == OP_PUSH) {
			uint64_t u = SL_vm_read_u64(vm);
			SL_vm_push(vm, u);
			continue;
		}

		if (opcode == OP_ADD) {
			uint64_t u = SL_vm_pop(vm);
			uint64_t v = SL_vm_pop(vm);
			SL_vm_push(vm, u + v);
			continue;
		}

		if (opcode == OP_SUB) {
			uint64_t u = SL_vm_pop(vm);
			uint64_t v = SL_vm_pop(vm);
			SL_vm_push(vm, v - u);
			continue;
		}

		if (opcode == OP_MUL) {
			uint64_t u = SL_vm_pop(vm);
			uint64_t v = SL_vm_pop(vm);
			SL_vm_push(vm, u * v);
			continue;
		}

		if (opcode == OP_DIV) {
			uint64_t u = SL_vm_pop(vm);
			uint64_t v = SL_vm_pop(vm);
			SL_vm_push(vm, v / u);
			continue;
		}

		if (opcode == OP_ASSIGN) {
			uint64_t v = SL_vm_pop(vm);
			uint64_t var_addr = SL_vm_pop(vm);
			vm->bytecode->var_data[var_addr] = v;
			continue;
		}

		fprintf(stderr, "[ERROR] Unknonwn opcode 0x%02X (%u) at ip %lu\n", opcode, opcode, vm->ip);
		exit(1);
	}
}
