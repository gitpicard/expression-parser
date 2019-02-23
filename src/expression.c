#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <setjmp.h>
#include <inttypes.h>
#include "expression.h"

/* Helper macros to read from the incoming stream of characters. */
#define PEEK (*at)
#define POP (*at++)

enum {
	TOK_END = '\0',									/* Represents the end of the expression. */
	TOK_NUM,										/* Represents a number literal (floating point). */
	TOK_IDENT,										/* Represents a function identifier. */
	TOK_PLUS = '+',									/* The plus '+' sign. */
	TOK_MIN = '-',									/* The minus '-' sign. */
	TOK_MULT = '*',									/* The multiplication '*' sign. */
	TOK_DIV = '/',									/* The division '/' sign. */
	TOK_OPEN = '(',									/* The left parenthesis '(' symbol. */
	TOK_CLOSE = ')',								/* The right parenthesis ')' symbol. */
	TOK_COMMA = ','									/* The comma ',' symbol. */
};

static int err_code = EXPR_ERR_NONE;				/* Holds a current error state of the evaluator. */
static jmp_buf err_jmp;								/* The context to restore if something goes wrong. */
static char* at;									/* Where we are currently parsing. */
static char buffer[MAX_LIT + 1] = { '\0' };			/* Holds a literal or identifier during parsing. */
static int tok;										/* The current token. */
static identifier idents[26][27][27];				/* The user-implemented functions. */
static double addition(void);						/* Function definition needed later. */

static void err(int code) {
	err_code = code;
	longjmp(err_jmp, code);
}

/* Using a tokenizer for a simple expression parsing library is defiantly a little over the top but
I wrote this mini-tokenizer anyways just for practice and in-case I want to add more advanced features
later. It also helps make the other parts a bit more readable. */
static int next_tok(void) {
	/* Get rid of any leading whitespace. */
	while (isspace(PEEK)) POP;
	if (isdigit(PEEK) || PEEK == '.') {
		uint8_t dec = 0;
		uint8_t len = 0;
		/* Keep copying as long as we have a digit or a decimal point. */
		while (isdigit(PEEK) || (PEEK == '.' && !dec)) {
			if (PEEK == '.')
				dec = 1;
			if (len == MAX_LIT)
				err(EXPR_ERR_MAX_LIT);
			buffer[len++] = POP;
		}
		buffer[len] = '\0';
		return TOK_NUM;
	}
	else if (isalpha(PEEK)) {
		uint8_t len = 0;
		/* Keep copying as long as it is a alphabetic character. This parser
		does not support using anything other than alphabetic characters. */
		while (isalpha(PEEK) && len != MAX_LIT)
			buffer[len++] = POP;
		/* Did we quit copying because we ran out of room? */
		if (len == MAX_LIT)
			err(EXPR_ERR_MAX_LIT);
		buffer[len] = '\0';
		return TOK_IDENT;
	}
	/* Check for any operators. We can just return the character
	because the value of the token type lines up with the ASCII
	characters for that token. */
	else if (PEEK == '+')
		return POP;
	else if (PEEK == '-')
		return POP;
	else if (PEEK == '*')
		return POP;
	else if (PEEK == '/')
		return POP;
	else if (PEEK == '(')
		return POP;
	else if (PEEK == ')')
		return POP;
	else if (PEEK == ',')
		return POP;
	/* Make sure to detect the end of the expression! */
	else if (PEEK == '\0')
		return TOK_END;
	/* When the user provides an unknown token in the expression. */
	else
		err(EXPR_ERR_TOK);
	return TOK_END;
}

