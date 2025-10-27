/* AST IMPLEMENTATION
 * Functions to create and manipulate Abstract Syntax Tree nodes
 * The AST is built during parsing and used for all subsequent phases
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* Create a number literal node */
ASTNode* createNum(int value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_NUM;
    node->data.num = value;  /* Store the integer value */
    return node;
}

/* Create a float literal node */
ASTNode* createFloat(double value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_FNUM;
    node->data.fnum = value;
    return node;
}

/* Create a variable reference node */
ASTNode* createVar(char* name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_VAR;
    node->data.name = strdup(name);  /* Copy the variable name */
    return node;
}

/* Create a binary operation node (for addition) */
ASTNode* createBinOp(char op, ASTNode* left, ASTNode* right) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_BINOP;
    node->data.binop.op = op;        /* Store operator (+) */
    node->data.binop.left = left;    /* Left subtree */
    node->data.binop.right = right;  /* Right subtree */
    return node;
}

/* Create a variable declaration node */
ASTNode* createDecl(char* name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_DECL;
    node->data.name = strdup(name);  /* Store variable name */
    return node;
}

/* Create a float variable declaration node */
ASTNode* createDeclFloat(char* name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_DECL_FLOAT;
    node->data.decl_float.name = strdup(name);
    return node;
}

/* Create an assignment statement node */
ASTNode* createAssign(char* var, ASTNode* value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_ASSIGN;
    node->data.assign.var = strdup(var);  /* Variable name */
    node->data.assign.value = value;      /* Expression tree */
    return node;
}

/* Create a print statement node */
ASTNode* createPrint(ASTNode* expr) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_PRINT;
    node->data.expr = expr;  /* Expression to print */
    return node;
}

/* Create an array declaration node */
ASTNode* createArrayDecl(char* name, int size) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_ARRAY_DECL;
    node->data.array_decl.name = strdup(name); /* Array name */
    node->data.array_decl.size = size;         /* Array size */
    return node;
}

/* Create an array element assignment node */
ASTNode* createArrayAssign(char* name, ASTNode* index, ASTNode* value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_ARRAY_ASSIGN;
    node->data.array_assign.name = strdup(name); /* Array name */
    node->data.array_assign.index = index;       /* Index expression */
    node->data.array_assign.value = value;       /* Value expression */
    return node;
}

/* Create an array element access node */
ASTNode* createArrayAccess(char* name, ASTNode* index) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_ARRAY_ACCESS;
    node->data.array_access.name = strdup(name); /* Array name */
    node->data.array_access.index = index;       /* Index expression */
    return node;
}


/*
 * Example helper (commented out):
 *
 * ASTNode* createDeclWithAssgn(char* name, int value) {
 *     ASTNode* node = malloc(sizeof(ASTNode));
 *     node->type = NODE_DECL;
 *     node->data.name = strdup(name);
 *     // To add an assignment you'd create an assignment node and link it
 *     return node;
 * }
 */

/* Create a statement list node (links statements together) */
ASTNode* createStmtList(ASTNode* stmt1, ASTNode* stmt2) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_STMT_LIST;
    node->data.stmtlist.stmt = stmt1;  /* First statement */
    node->data.stmtlist.next = stmt2;  /* Rest of list */
    return node;
}

/* Create a function declaration node */
ASTNode* createFuncDecl(char* name, ASTNode* params, ASTNode* body, ASTNode* ret) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_FUNC_DECL;
    node->data.func_decl.name = strdup(name);
    node->data.func_decl.params = params;
    node->data.func_decl.body = body;
    node->data.func_decl.ret = ret;
    return node;
}

/* Create a function call node */
ASTNode* createFuncCall(char* name, ASTNode* args) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_FUNC_CALL;
    node->data.func_call.name = strdup(name);
    node->data.func_call.args = args;
    return node;
}

/* Parameter list helpers */
ASTNode* createParamList(char* name, int vtype) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_PARAM_LIST;
    node->data.param_list.name = strdup(name);
    node->data.param_list.vtype = vtype;
    node->data.param_list.next = NULL;
    return node;
}

ASTNode* appendParam(ASTNode* list, char* name, int vtype) {
    if (!list) return createParamList(name, vtype);
    ASTNode* cur = list;
    while (cur->data.param_list.next) cur = cur->data.param_list.next;
    ASTNode* n = createParamList(name, vtype);
    cur->data.param_list.next = n;
    return list;
}

