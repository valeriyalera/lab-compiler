%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

int yylex(void);
void yyerror(const char *s);
extern int yylineno;

Node *root = NULL;
%}

%define parse.error verbose

%union {
    double num;
    int ival;
    char *str;
    Node *node;
}

%token <num> INUM DNUM
%token <str> IDENT
%token KW_INT KW_DOUBLE KW_VOID
%token IF ELSE WHILE FOR RETURN PRINT
%token EQ NE LE GE AND OR NOT INC DEC

%type <ival> type
%type <node> func_list func params_opt params param block stmt_list stmt
%type <node> simple opt_expr args_opt args call expr

%nonassoc IFX
%nonassoc ELSE
%left OR
%left AND
%left EQ NE
%left '<' '>' LE GE
%left '+' '-'
%left '*' '/' '%'
%right UMINUS NOT

%%

program:
    func_list                         { root = mk_program($1); }
    ;

func_list:
    func                              { $$ = $1; }
    | func_list func                  { $$ = append_stmt($1, $2); }
    ;

type:
    KW_INT                            { $$ = T_INT; }
    | KW_DOUBLE                       { $$ = T_DOUBLE; }
    | KW_VOID                         { $$ = T_VOID; }
    ;

func:
    type IDENT '(' params_opt ')' block   { $$ = mk_func($1, $2, $4, $6); }
    ;

params_opt:
    /* пусто */                       { $$ = NULL; }
    | params                          { $$ = $1; }
    ;

params:
    param                             { $$ = $1; }
    | params ',' param                { $$ = append_stmt($1, $3); }
    ;

param:
    type IDENT                        { $$ = mk_param($1, $2); }
    ;

block:
    '{' stmt_list '}'                 { $$ = mk_block($2); }
    ;

stmt_list:
    /* пусто */                       { $$ = NULL; }
    | stmt_list stmt                  { $$ = append_stmt($1, $2); }
    ;

/* "простой оператор" без точки с запятой: используется и в for(...) */
simple:
    /* пусто */                       { $$ = NULL; }
    | type IDENT                      { $$ = mk_decl($1, $2, NULL); }
    | type IDENT '=' expr             { $$ = mk_decl($1, $2, $4); }
    | IDENT '=' expr                  { $$ = mk_assign($1, $3); }
    | IDENT INC                       { $$ = mk_assign($1, mk_binop('+', mk_var($1), mk_num(1, T_INT))); }
    | IDENT DEC                       { $$ = mk_assign($1, mk_binop('-', mk_var($1), mk_num(1, T_INT))); }
    ;

stmt:
    simple ';'                        { $$ = $1; }
    | call ';'                        { $$ = $1; }
    | RETURN ';'                      { $$ = mk_return(NULL); }
    | RETURN expr ';'                 { $$ = mk_return($2); }
    | PRINT '(' expr ')' ';'          { $$ = mk_print($3); }
    | IF '(' expr ')' stmt %prec IFX  { $$ = mk_if($3, $5, NULL); }
    | IF '(' expr ')' stmt ELSE stmt  { $$ = mk_if($3, $5, $7); }
    | WHILE '(' expr ')' stmt         { $$ = mk_while($3, $5); }
    | FOR '(' simple ';' opt_expr ';' simple ')' stmt
                                      { $$ = mk_for($3, $5, $7, $9); }
    | block                           { $$ = $1; }
    ;

opt_expr:
    /* пусто */                       { $$ = NULL; }
    | expr                            { $$ = $1; }
    ;

call:
    IDENT '(' args_opt ')'            { $$ = mk_call($1, $3); }
    ;

args_opt:
    /* пусто */                       { $$ = NULL; }
    | args                            { $$ = $1; }
    ;

args:
    expr                              { $$ = $1; }
    | args ',' expr                   { $$ = append_stmt($1, $3); }
    ;

expr:
    INUM                              { $$ = mk_num($1, T_INT); }
    | DNUM                            { $$ = mk_num($1, T_DOUBLE); }
    | IDENT                           { $$ = mk_var($1); }
    | call                            { $$ = $1; }
    | expr '+' expr                   { $$ = mk_binop('+', $1, $3); }
    | expr '-' expr                   { $$ = mk_binop('-', $1, $3); }
    | expr '*' expr                   { $$ = mk_binop('*', $1, $3); }
    | expr '/' expr                   { $$ = mk_binop('/', $1, $3); }
    | expr '%' expr                   { $$ = mk_binop('%', $1, $3); }
    | expr '<' expr                   { $$ = mk_binop('<', $1, $3); }
    | expr '>' expr                   { $$ = mk_binop('>', $1, $3); }
    | expr LE expr                    { $$ = mk_binop('l', $1, $3); }
    | expr GE expr                    { $$ = mk_binop('g', $1, $3); }
    | expr EQ expr                    { $$ = mk_binop('e', $1, $3); }
    | expr NE expr                    { $$ = mk_binop('n', $1, $3); }
    | expr AND expr                   { $$ = mk_binop('&', $1, $3); }
    | expr OR expr                    { $$ = mk_binop('|', $1, $3); }
    | '-' expr %prec UMINUS           { $$ = mk_unary('-', $2); }
    | NOT expr %prec UMINUS           { $$ = mk_unary('!', $2); }
    | '(' expr ')'                    { $$ = $2; }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Помилка у рядку %d: %s\n", yylineno, s);
}
