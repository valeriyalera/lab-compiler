#ifndef AST_H
#define AST_H

typedef enum { T_INT, T_DOUBLE, T_VOID } VType;

typedef enum {
    N_NUM, N_VAR, N_BINOP, N_UNARY,
    N_ASSIGN, N_PRINT, N_IF, N_WHILE, N_BLOCK,
    N_DECL, N_RETURN, N_FOR, N_CALL, N_FUNC, N_PARAM, N_PROGRAM
} NodeType;

typedef struct Node {
    NodeType type;
    double num;          /* N_NUM */
    char *name;          /* имя переменной / функции */
    char op;             /* + - * / % < > e(==) n(!=) l(<=) g(>=) &(&&) |(||) !(унарный) */
    int vtype;           /* VType: тип числа, переменной, параметра, возвращаемого значения */
    struct Node *a;
    struct Node *b;
    struct Node *c;
    struct Node *d;
    struct Node *next;   /* следующий элемент списка: оператор, аргумент, параметр, функция */
} Node;

Node *mk_num(double v, int vtype);
Node *mk_var(const char *name);
Node *mk_binop(char op, Node *l, Node *r);
Node *mk_unary(char op, Node *e);
Node *mk_assign(const char *name, Node *e);
Node *mk_print(Node *e);
Node *mk_if(Node *cond, Node *then_b, Node *else_b);
Node *mk_while(Node *cond, Node *body);
Node *mk_block(Node *first);
Node *mk_decl(int vtype, const char *name, Node *init);
Node *mk_return(Node *e);
Node *mk_for(Node *init, Node *cond, Node *step, Node *body);
Node *mk_call(const char *name, Node *args);
Node *mk_func(int vtype, const char *name, Node *params, Node *body);
Node *mk_param(int vtype, const char *name);
Node *mk_program(Node *funcs);
Node *append_stmt(Node *list, Node *stmt);

const char *op_str(char op);
const char *type_str(int vtype);
void print_tree(Node *n, int depth);

#endif
