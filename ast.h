#ifndef AST_H
#define AST_H

/* ABSTRACT SYNTAX TREE (AST)
 * The AST is an intermediate representation of the program structure
 * It represents the hierarchical syntax of the source code
 * Each node represents a construct in the language
 */

/* NODE TYPES - Different kinds of AST nodes in our language */
typedef enum {
    NODE_NUM,           /* Numeric literal (e.g., 42) */
    NODE_VAR,           /* Variable reference (e.g., x) */
    NODE_BINOP,         /* Binary operation (e.g., x + y) */
    NODE_DECL,          /* Variable declaration (e.g., int x) */
    NODE_ASSIGN,        /* Assignment statement (e.g., x = 10) */
    NODE_PRINT,         /* Print statement (e.g., print(x)) */
    NODE_STMT_LIST,     /* List of statements (program structure) */
    NODE_ARRAY_DECL,    /* Array declaration (e.g., int arr[10]) */
    NODE_ARRAY_ASSIGN,  /* Array element assignment (e.g., arr[0] = 5) */
    NODE_ARRAY_ACCESS   /* Array element access (e.g., arr[0]) */
    ,NODE_FUNC_DECL     /* Function declaration */
    ,NODE_FUNC_CALL     /* Function call expression */
    ,NODE_PARAM_LIST    /* Parameter list for function */
    ,NODE_ARG_LIST      /* Argument list for function call */
    ,NODE_RETURN        /* Return statement */
} NodeType;

/* AST NODE STRUCTURE
 * Uses a union to efficiently store different node data
 * Only the relevant fields for each node type are used
 */
/* AST NODE STRUCTURE
 * Uses a union to efficiently store different node data
 * Only the relevant fields for each node type are used
 */
typedef struct ASTNode {
    NodeType type;  /* Identifies what kind of node this is */
    
    /* Union allows same memory to store different data types */
    union {
        /* Literal number value (NODE_NUM) */
        int num;
        
        /* Variable or declaration name (NODE_VAR, NODE_DECL) */
        char* name;
        
        /* Binary operation structure (NODE_BINOP) */
        struct {
            char op;                    /* Operator character ('+') */
            struct ASTNode* left;       /* Left operand */
            struct ASTNode* right;      /* Right operand */
        } binop;
        
        /* Assignment structure (NODE_ASSIGN) */
        struct {
            char* var;                  /* Variable being assigned to */
            struct ASTNode* value;      /* Expression being assigned */
        } assign;
        
        /* --- Array-related structures --- */

        /* Array declaration (NODE_ARRAY_DECL) */
        struct {
            char* name;                 /* Name of the array */
            int size;                   /* Size of the array */
        } array_decl;

        /* Array element assignment (NODE_ARRAY_ASSIGN) */
        struct {
            char* name;                 /* Name of the array */
            struct ASTNode* index;      /* Index expression */
            struct ASTNode* value;      /* Value expression to assign */
        } array_assign;

        /* Array element access (NODE_ARRAY_ACCESS) */
        struct {
            char* name;                 /* Name of the array */
            struct ASTNode* index;      /* Index expression */
        } array_access;

        /* --- End of array structures --- */
        
        /* Print expression (NODE_PRINT) */
        struct ASTNode* expr;
        
        /* Statement list structure (NODE_STMT_LIST) */
        struct {
            struct ASTNode* stmt;       /* Current statement */
            struct ASTNode* next;       /* Rest of the list */
        } stmtlist;
        /* Function declaration (NODE_FUNC_DECL) */
        struct {
            char* name;               /* Function name */
            struct ASTNode* params;   /* Parameter list */
            struct ASTNode* body;     /* Body statement list */
            struct ASTNode* ret;      /* Return expression (wrapped in return node) */
        } func_decl;

        /* Function call (NODE_FUNC_CALL) */
        struct {
            char* name;               /* Function name */
            struct ASTNode* args;     /* Argument list */
        } func_call;

        /* Parameter list node (linked list) */
        struct {
            char* name;
            struct ASTNode* next;
        } param_list;

        /* Argument list node (linked list) */
        struct {
            struct ASTNode* expr;
            struct ASTNode* next;
        } arg_list;

        /* Return statement (NODE_RETURN) */
        struct ASTNode* return_expr;
    } data;
} ASTNode;

/* AST CONSTRUCTION FUNCTIONS
 * These functions are called by the parser to build the tree
 */
ASTNode* createNum(int value);                                   /* Create number node */
ASTNode* createVar(char* name);                                  /* Create variable node */
ASTNode* createBinOp(char op, ASTNode* left, ASTNode* right);   /* Create binary op node */
ASTNode* createDecl(char* name);                                 /* Create declaration node */
/* ASTNode* create*/
ASTNode* createAssign(char* var, ASTNode* value);               /* Create assignment node */
ASTNode* createPrint(ASTNode* expr);                            /* Create print node */
ASTNode* createStmtList(ASTNode* stmt1, ASTNode* stmt2);        /* Create statement list */
ASTNode* createArrayDecl(char* name, int size);
ASTNode* createArrayAssign(char* name, ASTNode* index, ASTNode* value);
ASTNode* createArrayAccess(char* name, ASTNode* index);
ASTNode* createFuncDecl(char* name, ASTNode* params, ASTNode* body, ASTNode* ret);
ASTNode* createFuncCall(char* name, ASTNode* args);
ASTNode* createParamList(char* name);
ASTNode* appendParam(ASTNode* list, char* name);
ASTNode* createArgList(ASTNode* expr);
ASTNode* appendArg(ASTNode* list, ASTNode* expr);
ASTNode* createReturn(ASTNode* expr);
/* Note: For minimal function support we currently accept only no-argument functions.
    Param/arg list helpers remain for future extension. */
/* AST DISPLAY FUNCTION */
void printAST(ASTNode* node, int level);                        /* Pretty-print the AST */

#endif