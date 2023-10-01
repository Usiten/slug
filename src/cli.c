#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bytecode.h"
#include "parser.h"
#include "lex.h"
#include "gen.h"
#include "vm.h"
#include "cli.h"

enum USAGE {
	TITLE = 0,
	COMPILE,
	EXECUTE,
	RUN,
	RUN_N_DUMP
};

static const char *usage[] = {
	[TITLE] 		= "Usage:\n",
	[COMPILE] 		= "    -c, --compile  `file.sl` [`out.slx`]   Compile `file.sl` to bytecode assembly `file.slx` (or `out.slx` if provided)\n",
	[EXECUTE] 		= "    -x, --execute  `file.slx`              Execute bytecode assembly `file.slx`\n",
	[RUN] 			= "    -r, --run  `file.sl`                   Compile and execute `file.sl` without generating bytecode assembly\n",
	[RUN_N_DUMP] 	= "    -R, --run-n-dump  `file.sl`            Compile and execute `file.sl` without generating bytecode assembly, and dump stack and vars to stdout\n",
	NULL,
};

void SL_print_usage(void)
{
	size_t i = 0;

	while (usage[i] != NULL) {
		fprintf(stderr, "%s", usage[i]);
		++i;
	}
}

void SL_load_source_from_sl_file(char *file_name, char **buffer)
{
	// TODO: Handle errors
	FILE *f = fopen(file_name, "r");
	fseek(f, 0L, SEEK_END);
	size_t file_len = ftell(f);
	fseek(f, 0L, SEEK_SET);	

	*buffer = calloc(file_len + 1, sizeof(char));
	(*buffer)[file_len] = '\0';
	fread(*buffer, sizeof(char), file_len, f);
	fclose(f);
}

SL_bytecode *SL_load_bytecode_from_slx_file(char *file_name)
{
	// TODO: Handle errors
	FILE *f = fopen(file_name, "rb");
	fseek(f, 0L, SEEK_END);
	size_t file_len = ftell(f);
	fseek(f, 0L, SEEK_SET);	

	SL_bytecode *bc = SL_bytecode_new();
	bc->code = calloc(file_len, sizeof(uint8_t));
	bc->size = file_len;
	fread(bc->code, sizeof(uint8_t), file_len, f);
	fclose(f);

	return bc;
}

void SL_compile(char *input, char *output) 
{ 
	char *buffer;
	SL_load_source_from_sl_file(input, &buffer);
	char * start = buffer;
	SL_token *token = SL_next_token_from_input(&buffer); 
	SL_token *first = token;
	SL_token *head = token;
	while (token->type != TOKEN_EOF)
	{
		token->file = input;
		token->next = SL_next_token_from_input(&buffer);
		token = token->next;
	}
	SL_parser_node *root = SL_parser_parse(&first);
	SL_bytecode *bc = SL_bytecode_new();
	SL_parser_node_to_bytecode(bc, root);
	SL_parser_free_all_nodes();
	SL_token_free(&head);
	free(start);
	SL_bytecode_dump(bc, output);
	SL_bytecode_free(&bc);
}

void SL_execute(char *input) 
{ 
	SL_bytecode *bc = SL_load_bytecode_from_slx_file(input);
	SL_vm *vm = SL_vm_new(bc);
	SL_vm_execute(vm);
	SL_vm_print_stack(vm);
	SL_vm_free(&vm);
}

void SL_run(char *input)
{
	char *buffer;
	SL_load_source_from_sl_file(input, &buffer);
	char * start = buffer;
	SL_token *token = SL_next_token_from_input(&buffer); 
	SL_token *first = token;
	SL_token *head = token;
	while (token->type != TOKEN_EOF)
	{
		token->file = input;
		token->next = SL_next_token_from_input(&buffer);
		token = token->next;
	}

	// SL_token_print_list(first);
	SL_parser_node *root = SL_parser_parse(&first);
	SL_bytecode *bc = SL_bytecode_new();
	// SL_parser_print_nodes(root, 0);
	SL_parser_node_to_bytecode(bc, root);
	SL_parser_free_all_nodes();
	SL_token_free(&head);
	free(start);
	// SL_bytecode_print(bc);
	SL_vm *vm = SL_vm_new(bc);
	SL_vm_execute(vm);
	SL_vm_free(&vm);
}

void SL_run_n_dump(char *input)
{
	char *buffer;
	SL_load_source_from_sl_file(input, &buffer);
	char * start = buffer;
	SL_token *token = SL_next_token_from_input(&buffer); 
	SL_token *first = token;
	SL_token *head = token;
	while (token->type != TOKEN_EOF)
	{
		token->file = input;
		token->next = SL_next_token_from_input(&buffer);
		token = token->next;
	}

	// SL_token_print_list(first);
	SL_parser_node *root = SL_parser_parse(&first);
	SL_bytecode *bc = SL_bytecode_new();
	// SL_parser_print_nodes(root, 0);
	SL_parser_node_to_bytecode(bc, root);
	SL_parser_free_all_nodes();
	SL_token_free(&head);
	free(start);
	// SL_bytecode_print(bc);
	SL_vm *vm = SL_vm_new(bc);
	SL_vm_execute(vm);
	SL_vm_print_stack(vm);
	SL_vm_print_vars(vm);
	SL_vm_free(&vm);
}


int SL_cli_handle(int argc, char** argv)
{
	char *input_file = NULL;
    char *output_file = NULL;
    
    char *program = SL_shift_args(&argc, &argv);
	(void) program;
    
    if (argc < 1) {
        SL_print_usage();
        exit(1);
    }

	char *command = argv[0];
    
	if (strcmp(command, "-c") == 0 || strcmp(command, "--compile") == 0) {
        if (argc < 2) {
            fprintf(stderr, "[ERROR] Missing argument\n%s%s", usage[TITLE], usage[COMPILE]);
            exit(1);
        }

        input_file = argv[1];

		if (argv[2] == NULL) {
			output_file = calloc(strlen(input_file) + 1 + 1, sizeof(char));
			strcpy(output_file, input_file);
			output_file[strlen(input_file)] = 'x'; // TODO : add an output file option 
		}
		else {
			output_file = argv[2];
		}

		SL_compile(input_file, output_file);
    } 
	else if (strcmp(command, "-x") == 0 || strcmp(command, "--execute") == 0) {
        if (argc < 2) {
            fprintf(stderr, "[ERROR] Missing argument\n%s%s", usage[TITLE], usage[EXECUTE]);
            exit(1);
        }

		SL_execute(argv[1]);
    } 
	else if (strcmp(command, "-r") == 0 || strcmp(command, "--run") == 0) {
        if (argc < 2) {
            fprintf(stderr, "[ERROR] Missing argument\n%s%s", usage[TITLE], usage[RUN]);
            exit(1);
        }

		SL_run(argv[1]);
    } 
	else if (strcmp(command, "-R") == 0 || strcmp(command, "--run-n-dump") == 0) {
        if (argc < 2) {
            fprintf(stderr, "[ERROR] Missing argument\n%s%s", usage[TITLE], usage[RUN_N_DUMP]);
            exit(1);
        }

		SL_run_n_dump(argv[1]);
    } 
	else {
        fprintf(stderr, "[ERROR] Unknown command: %s\n", command);
		SL_print_usage();
        exit(1);
    }
    
    return 0;
}