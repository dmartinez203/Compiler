/* AST IMPLEMENTATION
 * Functions to create and manipulate Abstract Syntax Tree nodes
 * The AST is built during parsing and used for all subsequent phases
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* NEW: Helper function to print data types */
static const char* typeToString(DataType type) {
    switch (type) {
        case TYPE_INT:   return "int";
        case TYPE_FLOAT: return "float";
        default:         return "unknown";
    }
}

/* Create a number literal node */
ASTNode* createNum(int value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_NUM;
    node->data.num = value;  /* Store the integer value */
    return node;
}

/* NEW: Create a float literal node */
ASTNode* createFloatNum(double value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_FLOAT_NUM;
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

/* UPDATED: Create a variable declaration node */
ASTNode* createDecl(DataType type, char* name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_DECL;
    node->data.decl.type = type; /* Store type */
    node->data.decl.name = strdup(name);
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

/* UPDATED: Create an array declaration node */
ASTNode* createArrayDecl(DataType type, char* name, int size) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_ARRAY_DECL;
    node->data.array_decl.type = type; /* Store type */
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
ASTNode* createParamList(char* name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_PARAM_LIST;
    node->data.param_list.name = strdup(name);
    node->data.param_list.next = NULL;
    return node;
}

ASTNode* appendParam(ASTNode* list, char* name) {
    if (!list) return createParamList(name);
    ASTNode* cur = list;
    while (cur->data.param_list.next) cur = cur->data.param_list.next;
    ASTNode* n = createParamList(name);
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

/* Create an if statement node */
ASTNode* createIf(ASTNode* condition, ASTNode* then_branch, ASTNode* else_branch) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_IF;
    node->data.if_stmt.condition = condition;
    node->data.if_stmt.then_branch = then_branch;
    node->data.if_stmt.else_branch = else_branch;  /* Can be NULL for simple if */
    return node;
}

/* Create a relational operation node */
ASTNode* createRelOp(char* op, ASTNode* left, ASTNode* right) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_RELOP;
    node->data.relop.op = strdup(op);
    node->data.relop.left = left;
    node->data.relop.right = right;
    return node;
}

/* Create a logical operation node */
ASTNode* createLogicOp(char* op, ASTNode* left, ASTNode* right) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_LOGICOP;
    node->data.logicop.op = strdup(op);
    node->data.logicop.left = left;
    node->data.logicop.right = right;
    return node;
}

/* Create a unary operation node */
ASTNode* createUnaryOp(char* op, ASTNode* operand) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_UNARYOP;
    node->data.unaryop.op = strdup(op);
    node->data.unaryop.operand = operand;
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
        /* NEW: Print float literals */
        case NODE_FLOAT_NUM:
            printf("FLOAT: %f\n", node->data.fnum);
            break;
        case NODE_VAR:
            printf("VAR: %s\n", node->data.name);
            break;
        case NODE_BINOP:
            printf("BINOP: %c\n", node->data.binop.op);
            printAST(node->data.binop.left, level + 1);
            printAST(node->data.binop.right, level + 1);
            break;
        /* UPDATED: Print declaration type */
        case NODE_DECL:
            printf("DECL: %s (%s)\n", node->data.decl.name, typeToString(node->data.decl.type));
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
        
        /* UPDATED: Print array declaration type */
        case NODE_ARRAY_DECL:
            printf("ARRAY_DECL: %s %s[%d]\n", 
                typeToString(node->data.array_decl.type), 
                node->data.array_decl.name, 
                node->data.array_decl.size);
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
                    printf("%s\n", p->data.param_list.name);
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
        case NODE_IF:
            printf("IF\n");
            for (int i = 0; i < level + 1; i++) printf("  ");
            printf("Condition:\n");
            printAST(node->data.if_stmt.condition, level + 2);
            for (int i = 0; i < level + 1; i++) printf("  ");
            printf("Then:\n");
            printAST(node->data.if_stmt.then_branch, level + 2);
            if (node->data.if_stmt.else_branch) {
                for (int i = 0; i < level + 1; i++) printf("  ");
                printf("Else:\n");
                printAST(node->data.if_stmt.else_branch, level + 2);
            }
            break;
        case NODE_RELOP:
            printf("RELOP: %s\n", node->data.relop.op);
            printAST(node->data.relop.left, level + 1);
            printAST(node->data.relop.right, level + 1);
            break;
        case NODE_LOGICOP:
            printf("LOGICOP: %s\n", node->data.logicop.op);
            printAST(node->data.logicop.left, level + 1);
            printAST(node->data.logicop.right, level + 1);
            break;
        case NODE_UNARYOP:
            printf("UNARYOP: %s\n", node->data.unaryop.op);
            printAST(node->data.unaryop.operand, level + 1);
            break;
        default:
            fprintf(stderr, "Unknown AST node type: %d\n", node->type);
            break;
    }
}

