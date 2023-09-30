#include <stdio.h>

#include "tool.h"
#include "bytecode.h"

SL_bytecode *SL_bytecode_new(void)
{
	SL_bytecode *bc = calloc(1, sizeof(SL_bytecode));
	SL_ALLOC_CHECK(bc)
	return bc;
}

void SL_bytecode_free(SL_bytecode **bc)
{
	assert(*bc != NULL);

	free((*bc)->data);
	(*bc)->data = NULL;

	free(*bc);
	*bc = NULL;
}

void SL_bytecode_write_u8(SL_bytecode * bc, uint8_t u)
{
	assert(bc != NULL);

	bc->data = realloc(bc->data, bc->size + 1);
	SL_ALLOC_CHECK(bc->data)
	bc->data[bc->size] = u;
	bc->size++;
}

void SL_bytecode_write_u64(SL_bytecode *bc, uint64_t u)
{
	assert(bc != NULL);

	// TODO: Optimize the hell out of this crap
	SL_bytecode_write_u8(bc, (u & 0xFF00000000000000) >> 7 * 8);
	SL_bytecode_write_u8(bc, (u & 0xFF000000000000) >> 6 * 8);
	SL_bytecode_write_u8(bc, (u & 0xFF0000000000) >> 5 * 8);
	SL_bytecode_write_u8(bc, (u & 0xFF00000000) >> 4 * 8);
	SL_bytecode_write_u8(bc, (u & 0xFF000000) >> 3 * 8);
	SL_bytecode_write_u8(bc, (u & 0xFF0000) >> 2 * 8);
	SL_bytecode_write_u8(bc, (u & 0xFF00) >> 1 * 8);
	SL_bytecode_write_u8(bc, (u & 0xFF) >> 0 * 8);
}

uint8_t SL_bytecode_read_u8(SL_bytecode *bc, uint64_t addr)
{
	assert(bc != NULL);
	assert(bc->data != NULL);
	assert(bc->size - addr >= 1);

	uint8_t u = bc->data[addr];
	return u;
}

uint64_t SL_bytecode_read_u64(SL_bytecode *bc, uint64_t addr)
{
	assert(bc != NULL);
	assert(bc->data != NULL);
	assert(bc->size >= 8);
	assert(bc->size - addr >= 8);

	// TODO: Optimize this as well
	uint8_t a, b, c, d, e, f, g, h;
	a = SL_bytecode_read_u8(bc, addr + 0);
	b = SL_bytecode_read_u8(bc, addr + 1);
	c = SL_bytecode_read_u8(bc, addr + 2);
	d = SL_bytecode_read_u8(bc, addr + 3);
	e = SL_bytecode_read_u8(bc, addr + 4);
	f = SL_bytecode_read_u8(bc, addr + 5);
	g = SL_bytecode_read_u8(bc, addr + 6);
	h = SL_bytecode_read_u8(bc, addr + 7);

	uint64_t u 	= (uint64_t) a << 7 * 8
				| (uint64_t) b << 6 * 8
				| (uint64_t) c << 5 * 8
				| (uint64_t) d << 4 * 8
				| (uint64_t) e << 3 * 8
				| (uint64_t) f << 2 * 8
				| (uint64_t) g << 1 * 8
				| (uint64_t) h;

	return u;
}

void SL_bytecode_print(SL_bytecode *bc)
{
	assert(bc != NULL);
	assert(bc->data != NULL);

	fprintf(stdout, "-- Bytecode --\n");

	for (size_t i = 0; i < bc->size; ++i)
	{
		const uint8_t u = SL_bytecode_read_u8(bc, i);
		fprintf(stdout, "%02hhx ", u);

		if ((i + 1) % 8 == 0) {
			putchar('\n');
		}
	}

	putchar('\n');
	fprintf(stdout, "--------------\n");
}

void SL_bytecode_dump(SL_bytecode *bc, char *file_name)
{
	FILE *f = fopen(file_name, "wb");

	for (size_t i = 0; i < bc->size; ++i) {
		uint8_t n = bc->data[i];
		fwrite(&n, 1, 1, f);
	}

	fclose(f);
}