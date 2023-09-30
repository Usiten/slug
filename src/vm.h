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
	OP_ASSIGN,
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
void SL_vm_print_vars(SL_vm *vm);

// Execution
void SL_vm_execute(SL_vm *vm);

#endif // SL_VM_H


