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
SL_parser_node *terminal_##name(SL_token **token)				\
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

SL_parser_node *SL_parse_comp_op(SL_token **token)
{
	SL_parser_node *less_than = terminal_less_than(token);
	if (less_than && !less_than->is_unexpected) 
		return less_than;

	return NULL;
}

SL_parser_node *SL_parse_factor(SL_token **token)
{
	SL_token *save = *token;

	SL_parser_node *integer = terminal_integer(token);
	if (integer && !integer->is_unexpected)
		return integer;

	*token = save;
	SL_parser_node *id = terminal_identifier(token);
	if (id && !id->is_unexpected) {
		id->is_rhs = true;
		return id;
	}

	*token = save;
	SL_parser_node *lparen = terminal_lparen(token);
	if (!lparen || lparen->is_unexpected)
		return NULL;

	SL_parser_node *expr = SL_parse_expr(token);
	if (!expr || expr->is_unexpected) 
		return NULL;

	SL_parser_node *rparen = terminal_rparen(token);
	if (!rparen || rparen->is_unexpected)
		return NULL;
	
	return expr;
}

SL_parser_node *SL_parse_factor_op(SL_token **token)
{
	SL_parser_node *multiply = terminal_multiply(token);
	if (multiply && !multiply->is_unexpected) 
		return multiply;

	SL_parser_node *divide = terminal_divide(token);
	if (divide && !divide->is_unexpected) 
		return divide;

	return NULL;
}


SL_parser_node *SL_parse_term(SL_token **token)
{
	SL_parser_node *factor1 = SL_parse_factor(token);
	if (!factor1 || factor1->is_unexpected)
		return NULL;

	while (*token && ((*token)->type == TOKEN_MULTIPLY || (*token)->type == TOKEN_DIVIDE))
	{
		SL_parser_node *factor_op = SL_parse_factor_op(token);
		if (!factor_op || factor_op->is_unexpected)
			return NULL;

		SL_parser_node *factor2 = SL_parse_factor(token);
		if (!factor2 || factor2->is_unexpected)
			return NULL;

		factor1 = SL_parser_node_new(factor_op->token, factor1, factor2);
	}

	return factor1;
}

SL_parser_node *SL_parse_term_op(SL_token **token)
{
	SL_parser_node *plus = terminal_plus(token);
	if (plus && !plus->is_unexpected) 
		return plus;

	SL_parser_node *minus = terminal_minus(token);
	if (minus && !minus->is_unexpected) 
		return minus;

	return NULL;
}

