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

void SL_token_free(SL_token **token)
{
	if ((*token)->next != NULL)
		SL_token_free(&((*token)->next));

	if ((*token)->type == TOKEN_IDENTIFIER || (*token)->type == TOKEN_INTEGER || (*token)->type == TOKEN_IF) 
	{ // Both points to an strdup of the original source
		free((*token)->raw_text);
		(*token)->raw_text = NULL;
	}

	free(*token);
	*token = NULL;
}

#define MAKE_TOKEN(type, text)				\
token = SL_TOKEN_NEW(type, text);			\
(*input)++;									\
column++;									\
return token;								

SL_token *SL_next_token_from_input(char **input)
{
	static_assert(17 == __TOKEN_TYPE_COUNT__, "Not all token type are hadled"); // If this assertion fail, implement the missing token and increment it

	SL_token *token = NULL;
	static size_t column = 1;
	static size_t line = 1;

	while (isspace(**input)) {
		column++;

		if (**input == '\n') {
			column = 1;
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

	if (**input == '<') {
        MAKE_TOKEN(TOKEN_LESS_THAN, "<")
    }

	if (**input == '=') {
        MAKE_TOKEN(TOKEN_ASSIGN, "=")
    }

	if (**input == '(') {
        MAKE_TOKEN(TOKEN_LPAREN, "(")
    }

	if (**input == ')') {
        MAKE_TOKEN(TOKEN_RPAREN, ")")
    }

	if (**input == '{') {
        MAKE_TOKEN(TOKEN_LBRACKET, "{")
    }

	if (**input == '}') {
        MAKE_TOKEN(TOKEN_RBRACKET, "}")
    }

	if (**input == ';') {
        MAKE_TOKEN(TOKEN_SEMICOLON, ";")
    }

	if (**input == '+') {
        MAKE_TOKEN(TOKEN_PLUS, "+")
    }

	if (**input == '-') {
        MAKE_TOKEN(TOKEN_MINUS, "-")
    }

	if (**input == '*') {
        MAKE_TOKEN(TOKEN_MULTIPLY, "*")
    }

	if (**input == '/') {
        MAKE_TOKEN(TOKEN_DIVIDE, "/")
    }

	if (isalpha(**input) || **input == '_')	{
		char *start = *input;

		while (isalnum(**input) || **input == '_') {
			(*input)++;
		}

		char *text = SL_strndup(start, *input - start);
		SL_token_type type = TOKEN_IDENTIFIER;

		if (strcmp(text, "if") == 0) type = TOKEN_IF;

		token = SL_TOKEN_NEW(type, text);
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
