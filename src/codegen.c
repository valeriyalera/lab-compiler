#include <stdio.h>
#include <string.h>
#include "codegen.h"

static void gen_expr(Node *n, FILE *out) {
    switch (n->type) {
        case N_NUM: {
            char buf[64];
            if (n->vtype == T_INT) {
                snprintf(buf, sizeof buf, "%.0f", n->num);
            } else {
                snprintf(buf, sizeof buf, "%g", n->num);
                if (!strpbrk(buf, ".eE")) strcat(buf, ".0");
            }
            fprintf(out, "%s", buf);
            break;
        }
        case N_VAR: fprintf(out, "%s", n->name); break;
        case N_UNARY:
            fprintf(out, "(%s", op_str(n->op));
            gen_expr(n->a, out);
            fprintf(out, ")");
            break;
        case N_BINOP:
            fprintf(out, "(");
            gen_expr(n->a, out);
            fprintf(out, " %s ", op_str(n->op));
            gen_expr(n->b, out);
            fprintf(out, ")");
            break;
        case N_CALL:
            fprintf(out, "%s(", n->name);
            for (Node *a = n->a; a; a = a->next) {
                if (a != n->a) fprintf(out, ", ");
                gen_expr(a, out);
            }
            fprintf(out, ")");
            break;
        default: break;
    }
}

static void gen_simple(Node *n, FILE *out) {
    if (!n) return;
    if (n->type == N_DECL) {
        fprintf(out, "%s %s = ", type_str(n->vtype), n->name);
        if (n->a) gen_expr(n->a, out); else fprintf(out, "0");
    } else if (n->type == N_ASSIGN) {
        fprintf(out, "%s = ", n->name);
        gen_expr(n->a, out);
    }
}

static void indent(FILE *out, int d) {
    for (int i = 0; i < d; i++) fprintf(out, "    ");
}

static void gen_stmt(Node *n, int d, FILE *out);

static void gen_body(Node *n, int d, FILE *out) {
    if (!n) return;
    if (n->type == N_BLOCK) gen_stmt(n->a, d, out);
    else gen_stmt(n, d, out);
}

static void gen_stmt(Node *n, int d, FILE *out) {
    for (; n; n = n->next) {
        switch (n->type) {
            case N_DECL:
            case N_ASSIGN:
                indent(out, d);
                gen_simple(n, out);
                fprintf(out, ";\n");
                break;
            case N_CALL:
                indent(out, d);
                gen_expr(n, out);
                fprintf(out, ";\n");
                break;
            case N_RETURN:
                indent(out, d);
                if (n->a) { fprintf(out, "return "); gen_expr(n->a, out); fprintf(out, ";\n"); }
                else fprintf(out, "return;\n");
                break;
            case N_PRINT:
                indent(out, d);
                fprintf(out, "PRINT(");
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
            case N_FOR:
                indent(out, d);
                fprintf(out, "for (");
                gen_simple(n->a, out);
                fprintf(out, "; ");
                if (n->b) gen_expr(n->b, out);
                fprintf(out, "; ");
                gen_simple(n->c, out);
                fprintf(out, ") {\n");
                gen_body(n->d, d + 1, out);
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

static void gen_params(Node *p, FILE *out) {
    if (!p) { fprintf(out, "void"); return; }
    for (Node *q = p; q; q = q->next) {
        if (q != p) fprintf(out, ", ");
        fprintf(out, "%s %s", type_str(q->vtype), q->name);
    }
}

void generate_c(Node *root, FILE *out) {
    fprintf(out, "#include <stdio.h>\n\n");
    fprintf(out, "/* print(x): формат обирається за типом виразу */\n");
    fprintf(out, "#define PRINT(x) printf(_Generic((x), int: \"%%d\\n\", double: \"%%g\\n\"), (x))\n\n");

    for (Node *f = root->a; f; f = f->next) {
        fprintf(out, "%s %s(", type_str(f->vtype), f->name);
        gen_params(f->a, out);
        fprintf(out, ");\n");
    }
    fprintf(out, "\n");

    for (Node *f = root->a; f; f = f->next) {
        fprintf(out, "%s %s(", type_str(f->vtype), f->name);
        gen_params(f->a, out);
        fprintf(out, ") {\n");
        gen_stmt(f->b->a, 1, out);
        fprintf(out, "}\n\n");
    }
}
