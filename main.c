#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>

#define error(...) {fprintf(stderr, __VA_ARGS__); exit(1);}

typedef enum {
	TOKEN_PLUS,
	TOKEN_MINUS,
	TOKEN_LESSER,
	TOKEN_EQ,
	TOKEN_DOT,
	TOKEN_INTLIT,
	TOKEN_LPAREN,
	TOKEN_RPAREN,
	TOKEN_IDENT,
	TOKEN_NOTHING
} TokenKind;

typedef struct {
	TokenKind kind;
	union {
		int64_t intval;
		char* ident;
	};
} Token;

typedef enum {
	EXPR_ADD,
	EXPR_SUB,
	EXPR_LESSER,
	EXPR_INT,
	EXPR_ARG,
	EXPR_FUNCDEF,
	EXPR_FUNCCALL,
	EXPR_IF,
	EXPR_NOTHING
} ExprKind;

typedef struct _Expr {
	ExprKind kind;
	struct _Expr* l;
	struct _Expr* r;
	int64_t intval;
	char* fnname;
	struct _Expr* fnbody;
	struct _Expr* callarg;
	struct _Expr* cond;
	struct _Expr* tbranch;
	struct _Expr* fbranch;
} Expr;

typedef struct {
	char* name;
	Expr* fnexpr;
} Function;

Function funcs[1000];
int funcnum = 0;
int64_t funcarg = 0;

Expr* parse();

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

	if (c == ';') {
		return (Token){TOKEN_NOTHING};
	} else if (c == '+') {
		return (Token){TOKEN_PLUS};
	} else if (c == '-') {
		return (Token){TOKEN_MINUS};
	} else if (c == '<') {
		return (Token){TOKEN_LESSER};
	} else if (c == '=') {
		return (Token){TOKEN_EQ};
	} else if (c == '.') {
		return (Token){TOKEN_DOT};
	} else if (c == '(') {
		return (Token){TOKEN_LPAREN};
	} else if (c == ')') {
		return (Token){TOKEN_NOTHING};
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
	} else if (isalpha(c)) { // parse identifier
		char buf[256] = {};
		buf[0] = c;
		for (int i=1; ; i++) {
			char nc = getc(stdin);
			if (!isalpha(nc)) {
				ungetc(nc, stdin);
				break;
			}
			buf[i] = nc;
		}
		Token t;
		t.kind = TOKEN_IDENT;
		t.ident = strdup(buf);
		return t;
	} else {
		return (Token){TOKEN_NOTHING};
	}
}

Expr* parse_intlit() {
	Token t = lex();
	if (t.kind == TOKEN_INTLIT) {
		Expr* e = malloc(sizeof(Expr));
		e->kind = EXPR_INT;
		e->intval = t.intval;
		return e;
	} else if (t.kind == TOKEN_LPAREN) {
		return parse();
	} else if (t.kind == TOKEN_DOT) {
		Expr* e = malloc(sizeof(Expr));
		e->kind = EXPR_ARG;
		return e;
	} else if (t.kind == TOKEN_IDENT) {
		if (strcmp(t.ident, "if") == 0) { // if syntax
			Expr* e = malloc(sizeof(Expr));
			e->kind = EXPR_IF;
			e->cond = parse();
			e->tbranch = parse();
			e->fbranch = parse();
			return e;
		}

		Token eq = lex();
		if (eq.kind == TOKEN_EQ) { // function-def syntax
			Expr* e = malloc(sizeof(Expr));
			e->kind = EXPR_FUNCDEF;
			e->fnname = t.ident;
			e->fnbody = parse();
			return e;
		} else if (eq.kind == TOKEN_DOT) { // function-call syntax
			Expr* e = malloc(sizeof(Expr));
			e->kind = EXPR_FUNCCALL;
			e->fnname = t.ident;
			e->callarg = parse();
			return e;
		} else {
			assert(false);
		}
	} else {
		Expr* e = malloc(sizeof(Expr));
		e->kind = EXPR_NOTHING;
		return e;
	}
}

Expr* parse() {
	Expr* l = parse_intlit();
	if (l->kind == EXPR_FUNCDEF) return l;
	if (l->kind == EXPR_IF) return l;
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
		} else if (op.kind == TOKEN_LESSER) {
			Expr* r = parse_intlit();
			Expr* e = malloc(sizeof(Expr));
			e->kind = EXPR_LESSER;
			e->l = l;
			e->r = r;
			l = e;
		} else {
			assert(false);
		}
	}
	return l;
}

int64_t eval(Expr* e) {
	if (e->kind == EXPR_ADD) {
		return eval(e->l) + eval(e->r);
	} else if (e->kind == EXPR_SUB) {
		return eval(e->l) - eval(e->r);
	} else if (e->kind == EXPR_LESSER) {
		return eval(e->l) < eval(e->r);
	} else if (e->kind == EXPR_INT) {
		return e->intval;
	} else if (e->kind == EXPR_ARG) {
		return funcarg;
	} else if (e->kind == EXPR_FUNCDEF) {
		funcs[funcnum] = (Function){e->fnname, e->fnbody};
		funcnum++;
		return 0;
	} else if (e->kind == EXPR_FUNCCALL) {
		for (int i=0; i<funcnum; i++) {
			Function f = funcs[i];
			if (strcmp(f.name, e->fnname) == 0) {
				int64_t tmparg = funcarg;
				funcarg = eval(e->callarg);
				int64_t result = eval(f.fnexpr);
				funcarg = tmparg;
				return result;
			}
		}
		error("undeclared %s function.", e->fnname);
	} else if (e->kind == EXPR_IF) {
		int64_t c = eval(e->cond);
		if (c) {
			return eval(e->tbranch);
		} else {
			return eval(e->fbranch);
		}
	} else {
		assert(false);
	}
}

int main() {
	for (;;) {
		Expr* e = parse();
		if (e->kind == EXPR_NOTHING) break;
		printf("%d ", eval(e));
	}
}