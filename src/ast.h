#ifndef AST_H
#define AST_H

typedef enum {
    N_NUM, N_VAR, N_BINOP, N_UNARY,
    N_ASSIGN, N_PRINT, N_IF, N_WHILE, N_BLOCK
} NodeType;

typedef struct Node {
    NodeType type;
    double num;          /* для N_NUM */
    char *name;          /* для N_VAR, N_ASSIGN */
    char op;             /* для N_BINOP, N_UNARY: + - * / < > e(==) n(!=) l(<=) g(>=) */
    struct Node *a;      /* первый ребёнок */
    struct Node *b;      /* второй ребёнок */
    struct Node *c;      /* третий ребёнок (else) */
    struct Node *next;   /* следующий оператор в блоке */
} Node;

Node *mk_num(double v);
Node *mk_var(const char *name);
Node *mk_binop(char op, Node *l, Node *r);
Node *mk_unary(char op, Node *e);
Node *mk_assign(const char *name, Node *e);
Node *mk_print(Node *e);
Node *mk_if(Node *cond, Node *then_b, Node *else_b);
Node *mk_while(Node *cond, Node *body);
Node *mk_block(Node *first);
Node *append_stmt(Node *list, Node *stmt);

void print_tree(Node *n, int depth);

#endif
