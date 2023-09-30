#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "tool.h"
#include "lex.h"

SL_token *SL_token_new(SL_token_type type, char *type_as_string, char* raw_text, size_t line_no, size_t column_no)
{
	SL_token *token = calloc(1, sizeof(SL_token));
	SL_ALLOC_CHECK(token)

	token->type = type;
	token->type_as_string = type_as_string;
	token->raw_text = raw_text;
	token->line_no = line_no;
	token->column_no = column_no;
	token->file = ".";

	return token;
}

SL_token *SL_next_token_from_input(char **input)
{
	static_assert(12 == __TOKEN_TYPE_COUNT__, "Not all token type are hadled"); // If this assertion fail, implement the missing token and increment it

	SL_token *token = NULL;
	static size_t column = 0;
	static size_t line = 0;

	while (isspace(**input)) {
		column++;

		if (**input == '\n') {
			column = 0;
			line++;
		}

		(*input)++;
    }

	if (**input == '\0') {
		token = SL_TOKEN_NEW(TOKEN_EOF, "(End of file)");
		return token;
	}

	if (isdigit(**input)) {
		char *start = *input;

        while (isdigit(**input)) {
            (*input)++;
        }

		token = SL_TOKEN_NEW(TOKEN_INTEGER, SL_strndup(start, *input - start));
		column += *input - start;
		return token;
	}

	if (**input == '=') {
        token = SL_TOKEN_NEW(TOKEN_ASSIGN, "=");
        (*input)++;
		column++;
		return token;
    }

	if (**input == '(') {
        token = SL_TOKEN_NEW(TOKEN_LPAREN, "(");
        (*input)++;
		column++;
		return token;
    }

	if (**input == ')') {
        token = SL_TOKEN_NEW(TOKEN_RPAREN, ")");
        (*input)++;
		column++;
		return token;
    }

	if (**input == ';') {
        token = SL_TOKEN_NEW(TOKEN_SEMICOLON, ";");
        (*input)++;
		column++;
		return token;
    }

	if (**input == '+') {
        token = SL_TOKEN_NEW(TOKEN_PLUS, "+");
        (*input)++;
		column++;
		return token;
    }

	if (**input == '-') {
        token = SL_TOKEN_NEW(TOKEN_MINUS, "-");
        (*input)++;
		column++;
		return token;
    }

	if (**input == '*') {
        token = SL_TOKEN_NEW(TOKEN_MULTIPLY, "*");
        (*input)++;
		column++;
		return token;
    }

	if (**input == '/') {
        token = SL_TOKEN_NEW(TOKEN_DIVIDE, "/");
        (*input)++;
		column++;
		return token;
    }

	if (isalpha(**input) || **input == '_')	{
		char *start = *input;

		while (isalnum(**input) || **input == '_') {
			(*input)++;
		}

		token = SL_TOKEN_NEW(TOKEN_IDENTIFIER, SL_strndup(start, *input - start));
		column += *input - start;
		return token;
	}

	token = SL_TOKEN_NEW(TOKEN_UNKNOWN, *input);
	(*input)++;
	return token;
}

void SL_token_print_list(SL_token *token)
{
	while (token)
	{
		fprintf(stdout, "%s: %s\n", token->type_as_string, token->raw_text);
		token = token->next;
	}
}
