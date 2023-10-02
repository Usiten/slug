#include <assert.h>
#include <stdio.h>
#include "parser.h"

// static char *last_expected;
static SL_token *last_found = NULL;

static SL_parser_node **registar = NULL;
static size_t registar_count = 0;

SL_parser_node *terminal(SL_token **token, SL_token_type expected_type)
{
	static_assert(16 == __TOKEN_TYPE_COUNT__, "Ensure we don't forget to implement parser for a new token"); 

	SL_parser_node *node = calloc(1, sizeof(SL_parser_node));
	SL_ALLOC_CHECK(node)

	registar = realloc(registar, (registar_count + 1) * sizeof(SL_parser_node*));
	registar[registar_count] = node;
	registar_count ++;
	 
	if (!token || !*token)
		return NULL;

	node->token = *token;
	node->is_unexpected = false;

	if ((*token)->type == expected_type)
	{
		*token = (*token)->next;
		last_found = *token;
		return node;
	}

	node->is_unexpected = true;
	return node;
}

#define TERMINAL(name, expected_type)							\
SL_parser_node *SL_parse_terminal_##name(SL_token **token)				\
{																\
	SL_parser_node *node = terminal(token, expected_type);		\
	return node;												\
}

TERMINAL(integer, 		TOKEN_INTEGER)
TERMINAL(plus, 			TOKEN_PLUS)
TERMINAL(minus, 		TOKEN_MINUS)
TERMINAL(multiply, 		TOKEN_MULTIPLY)
TERMINAL(divide, 		TOKEN_DIVIDE)
TERMINAL(lparen, 		TOKEN_LPAREN)
TERMINAL(rparen, 		TOKEN_RPAREN)
TERMINAL(lbracket, 		TOKEN_LBRACKET)
TERMINAL(rbracket, 		TOKEN_RBRACKET)
TERMINAL(assign, 		TOKEN_ASSIGN)
TERMINAL(identifier, 	TOKEN_IDENTIFIER)
TERMINAL(semicolon, 	TOKEN_SEMICOLON)
TERMINAL(less_than, 	TOKEN_LESS_THAN)
TERMINAL(iff, 			TOKEN_IF)

SL_parser_node *SL_parser_node_new(SL_token *token, SL_parser_node *left, SL_parser_node *right)
{
	SL_parser_node *node = calloc(1, sizeof(SL_parser_node));
	SL_ALLOC_CHECK(node)

	registar = realloc(registar, (registar_count + 1) * sizeof(SL_parser_node*));
	registar[registar_count] = node;
	registar_count ++;

	node->token = token;
	node->is_unexpected = false;
	node->is_rhs = false;
	node->left = left;
	node->right = right;
	return node;
}

#define PARSE_NODE(name, type) SL_parser_node *name = SL_parse_##type(token);
#define RETURN_NODE_IF_NODE_IS_OK(node) if (node && !node->is_unexpected) return node;
#define RETURN_NULL_IF_NODE_IS_NOT_OK(node) if (!node || node->is_unexpected) return NULL;

#define OK(name, type) 	PARSE_NODE(name, type)	\
						RETURN_NODE_IF_NODE_IS_OK(name)

#define NOK(name, type) PARSE_NODE(name, type)	\
						RETURN_NULL_IF_NODE_IS_NOT_OK(name)

SL_parser_node *SL_parse_comp_op(SL_token **token)
{
	OK(less_than, terminal_less_than);
	
	return NULL;
}

SL_parser_node *SL_parse_factor(SL_token **token)
{
	SL_token *save = *token;
	OK(integer, terminal_integer);

	*token = save;
	// TODO: Special case, for now keep it that way, we'll see later when semantic analysis exist
	SL_parser_node *id = SL_parse_terminal_identifier(token);
	if (id && !id->is_unexpected) {
		id->is_rhs = true;
		return id;
	}

	*token = save;
	NOK(lparen, terminal_lparen);
	NOK(expr, expr);
	NOK(rparen, terminal_rparen);	

	return expr;
}

SL_parser_node *SL_parse_factor_op(SL_token **token)
{
	OK(multiply, terminal_multiply);
	OK(divide, terminal_divide);

	return NULL;
}


