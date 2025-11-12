#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"

/* UPDATED: The new code generator reads from the TAC list, not the AST,
   so it no longer needs the root node. */
void generateMIPS(const char* filename);

#endif

