#ifndef SL_BYTECODE_H
#define SL_BYTECODE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "tool.h"
#include "map.h"

struct SL_bytecode
{
	uint8_t		*code;
	size_t		size;

	char **str_registar;
	size_t str_count;
};

typedef struct SL_bytecode SL_bytecode;

SL_bytecode *SL_bytecode_new(void);
void SL_bytecode_free(SL_bytecode **sl);

// Write
void SL_bytecode_write_u8(SL_bytecode *bc, uint8_t u);
void SL_bytecode_write_u64(SL_bytecode *bc, uint64_t u);
void SL_bytecode_write_u8_at_addr(SL_bytecode *bc, uint8_t u, uint64_t addr);
void SL_bytecode_write_u64_at_addr(SL_bytecode *bc, uint64_t u, uint64_t addr);
void SL_bytecode_write_str(SL_bytecode *bc, char *str);

// Read
uint8_t SL_bytecode_read_u8(SL_bytecode *bc, uint64_t addr);
uint64_t SL_bytecode_read_u64(SL_bytecode *bc, uint64_t addr);
char *SL_bytecode_read_str(SL_bytecode *bc, uint64_t addr);

// IO
void SL_bytecode_print(SL_bytecode *bc);
void SL_bytecode_dump(SL_bytecode *bc, char *file_name);

#endif	// SL_BYTECODE_H
