#ifndef SL_VM_H
#define SL_VM_H

#include <stdint.h>
#include "bytecode.h"
#include "op.h"

#define SL_STACK_CAPACITY 512

struct SL_vm
{
	SL_bytecode	*bytecode;
	uint64_t	*stack;
	long		sp, ip;		// Stack and instruction pointer
	SL_hash_map	*var_to_addr;
	uint64_t	*var_data;
	size_t		var_count;
};

typedef struct SL_vm SL_vm;

SL_vm *SL_vm_new(SL_bytecode *bc);
void SL_vm_free(SL_vm **vm);
void SL_vm_print_vars(SL_vm *vm);

// Read
SL_opcode SL_vm_read_opcode(SL_vm *vm);
uint64_t SL_vm_read_u64(SL_vm *vm);
char *SL_vm_read_str(SL_vm *vm);

// Stack manipulation
void SL_vm_stack_push(SL_vm *vm, uint64_t u);
uint64_t SL_vm_stack_pop(SL_vm *vm);
void SL_vm_print_stack(SL_vm *vm);

// Execution
void SL_vm_execute(SL_vm *vm);

#endif // SL_VM_H


