#ifndef	SL_PARSER_H
#define	SL_PARSER_H

#include <stdbool.h>
#include <stdlib.h>
#include "lex.h"

struct SL_parser_node
{
	SL_token	*token;
	bool		is_token_error;
	bool		rhs;
	struct SL_parser_node	*left;
	struct SL_parser_node	*right;
};

typedef struct SL_parser_node SL_parser_node;

SL_parser_node *SL_parser_node_new(SL_token *token, SL_parser_node *left, SL_parser_node *right);
SL_parser_node *SL_parser_parse(SL_token **token);
SL_parser_node *SL_parse_expr(SL_token **token);
void SL_parser_print_nodes(SL_parser_node *root, int ident);

#endif // SL_PARSER_H
