#include <assert.h>
#include "bytecode.h"
#include "parser.h"
#include "op.h"
#include "gen.h"

void SL_parser_node_to_bytecode(SL_bytecode *bc, SL_parser_node *root)
{
	static_assert(13 == __TOKEN_TYPE_COUNT__); // If this assertion fail, implement the missing operation and increment it
	static_assert(8 == __OP_COUNT__);

	assert(bc != NULL);
	assert(root != NULL);

	if (root->token == NULL) {
		SL_parser_node_to_bytecode(bc, root->left);
		SL_parser_node_to_bytecode(bc, root->right);
		return;
	}

	if (root->token->type == TOKEN_IDENTIFIER) {
		if (!root->is_rhs) {
			SL_bytecode_write_str(bc, root->token->raw_text);
		} else {
			SL_bytecode_write_u8(bc, OP_PUSH_VAR);
			SL_bytecode_write_str(bc, root->token->raw_text);
		}
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
