#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "interp.h"
#include "codegen.h"
#include "ast_json.h"

extern int yyparse(void);
extern FILE *yyin;
extern Node *root;

int main(int argc, char **argv) {
    int show_tree = 0, emit_c = 0, emit_web = 0;
    const char *file = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--tree") == 0) show_tree = 1;
        else if (strcmp(argv[i], "--c") == 0) emit_c = 1;
        else if (strcmp(argv[i], "--web") == 0) emit_web = 1;
        else file = argv[i];
    }

    if (file) {
        yyin = fopen(file, "r");
        if (!yyin) { perror(file); return 1; }
    }

    if (yyparse() != 0) return 1;

    if (show_tree) print_tree(root, 0);
    else if (emit_c) generate_c(root, stdout);
    else if (emit_web) print_ast_js(root, stdout);
    else run_program(root);
    return 0;
}
