%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

int yylex(void);
void yyerror(const char *s);
extern int yylineno;

Node *root = NULL;
%}

%union {
    double num;
    char *str;
    Node *node;
}

%token <num> NUMBER
%token <str> IDENT
%token IF ELSE WHILE PRINT
%token EQ NE LE GE

%type <node> program stmt_list stmt block expr

%nonassoc IFX
%nonassoc ELSE
%left EQ NE
%left '<' '>' LE GE
%left '+' '-'
%left '*' '/'
%right UMINUS

%%

program:
    stmt_list                         { root = mk_block($1); }
    ;

stmt_list:
    /* пусто */                       { $$ = NULL; }
    | stmt_list stmt                  { $$ = append_stmt($1, $2); }
    ;

block:
    '{' stmt_list '}'                 { $$ = mk_block($2); }
    ;

stmt:
    IDENT '=' expr ';'                { $$ = mk_assign($1, $3); }
    | PRINT '(' expr ')' ';'          { $$ = mk_print($3); }
    | IF '(' expr ')' stmt %prec IFX  { $$ = mk_if($3, $5, NULL); }
    | IF '(' expr ')' stmt ELSE stmt  { $$ = mk_if($3, $5, $7); }
    | WHILE '(' expr ')' stmt         { $$ = mk_while($3, $5); }
    | block                           { $$ = $1; }
    ;

expr:
    NUMBER                            { $$ = mk_num($1); }
    | IDENT                           { $$ = mk_var($1); }
    | expr '+' expr                   { $$ = mk_binop('+', $1, $3); }
    | expr '-' expr                   { $$ = mk_binop('-', $1, $3); }
    | expr '*' expr                   { $$ = mk_binop('*', $1, $3); }
    | expr '/' expr                   { $$ = mk_binop('/', $1, $3); }
    | expr '<' expr                   { $$ = mk_binop('<', $1, $3); }
    | expr '>' expr                   { $$ = mk_binop('>', $1, $3); }
    | expr LE expr                    { $$ = mk_binop('l', $1, $3); }
    | expr GE expr                    { $$ = mk_binop('g', $1, $3); }
    | expr EQ expr                    { $$ = mk_binop('e', $1, $3); }
    | expr NE expr                    { $$ = mk_binop('n', $1, $3); }
    | '-' expr %prec UMINUS           { $$ = mk_unary('-', $2); }
    | '(' expr ')'                    { $$ = $2; }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Ошибка на строке %d: %s\n", yylineno, s);
}
