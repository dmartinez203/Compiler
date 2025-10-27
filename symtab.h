#ifndef SYMTAB_H
#define SYMTAB_H

/* SYMBOL TABLE
 * Tracks declared identifiers during compilation.
 * In this educational compiler, we map variable names to stack offsets.
 * Scope management is supported via a simple scope stack (global + per function).
 */

#define MAX_VARS 1000000  /* Maximum number of variables supported */

/* SYMBOL ENTRY - Information about each variable */
// In symtab.h
typedef struct {
    char* name;         /* Variable name */
    int offset;         /* Stack offset */
    int isArray;        /* Flag: 1 if it's an array, 0 otherwise */
    int arraySize;      /* Number of elements if it's an array */
} Symbol;

/* SYMBOL TABLE STRUCTURE */
typedef struct {
    Symbol vars[MAX_VARS];  /* Array of all variables */
    int count;              /* Number of variables declared */
    int nextOffset;         /* Next available stack offset */
} SymbolTable;

/* SYMBOL TABLE OPERATIONS */
void initSymTab();               /* Initialize empty symbol table */
void pushScope(const char* name);/* Enter a new lexical scope (e.g., function) */
void popScope();                 /* Exit current scope */
int addVar(char* name);          /* Add new variable, returns offset or -1 if duplicate */
int getVarOffset(char* name);    /* Get stack offset for variable, -1 if not found */
int isVarDeclared(char* name);   /* Check if variable exists (1=yes, 0=no) */
void printSymTab();              /* Debug: print current symbol table contents */

#endif