SL_parser_node *SL_parse_expr(SL_token **token)
{
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

SL_parser_node *SL_parse_comp(SL_token **token)
{
	SL_parser_node *expr1 = SL_parse_expr(token);
	if (!expr1 || expr1->is_unexpected)
		return NULL;
	
	if (*token && ((*token)->type == TOKEN_LESS_THAN))
	{
		SL_parser_node *comp_op = SL_parse_comp_op(token);
		if (!comp_op || comp_op->is_unexpected)
			return NULL;
		
		SL_parser_node *expr2 = SL_parse_expr(token);
		if (!expr2 || expr2->is_unexpected)
			return NULL;

		expr1 = SL_parser_node_new(comp_op->token, expr1, expr2);
	}	

	return expr1;
}

SL_parser_node *SL_parse_expr_of_if(SL_token **token)
{
	SL_token *save = *token;
	SL_parser_node *expr1 = SL_parse_expr(token);
	if (expr1 && !expr1->is_unexpected)
		return expr1;
	
	*token = save;
	SL_parser_node *iff = SL_parse_if(token);
	if (iff && !iff->is_unexpected)
		return iff;

	return NULL;
}

SL_parser_node *SL_parse_exprs(SL_token **token)
{
	SL_parser_node *expr1 = SL_parse_expr_of_if(token);
	if (!expr1 || expr1->is_unexpected)
		return NULL;
	
	while (*token && ((*token)->type == TOKEN_SEMICOLON))
	{
		SL_parser_node *semi = terminal_semicolon(token);
		if (!semi)
			return NULL;

		SL_parser_node *expr2 = SL_parse_exprs(token);
		if (!expr2)
			return NULL;

		expr1 = SL_parser_node_new(NULL, expr1, expr2);
	}

	return expr1;
}

SL_parser_node *SL_parse_if(SL_token **token)
{
	SL_parser_node *iff = terminal_iff(token);
	if (!iff || iff->is_unexpected)
		return NULL;

	SL_parser_node *lparen = terminal_lparen(token);
	if (!lparen || lparen->is_unexpected)
		return NULL;

	SL_parser_node *comp = SL_parse_comp(token);
	if (!comp || comp->is_unexpected) 
		return NULL;

	SL_parser_node *rparen = terminal_rparen(token);
	if (!rparen || rparen->is_unexpected)
		return NULL;

	SL_parser_node *lbracket = terminal_lbracket(token);
	if (!lbracket || lbracket->is_unexpected)
		return NULL;

	SL_parser_node *exprs = SL_parse_exprs(token);
	if (!exprs || exprs->is_unexpected)
		return NULL;
	
	SL_parser_node *rbracket = terminal_rbracket(token);
	if (!rbracket || rbracket->is_unexpected)
		return NULL;

	iff->left = comp;
	iff->right = exprs;
	return iff;
}

SL_parser_node *SL_parse_assign(SL_token **token)
{
	SL_parser_node *id = terminal_identifier(token);
	if (!id || id->is_unexpected)
		return NULL;
	
	SL_parser_node *assign = terminal_assign(token);
	if (!assign || assign->is_unexpected)
		return NULL;

	SL_token *save = *token;
	SL_parser_node *iff1 = SL_parse_if(token);
	if (iff1 && !iff1->is_unexpected)
	{
		assign->left = id;
		assign->right = iff1;
		return assign;
	}

	*token = save;
	SL_parser_node *comp = SL_parse_comp(token);
	if (comp && !comp->is_unexpected) {
		assign->left = id;
		assign->right = comp;
		return assign;
	}
	
	return NULL;
}

SL_parser_node *SL_parse_linear(SL_token **token)
{
	SL_parser_node *assign = terminal_assign(token);
	if (!assign || assign->is_unexpected)
		return NULL;

	SL_token *save = *token;
	SL_parser_node *comp1 = SL_parse_comp(token);
	if (comp1 && !comp1->is_unexpected)
		return comp1;

	*token = save;
	SL_parser_node *iff1 = SL_parse_if(token);
	if (iff1 && !iff1->is_unexpected)
		return iff1;

	return NULL;
}

SL_parser_node *SL_parse_assign_or_linear(SL_token **token)
{
	SL_parser_node *assign1 = SL_parse_assign(token);
	if (assign1)
		return assign1;

	SL_parser_node *linear = SL_parse_linear(token);
	if (linear)
		return linear;


	return NULL;
}

SL_parser_node *SL_parse_stmts(SL_token **token)
{
	SL_parser_node *assign1 = SL_parse_assign_or_linear(token);
	if (!assign1 || assign1->is_unexpected)
		return NULL;
	
	SL_parser_node *semi1 = terminal_semicolon(token);
	if (!semi1 || semi1->is_unexpected)
		return NULL;
	
	while ((*token)->next)
	{
		SL_parser_node *assign2 = SL_parse_assign_or_linear(token);
		if (!assign2 || assign2->is_unexpected)
			return NULL;

		SL_parser_node *semi2 = terminal_semicolon(token);
		if (!semi2 || semi2->is_unexpected)
			return NULL;

		assign1 = SL_parser_node_new(NULL, assign1, assign2);
	}

	return assign1;

}

SL_parser_node *SL_parser_parse(SL_token **token)
{
	last_found = *token;
	SL_parser_node *node = SL_parse_stmts(token);

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