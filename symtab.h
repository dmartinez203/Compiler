#ifndef SYMTAB_H
#define SYMTAB_H

/* SYMBOL TABLE
 * Tracks all declared variables during compilation
 * Maps variable names to their memory locations (stack offsets)
 * Used for semantic checking and code generation
 */

#define MAX_VARS 1000000  /* Maximum number of variables supported */

/* NEW: Data type enum */
typedef enum {
    TYPE_INT,
    TYPE_FLOAT
} DataType;

/* SYMBOL ENTRY - Information about each variable */
typedef struct {
    char* name;         /* Variable name */
    DataType type;      /* NEW: Store the type */
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
void initSymTab();                                  /* Initialize empty symbol table */
int addVar(char* name, DataType type);              /* UPDATED: Add new variable with type */
int addArray(char* name, DataType type, int size);  /* UPDATED: Add new array with type */
int getVarOffset(char* name);                       /* Get stack offset for variable, -1 if not found */
DataType getVarType(char* name);                    /* NEW: Get the type of a variable */
int isVarDeclared(char* name);                      /* Check if variable exists (1=yes, 0=no) */

#endif

