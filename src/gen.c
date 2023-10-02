#include <assert.h>
#include "bytecode.h"
#include "parser.h"
#include "op.h"
#include "gen.h"


void SL_parser_node_to_bytecode_impl(SL_bytecode *bc, SL_parser_node *root)
{
	static_assert(17 == __TOKEN_TYPE_COUNT__); // If this assertion fail, implement the missing operation and increment it
	static_assert(9 == __OP_COUNT__);

	assert(bc != NULL);
	assert(root != NULL);

	if (root->token == NULL) {
		SL_parser_node_to_bytecode(bc, root->left);
		SL_parser_node_to_bytecode(bc, root->right);
		return;
	}

	if (root->token->type == TOKEN_IDENTIFIER) {
		SL_bytecode_write_str(bc, root->token->raw_text);
		return;
	}

	if (root->token->type == TOKEN_IDENTIFIER_VALUE) {
		SL_bytecode_write_u8(bc, OP_PUSH_VAR);
		SL_bytecode_write_str(bc, root->token->raw_text);
		return;
	}

	if (root->token->type == TOKEN_IF) {
		SL_parser_node_to_bytecode(bc, root->left); // comparison
		SL_bytecode_write_u8(bc, OP_JUMP_IF_ZERO);
		uint64_t placeholder_position = bc->size;
		SL_bytecode_write_u64(bc, UINT64_MAX);
		SL_parser_node_to_bytecode(bc, root->right); // Body
		SL_bytecode_write_u64_at_addr(bc, bc->size, placeholder_position);
		return;
	}

	if (root->token->type == TOKEN_ASSIGN) {
		SL_parser_node_to_bytecode(bc, root->right);
		SL_bytecode_write_u8(bc, OP_ASSIGN);
		SL_parser_node_to_bytecode(bc, root->left);
		return;
	}

	if (root->token->type == TOKEN_INTEGER)	{
		uint64_t u = strtoull(root->token->raw_text, NULL, 10);
		SL_bytecode_write_u8(bc, OP_PUSH);
		SL_bytecode_write_u64(bc, u);
		return;
	}

	SL_parser_node_to_bytecode(bc, root->left);
	SL_parser_node_to_bytecode(bc, root->right);

	if (root->token->type == TOKEN_PLUS) {
		SL_bytecode_write_u8(bc, OP_ADD);
		return;
	}

	if (root->token->type == TOKEN_MINUS) {
		SL_bytecode_write_u8(bc, OP_SUB);
		return;
	}

	if (root->token->type == TOKEN_MULTIPLY) {
		SL_bytecode_write_u8(bc, OP_MUL);
		return;
	}

	if (root->token->type == TOKEN_DIVIDE) {
		SL_bytecode_write_u8(bc, OP_DIV);
		return;
	}

	if (root->token->type == TOKEN_LESS_THAN) {
		SL_bytecode_write_u8(bc, OP_LESS_THAN);
		return;
	}
}

void SL_parser_node_to_bytecode(SL_bytecode *bc, SL_parser_node *root)
{
	SL_parser_node_to_bytecode_impl(bc, root);
}