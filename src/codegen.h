#ifndef CODEGEN_H
#define CODEGEN_H
#include <stdio.h>
#include "ast.h"

void generate_c(Node *root, FILE *out);

#endif
