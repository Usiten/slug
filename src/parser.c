#include <assert.h>
#include <stdio.h>
#include "parser.h"

void die_on_bad_terminal(SL_parser_node *node, char *expected)
{
	assert(node);

	if (node->is_token_error) 
	{
		fprintf(stderr, "[ERROR] At %s:%llu:%llu: Expected %s, but found '%s' instead.\n", 
			node->token->file,
			node->token->line_no,
			node->token->column_no,
			expected,
			node->token->raw_text);

		exit(1);
	}
}

SL_parser_node *terminal(SL_token **token, SL_token_type expected_type)
{
	SL_parser_node *node = calloc(1, sizeof(SL_parser_node));
	SL_ALLOC_CHECK(node)

	if (!token || !*token)
		return NULL;

	node->token = *token;
	node->is_token_error = false;

	if ((*token)->type == expected_type)
	{
		*token = (*token)->next;
		return node;
	}

	node->is_token_error = true;
	return node;
}

#define TERMINAL(name, expected_type)	\
SL_parser_node *terminal_##name(SL_token **token)	\
{	\
	SL_parser_node *node = terminal(token, expected_type);	\
	return node;	\
}

TERMINAL(integer, TOKEN_INTEGER)
TERMINAL(plus, TOKEN_PLUS)
TERMINAL(minus, TOKEN_MINUS)
TERMINAL(multiply, TOKEN_MULTIPLY)
TERMINAL(divide, TOKEN_DIVIDE)
TERMINAL(lparen, TOKEN_LPAREN)
TERMINAL(rparen, TOKEN_RPAREN)
TERMINAL(semicolon, TOKEN_SIMICOLON)

SL_parser_node *SL_parser_node_new(SL_token *token, SL_parser_node *left, SL_parser_node *right)
{
	SL_parser_node *node = calloc(1, sizeof(SL_parser_node));
	node->token = token;
	node->is_token_error = false;
	node->left = left;
	node->right = right;
	return node;
}

SL_parser_node *SL_parse_factor(SL_token **token)
{
	SL_parser_node *integer = terminal_integer(token);
	
	if (integer->is_token_error)
	{
		// Try to parse an (expr) instead
		SL_parser_node *lparen = terminal_lparen(token);
		die_on_bad_terminal(lparen, "'(' or an integer");

		SL_parser_node *expr = SL_parse_expr(token);

		SL_parser_node *rparen = terminal_rparen(token);
		die_on_bad_terminal(rparen, "')'");
		return expr;
	}

	return integer;
}

SL_parser_node *SL_parse_factor_op(SL_token **token)
{
	SL_parser_node *multiply, *divide;
	
	multiply = terminal_multiply(token);
	if (!multiply->is_token_error) return multiply;

	divide = terminal_divide(token);
	if (!divide->is_token_error) return divide;

	return NULL;
}


SL_parser_node *SL_parse_term(SL_token **token)
{
	SL_parser_node *factor1 = SL_parse_factor(token);

	while (*token && ((*token)->type == TOKEN_MULTIPLY || (*token)->type == TOKEN_DIVIDE))
	{
		SL_parser_node *factor_op = SL_parse_factor_op(token);
		SL_parser_node *factor2 = SL_parse_factor(token);
		factor1 = SL_parser_node_new(factor_op->token, factor1, factor2);
	}

	return factor1;
}

SL_parser_node *SL_parse_term_op(SL_token **token)
{
	SL_parser_node *plus, *minus;
	
	plus = terminal_plus(token);
	if (!plus->is_token_error) return plus;

	minus = terminal_minus(token);
	if (!minus->is_token_error) return minus;

	return NULL;
}

SL_parser_node *SL_parse_expr(SL_token **token)
{
	SL_parser_node *term1 = SL_parse_term(token);

	while (*token && ((*token)->type == TOKEN_PLUS || (*token)->type == TOKEN_MINUS))
	{
		SL_parser_node *term_op = SL_parse_term_op(token);
		SL_parser_node *term2 = SL_parse_term(token);
		term1 = SL_parser_node_new(term_op->token, term1, term2);
	}

	return term1;
}

SL_parser_node *SL_parse_exprs(SL_token **token)
{
	SL_parser_node *expr1 = SL_parse_expr(token);
	SL_parser_node *semi1 = terminal_semicolon(token);
	die_on_bad_terminal(semi1, "';'");

	while ((*token)->next)
	{
		SL_parser_node *expr2 = SL_parse_expr(token);
		SL_parser_node *semi2 = terminal_semicolon(token);
		die_on_bad_terminal(semi2, "';'");
		expr1 = SL_parser_node_new(NULL, expr1, expr2);
	}

	return expr1;
}

SL_parser_node *SL_parser_parse(SL_token **token)
{
	SL_parser_node *node = SL_parse_exprs(token);
	return node;
}

void SL_parser_print_nodes(SL_parser_node *root, int ident)
{
	if (root->token) {
		printf("%*s (%s)\n", ident * 4, root->token->raw_text, root->token->type_as_string);
	}
	else {
		printf("%*s\n", ident * 4, ".");
	}

	if (root->left) SL_parser_print_nodes(root->left, ident + 1);
	if (root->right) SL_parser_print_nodes(root->right, ident + 1);
}