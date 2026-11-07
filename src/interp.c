#include <stdio.h>
#include <string.h>
#include "interp.h"

#define MAX_VARS 64

typedef struct { double v; int is_int; } Value;
typedef struct { const char *name; int type; Value val; } Var;
typedef struct { Var vars[MAX_VARS]; int n; Value ret; } Frame;

static Node *program = NULL;

static Value mkv(double v, int is_int) { Value x; x.v = v; x.is_int = is_int; return x; }

static Value coerce(Value v, int type) {
    if (type == T_INT)    return mkv((double)(long long)v.v, 1);
    if (type == T_DOUBLE) return mkv(v.v, 0);
    return v;
}

static Node *find_func(const char *name) {
    for (Node *f = program->a; f; f = f->next)
        if (strcmp(f->name, name) == 0) return f;
    return NULL;
}

static Var *lookup(Frame *f, const char *name) {
    for (int i = 0; i < f->n; i++)
        if (strcmp(f->vars[i].name, name) == 0) return &f->vars[i];
    return NULL;
}

static void declare(Frame *f, const char *name, int type, Value v) {
    Var *p = lookup(f, name);
    if (!p) {
        if (f->n >= MAX_VARS) { fprintf(stderr, "Помилка виконання: забагато змінних\n"); return; }
        p = &f->vars[f->n++];
        p->name = name;
    }
    p->type = type;
    p->val = coerce(v, type);
}

static Value eval(Node *n, Frame *f);
static int exec(Node *n, Frame *f);

static Value call_function(Node *call, Frame *caller) {
    Node *fn = find_func(call->name);
    if (!fn) {
        fprintf(stderr, "Помилка виконання: функцію '%s' не визначено\n", call->name);
        return mkv(0, 1);
    }
    Frame fr;
    fr.n = 0;
    fr.ret = mkv(0, 1);
    Node *p = fn->a, *a = call->a;
    for (; p && a; p = p->next, a = a->next)
        declare(&fr, p->name, p->vtype, eval(a, caller));
    if (p || a) {
        fprintf(stderr, "Помилка виконання: невірна кількість аргументів у виклику '%s'\n", call->name);
        return mkv(0, 1);
    }
    exec(fn->b, &fr);
    return coerce(fr.ret, fn->vtype);
}

static Value eval(Node *n, Frame *f) {
    switch (n->type) {
        case N_NUM: return mkv(n->num, n->vtype == T_INT);
        case N_VAR: {
            Var *p = lookup(f, n->name);
            if (!p) {
                fprintf(stderr, "Помилка виконання: змінну '%s' не оголошено (вважаю 0)\n", n->name);
                return mkv(0, 1);
            }
            return p->val;
        }
        case N_UNARY: {
            Value x = eval(n->a, f);
            if (n->op == '!') return mkv(x.v == 0, 1);
            return mkv(-x.v, x.is_int);
        }
        case N_CALL: return call_function(n, f);
        case N_BINOP: {
            /* && и || вычисляются лениво, как в C */
            if (n->op == '&') {
                Value l = eval(n->a, f);
                if (l.v == 0) return mkv(0, 1);
                return mkv(eval(n->b, f).v != 0, 1);
            }
            if (n->op == '|') {
                Value l = eval(n->a, f);
                if (l.v != 0) return mkv(1, 1);
                return mkv(eval(n->b, f).v != 0, 1);
            }
            Value l = eval(n->a, f);
            Value r = eval(n->b, f);
            int both = l.is_int && r.is_int;
            switch (n->op) {
                case '+': return mkv(l.v + r.v, both);
                case '-': return mkv(l.v - r.v, both);
                case '*': return mkv(l.v * r.v, both);
                case '/':
                    if (r.v == 0) { fprintf(stderr, "Помилка виконання: ділення на нуль\n"); return mkv(0, both); }
                    if (both) return mkv((double)((long long)l.v / (long long)r.v), 1);
                    return mkv(l.v / r.v, 0);
                case '%':
                    if (!both || r.v == 0) { fprintf(stderr, "Помилка виконання: %% вимагає двох int і ненульового дільника\n"); return mkv(0, 1); }
                    return mkv((double)((long long)l.v % (long long)r.v), 1);
                case '<': return mkv(l.v < r.v, 1);
                case '>': return mkv(l.v > r.v, 1);
                case 'l': return mkv(l.v <= r.v, 1);
                case 'g': return mkv(l.v >= r.v, 1);
                case 'e': return mkv(l.v == r.v, 1);
                case 'n': return mkv(l.v != r.v, 1);
            }
            return mkv(0, 1);
        }
        default: return mkv(0, 1);
    }
}

static int exec(Node *n, Frame *f) {
    for (; n; n = n->next) {
        switch (n->type) {
            case N_DECL:
                declare(f, n->name, n->vtype, n->a ? eval(n->a, f) : mkv(0, 1));
                break;
            case N_ASSIGN: {
                Var *p = lookup(f, n->name);
                Value v = eval(n->a, f);
                if (!p) fprintf(stderr, "Помилка виконання: змінну '%s' не оголошено\n", n->name);
                else p->val = coerce(v, p->type);
                break;
            }
            case N_PRINT: {
                Value v = eval(n->a, f);
                if (v.is_int) printf("%lld\n", (long long)v.v);
                else printf("%g\n", v.v);
                break;
            }
            case N_IF:
                if (eval(n->a, f).v != 0) { if (exec(n->b, f)) return 1; }
                else if (n->c) { if (exec(n->c, f)) return 1; }
                break;
            case N_WHILE:
                while (eval(n->a, f).v != 0)
                    if (exec(n->b, f)) return 1;
                break;
            case N_FOR:
                exec(n->a, f);
                while (!n->b || eval(n->b, f).v != 0) {
                    if (exec(n->d, f)) return 1;
                    exec(n->c, f);
                }
                break;
            case N_BLOCK:
                if (exec(n->a, f)) return 1;
                break;
            case N_RETURN:
                if (n->a) f->ret = eval(n->a, f);
                return 1;
            case N_CALL:
                eval(n, f);
                break;
            default: break;
        }
    }
    return 0;
}

void run_program(Node *root) {
    program = root;
    if (!find_func("main")) {
        fprintf(stderr, "Помилка: немає функції main\n");
        return;
    }
    Node call = {0};
    call.type = N_CALL;
    call.name = "main";
    Frame dummy;
    dummy.n = 0;
    call_function(&call, &dummy);
}
