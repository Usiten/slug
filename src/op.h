#ifndef SL_OP_H
#define SL_OP_H

enum SL_opcode
{
	OP_PUSH	= 0,
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_LESS_THAN,
	OP_ASSIGN,
	OP_PUSH_VAR,
	OP_IF,
	OP_END_IF,
	__OP_COUNT__
};

typedef enum SL_opcode SL_opcode;

#endif // SL_OP_H