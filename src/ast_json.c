#include <stdio.h>
#include "ast_json.h"

static void json_node(Node *n, FILE *out);

static void json_chain(Node *s, FILE *out, int *cnt) {
    for (; s; s = s->next) {
        if ((*cnt)++) fputc(',', out);
        json_node(s, out);
    }
}

static void json_node(Node *n, FILE *out) {
    char label[160];
    const char *kind = "stmt";

    switch (n->type) {
        case N_NUM:     snprintf(label, sizeof label, "%g", n->num); kind = "num"; break;
        case N_VAR:     snprintf(label, sizeof label, "%s", n->name); kind = "var"; break;
        case N_BINOP:   snprintf(label, sizeof label, "%s", op_str(n->op)); kind = "op"; break;
        case N_UNARY:   snprintf(label, sizeof label, "unary %s", op_str(n->op)); kind = "op"; break;
        case N_CALL:    snprintf(label, sizeof label, "%s()", n->name); kind = "op"; break;
        case N_ASSIGN:  snprintf(label, sizeof label, "%s =", n->name); break;
        case N_PRINT:   snprintf(label, sizeof label, "print"); break;
        case N_DECL:    snprintf(label, sizeof label, "%s %s", type_str(n->vtype), n->name); break;
        case N_PARAM:   snprintf(label, sizeof label, "%s %s", type_str(n->vtype), n->name); kind = "var"; break;
        case N_IF:      snprintf(label, sizeof label, "if"); kind = "ctrl"; break;
        case N_WHILE:   snprintf(label, sizeof label, "while"); kind = "ctrl"; break;
        case N_FOR:     snprintf(label, sizeof label, "for"); kind = "ctrl"; break;
        case N_RETURN:  snprintf(label, sizeof label, "return"); kind = "ctrl"; break;
        case N_FUNC:    snprintf(label, sizeof label, "%s %s()", type_str(n->vtype), n->name); kind = "ctrl"; break;
        case N_BLOCK:   snprintf(label, sizeof label, "block"); kind = "block"; break;
        case N_PROGRAM: snprintf(label, sizeof label, "program"); kind = "block"; break;
        default:        snprintf(label, sizeof label, "?"); break;
    }

    fprintf(out, "{\"name\":\"%s\",\"kind\":\"%s\",\"children\":[", label, kind);
    int cnt = 0;
    json_chain(n->a, out, &cnt);
    json_chain(n->b, out, &cnt);
    json_chain(n->c, out, &cnt);
    json_chain(n->d, out, &cnt);
    fputs("]}", out);
}

void print_ast_js(Node *root, FILE *out) {
    fputs("const AST = ", out);
    json_node(root, out);
    fputs(";\n", out);
}
