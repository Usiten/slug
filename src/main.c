#include <stdio.h>

#define SL_TOOL_IMPLEMENTATION
#include "tool.h"

#define SL_BYTECODE_IMPLEMENTATION
#include "bytecode.h"

#define SL_VM_IMPLEMETATION
#include "vm.h"

#define SL_LEX_IMPLEMENTATION
#include "lex.h"

#define SL_PARSER_IMPLEMENTATION
#include "parser.h"

#define SL_GEN_IMPLEMENTATION
#include "gen.h"

int main(int argc, char **argv)
{
	const char *program = SL_shift_args(&argc, &argv);
	(void) program;

	char *input = "1 + 2 * 3; 8 / 4 / 2; 0; 15/(5*3);";
	SL_token *token = SL_next_token_from_input(&input); 
	SL_token *first = token;

	while (token->type != TOKEN_EOF)
	{
		token->next = SL_next_token_from_input(&input);
		token = token->next;
	}	

	SL_parser_node *root = SL_parser_parse(&first);
	SL_bytecode *bc = SL_bytecode_new();
	SL_parser_node_to_bytecode(bc, root);

	SL_vm *vm = SL_vm_new(bc);
	SL_vm_execute(vm);
	SL_vm_print_stack(vm);
	SL_vm_free(&vm);

	return 0;
}