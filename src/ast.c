#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

static Node *new_node(NodeType t) {
    Node *n = calloc(1, sizeof(Node));
    n->type = t;
    return n;
}

Node *mk_num(double v, int vtype) { Node *n = new_node(N_NUM); n->num = v; n->vtype = vtype; return n; }
Node *mk_var(const char *name) { Node *n = new_node(N_VAR); n->name = strdup(name); return n; }
Node *mk_binop(char op, Node *l, Node *r) {
    Node *n = new_node(N_BINOP); n->op = op; n->a = l; n->b = r; return n;
}
Node *mk_unary(char op, Node *e) { Node *n = new_node(N_UNARY); n->op = op; n->a = e; return n; }
Node *mk_assign(const char *name, Node *e) {
    Node *n = new_node(N_ASSIGN); n->name = strdup(name); n->a = e; return n;
}
Node *mk_print(Node *e) { Node *n = new_node(N_PRINT); n->a = e; return n; }
Node *mk_if(Node *cond, Node *t, Node *e) {
    Node *n = new_node(N_IF); n->a = cond; n->b = t; n->c = e; return n;
}
Node *mk_while(Node *cond, Node *body) {
    Node *n = new_node(N_WHILE); n->a = cond; n->b = body; return n;
}
Node *mk_block(Node *first) { Node *n = new_node(N_BLOCK); n->a = first; return n; }
Node *mk_decl(int vtype, const char *name, Node *init) {
    Node *n = new_node(N_DECL); n->vtype = vtype; n->name = strdup(name); n->a = init; return n;
}
Node *mk_return(Node *e) { Node *n = new_node(N_RETURN); n->a = e; return n; }
Node *mk_for(Node *init, Node *cond, Node *step, Node *body) {
    Node *n = new_node(N_FOR); n->a = init; n->b = cond; n->c = step; n->d = body; return n;
}
Node *mk_call(const char *name, Node *args) {
    Node *n = new_node(N_CALL); n->name = strdup(name); n->a = args; return n;
}
Node *mk_func(int vtype, const char *name, Node *params, Node *body) {
    Node *n = new_node(N_FUNC); n->vtype = vtype; n->name = strdup(name);
    n->a = params; n->b = body; return n;
}
Node *mk_param(int vtype, const char *name) {
    Node *n = new_node(N_PARAM); n->vtype = vtype; n->name = strdup(name); return n;
}
Node *mk_program(Node *funcs) { Node *n = new_node(N_PROGRAM); n->a = funcs; return n; }

Node *append_stmt(Node *list, Node *stmt) {
    if (!list) return stmt;
    Node *p = list;
    while (p->next) p = p->next;
    p->next = stmt;
    return list;
}

const char *op_str(char op) {
    switch (op) {
        case 'e': return "==";
        case 'n': return "!=";
        case 'l': return "<=";
        case 'g': return ">=";
        case '&': return "&&";
        case '|': return "||";
        default: { static char b[2]; b[0] = op; b[1] = 0; return b; }
    }
}

const char *type_str(int vtype) {
    switch (vtype) {
        case T_INT: return "int";
        case T_DOUBLE: return "double";
        default: return "void";
    }
}

void print_tree(Node *n, int depth) {
    for (; n; n = n->next) {
        char label[160];
        switch (n->type) {
            case N_NUM:     snprintf(label, sizeof label, "NUM %g", n->num); break;
            case N_VAR:     snprintf(label, sizeof label, "VAR %s", n->name); break;
            case N_BINOP:   snprintf(label, sizeof label, "BINOP %s", op_str(n->op)); break;
            case N_UNARY:   snprintf(label, sizeof label, "UNARY %s", op_str(n->op)); break;
            case N_ASSIGN:  snprintf(label, sizeof label, "ASSIGN %s", n->name); break;
            case N_PRINT:   snprintf(label, sizeof label, "PRINT"); break;
            case N_IF:      snprintf(label, sizeof label, "IF"); break;
            case N_WHILE:   snprintf(label, sizeof label, "WHILE"); break;
            case N_BLOCK:   snprintf(label, sizeof label, "BLOCK"); break;
            case N_DECL:    snprintf(label, sizeof label, "DECL %s %s", type_str(n->vtype), n->name); break;
            case N_RETURN:  snprintf(label, sizeof label, "RETURN"); break;
            case N_FOR:     snprintf(label, sizeof label, "FOR"); break;
            case N_CALL:    snprintf(label, sizeof label, "CALL %s", n->name); break;
            case N_FUNC:    snprintf(label, sizeof label, "FUNC %s %s", type_str(n->vtype), n->name); break;
            case N_PARAM:   snprintf(label, sizeof label, "PARAM %s %s", type_str(n->vtype), n->name); break;
            case N_PROGRAM: snprintf(label, sizeof label, "PROGRAM"); break;
            default:        snprintf(label, sizeof label, "?"); break;
        }
        for (int i = 0; i < depth; i++) printf("  ");
        printf("%s\n", label);
        print_tree(n->a, depth + 1);
        print_tree(n->b, depth + 1);
        print_tree(n->c, depth + 1);
        print_tree(n->d, depth + 1);
    }
}
