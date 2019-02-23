#include <stdio.h>
#include <math.h>
#include <string.h>
#include "expression.h"

double m_sin(double* args, int len) {
	return sin(args[0]);
}

double m_cos(double* args, int len) {
	return cos(args[0]);
}

double m_tan(double* args, int len) {
	return tan(args[0]);
}

double m_pow(double* args, int len) {
	return pow(args[0], args[1]);
}

double m_sqrt(double* args, int len) {
	return sqrt(args[0]);
}

double m_log(double* args, int len) {
	return log10(args[0]);
}

double m_abs(double* args, int len) {
	return fabs(args[0]);
}

double m_ceil(double* args, int len) {
	return ceil(args[0]);
}

double m_floor(double* args, int len) {
	return floor(args[0]);
}

double m_mod(double* args, int len) {
	return fmod(args[0], args[1]);
}

int main(int argc, char* argv[]) {

	/* Register a few built-in functions. */
	expr_add_identifier("sin", &m_sin);
	expr_add_identifier("cos", &m_cos);
	expr_add_identifier("tan", &m_tan);
	expr_add_identifier("pow", &m_pow);
	expr_add_identifier("sqr", &m_sqrt);
	expr_add_identifier("log", &m_log);
	expr_add_identifier("abs", &m_abs);
	expr_add_identifier("cei", &m_ceil);
	expr_add_identifier("flo", &m_floor);
	expr_add_identifier("mod", &m_mod);

	printf("Expression-Parser\nBy Philip Picard\n");

	/* Loop and take input from the user. */
	while (1) {
		printf("> ");
		char line[100];
		fgets(line, 100, stdin);
		/* Typing exit will quit the REPL */
		if (strcmp(line, "exit\n") == 0)
			return 0;
		double result = expr_eval(line);
		/* Only print the result if there was no error. Otherwise
		print the error message */
		if (expr_error() != EXPR_ERR_NONE)
			printf("Error code: %d\n", expr_error());
		else
			printf("%lf\n", result);
	}
}
