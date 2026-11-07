%{
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "lang.tab.h"
%}

%option noyywrap
%option yylineno

%%

"int"       { return KW_INT; }
"double"    { return KW_DOUBLE; }
"void"      { return KW_VOID; }
"if"        { return IF; }
"else"      { return ELSE; }
"while"     { return WHILE; }
"for"       { return FOR; }
"return"    { return RETURN; }
"print"     { return PRINT; }

"=="        { return EQ; }
"!="        { return NE; }
"<="        { return LE; }
">="        { return GE; }
"&&"        { return AND; }
"||"        { return OR; }
"++"        { return INC; }
"--"        { return DEC; }
"!"         { return NOT; }

[0-9]+\.[0-9]+          { yylval.num = atof(yytext); return DNUM; }
[0-9]+                  { yylval.num = atof(yytext); return INUM; }
[a-zA-Z_][a-zA-Z0-9_]*  { yylval.str = strdup(yytext); return IDENT; }

"//".*                          { /* однорядковий коментар */ }
"/*"([^*]|\*+[^*/])*\*+"/"      { /* багаторядковий коментар */ }
[ \t\r\n]+                      { /* пробели */ }
.                               { return yytext[0]; }

%%
