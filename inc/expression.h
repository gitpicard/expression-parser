#ifndef EXPRESSION_H
#define EXPRESSION_H

#define MAX_LIT 16
#define MAX_IDENTS 16
#define MAX_ARGS 4

enum {
	EXPR_ERR_NONE,
	EXPR_ERR_MAX_LIT,
	EXPR_ERR_MAX_ARGS,
	EXPR_ERR_TOK,
	EXPR_ERR_SYNTAX,
	EXPR_ERR_NO_IDENT,
};

typedef double(*identifier)(double* args, int arglen);

double expr_eval(char* source);
int expr_error(void);
void expr_add_identifier(char* name, identifier ident);

#endif