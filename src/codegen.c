#include <stdio.h>
#include <string.h>
#include "codegen.h"

#define MAX_VARS 256
static char *names[MAX_VARS];
static int nnames = 0;

/* Собираем имена всех переменных, чтобы объявить их в начале */
static void add_name(const char *s) {
    for (int i = 0; i < nnames; i++)
        if (strcmp(names[i], s) == 0) return;
    if (nnames < MAX_VARS) names[nnames++] = (char *)s;
}

static void collect(Node *n) {
    for (; n; n = n->next) {
        if (n->type == N_ASSIGN || n->type == N_VAR) add_name(n->name);
        collect(n->a);
        collect(n->b);
        collect(n->c);
    }
}

static const char *c_op(char op) {
    switch (op) {
        case 'e': return "==";
        case 'n': return "!=";
        case 'l': return "<=";
        case 'g': return ">=";
        default: { static char b[2]; b[0] = op; b[1] = 0; return b; }
    }
}

static void gen_expr(Node *n, FILE *out) {
    switch (n->type) {
        case N_NUM: {
            char buf[64];
            snprintf(buf, sizeof buf, "%g", n->num);
            if (!strpbrk(buf, ".eE")) strcat(buf, ".0");  /* 5 -> 5.0, чтобы не было целочисленного деления */
            fprintf(out, "%s", buf);
            break;
        }
        case N_VAR: fprintf(out, "%s", n->name); break;
        case N_UNARY:
            fprintf(out, "(-");
            gen_expr(n->a, out);
            fprintf(out, ")");
            break;
        case N_BINOP:
            fprintf(out, "(");
            gen_expr(n->a, out);
            fprintf(out, " %s ", c_op(n->op));
            gen_expr(n->b, out);
            fprintf(out, ")");
            break;
        default: break;
    }
}

static void indent(FILE *out, int d) {
    for (int i = 0; i < d; i++) fprintf(out, "    ");
}

static void gen_stmt(Node *n, int d, FILE *out);

/* Тело if/while: если это блок, берём его содержимое без лишних скобок */
static void gen_body(Node *n, int d, FILE *out) {
    if (n->type == N_BLOCK) gen_stmt(n->a, d, out);
    else gen_stmt(n, d, out);
}

static void gen_stmt(Node *n, int d, FILE *out) {
    for (; n; n = n->next) {
        switch (n->type) {
            case N_ASSIGN:
                indent(out, d);
                fprintf(out, "%s = ", n->name);
                gen_expr(n->a, out);
                fprintf(out, ";\n");
                break;
            case N_PRINT:
                indent(out, d);
                fprintf(out, "printf(\"%%g\\n\", ");
                gen_expr(n->a, out);
                fprintf(out, ");\n");
                break;
            case N_IF:
                indent(out, d);
                fprintf(out, "if (");
                gen_expr(n->a, out);
                fprintf(out, ") {\n");
                gen_body(n->b, d + 1, out);
                indent(out, d);
                if (n->c) {
                    fprintf(out, "} else {\n");
                    gen_body(n->c, d + 1, out);
                    indent(out, d);
                }
                fprintf(out, "}\n");
                break;
            case N_WHILE:
                indent(out, d);
                fprintf(out, "while (");
                gen_expr(n->a, out);
                fprintf(out, ") {\n");
                gen_body(n->b, d + 1, out);
                indent(out, d);
                fprintf(out, "}\n");
                break;
            case N_BLOCK:
                indent(out, d);
                fprintf(out, "{\n");
                gen_stmt(n->a, d + 1, out);
                indent(out, d);
                fprintf(out, "}\n");
                break;
            default: break;
        }
    }
}

void generate_c(Node *root, FILE *out) {
    nnames = 0;
    collect(root);

    fprintf(out, "#include <stdio.h>\n\nint main(void) {\n");
    for (int i = 0; i < nnames; i++) {
        indent(out, 1);
        fprintf(out, "double %s = 0;\n", names[i]);
    }
    if (nnames) fprintf(out, "\n");
    gen_stmt(root->a, 1, out);
    indent(out, 1);
    fprintf(out, "return 0;\n}\n");
}