static double primary(void) {
	/* Check if it is a number literal. */
	if (tok == TOK_NUM) {
		double lit;
		sscanf(buffer, "%lf", &lit);
		tok = next_tok();
		return lit;
	}

	/* Process any identifiers/functions. In this library all identifiers
	are function calls that have zero arguments. This makes the implementation
	smaller. */
	if (tok == TOK_IDENT) {
		/* Get the name of the function. Only the first three letters of
		the function are used in identifying the function. This is done
		to save memory. You do not need to use all three letters. One or
		two letter functions are fine. */
		char id[3] = { 0 };
		strncpy(id, buffer, 3);
		/* The parser is case-insensitive. */
		id[0] = toupper(id[0]);
		id[1] = toupper(id[1]);
		id[2] = toupper(id[2]);
		/* Remap the alphabet range of the ASCII table (65 to 90) to be between
		1 and 27. The 0 spot is kept for functions that are only one letter. */
		id[0] -= 65;
		if (id[1] != 0)
			id[1] -= 64;
		if (id[2] != 0)
			id[2] -= 64;

		tok = next_tok();

		/* Read all the arguments to the function call from the incoming
		stream of tokens. */
		double args[MAX_ARGS + 1] = { 0 };
		int arglen = 0;
		/* Check if there are any arguments to the function. */
		if (tok == TOK_OPEN) {
			while (tok != TOK_CLOSE) {
				tok = next_tok();
				/* Check to make sure we are not passing to many arguments. */
				if (arglen > MAX_ARGS)
					err(EXPR_ERR_MAX_ARGS);
				args[arglen++] = addition();
				if (tok != TOK_COMMA && tok != TOK_CLOSE)
					err(EXPR_ERR_SYNTAX);
			}
			/* Skip the close token. */
			tok = next_tok();
		}
		/* Call the function with any arguments that where found. */
		if (idents[id[0]][id[1]][id[2]] != NULL)
			return idents[id[0]][id[1]][id[2]](args, arglen);
		else
			err(EXPR_ERR_NO_IDENT);
	}

	/* A open parenthesis has the highest order of operations. */
	if (tok == TOK_OPEN) {
		tok = next_tok();
		double r = addition();
		if (tok != TOK_CLOSE)
			err(EXPR_ERR_SYNTAX);
		tok = next_tok();
		return r;
	}

	err(EXPR_ERR_SYNTAX);
	return 0.0;
}

static double unary(void) {
	if (tok == TOK_MIN) {
		tok = next_tok();
		return -unary();
	}
	return primary();
}

static double multiplication(void) {
	double result = unary();
	while (tok == TOK_MULT || tok == TOK_DIV) {
		if (tok == TOK_MULT) {
			tok = next_tok();
			result *= unary();
		}
		else if (tok == TOK_DIV) {
			tok = next_tok();
			result /= unary();
		}
	}

	return result;
}

static double addition(void) {
	double result = multiplication();
	while (tok == TOK_PLUS || tok == TOK_MIN) {
		if (tok == TOK_PLUS) {
			tok = next_tok();
			result += multiplication();
		}
		else if (tok == TOK_MIN) {
			tok = next_tok();
			result -= multiplication();
		}
	}

	return result;
}

double expr_eval(char* source) {
	err_code = EXPR_ERR_NONE;
	at = source;

	if (setjmp(err_jmp) == 0) {
		tok = next_tok();
		/* We evaluate expressions using the top down descent method which
		means that the least important operator gets called first. */
		double result = addition();
		/* Check for any handing tokens. */
		if (tok != TOK_END)
			err(EXPR_ERR_SYNTAX);
		return result;
	}
	else
		return 0.0f;
}

int expr_error(void) {
	return err_code;
}

void expr_add_identifier(char* name, identifier ident) {
	/* Remap the name to the identifier. */
	char id[3] = { toupper(name[0]), toupper(name[1]), toupper(name[2]) };
	/* The uppercase letters are from 65 to 90 so we need to remap them to
	0 to 26. However the second two letters need to support a no letter
	options so they are remapped to 0 to 27. */
	id[0] -= 65;
	if (id[1] != 0)
		id[1] -= 64;
	if (id[2] != 0)
		id[2] -= 64;
	/* Make sure the name is allowed and then assign the function. */
	if (strlen(name) <= 3)
		idents[id[0]][id[1]][id[2]] = ident;
}