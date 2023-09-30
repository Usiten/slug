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

static char *last_expected;
static SL_token *last_found = NULL;

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
		last_found = *token;
		return node;
	}

	node->is_token_error = true;
	return node;
}

#define TRACE //printf("%s -> %s (%s)\n", __FUNCTION__, (*token)->raw_text, (*token)->type_as_string);


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
TERMINAL(assign, TOKEN_ASSIGN)
TERMINAL(identifier, TOKEN_IDENTIFIER)
TERMINAL(semicolon, TOKEN_SEMICOLON)

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
	TRACE
	last_expected = "an factor";
	SL_parser_node *integer = terminal_integer(token);

	if (!integer || integer->is_token_error)
	{
		SL_parser_node *lparen = terminal_lparen(token);
		if (!lparen || lparen->is_token_error) 
			return NULL;

		SL_parser_node *expr = SL_parse_expr(token);
		if (!expr)
			return NULL;

		SL_parser_node *rparen = terminal_rparen(token);
		if (!rparen || rparen->is_token_error) 
			return NULL;

		return expr;
	}

	return integer;
}

SL_parser_node *SL_parse_factor_op(SL_token **token)
{
	TRACE
	last_expected = "an binary operator";
	SL_parser_node *multiply, *divide;
	
	multiply = terminal_multiply(token);
	if (multiply && !multiply->is_token_error) return multiply;

	divide = terminal_divide(token);
	if (divide && !divide->is_token_error) return divide;

	return NULL;
}


SL_parser_node *SL_parse_term(SL_token **token)
{
	TRACE
	last_expected = "an term";
	SL_parser_node *factor1 = SL_parse_factor(token);
	if (!factor1)
		return NULL;

	while (*token && ((*token)->type == TOKEN_MULTIPLY || (*token)->type == TOKEN_DIVIDE))
	{
		SL_parser_node *factor_op = SL_parse_factor_op(token);
		if (!factor_op)
			return NULL;

		SL_parser_node *factor2 = SL_parse_factor(token);
		if (!factor2)
			return NULL;

		factor1 = SL_parser_node_new(factor_op->token, factor1, factor2);
	}

	return factor1;
}

SL_parser_node *SL_parse_term_op(SL_token **token)
{
	TRACE
	last_expected = "a binary operator";
	SL_parser_node *plus, *minus;
	
	plus = terminal_plus(token);
	if (plus && !plus->is_token_error) return plus;

	minus = terminal_minus(token);
	if (minus && !minus->is_token_error) return minus;

	return NULL;
}

SL_parser_node *SL_parse_expr(SL_token **token)
{
	TRACE
	last_expected = "an expression";
	SL_parser_node *term1 = SL_parse_term(token);
	if (!term1)
		return NULL;

	while (*token && ((*token)->type == TOKEN_PLUS || (*token)->type == TOKEN_MINUS))
	{
		SL_parser_node *term_op = SL_parse_term_op(token);
		if (!term_op)
			return NULL;

		SL_parser_node *term2 = SL_parse_term(token);
		if (!term2)
			return NULL;

		term1 = SL_parser_node_new(term_op->token, term1, term2);
	}

	return term1;
}

SL_parser_node *SL_parse_assign(SL_token **token)
{
	TRACE
	last_expected = "an assignment";
	SL_parser_node *id = terminal_identifier(token);
	if (!id || id->is_token_error)
		return NULL;

	SL_parser_node *assign = terminal_assign(token);
	if (!assign || assign->is_token_error)
		return NULL;

	SL_parser_node *expr = SL_parse_expr(token);
	if (!expr)
		return NULL;

	assign->left = id;
	assign->right = expr;
	return assign;
}

SL_parser_node *SL_parse_expr_or_assign(SL_token **token)
{
	TRACE
	SL_token **save = token;
	SL_parser_node *expr1 = SL_parse_expr(token);
	if (expr1)
		return expr1;

	token = save;
	SL_parser_node *assign = SL_parse_assign(token);
	if (assign)
		return assign;

	return NULL;
}

SL_parser_node *SL_parse_exprs(SL_token **token)
{
	TRACE
	last_expected = "expressions or assignments";
	SL_parser_node *expr1 = SL_parse_expr_or_assign(token);
	if (!expr1)
		return NULL;

	SL_parser_node *semi1 = terminal_semicolon(token);
	if (!semi1 || semi1->is_token_error)
		return NULL;

	while ((*token)->next)
	{
		SL_parser_node *expr2 = SL_parse_expr_or_assign(token);
		if (!expr2)
			return NULL;

		SL_parser_node *semi2 = terminal_semicolon(token);
		if (!semi2 || semi2->is_token_error)
			return NULL;

		expr1 = SL_parser_node_new(NULL, expr1, expr2);
	}

	return expr1;
}

SL_parser_node *SL_parser_parse(SL_token **token)
{
	TRACE
	SL_parser_node *node = SL_parse_exprs(token);
	if (node == NULL)
	{
		fprintf(stderr, "[ERROR] At %s:%llu:%llu: Expected %s, but found '%s' instead.\n", 
			last_found->file,
			last_found->line_no,
			last_found->column_no,
			last_expected,
			last_found->raw_text);
		exit(1);
	}
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