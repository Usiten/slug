#ifndef SL_VM_H
#define SL_VM_H

#include <stdint.h>
#include "bytecode.h"

enum SL_opcode
{
	OP_PUSH	= 0,
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	__OP_COUNT__
};

typedef enum SL_opcode SL_opcode;

#define SL_STACK_CAPACITY 512

struct SL_vm
{
	SL_bytecode	*bytecode;
	uint64_t	*stack;
	long		sp, ip;		// Stack and instruction pointer
};

typedef struct SL_vm SL_vm;

SL_vm *SL_vm_new(SL_bytecode *bc);
void SL_vm_free(SL_vm **vm);

// Read
SL_opcode SL_vm_read_opcode(SL_vm *vm);
uint64_t SL_vm_read_u64(SL_vm *vm);

// Stack manipulation
void SL_vm_push(SL_vm *vm, uint64_t u);
uint64_t SL_vm_pop(SL_vm *vm);
void SL_vm_print_stack(SL_vm *vm);

// Execution
void SL_vm_execute(SL_vm *vm);

#ifdef SL_VM_IMPLEMETATION

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
	assert(vm->bytecode->data != NULL);
	assert(vm->bytecode->size - (size_t)vm->ip >= 1);

	SL_opcode opcode = SL_bytecode_read_u8(vm->bytecode, vm->ip);
	vm->ip += 1;
	return opcode;
}

uint64_t SL_vm_read_u64(SL_vm *vm)
{
	assert(vm != NULL);
	assert(vm->bytecode != NULL);
	assert(vm->bytecode->data != NULL);
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

void SL_vm_execute(SL_vm *vm)
{
	assert(vm != NULL);
	assert(vm->stack != NULL);
	assert(vm->bytecode != NULL);

	assert(5 == __OP_COUNT__); // If this assertion fail, implement the missing operation and increment it

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

		fprintf(stderr, "[ERROR] Unknonwn opcode 0x%02X (%u) at ip %lu\n", opcode, opcode, vm->ip);
		exit(1);
	}
}

#endif // SL_VM_IMPLEMETATION

#endif // SL_VM_H


