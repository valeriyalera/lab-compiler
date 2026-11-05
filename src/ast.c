#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

static Node *new_node(NodeType t) {
    Node *n = calloc(1, sizeof(Node));
    n->type = t;
    return n;
}

Node *mk_num(double v) { Node *n = new_node(N_NUM); n->num = v; return n; }
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

Node *append_stmt(Node *list, Node *stmt) {
    if (!list) return stmt;
    Node *p = list;
    while (p->next) p = p->next;
    p->next = stmt;
    return list;
}

static const char *opname(char op) {
    switch (op) {
        case 'e': return "==";
        case 'n': return "!=";
        case 'l': return "<=";
        case 'g': return ">=";
        default: { static char b[2]; b[0] = op; b[1] = 0; return b; }
    }
}

void print_tree(Node *n, int depth) {
    for (; n; n = n->next) {
        for (int i = 0; i < depth; i++) printf("  ");
        switch (n->type) {
            case N_NUM:    printf("NUM %g\n", n->num); break;
            case N_VAR:    printf("VAR %s\n", n->name); break;
            case N_BINOP:  printf("BINOP %s\n", opname(n->op));
                           print_tree(n->a, depth + 1); print_tree(n->b, depth + 1); break;
            case N_UNARY:  printf("UNARY %s\n", opname(n->op));
                           print_tree(n->a, depth + 1); break;
            case N_ASSIGN: printf("ASSIGN %s\n", n->name);
                           print_tree(n->a, depth + 1); break;
            case N_PRINT:  printf("PRINT\n"); print_tree(n->a, depth + 1); break;
            case N_IF:     printf("IF\n");
                           print_tree(n->a, depth + 1);
                           print_tree(n->b, depth + 1);
                           if (n->c) print_tree(n->c, depth + 1);
                           break;
            case N_WHILE:  printf("WHILE\n");
                           print_tree(n->a, depth + 1); print_tree(n->b, depth + 1); break;
            case N_BLOCK:  printf("BLOCK\n"); print_tree(n->a, depth + 1); break;
        }
        /* у выражений next не используется, поэтому цикл идёт только по спискам операторов */
    }
}
