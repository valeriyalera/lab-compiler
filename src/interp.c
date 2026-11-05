#include <stdio.h>
#include <string.h>
#include "interp.h"

#define MAX_VARS 256

static struct { char name[64]; double value; } vars[MAX_VARS];
static int nvars = 0;

static double get_var(const char *name) {
    for (int i = 0; i < nvars; i++)
        if (strcmp(vars[i].name, name) == 0) return vars[i].value;
    fprintf(stderr, "Ошибка выполнения: переменная '%s' не определена (считаю 0)\n", name);
    return 0;
}

static void set_var(const char *name, double v) {
    for (int i = 0; i < nvars; i++)
        if (strcmp(vars[i].name, name) == 0) { vars[i].value = v; return; }
    if (nvars >= MAX_VARS) { fprintf(stderr, "Слишком много переменных\n"); return; }
    strncpy(vars[nvars].name, name, 63);
    vars[nvars].value = v;
    nvars++;
}

/* Вычисление выражения: возвращает число */
static double eval(Node *n) {
    switch (n->type) {
        case N_NUM: return n->num;
        case N_VAR: return get_var(n->name);
        case N_UNARY: return -eval(n->a);
        case N_BINOP: {
            double l = eval(n->a);
            double r = eval(n->b);
            switch (n->op) {
                case '+': return l + r;
                case '-': return l - r;
                case '*': return l * r;
                case '/':
                    if (r == 0) { fprintf(stderr, "Ошибка выполнения: деление на ноль\n"); return 0; }
                    return l / r;
                case '<': return l < r;
                case '>': return l > r;
                case 'l': return l <= r;
                case 'g': return l >= r;
                case 'e': return l == r;
                case 'n': return l != r;
            }
        }
        default: return 0;
    }
}

/* Выполнение одного оператора или списка операторов (через next) */
static void exec(Node *n) {
    for (; n; n = n->next) {
        switch (n->type) {
            case N_ASSIGN: set_var(n->name, eval(n->a)); break;
            case N_PRINT:  printf("%g\n", eval(n->a)); break;
            case N_IF:
                if (eval(n->a) != 0) exec(n->b);
                else if (n->c) exec(n->c);
                break;
            case N_WHILE:
                while (eval(n->a) != 0) exec(n->b);
                break;
            case N_BLOCK:  exec(n->a); break;
            default: break;
        }
    }
}

void run_program(Node *root) {
    exec(root);
}
