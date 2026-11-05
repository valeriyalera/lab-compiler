#include <stdio.h>
#include "ast_json.h"

static void op_text(char op, char *buf) {
    switch (op) {
        case 'e': sprintf(buf, "=="); break;
        case 'n': sprintf(buf, "!="); break;
        case 'l': sprintf(buf, "<="); break;
        case 'g': sprintf(buf, ">="); break;
        default:  sprintf(buf, "%c", op);
    }
}

static void json_node(Node *n, FILE *out) {
    char op[4], label[128];
    const char *kind = "stmt";

    switch (n->type) {
        case N_NUM:    snprintf(label, sizeof label, "%g", n->num); kind = "num"; break;
        case N_VAR:    snprintf(label, sizeof label, "%s", n->name); kind = "var"; break;
        case N_BINOP:  op_text(n->op, op); snprintf(label, sizeof label, "%s", op); kind = "op"; break;
        case N_UNARY:  op_text(n->op, op); snprintf(label, sizeof label, "unary %s", op); kind = "op"; break;
        case N_ASSIGN: snprintf(label, sizeof label, "%s =", n->name); kind = "stmt"; break;
        case N_PRINT:  snprintf(label, sizeof label, "print"); kind = "stmt"; break;
        case N_IF:     snprintf(label, sizeof label, "if"); kind = "ctrl"; break;
        case N_WHILE:  snprintf(label, sizeof label, "while"); kind = "ctrl"; break;
        case N_BLOCK:  snprintf(label, sizeof label, "block"); kind = "block"; break;
    }

    fprintf(out, "{\"name\":\"%s\",\"kind\":\"%s\",\"children\":[", label, kind);

    int cnt = 0;
    if (n->type == N_BLOCK) {
        for (Node *s = n->a; s; s = s->next) {
            if (cnt++) fputc(',', out);
            json_node(s, out);
        }
    } else {
        Node *kids[3] = { n->a, n->b, n->c };
        for (int i = 0; i < 3; i++) {
            if (kids[i]) {
                if (cnt++) fputc(',', out);
                json_node(kids[i], out);
            }
        }
    }
    fputs("]}", out);
}

void print_ast_js(Node *root, FILE *out) {
    fputs("const AST = ", out);
    json_node(root, out);
    fputs(";\n", out);
}