SL_parser_node *SL_parse_term(SL_token **token)
{
	NOK(factor1, factor);
	while (*token && ((*token)->type == TOKEN_MULTIPLY || (*token)->type == TOKEN_DIVIDE))
	{
		NOK(factor_op, factor_op);
		NOK(factor2, factor);
		factor1 = SL_parser_node_new(factor_op->token, factor1, factor2);
	}

	return factor1;
}

SL_parser_node *SL_parse_term_op(SL_token **token)
{
	OK(plus, terminal_plus);
	OK(minus, terminal_minus);

	return NULL;
}

SL_parser_node *SL_parse_expr(SL_token **token)
{
	NOK(term1, term);
	while (*token && ((*token)->type == TOKEN_PLUS || (*token)->type == TOKEN_MINUS))
	{
		NOK(term_op, term_op);
		NOK(term2, term);
		term1 = SL_parser_node_new(term_op->token, term1, term2);
	}

	return term1;
}

SL_parser_node *SL_parse_comp(SL_token **token)
{
	NOK(expr1, expr);	
	if (*token && ((*token)->type == TOKEN_LESS_THAN))
	{
		NOK(comp_op, comp_op);
		NOK(expr2, expr);
		expr1 = SL_parser_node_new(comp_op->token, expr1, expr2);
	}	

	return expr1;
}

SL_parser_node *SL_parse_expr_of_if(SL_token **token)
{
	SL_token *save = *token;
	OK(expr1, expr)	

	*token = save;
	OK(iff, if)

	return NULL;
}

SL_parser_node *SL_parse_exprs(SL_token **token)
{
	NOK(expr1, expr_of_if);	
	while (*token && ((*token)->type == TOKEN_SEMICOLON))
	{
		NOK(semi, terminal_semicolon);
		NOK(expr2, exprs);
		expr1 = SL_parser_node_new(NULL, expr1, expr2);
	}

	return expr1;
}

SL_parser_node *SL_parse_if(SL_token **token)
{
	NOK(iff, terminal_iff);
	NOK(lparen, terminal_lparen);
	NOK(comp, comp);
	NOK(rparen, terminal_rparen);
	NOK(lbracket, terminal_lbracket);
	NOK(exprs, exprs);	
	NOK(rbracket, terminal_rbracket);

	iff->left = comp;
	iff->right = exprs;
	return iff;
}

SL_parser_node *SL_parse_assign(SL_token **token)
{
	NOK(id, terminal_identifier);
	NOK(assign, terminal_assign);
	NOK(comp, comp);
	
	assign->left = id;
	assign->right = comp;
	return assign;
}

SL_parser_node *SL_parse_linear(SL_token **token)
{
	NOK(assign, terminal_assign);

	SL_token *save = *token;
	OK(comp, comp);

	*token = save;
	OK(iff1, if);

	return NULL;
}

SL_parser_node *SL_parse_assign_or_linear(SL_token **token)
{
	OK(assign1, assign);
	OK(linear, linear);

	return NULL;
}

SL_parser_node *SL_parse_stmts(SL_token **token)
{
	NOK(assign1, assign_or_linear);	
	NOK(semi1, terminal_semicolon);	
	while ((*token)->next)
	{
		NOK(assign2, assign_or_linear);
		NOK(semi2, terminal_semicolon);
		assign1 = SL_parser_node_new(NULL, assign1, assign2);
	}

	return assign1;

}

SL_parser_node *SL_parser_parse(SL_token **token)
{
	last_found = *token;
	PARSE_NODE(node, stmts);
	if (node == NULL)
	{
		fprintf(stderr, "[ERROR] At %s:%llu:%llu: Syntax Error: Unexpected '%s'.\n", 
			last_found->file,
			last_found->line_no,
			last_found->column_no,
			last_found->raw_text);
		exit(1);
	}

	return node;
}

void SL_parser_print_nodes(SL_parser_node *root, int ident)
{
	assert(root != NULL);

	if (root->token) {
		printf("%*s (%s)\n", ident * 4, root->token->raw_text, root->token->type_as_string);
	}
	else {
		printf("%*s\n", ident * 4, ".");
	}

	if (root->left) { 
		SL_parser_print_nodes(root->left, ident + 1);
	}

	if (root->right) {
		SL_parser_print_nodes(root->right, ident + 1);
	}
}

void SL_parser_free_all_nodes(void)
{
	for (size_t i = 0; i < registar_count; ++i)
	{
		free(registar[i]);
		registar[i] = NULL;
	}
}