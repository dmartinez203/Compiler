/* SYMBOL TABLE IMPLEMENTATION (scope-aware, hash-table backed)
 * Maintains a stack of scopes (global + per-function). Each scope
 * has its own hash table. Offsets are allocated from a single
 * global counter for simplicity of code generation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

#define HASH_SIZE 257

typedef struct SymNode {
    char* name;
    int offset;
    int isArray;
    int arraySize;
    struct SymNode* next;
} SymNode;

typedef struct ScopeFrame {
    const char* name;          /* e.g., "global" or function name */
    SymNode* buckets[HASH_SIZE];
    struct ScopeFrame* parent; /* enclosing scope */
} ScopeFrame;

/* Flat stats for compatibility/printing; offset counter is global */
static SymbolTable symtab;
static ScopeFrame* current = NULL;  /* top of scope stack */

/* Keep a list of scopes for printing (in creation order) */
static ScopeFrame scopePool[128];
static int scopeUsed = 0;
static ScopeFrame* scopeList[128];
static int scopeCount = 0;

/* djb2 string hash */
static unsigned int hash(const char* s) {
    unsigned long h = 5381;
    int c;
    while ((c = *s++)) h = ((h << 5) + h) + (unsigned char)c;
    return (unsigned int)(h % HASH_SIZE);
}

static ScopeFrame* newScope(const char* name, ScopeFrame* parent) {
    if (scopeUsed >= (int)(sizeof(scopePool)/sizeof(scopePool[0]))) {
        fprintf(stderr, "Scope pool exhausted\n");
        exit(1);
    }
    ScopeFrame* s = &scopePool[scopeUsed++];
    s->name = name;
    s->parent = parent;
    for (int i = 0; i < HASH_SIZE; i++) s->buckets[i] = NULL;
    if (scopeCount < (int)(sizeof(scopeList)/sizeof(scopeList[0]))) scopeList[scopeCount++] = s;
    return s;
}

void initSymTab() {
    symtab.count = 0;
    symtab.nextOffset = 0;
    scopeUsed = 0;
    scopeCount = 0;
    current = newScope("global", NULL);
}

void pushScope(const char* name) {
    if (!current) initSymTab();
    current = newScope(name, current);
}

void popScope() {
    if (current && current->parent) current = current->parent;
}

static SymNode* lookupIn(ScopeFrame* s, const char* name) {
    unsigned int h = hash(name);
    for (SymNode* n = s->buckets[h]; n; n = n->next) {
        if (strcmp(n->name, name) == 0) return n;
    }
    return NULL;
}

static SymNode* lookup(const char* name) {
    for (ScopeFrame* s = current; s; s = s->parent) {
        SymNode* n = lookupIn(s, name);
        if (n) return n;
    }
    return NULL;
}

int addVar(char* name) {
    if (!current) initSymTab();
    if (lookupIn(current, name)) return -1; /* duplicate in current scope */
    SymNode* n = (SymNode*)malloc(sizeof(SymNode));
    if (!n) return -1;
    n->name = strdup(name);
    n->offset = symtab.nextOffset;
    n->isArray = 0;
    n->arraySize = 0;
    unsigned int h = hash(name);
    n->next = current->buckets[h];
    current->buckets[h] = n;
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
    if (!current) initSymTab();
    if (lookupIn(current, name)) return -1;
    SymNode* n = (SymNode*)malloc(sizeof(SymNode));
    if (!n) return -1;
    n->name = strdup(name);
    n->offset = symtab.nextOffset;
    n->isArray = 1;
    n->arraySize = size;
    unsigned int h = hash(name);
    n->next = current->buckets[h];
    current->buckets[h] = n;
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
    SymNode* n = lookup(name);
    return n ? n->offset : -1;
}

int isVarDeclared(char* name) {
    return lookup(name) != NULL;
}

void printSymTab() {
    printf("\n=== SYMBOL TABLE (by scope) ===\n");
    printf("Total Count: %d, Next Offset: %d\n", symtab.count, symtab.nextOffset);
    for (int i = 0; i < scopeCount; i++) {
        ScopeFrame* s = scopeList[i];
        printf("Scope: %s\n", s->name);
        int empty = 1;
        for (int b = 0; b < HASH_SIZE; b++) {
            for (SymNode* n = s->buckets[b]; n; n = n->next) {
                empty = 0;
                if (n->isArray) printf("  %s[%d] -> offset %d\n", n->name, n->arraySize, n->offset);
                else printf("  %s -> offset %d\n", n->name, n->offset);
            }
        }
        if (empty) printf("  (empty)\n");
    }
    printf("==============================\n\n");
}