/* Argument list helpers */
ASTNode* createArgList(ASTNode* expr) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_ARG_LIST;
    node->data.arg_list.expr = expr;
    node->data.arg_list.next = NULL;
    return node;
}

ASTNode* appendArg(ASTNode* list, ASTNode* expr) {
    if (!list) return createArgList(expr);
    ASTNode* cur = list;
    while (cur->data.arg_list.next) cur = cur->data.arg_list.next;
    ASTNode* n = createArgList(expr);
    cur->data.arg_list.next = n;
    return list;
}

/* Return statement node */
ASTNode* createReturn(ASTNode* expr) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_RETURN;
    node->data.return_expr = expr;
    return node;
}

/* Display the AST structure (for debugging and education) */
void printAST(ASTNode* node, int level) {
    if (!node) return;
    
    /* Indent based on tree depth */
    for (int i = 0; i < level; i++) printf("  ");
    
    /* Print node based on its type */
    switch(node->type) {
        case NODE_NUM:
            printf("NUM: %d\n", node->data.num);
            break;
        case NODE_FNUM:
            printf("FNUM: %g\n", node->data.fnum);
            break;
        case NODE_VAR:
            printf("VAR: %s\n", node->data.name);
            break;
        case NODE_BINOP:
            printf("BINOP: %c\n", node->data.binop.op);
            printAST(node->data.binop.left, level + 1);
            printAST(node->data.binop.right, level + 1);
            break;
        case NODE_DECL:
            printf("DECL: %s\n", node->data.name);
            break;
        case NODE_DECL_FLOAT:
            printf("DECL: %s (float)\n", node->data.decl_float.name);
            break;
        case NODE_ASSIGN:
            printf("ASSIGN TO: %s\n", node->data.assign.var);
            printAST(node->data.assign.value, level + 1);
            break;
        case NODE_PRINT:
            printf("PRINT\n");
            printAST(node->data.expr, level + 1);
            break;
        case NODE_STMT_LIST:
            printAST(node->data.stmtlist.stmt, level);
            printAST(node->data.stmtlist.next, level);
            break;
        
        /* --- ADD THESE NEW CASES FOR ARRAYS --- */
        case NODE_ARRAY_DECL:
            printf("ARRAY_DECL: %s[%d]\n", node->data.array_decl.name, node->data.array_decl.size);
            break;
        case NODE_ARRAY_ASSIGN:
            printf("ARRAY_ASSIGN TO: %s\n", node->data.array_assign.name);
            for (int i = 0; i < level + 1; i++) printf("   ");
            printf("Index:\n");
            printAST(node->data.array_assign.index, level + 2);
            for (int i = 0; i < level + 1; i++) printf("   ");
            printf("Value:\n");
            printAST(node->data.array_assign.value, level + 2);
            break;
        case NODE_ARRAY_ACCESS:
            printf("ARRAY_ACCESS: %s\n", node->data.array_access.name);
            for (int i = 0; i < level + 1; i++) printf("   ");
            printf("Index:\n");
            printAST(node->data.array_access.index, level + 2);
            break;
        case NODE_FUNC_DECL:
            printf("FUNC_DECL: %s\n", node->data.func_decl.name);
            if (node->data.func_decl.params) {
                printf("  Params:\n");
                ASTNode* p = node->data.func_decl.params;
                while (p) {
                    for (int i = 0; i < level + 2; i++) printf("  ");
                    if (p->data.param_list.vtype == TYPE_FLOAT)
                        printf("%s: float\n", p->data.param_list.name);
                    else
                        printf("%s: int\n", p->data.param_list.name);
                    p = p->data.param_list.next;
                }
            }
            printf("  Body:\n");
            printAST(node->data.func_decl.body, level + 2);
            if (node->data.func_decl.ret) {
                printf("  Return:\n");
                printAST(node->data.func_decl.ret, level + 2);
            }
            break;
        case NODE_FUNC_CALL:
            printf("FUNC_CALL: %s\n", node->data.func_call.name);
            if (node->data.func_call.args) {
                ASTNode* a = node->data.func_call.args;
                printf("  Args:\n");
                while (a) {
                    printAST(a->data.arg_list.expr, level + 2);
                    a = a->data.arg_list.next;
                }
            }
            break;
        case NODE_RETURN:
            printf("RETURN\n");
            printAST(node->data.return_expr, level + 1);
            break;
    }
}