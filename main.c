#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

typedef enum {
	TOKEN_PLUS,
	TOKEN_MINUS,
	TOKEN_INTLIT,
	TOKEN_NOTHING
} TokenKind;

typedef struct {
	TokenKind kind;
	union {
		int intval;		
	};
} Token;

typedef enum {
	EXPR_ADD,
	EXPR_SUB,
	EXPR_INT
} ExprKind;

typedef struct _Expr {
	ExprKind kind;
	struct _Expr* l;
	struct _Expr* r;
	int intval;
} Expr;

Token lex() {
	char c = getc(stdin);
	if (c == ' ') {
		for (;;) {
			c = getc(stdin);
			if (c != ' ') {
				break;
			}
		}
	}
		
	if (c == '+') {
		return (Token){TOKEN_PLUS};
	} else if (c == '-') {
		return (Token){TOKEN_MINUS};
	} else if (isdigit(c)) { // parse integer literal
		char buf[256] = {};
		buf[0] = c;
		for (int i=1; ; i++) {
			char nc = getc(stdin);
			if (!isdigit(nc)) {
				ungetc(nc, stdin);
				break;
			}
			buf[i] = nc;
		}
		return (Token){TOKEN_INTLIT, strtoll(buf, NULL, 0)};
	} else {
		return (Token){TOKEN_NOTHING};
	}
}

Expr* parse_intlit() {
	Token t = lex();
	Expr* e = malloc(sizeof(Expr));
	e->kind = EXPR_INT;
	e->intval = t.intval;
	return e;
}

Expr* parse() {
	Expr* l = parse_intlit();
	for (;;) {
		Token op = lex();
		if (op.kind == TOKEN_NOTHING) break;
		if (op.kind == TOKEN_PLUS) {
			Expr* r = parse_intlit();
			Expr* e = malloc(sizeof(Expr));
			e->kind = EXPR_ADD;
			e->l = l;
			e->r = r;
			l = e;
		} else if (op.kind == TOKEN_MINUS) {
			Expr* r = parse_intlit();
			Expr* e = malloc(sizeof(Expr));
			e->kind = EXPR_SUB;
			e->l = l;
			e->r = r;
			l = e;
		} else {
			break;
		}
	}
	return l;
}

int eval(Expr* e) {
	if (e->kind == EXPR_ADD) {
		return eval(e->l) + eval(e->r);
	} else if (e->kind == EXPR_SUB) {
		return eval(e->l) - eval(e->r);
	} else if (e->kind == EXPR_INT) {
		return e->intval;
	} else {
		assert(false);
	}
}

int main() {
	Expr* e = parse();
	printf("%d", eval(e));
}