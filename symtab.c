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

typedef struct SymNode {
    char* name;
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
                    printf("  [%d] %s[%d] -> offset %d\n", idx++, n->name, n->arraySize, n->offset);
                } else {
                    printf("  [%d] %s -> offset %d\n", idx++, n->name, n->offset);
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

int addVar(char* name) {
    if (lookupNode(name)) return -1; /* duplicate */

    SymNode* n = malloc(sizeof(SymNode));
    if (!n) return -1;
    n->name = strdup(name);
    n->offset = symtab.nextOffset;
    n->isArray = 0;
    n->arraySize = 0;
    unsigned int h = hash(name);
    n->next = table[h];
    table[h] = n;

    /* Also store in symtab.vars array for compatibility/printing convenience */
    if (symtab.count < MAX_VARS) {
        symtab.vars[symtab.count].name = n->name;
        symtab.vars[symtab.count].offset = n->offset;
        symtab.vars[symtab.count].isArray = n->isArray;
        symtab.vars[symtab.count].arraySize = n->arraySize;
    }
    symtab.count++;
    symtab.nextOffset += 4; /* 4 bytes per int */
    return n->offset;
}

int addArray(char* name, int size) {
    if (lookupNode(name)) return -1; /* duplicate */

    SymNode* n = malloc(sizeof(SymNode));
    if (!n) return -1;
    n->name = strdup(name);
    n->offset = symtab.nextOffset;
    n->isArray = 1;
    n->arraySize = size;
    unsigned int h = hash(name);
    n->next = table[h];
    table[h] = n;

    if (symtab.count < MAX_VARS) {
        symtab.vars[symtab.count].name = n->name;
        symtab.vars[symtab.count].offset = n->offset;
        symtab.vars[symtab.count].isArray = n->isArray;
        symtab.vars[symtab.count].arraySize = n->arraySize;
    }
    symtab.count++;
    symtab.nextOffset += size * 4;
    return n->offset;
}

int getVarOffset(char* name) {
    SymNode* n = lookupNode(name);
    if (!n) return -1;
    return n->offset;
}

int isVarDeclared(char* name) {
    return lookupNode(name) != NULL;
}
