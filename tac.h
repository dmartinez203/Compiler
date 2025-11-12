#ifndef TAC_H
#define TAC_H

#include "ast.h"
/* NEW: Include symtab.h to get DataType enum */
#include "symtab.h"

/* THREE-ADDRESS CODE (TAC)
 * Intermediate representation between AST and machine code
 * Each instruction has at most 3 operands (result = arg1 op arg2)
 * Makes optimization and code generation easier
 */

/* TAC INSTRUCTION TYPES */
typedef enum {
    /* Integer Ops */
    TAC_ADD,
    TAC_SUB,
    TAC_MUL,
    TAC_DIV,
    TAC_ASSIGN,
    TAC_PRINT,
    TAC_DECL,
    
    /* NEW: Float Ops */
    TAC_FADD,
    TAC_FSUB,
    TAC_FMUL,
    TAC_FDIV,
    TAC_FPRINT,
    TAC_DECL_FLOAT,

    /* NEW: Type Conversion Ops */
    TAC_INT_TO_FLOAT,
    TAC_FLOAT_TO_INT,

    /* Array Ops */
    TAC_DECL_ARRAY, /* Declare array: DECL_ARRAY name, size, type */
    TAC_STORE,      /* Store into array: STORE name, index, value (e.g., arr[i] = x) */
    TAC_LOAD,       /* Load from array: LOAD result, name, index (e.g., x = arr[i]) */
    
    /* Function/Control Flow Ops */
    TAC_LABEL,
    TAC_PARAM,
    TAC_CALL,
    TAC_RETURN,
    TAC_FUNC_BEGIN,
    TAC_FUNC_END,
    
    /* Conditional/Branch Ops */
    TAC_IF_FALSE,   /* Conditional jump: if arg1 is false, goto result */
    TAC_GOTO,       /* Unconditional jump: goto result */
    TAC_EQ,         /* result = arg1 == arg2 */
    TAC_NE,         /* result = arg1 != arg2 */
    TAC_LT,         /* result = arg1 < arg2 */
    TAC_LE,         /* result = arg1 <= arg2 */
    TAC_GT,         /* result = arg1 > arg2 */
    TAC_GE,         /* result = arg1 >= arg2 */
    
    /* Logical/Boolean Ops */
    TAC_AND,        /* result = arg1 && arg2 (logical AND) */
    TAC_OR,         /* result = arg1 || arg2 (logical OR) */
    TAC_NOT         /* result = !arg1 (logical NOT) */
} TACOp;

/* TAC INSTRUCTION STRUCTURE */
typedef struct TACInstr {
    TACOp op;               /* Operation type */
    char* arg1;             /* First operand (if needed) */
    char* arg2;             /* Second operand (for binary ops) */
    char* result;           /* Result/destination */
    int paramCount;         /* For CALL instructions: number of params */
    struct TACInstr* next;  /* Linked list pointer */
} TACInstr;

/* TAC LIST MANAGEMENT */
typedef struct {
    TACInstr* head;    /* First instruction */
    TACInstr* tail;    /* Last instruction (for efficient append) */
    int tempCount;     /* Counter for temporary variables (t0, t1, ...) */
    int labelCount;    /* Counter for labels (L0, L1, ...) */
} TACList;

/* TAC GENERATION FUNCTIONS */
void initTAC();                                                    /* Initialize TAC lists */
char* newTemp();                                                   /* Generate new temp variable */
char* newLabel();                                                  /* Generate new label */
TACInstr* createTAC(TACOp op, char* arg1, char* arg2, char* result); /* Create TAC instruction */
void appendTAC(TACInstr* instr);                                  /* Add instruction to list */
void generateTAC(ASTNode* node);                                  /* Convert AST to TAC */
char* generateTACExpr(ASTNode* node);                             /* Generate TAC for expression */
DataType getTypeOfASTNode(ASTNode* node);                          /* NEW: Helper to find expression type */

/* TAC OPTIMIZATION AND OUTPUT */
void printTAC();                                                   /* Display unoptimized TAC */
void optimizeTAC();                                                /* Apply optimizations */
void printOptimizedTAC();                                          /* Display optimized TAC */

#endif

