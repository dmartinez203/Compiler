/* SYMBOL TABLE IMPLEMENTATION (hash-table backed)
 * Keeps the same public API from symtab.h but uses a simple
 * chained hash table to speed up lookups from O(n) to O(1) average.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

/* Small fixed hash table size - must be > MAX_VARS for low collision rate */
#define HASH_SIZE 257

/* Internal node for the hash table */
typedef struct SymNode {
    char* name;
    DataType type;      /* NEW: Store the type */
    int offset;
    int isArray;
    int arraySize;
    struct SymNode* next;
} SymNode;

/* The global symbol table keeps nodes in a table and also an array for ordered printing */
static SymNode* table[HASH_SIZE];
static SymbolTable symtab; /* preserves original struct for compatibility */

/* djb2 string hash */
static unsigned int hash(const char* s) {
    unsigned long h = 5381;
    int c;
    while ((c = *s++)) h = ((h << 5) + h) + (unsigned char)c; /* h * 33 + c */
    return (unsigned int)(h % HASH_SIZE);
}

void initSymTab() {
    symtab.count = 0;
    symtab.nextOffset = 0;
    for (int i = 0; i < HASH_SIZE; i++) table[i] = NULL;
}

/* NEW: Helper to convert type enum to string for printing */
static const char* typeToString(DataType type) {
    switch (type) {
        case TYPE_INT:   return "int";
        case TYPE_FLOAT: return "float";
        default:         return "unknown";
    }
}

void printSymTab() {
    printf("\n=== SYMBOL TABLE STATE ===\n");
    printf("Count: %d, Next Offset: %d\n", symtab.count, symtab.nextOffset);
    if (symtab.count == 0) {
        printf("(empty)\n");
    } else {
        printf("Variables:\n");
        /* Print by scanning the buckets - order is unspecified but consistent */
        int idx = 0;
        for (int b = 0; b < HASH_SIZE; b++) {
            for (SymNode* n = table[b]; n; n = n->next) {
                if (n->isArray) {
                    /* UPDATED: Print type */
                    printf("  [%d] %s %s[%d] -> offset %d\n", idx++, typeToString(n->type), n->name, n->arraySize, n->offset);
                } else {
                    /* UPDATED: Print type */
                    printf("  [%d] %s %s -> offset %d\n", idx++, typeToString(n->type), n->name, n->offset);
                }
            }
        }
    }
    printf("==========================\n\n");
}

/* Internal lookup returning node pointer or NULL */
static SymNode* lookupNode(const char* name) {
    unsigned int h = hash(name);
    for (SymNode* n = table[h]; n; n = n->next) {
        if (strcmp(n->name, name) == 0) return n;
    }
    return NULL;
}

/* UPDATED: Function signature now takes a DataType */
int addVar(char* name, DataType type) {
    if (lookupNode(name)) return -1; /* duplicate */

    SymNode* n = malloc(sizeof(SymNode));
    if (!n) return -1;
    n->name = strdup(name);
    n->type = type; /* NEW: Store type */
    n->offset = symtab.nextOffset;
    n->isArray = 0;
    n->arraySize = 0;
    unsigned int h = hash(name);
    n->next = table[h];
    table[h] = n;

    /* Also store in symtab.vars array for compatibility/printing convenience */
    if (symtab.count < MAX_VARS) {
        symtab.vars[symtab.count].name = n->name;
        symtab.vars[symtab.count].type = n->type; /* NEW: Store type */
        symtab.vars[symtab.count].offset = n->offset;
        symtab.vars[symtab.count].isArray = n->isArray;
        symtab.vars[symtab.count].arraySize = n->arraySize;
    }
    symtab.count++;
    symtab.nextOffset += 4; /* 4 bytes per int or float */
    return n->offset;
}

/* UPDATED: Function signature now takes a DataType */
int addArray(char* name, DataType type, int size) {
    if (lookupNode(name)) return -1; /* duplicate */

    SymNode* n = malloc(sizeof(SymNode));
    if (!n) return -1;
    n->name = strdup(name);
    n->type = type; /* NEW: Store type */
    n->offset = symtab.nextOffset;
    n->isArray = 1;
    n->arraySize = size;
    unsigned int h = hash(name);
    n->next = table[h];
    table[h] = n;

    if (symtab.count < MAX_VARS) {
        symtab.vars[symtab.count].name = n->name;
        symtab.vars[symtab.count].type = n->type; /* NEW: Store type */
        symtab.vars[symtab.count].offset = n->offset;
        symtab.vars[symtab.count].isArray = n->isArray;
        symtab.vars[symtab.count].arraySize = n->arraySize;
    }
    symtab.count++;
    symtab.nextOffset += size * 4; /* 4 bytes per int or float */
    return n->offset;
}

int getVarOffset(char* name) {
    SymNode* n = lookupNode(name);
    if (!n) return -1;
    return n->offset;
}

/* NEW: Implementation for getVarType */
DataType getVarType(char* name) {
    SymNode* n = lookupNode(name);
    if (!n) {
        /* This should not happen if parser checks isVarDeclared first */
        /* Return a default, but this indicates a compiler bug */
        return TYPE_INT; 
    }
    return n->type;
}

int isVarDeclared(char* name) {
    return lookupNode(name) != NULL;
}

