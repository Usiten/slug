#ifndef SL_LEX_H
#define SL_LEX_H

#include <string.h>
#include <ctype.h>
#include "tool.h"

enum SL_token_type
{
	TOKEN_INTEGER	= 0,
	TOKEN_PLUS,
	TOKEN_MINUS,
	TOKEN_MULTIPLY,
	TOKEN_DIVIDE,
	TOKEN_SEMICOLON,

	TOKEN_LESS_THAN,

	TOKEN_IF,

	TOKEN_LPAREN,
	TOKEN_RPAREN,
	TOKEN_LBRACKET,
	TOKEN_RBRACKET,
	TOKEN_ASSIGN,
	TOKEN_IDENTIFIER,
	TOKEN_IDENTIFIER_VALUE,

	TOKEN_EOF,
	TOKEN_UNKNOWN,
	__TOKEN_TYPE_COUNT__
};

typedef enum SL_token_type SL_token_type;

struct SL_token
{
	SL_token_type	type;
	char			*type_as_string;
	char			*raw_text;
	size_t			line_no;
	size_t			column_no;
	char			*file;
	struct SL_token *next;
};

typedef struct SL_token SL_token;

SL_token *SL_token_new(SL_token_type type, char *type_as_string, char* raw_text, size_t line_no, size_t column_no);
#define SL_TOKEN_NEW(type, raw_text) SL_token_new(type, #type, raw_text, line, column)

void SL_token_free(SL_token **token);

SL_token *SL_next_token_from_input(char **input);
void SL_token_print_list(SL_token *token);

#endif // SL_LEX_H
