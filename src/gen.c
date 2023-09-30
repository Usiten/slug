#include <assert.h>
#include "bytecode.h"
#include "parser.h"
#include "vm.h"
#include "gen.h"

void SL_parser_node_to_bytecode(SL_bytecode *bc, SL_parser_node *root)
{
	assert(12 == __TOKEN_TYPE_COUNT__); // If this assertion fail, implement the missing operation and increment it

	assert(bc != NULL);
	assert(root != NULL);

	if (root->token == NULL) {
		SL_parser_node_to_bytecode(bc, root->left);
		SL_parser_node_to_bytecode(bc, root->right);
		return;
	}

	if (root->token->type == TOKEN_IDENTIFIER) {
		uint64_t *var = SL_hash_map_get(bc->var_to_addr, root->token->raw_text);
		if (var == NULL) {
			SL_hash_map_insert(bc->var_to_addr, root->token->raw_text, bc->var_count);
			SL_bytecode_write_u8(bc, OP_PUSH);
			SL_bytecode_write_u64(bc, bc->var_count);
			bc->var_count++;
		} else {
			SL_bytecode_write_u8(bc, OP_PUSH);
			SL_bytecode_write_u64(bc, *var);
		}
		return;
	}

	if (root->token->type == TOKEN_ASSIGN) {
		SL_parser_node_to_bytecode(bc, root->left);
		SL_parser_node_to_bytecode(bc, root->right);
		SL_bytecode_write_u8(bc, OP_ASSIGN);
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
}
