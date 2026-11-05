CC = gcc

all: lang

lang.tab.c lang.tab.h: lang.y
	bison -d lang.y

lex.yy.c: lang.l lang.tab.h
	flex lang.l

lang: lang.tab.c lex.yy.c ast.c interp.c codegen.c ast_json.c main.c ast.h interp.h codegen.h ast_json.h
	$(CC) -o lang lang.tab.c lex.yy.c ast.c interp.c codegen.c ast_json.c main.c

clean:
	rm -f lang lang.tab.c lang.tab.h lex.yy.c
