%{
#include <string.h>
#include "ast.h"
#include "lang.tab.h"
%}

%option noyywrap
%option yylineno

%%

"if"        { return IF; }
"else"      { return ELSE; }
"while"     { return WHILE; }
"print"     { return PRINT; }

"=="        { return EQ; }
"!="        { return NE; }
"<="        { return LE; }
">="        { return GE; }

[0-9]+(\.[0-9]+)?       { yylval.num = atof(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]*  { yylval.str = strdup(yytext); return IDENT; }

"//".*      { /* комментарий до конца строки */ }
[ \t\r\n]+  { /* пробелы и переводы строк пропускаем */ }
.           { return yytext[0]; }

%%
