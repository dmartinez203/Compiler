#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "tac.h"
#include "symtab.h" /* NEW: Include for type checking */

TACList tacList;
TACList optimizedList;

void initTAC() {
    tacList.head = NULL;
    tacList.tail = NULL;
    tacList.tempCount = 0;
    tacList.labelCount = 0;
    optimizedList.head = NULL;
    optimizedList.tail = NULL;
}

char* newTemp() {
    char* temp = malloc(10);
    sprintf(temp, "t%d", tacList.tempCount++);
    return temp;
}

char* newLabel() {
    char* label = malloc(10);
    sprintf(label, "L%d", tacList.labelCount++);
    return label;
}

TACInstr* createTAC(TACOp op, char* arg1, char* arg2, char* result) {
    TACInstr* instr = malloc(sizeof(TACInstr));
    instr->op = op;
    instr->arg1 = arg1 ? strdup(arg1) : NULL;
    instr->arg2 = arg2 ? strdup(arg2) : NULL;
    instr->result = result ? strdup(result) : NULL;
    instr->paramCount = 0;
    instr->next = NULL;
    return instr;
}

void appendTAC(TACInstr* instr) {
    if (!tacList.head) {
        tacList.head = tacList.tail = instr;
    } else {
        tacList.tail->next = instr;
        tacList.tail = instr;
    }
}

void appendOptimizedTAC(TACInstr* instr) {
    if (!optimizedList.head) {
        optimizedList.head = optimizedList.tail = instr;
    } else {
        optimizedList.tail->next = instr;
        optimizedList.tail = instr;
    }
}

/* NEW: Helper function to determine the type of an AST expression */
DataType getTypeOfASTNode(ASTNode* node) {
    if (!node) return TYPE_INT; /* Default, though should not happen */

    switch(node->type) {
        case NODE_NUM:
            return TYPE_INT;
        case NODE_FLOAT_NUM:
            return TYPE_FLOAT;
        case NODE_VAR:
            return getVarType(node->data.name);
        case NODE_BINOP: {
            DataType leftType = getTypeOfASTNode(node->data.binop.left);
            DataType rightType = getTypeOfASTNode(node->data.binop.right);
            /* Promote to float if either operand is float */
            if (leftType == TYPE_FLOAT || rightType == TYPE_FLOAT) {
                return TYPE_FLOAT;
            }
            return TYPE_INT;
        }
        case NODE_ARRAY_ACCESS:
            return getVarType(node->data.array_access.name); /* Returns element type */
        case NODE_FUNC_CALL:
            /* TODO: Need to store function return types in symtab */
            /* For now, assume functions return int */
            return TYPE_INT; 
        default:
            return TYPE_INT;
    }
}


char* generateTACExpr(ASTNode* node) {
    if (!node) return NULL;
    
    switch(node->type) {
        case NODE_NUM: {
            char* temp = malloc(20);
            sprintf(temp, "%d", node->data.num);
            return temp;
        }
        /* NEW: Handle float literals */
        case NODE_FLOAT_NUM: {
            char* temp = malloc(30);
            sprintf(temp, "%f", node->data.fnum);
            return temp;
        }
        
        case NODE_VAR:
            return strdup(node->data.name);
        
        case NODE_BINOP: {
            /* NEW: Type-aware expression generation */
            DataType leftType = getTypeOfASTNode(node->data.binop.left);
            DataType rightType = getTypeOfASTNode(node->data.binop.right);
            
            char* left = generateTACExpr(node->data.binop.left);
            char* right = generateTACExpr(node->data.binop.right);
            char* temp = newTemp();
            
            bool isFloat = (leftType == TYPE_FLOAT || rightType == TYPE_FLOAT);

            /* Insert type conversion TAC if types are mixed */
            if (isFloat) {
                if (leftType == TYPE_INT) {
                    char* newLeft = newTemp();
                    appendTAC(createTAC(TAC_INT_TO_FLOAT, left, NULL, newLeft));
                    left = newLeft;
                }
                if (rightType == TYPE_INT) {
                    char* newRight = newTemp();
                    appendTAC(createTAC(TAC_INT_TO_FLOAT, right, NULL, newRight));
                    right = newRight;
                }

                /* Emit float operations */
                switch(node->data.binop.op) {
                    case '+': appendTAC(createTAC(TAC_FADD, left, right, temp)); break;
                    case '-': appendTAC(createTAC(TAC_FSUB, left, right, temp)); break;
                    case '*': appendTAC(createTAC(TAC_FMUL, left, right, temp)); break;
                    case '/': appendTAC(createTAC(TAC_FDIV, left, right, temp)); break;
                }
            } else {
                /* Emit integer operations (as before) */
                switch(node->data.binop.op) {
                    case '+': appendTAC(createTAC(TAC_ADD, left, right, temp)); break;
                    case '-': appendTAC(createTAC(TAC_SUB, left, right, temp)); break;
                    case '*': appendTAC(createTAC(TAC_MUL, left, right, temp)); break;
                    case '/': appendTAC(createTAC(TAC_DIV, left, right, temp)); break;
                }
            }
            return temp;
        }
        
        case NODE_ARRAY_ACCESS: {
            char* index = generateTACExpr(node->data.array_access.index);
            char* temp = newTemp();
            /* Note: This assumes array stores elements matching its declared type.
               CodeGen will need to check type to use lw or l.s */
            appendTAC(createTAC(TAC_LOAD, node->data.array_access.name, index, temp));
            return temp;
        }
        case NODE_FUNC_CALL: {
            /* Generate TAC for each argument (if any) */
            int paramCount = 0;
            ASTNode* a = node->data.func_call.args;
            while (a) {
                char* argVal = generateTACExpr(a->data.arg_list.expr);
                appendTAC(createTAC(TAC_PARAM, argVal, NULL, NULL));
                paramCount++;
                a = a->data.arg_list.next;
            }
            /* Emit CALL and return temp holding result */
            char* temp = newTemp();
            TACInstr* callInstr = createTAC(TAC_CALL, node->data.func_call.name, NULL, temp);
            callInstr->paramCount = paramCount;
            appendTAC(callInstr);
            return temp;
        }
        
        case NODE_RELOP: {
            /* Generate relational comparison */
            char* left = generateTACExpr(node->data.relop.left);
            char* right = generateTACExpr(node->data.relop.right);
            char* temp = newTemp();
            
            /* Map operator string to TAC operation */
            TACOp op;
            if (strcmp(node->data.relop.op, "==") == 0) op = TAC_EQ;
            else if (strcmp(node->data.relop.op, "!=") == 0) op = TAC_NE;
            else if (strcmp(node->data.relop.op, "<") == 0) op = TAC_LT;
            else if (strcmp(node->data.relop.op, "<=") == 0) op = TAC_LE;
            else if (strcmp(node->data.relop.op, ">") == 0) op = TAC_GT;
            else if (strcmp(node->data.relop.op, ">=") == 0) op = TAC_GE;
            else op = TAC_EQ; /* Default fallback */
            
            appendTAC(createTAC(op, left, right, temp));
            return temp;
        }
        
        case NODE_LOGICOP: {
            /* Generate logical operation (AND, OR) */
            char* left = generateTACExpr(node->data.logicop.left);
            char* right = generateTACExpr(node->data.logicop.right);
            char* temp = newTemp();
            
            TACOp op;
            if (strcmp(node->data.logicop.op, "&&") == 0) op = TAC_AND;
            else if (strcmp(node->data.logicop.op, "||") == 0) op = TAC_OR;
            else op = TAC_AND; /* Default fallback */
            
            appendTAC(createTAC(op, left, right, temp));
            return temp;
        }
        
        case NODE_UNARYOP: {
            /* Generate unary operation (NOT) */
            char* operand = generateTACExpr(node->data.unaryop.operand);
            char* temp = newTemp();
            
            if (strcmp(node->data.unaryop.op, "!") == 0) {
                appendTAC(createTAC(TAC_NOT, operand, NULL, temp));
            }
            return temp;
        }
        
        default:
            return NULL;
    }
}

void generateTAC(ASTNode* node) {
    if (!node) return;
    
    switch(node->type) {
        case NODE_DECL:
            /* NEW: Add to symtab with type and emit correct DECL */
            if (node->data.decl.type == TYPE_FLOAT) {
                addVar(node->data.decl.name, TYPE_FLOAT);
                appendTAC(createTAC(TAC_DECL_FLOAT, NULL, NULL, node->data.decl.name));
            } else {
                addVar(node->data.decl.name, TYPE_INT);
                appendTAC(createTAC(TAC_DECL, NULL, NULL, node->data.decl.name));
            }
            break;

        case NODE_ARRAY_DECL: {
            char sizeStr[20];
            sprintf(sizeStr, "%d", node->data.array_decl.size);
            /* NEW: Add array to symtab with type */
            addArray(node->data.array_decl.name, node->data.array_decl.type, node->data.array_decl.size);
            /* Note: We still use one DECL_ARRAY op; codegen will check type */
            appendTAC(createTAC(TAC_DECL_ARRAY, strdup(sizeStr), NULL, node->data.array_decl.name));
            break;
        }
            
        case NODE_ASSIGN: {
            /* NEW: Handle type conversions on assignment */
            DataType varType = getVarType(node->data.assign.var);
            ASTNode* exprNode = node->data.assign.value;
            DataType exprType = getTypeOfASTNode(exprNode);
            char* expr = generateTACExpr(exprNode);

            if (varType == TYPE_FLOAT && exprType == TYPE_INT) {
                char* temp = newTemp();
                appendTAC(createTAC(TAC_INT_TO_FLOAT, expr, NULL, temp));
                expr = temp;
            } else if (varType == TYPE_INT && exprType == TYPE_FLOAT) {
                char* temp = newTemp();
                appendTAC(createTAC(TAC_FLOAT_TO_INT, expr, NULL, temp));
                expr = temp;
            }

            appendTAC(createTAC(TAC_ASSIGN, expr, NULL, node->data.assign.var));
            break;
        }

        case NODE_ARRAY_ASSIGN: {
            char* index = generateTACExpr(node->data.array_assign.index);
            char* value = generateTACExpr(node->data.array_assign.value);
            
            /* NEW: Handle type conversions on array assignment */
            DataType arrType = getVarType(node->data.array_assign.name);
            DataType valType = getTypeOfASTNode(node->data.array_assign.value);

            if (arrType == TYPE_FLOAT && valType == TYPE_INT) {
                char* temp = newTemp();
                appendTAC(createTAC(TAC_INT_TO_FLOAT, value, NULL, temp));
                value = temp;
            } else if (arrType == TYPE_INT && valType == TYPE_FLOAT) {
                char* temp = newTemp();
                appendTAC(createTAC(TAC_FLOAT_TO_INT, value, NULL, temp));
                value = temp;
            }

            appendTAC(createTAC(TAC_STORE, index, value, node->data.array_assign.name));
            break;
        }
        
        case NODE_PRINT: {
            /* NEW: Check type to emit PRINT or FPRINT */
            char* expr = generateTACExpr(node->data.expr);
            DataType exprType = getTypeOfASTNode(node->data.expr);
            
            if (exprType == TYPE_FLOAT) {
                appendTAC(createTAC(TAC_FPRINT, expr, NULL, NULL));
            } else {
                appendTAC(createTAC(TAC_PRINT, expr, NULL, NULL));
            }
            break;
        }
        
        case NODE_WRITE: {
            char* expr = generateTACExpr(node->data.expr);
            appendTAC(createTAC(TAC_WRITE, expr, NULL, NULL));
            break;
        }
        
        case NODE_WRITELN: {
            appendTAC(createTAC(TAC_WRITELN, NULL, NULL, NULL));
            break;
        }
        
        case NODE_STMT_LIST:
            generateTAC(node->data.stmtlist.stmt);
            generateTAC(node->data.stmtlist.next);
            break;
        case NODE_FUNC_DECL: {
            /* Mark function begin and label */
            appendTAC(createTAC(TAC_FUNC_BEGIN, NULL, NULL, node->data.func_decl.name));
            appendTAC(createTAC(TAC_LABEL, NULL, NULL, node->data.func_decl.name));
            
            /* Process parameters - add them to symbol table as local variables */
            ASTNode* param = node->data.func_decl.params;
            while (param) {
                addVar(param->data.param_list.name, TYPE_INT);
                appendTAC(createTAC(TAC_DECL, NULL, NULL, param->data.param_list.name));
                param = param->data.param_list.next;
            }
            
            /* Generate TAC for the body */
            generateTAC(node->data.func_decl.body);
            appendTAC(createTAC(TAC_FUNC_END, NULL, NULL, node->data.func_decl.name));
            break;
        }
        case NODE_FUNC_CALL: {
            /* Generate TAC for call with no args (minimal) */
            int paramCount = 0;
            ASTNode* a = node->data.func_call.args;
            while (a) {
                char* argVal = generateTACExpr(a->data.arg_list.expr);
                appendTAC(createTAC(TAC_PARAM, argVal, NULL, NULL));
                paramCount++;
                a = a->data.arg_list.next;
            }
            char* temp = newTemp();
            TACInstr* callInstr = createTAC(TAC_CALL, node->data.func_call.name, NULL, temp);
            callInstr->paramCount = paramCount;
            appendTAC(callInstr);
            /* Return value represented by temp */
            break;
        }
        case NODE_RETURN: {
            if (node->data.return_expr) {
                char* ret = generateTACExpr(node->data.return_expr);
                /* TODO: Add type check for function return */
                appendTAC(createTAC(TAC_RETURN, ret, NULL, NULL));
            } else {
                appendTAC(createTAC(TAC_RETURN, NULL, NULL, NULL));
            }
            break;
        }
        
        case NODE_IF: {
            /* Generate TAC for if statement with proper control flow */
            char* condTemp = generateTACExpr(node->data.if_stmt.condition);
            
            if (node->data.if_stmt.else_branch) {
                /* If-else case */
                char* elseLabel = newLabel();
                char* endLabel = newLabel();
                
                /* If condition is false, jump to else */
                appendTAC(createTAC(TAC_IF_FALSE, condTemp, NULL, elseLabel));
                
                /* Generate then branch */
                generateTAC(node->data.if_stmt.then_branch);
                
                /* Jump to end after then branch */
                appendTAC(createTAC(TAC_GOTO, NULL, NULL, endLabel));
                
                /* Else branch */
                appendTAC(createTAC(TAC_LABEL, NULL, NULL, elseLabel));
                generateTAC(node->data.if_stmt.else_branch);
                
                /* End label */
                appendTAC(createTAC(TAC_LABEL, NULL, NULL, endLabel));
            } else {
                /* Simple if case (no else) */
                char* endLabel = newLabel();
                
                /* If condition is false, jump to end */
                appendTAC(createTAC(TAC_IF_FALSE, condTemp, NULL, endLabel));
                
                /* Generate then branch */
                generateTAC(node->data.if_stmt.then_branch);
                
                /* End label */
                appendTAC(createTAC(TAC_LABEL, NULL, NULL, endLabel));
            }
            break;
        }
        
        case NODE_WHILE: {
            /* Generate TAC for while loop with proper control flow */
            char* startLabel = newLabel();
            char* endLabel = newLabel();
            
            /* Start label - beginning of loop */
            appendTAC(createTAC(TAC_LABEL, NULL, NULL, startLabel));
            
            /* Evaluate condition */
            char* condTemp = generateTACExpr(node->data.while_stmt.condition);
            
            /* If condition is false, jump to end */
            appendTAC(createTAC(TAC_IF_FALSE, condTemp, NULL, endLabel));
            
            /* Generate loop body */
            generateTAC(node->data.while_stmt.body);
            
            /* Jump back to start */
            appendTAC(createTAC(TAC_GOTO, NULL, NULL, startLabel));
            
            /* End label */
            appendTAC(createTAC(TAC_LABEL, NULL, NULL, endLabel));
            break;
        }
            
        default:
            break;
    }
}

void printTAC() {
    printf("Unoptimized TAC Instructions:\n");
    printf("─────────────────────────────\n");
    TACInstr* curr = tacList.head;
    int lineNum = 1;
    while (curr) {
        printf("%2d: ", lineNum++);
        switch(curr->op) {
            case TAC_DECL:
                printf("DECL %s", curr->result);
                printf("           // Declare int '%s'\n", curr->result);
                break;
            /* NEW: Print float ops */
            case TAC_DECL_FLOAT:
                printf("DECL_FLOAT %s", curr->result);
                printf("     // Declare float '%s'\n", curr->result);
                break;
            case TAC_ADD:
                printf("%s = %s + %s", curr->result, curr->arg1, curr->arg2);
                printf("     // Add (int)\n");
                break;
            case TAC_FADD:
                printf("%s = %s + %s", curr->result, curr->arg1, curr->arg2);
                printf("     // Add (float)\n");
                break;
            case TAC_SUB:
                printf("%s = %s - %s", curr->result, curr->arg1, curr->arg2);
                printf("     // Subtract (int)\n");
                break;
            case TAC_FSUB:
                printf("%s = %s - %s", curr->result, curr->arg1, curr->arg2);
                printf("     // Subtract (float)\n");
                break;
            case TAC_MUL:
                printf("%s = %s * %s", curr->result, curr->arg1, curr->arg2);
                printf("     // Multiply (int)\n");
                break;
            case TAC_FMUL:
                printf("%s = %s * %s", curr->result, curr->arg1, curr->arg2);
                printf("     // Multiply (float)\n");
                break;
            case TAC_DIV:
                printf("%s = %s / %s", curr->result, curr->arg1, curr->arg2);
                printf("     // Divide (int)\n");
                break;
            case TAC_FDIV:
                printf("%s = %s / %s", curr->result, curr->arg1, curr->arg2);
                printf("     // Divide (float)\n");
                break;
            case TAC_ASSIGN:
                printf("%s = %s", curr->result, curr->arg1);
                printf("           // Assign value to %s\n", curr->result);
                break;
            case TAC_PRINT:
                printf("PRINT %s", curr->arg1);
                printf("           // Output int value of %s\n", curr->arg1);
                break;
            case TAC_WRITE:
                printf("WRITE %s", curr->arg1);
                printf("           // Output value of %s (no newline)\n", curr->arg1);
                break;
            case TAC_WRITELN:
                printf("WRITELN");
                printf("           // Output newline\n");
                break;
            case TAC_FPRINT:
                printf("FPRINT %s", curr->arg1);
                printf("          // Output float value of %s\n", curr->arg1);
                break;
            case TAC_INT_TO_FLOAT:
                printf("%s = (float) %s", curr->result, curr->arg1);
                printf(" // Convert int to float\n");
                break;
            case TAC_FLOAT_TO_INT:
                printf("%s = (int) %s", curr->result, curr->arg1);
                printf("   // Convert float to int\n");
                break;
            /* End new print cases */
            case TAC_DECL_ARRAY:
                printf("DECL_ARRAY %s[%s]", curr->result, curr->arg1);
                printf("   // Declare array '%s' of size %s\n", curr->result, curr->arg1);
                break;
            case TAC_LABEL:
                printf("LABEL %s\n", curr->result);
                break;
            case TAC_PARAM:
                printf("PARAM %s\n", curr->arg1);
                break;
            case TAC_CALL:
                printf("%s = CALL %s, %d\n", curr->result, curr->arg1, curr->paramCount);
                break;
            case TAC_RETURN:
                if (curr->arg1) printf("RETURN %s\n", curr->arg1);
                else printf("RETURN\n");
                break;
            case TAC_FUNC_BEGIN:
                printf("FUNC_BEGIN %s\n", curr->result);
                break;
            case TAC_FUNC_END:
                printf("FUNC_END %s\n", curr->result);
                break;
            case TAC_STORE:
                printf("%s[%s] = %s", curr->result, curr->arg1, curr->arg2);
                printf("       // Store value in array '%s'\n", curr->result);
                break;
            case TAC_LOAD:
                printf("%s = %s[%s]", curr->result, curr->arg1, curr->arg2);
                printf("       // Load value from array '%s'\n", curr->arg1);
                break;
            case TAC_EQ:
                printf("%s = %s == %s", curr->result, curr->arg1, curr->arg2);
                printf("    // Relational: equal\n");
                break;
            case TAC_NE:
                printf("%s = %s != %s", curr->result, curr->arg1, curr->arg2);
                printf("    // Relational: not equal\n");
                break;
            case TAC_LT:
                printf("%s = %s < %s", curr->result, curr->arg1, curr->arg2);
                printf("     // Relational: less than\n");
                break;
            case TAC_LE:
                printf("%s = %s <= %s", curr->result, curr->arg1, curr->arg2);
                printf("    // Relational: less or equal\n");
                break;
            case TAC_GT:
                printf("%s = %s > %s", curr->result, curr->arg1, curr->arg2);
                printf("     // Relational: greater than\n");
                break;
            case TAC_GE:
                printf("%s = %s >= %s", curr->result, curr->arg1, curr->arg2);
                printf("    // Relational: greater or equal\n");
                break;
            case TAC_IF_FALSE:
                printf("IF_FALSE %s GOTO %s", curr->arg1, curr->result);
                printf(" // Jump if false\n");
                break;
            case TAC_GOTO:
                printf("GOTO %s", curr->result);
                printf("           // Unconditional jump\n");
                break;
            case TAC_AND:
                printf("%s = %s && %s", curr->result, curr->arg1, curr->arg2);
                printf("    // Logical: AND\n");
                break;
            case TAC_OR:
                printf("%s = %s || %s", curr->result, curr->arg1, curr->arg2);
                printf("     // Logical: OR\n");
                break;
            case TAC_NOT:
                printf("%s = !%s", curr->result, curr->arg1);
                printf("         // Logical: NOT\n");
                break;
            default:
                printf("Unknown Op\n");
                break;
        }
        curr = curr->next;
    }
}

// Simple optimization: constant folding and copy propagation
void optimizeTAC() {
    TACInstr* curr = tacList.head;
    
    // Copy propagation table
    typedef struct {
        char* var;
        char* value;
    } VarValue;

    /* Use a dynamically-sized array to avoid stack overflow when
       the input produces many temporaries/assignments. */
    int valueCount = 0;
    int valuesCapacity = 256;
    VarValue* values = malloc(sizeof(VarValue) * valuesCapacity);
    if (!values) {
        fprintf(stderr, "Out of memory in optimizeTAC\n");
        exit(1);
    }
    
    while (curr) {
        TACInstr* newInstr = NULL;
        
    switch(curr->op) {
            case TAC_DECL:
                newInstr = createTAC(TAC_DECL, NULL, NULL, curr->result);
                break;
            
            /* NEW: Pass-through for float ops (no optimization) */
            case TAC_DECL_FLOAT:
                newInstr = createTAC(TAC_DECL_FLOAT, NULL, NULL, curr->result);
                break;
            case TAC_FADD:
                newInstr = createTAC(TAC_FADD, curr->arg1, curr->arg2, curr->result);
                break;
            case TAC_FSUB:
                newInstr = createTAC(TAC_FSUB, curr->arg1, curr->arg2, curr->result);
                break;
            case TAC_FMUL:
                newInstr = createTAC(TAC_FMUL, curr->arg1, curr->arg2, curr->result);
                break;
            case TAC_FDIV:
                newInstr = createTAC(TAC_FDIV, curr->arg1, curr->arg2, curr->result);
                break;
            case TAC_FPRINT:
                newInstr = createTAC(TAC_FPRINT, curr->arg1, NULL, NULL);
                break;
            case TAC_INT_TO_FLOAT:
                newInstr = createTAC(TAC_INT_TO_FLOAT, curr->arg1, NULL, curr->result);
                break;
            case TAC_FLOAT_TO_INT:
                newInstr = createTAC(TAC_FLOAT_TO_INT, curr->arg1, NULL, curr->result);
                break;
            /* End float pass-through */

            case TAC_ADD: {
                char* left = curr->arg1;
                char* right = curr->arg2;
                
                for (int i = valueCount - 1; i >= 0; i--) {
                    if (strcmp(values[i].var, left) == 0) {
                        left = values[i].value;
                        break;
                    }
                }
                for (int i = valueCount - 1; i >= 0; i--) {
                    if (strcmp(values[i].var, right) == 0) {
                        right = values[i].value;
                        break;
                    }
                }
                
                if (isdigit(left[0]) && isdigit(right[0])) {
                    int result = atoi(left) + atoi(right);
                    char* resultStr = malloc(20);
                    sprintf(resultStr, "%d", result);
                    
                    if (valueCount >= valuesCapacity) {
                        valuesCapacity *= 2;
                        values = realloc(values, sizeof(VarValue) * valuesCapacity);
                        if (!values) {
                            fprintf(stderr, "Out of memory in optimizeTAC (realloc)\n");
                            exit(1);
                        }
                    }
                    values[valueCount].var = strdup(curr->result);
                    values[valueCount].value = resultStr;
                    valueCount++;
                    
                    newInstr = createTAC(TAC_ASSIGN, resultStr, NULL, curr->result);
                } else {
                    newInstr = createTAC(TAC_ADD, left, right, curr->result);
                }
                break;
            }

            case TAC_SUB: {
                char* left = curr->arg1;
                char* right = curr->arg2;
                
                for (int i = valueCount - 1; i >= 0; i--) {
                    if (strcmp(values[i].var, left) == 0) {
                        left = values[i].value;
                        break;
                    }
                }
                for (int i = valueCount - 1; i >= 0; i--) {
                    if (strcmp(values[i].var, right) == 0) {
                        right = values[i].value;
                        break;
                    }
                }
                
                if (isdigit(left[0]) && isdigit(right[0])) {
                    int result = atoi(left) - atoi(right);
                    char* resultStr = malloc(20);
                    sprintf(resultStr, "%d", result);
                    
                    if (valueCount >= valuesCapacity) {
                        valuesCapacity *= 2;
                        values = realloc(values, sizeof(VarValue) * valuesCapacity);
                        if (!values) {
                            fprintf(stderr, "Out of memory in optimizeTAC (realloc)\n");
                            exit(1);
                        }
                    }
                    values[valueCount].var = strdup(curr->result);
                    values[valueCount].value = resultStr;
                    valueCount++;
                    
                    newInstr = createTAC(TAC_ASSIGN, resultStr, NULL, curr->result);
                } else {
                    newInstr = createTAC(TAC_SUB, left, right, curr->result);
                }
                break;
            }
            case TAC_MUL: {
                char* left = curr->arg1;
                char* right = curr->arg2;

                for (int i = valueCount - 1; i >= 0; i--) {
                    if (strcmp(values[i].var, left) == 0) {
                        left = values[i].value;
                        break;
                    }
                }
                for (int i = valueCount - 1; i >= 0; i--) {
                    if (strcmp(values[i].var, right) == 0) {
                        right = values[i].value;
                        break;
                    }
                }

                if (isdigit(left[0]) && isdigit(right[0])) {
                    int result = atoi(left) * atoi(right);
                    char* resultStr = malloc(20);
                    sprintf(resultStr, "%d", result);

                    if (valueCount >= valuesCapacity) {
                        valuesCapacity *= 2;
                        values = realloc(values, sizeof(VarValue) * valuesCapacity);
                        if (!values) {
                            fprintf(stderr, "Out of memory in optimizeTAC (realloc)\n");
                            exit(1);
                        }
                    }
                    values[valueCount].var = strdup(curr->result);
                    values[valueCount].value = resultStr;
                    valueCount++;

                    newInstr = createTAC(TAC_ASSIGN, resultStr, NULL, curr->result);
                } else {
                    newInstr = createTAC(TAC_MUL, left, right, curr->result);
                }
                break;
            }
            case TAC_DIV: {
                char* left = curr->arg1;
                char* right = curr->arg2;

                for (int i = valueCount - 1; i >= 0; i--) {
                    if (strcmp(values[i].var, left) == 0) {
                        left = values[i].value;
                        break;
                    }
                }
                for (int i = valueCount - 1; i >= 0; i--) {
                    if (strcmp(values[i].var, right) == 0) {
                        right = values[i].value;
                        break;
                    }
                }

                // Only fold if both are numeric and right != 0
                if (isdigit(left[0]) && isdigit(right[0]) && atoi(right) != 0) {
                    int result = atoi(left) / atoi(right);
                    char* resultStr = malloc(20);
                    sprintf(resultStr, "%d", result);

                    values[valueCount].var = strdup(curr->result);
                    values[valueCount].value = resultStr;
                    valueCount++;

                    newInstr = createTAC(TAC_ASSIGN, resultStr, NULL, curr->result);
                } else {
                    newInstr = createTAC(TAC_DIV, left, right, curr->result);
                }
                break;
            }
            
            case TAC_ASSIGN: {
                char* value = curr->arg1;
                
                for (int i = valueCount - 1; i >= 0; i--) {
                    if (strcmp(values[i].var, value) == 0) {
                        value = values[i].value;
                        break;
                    }
                }
                
                if (valueCount >= valuesCapacity) {
                    valuesCapacity *= 2;
                    values = realloc(values, sizeof(VarValue) * valuesCapacity);
                    if (!values) {
                        fprintf(stderr, "Out of memory in optimizeTAC (realloc)\n");
                        exit(1);
                    }
                }
                values[valueCount].var = strdup(curr->result);
                values[valueCount].value = strdup(value);
                valueCount++;
                
                newInstr = createTAC(TAC_ASSIGN, value, NULL, curr->result);
                break;
            }
            
            case TAC_PRINT: {
                char* value = curr->arg1;
                
                for (int i = valueCount - 1; i >= 0; i--) {
                    if (strcmp(values[i].var, value) == 0) {
                        value = values[i].value;
                        break;
                    }
                }
                
                newInstr = createTAC(TAC_PRINT, value, NULL, NULL);
                break;
            }

            case TAC_WRITE: {
                char* value = curr->arg1;
                
                for (int i = valueCount - 1; i >= 0; i--) {
                    if (strcmp(values[i].var, value) == 0) {
                        value = values[i].value;
                        break;
                    }
                }
                
                newInstr = createTAC(TAC_WRITE, value, NULL, NULL);
                break;
            }

            case TAC_WRITELN:
                newInstr = createTAC(TAC_WRITELN, NULL, NULL, NULL);
                break;

            case TAC_DECL_ARRAY:
                newInstr = createTAC(TAC_DECL_ARRAY, curr->arg1, NULL, curr->result);
                break;

            case TAC_LOAD:
                newInstr = createTAC(TAC_LOAD, curr->arg1, curr->arg2, curr->result);
                break;
            case TAC_LABEL:
                newInstr = createTAC(TAC_LABEL, NULL, NULL, curr->result);
                break;
            case TAC_PARAM:
                newInstr = createTAC(TAC_PARAM, curr->arg1, NULL, NULL);
                break;
            case TAC_CALL:
                newInstr = createTAC(TAC_CALL, curr->arg1, NULL, curr->result);
                newInstr->paramCount = curr->paramCount;
                break;
            case TAC_RETURN:
                newInstr = createTAC(TAC_RETURN, curr->arg1, NULL, NULL);
                break;
            case TAC_FUNC_BEGIN:
                newInstr = createTAC(TAC_FUNC_BEGIN, NULL, NULL, curr->result);
                break;
            case TAC_FUNC_END:
                newInstr = createTAC(TAC_FUNC_END, NULL, NULL, curr->result);
                break;
            case TAC_STORE:
                newInstr = createTAC(TAC_STORE, curr->arg1, curr->arg2, curr->result);
                break;
            
            /* Relational and control flow ops - pass through */
            case TAC_EQ:
                newInstr = createTAC(TAC_EQ, curr->arg1, curr->arg2, curr->result);
                break;
            case TAC_NE:
                newInstr = createTAC(TAC_NE, curr->arg1, curr->arg2, curr->result);
                break;
            case TAC_LT:
                newInstr = createTAC(TAC_LT, curr->arg1, curr->arg2, curr->result);
                break;
            case TAC_LE:
                newInstr = createTAC(TAC_LE, curr->arg1, curr->arg2, curr->result);
                break;
            case TAC_GT:
                newInstr = createTAC(TAC_GT, curr->arg1, curr->arg2, curr->result);
                break;
            case TAC_GE:
                newInstr = createTAC(TAC_GE, curr->arg1, curr->arg2, curr->result);
                break;
            case TAC_IF_FALSE:
                newInstr = createTAC(TAC_IF_FALSE, curr->arg1, NULL, curr->result);
                break;
            case TAC_GOTO:
                newInstr = createTAC(TAC_GOTO, NULL, NULL, curr->result);
                break;
            case TAC_AND:
                newInstr = createTAC(TAC_AND, curr->arg1, curr->arg2, curr->result);
                break;
            case TAC_OR:
                newInstr = createTAC(TAC_OR, curr->arg1, curr->arg2, curr->result);
                break;
            case TAC_NOT:
                newInstr = createTAC(TAC_NOT, curr->arg1, NULL, curr->result);
                break;
            
            default: {
                /* Preserve unknown or unhandled ops by copying raw strings */
                newInstr = createTAC(curr->op, curr->arg1, curr->arg2, curr->result);
                newInstr->paramCount = curr->paramCount;
                break;
            }
        }
        
        if (newInstr) {
            appendOptimizedTAC(newInstr);
        }

        curr = curr->next;
    }

    /* Cleanup propagation table memory */
    for (int i = 0; i < valueCount; i++) {
        free(values[i].var);
        free(values[i].value);
    }
    free(values);

    // After building the optimized list, run a simple dead-code elimination
    // that removes instructions that write to temporaries (t*) which are
    // never used later.
    // Build a table of used variable names by scanning args in the optimized list
    TACInstr* scan = optimizedList.head;
    int maxUsed = 2048;
    char** used = malloc(sizeof(char*) * maxUsed);
    int usedCount = 0;

    while (scan) {
        if (scan->arg1) {
            if (usedCount < maxUsed) used[usedCount++] = scan->arg1;
        }
        if (scan->arg2) {
            if (usedCount < maxUsed) used[usedCount++] = scan->arg2;
        }
        scan = scan->next;
    }

    // Rebuild list skipping dead writes to temporaries that aren't in 'used'
    TACInstr* newHead = NULL;
    TACInstr* newTail = NULL;
    scan = optimizedList.head;
    while (scan) {
        bool skip = false;
        if (scan->result && scan->result[0] == 't') {
            // check if result is referenced in used[]
            bool referenced = false;
            for (int i = 0; i < usedCount; i++) {
                if (strcmp(used[i], scan->result) == 0) { referenced = true; break; }
            }
            if (!referenced) {
                // safe to drop this instruction
                skip = true;
            }
        }

        TACInstr* next = scan->next;
        if (!skip) {
            // append to new list
            scan->next = NULL;
            if (!newHead) newHead = newTail = scan;
            else { newTail->next = scan; newTail = scan; }
        } else {
            // unlink and free the strings owned by this instr to avoid leaks
            if (scan->arg1) free(scan->arg1);
            if (scan->arg2) free(scan->arg2);
            if (scan->result) free(scan->result);
            free(scan);
        }

        scan = next;
    }

    optimizedList.head = newHead;
    optimizedList.tail = newTail;
    free(used);
}

void printOptimizedTAC() {
    printf("\nOptimized TAC Instructions:\n");
    printf("───────────────────────────\n");
    TACInstr* curr = optimizedList.head;
    int lineNum = 1;
    while (curr) {
        printf("%2d: ", lineNum++);
        switch(curr->op) {
            case TAC_DECL:
                printf("DECL %s\n", curr->result);
                break;
            /* NEW: Print float ops */
            case TAC_DECL_FLOAT:
                printf("DECL_FLOAT %s\n", curr->result);
                break;
            case TAC_ADD:
                printf("%s = %s + %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_FADD:
                printf("%s = %s + %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_SUB:
                printf("%s = %s - %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_FSUB:
                printf("%s = %s - %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_MUL:
                printf("%s = %s * %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_FMUL:
                printf("%s = %s * %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_DIV:
                printf("%s = %s / %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_FDIV:
                printf("%s = %s / %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_ASSIGN:
                printf("%s = %s\n", curr->result, curr->arg1);
                break;
            case TAC_PRINT:
                printf("PRINT %s\n", curr->arg1);
                break;
            case TAC_WRITE:
                printf("WRITE %s\n", curr->arg1);
                break;
            case TAC_WRITELN:
                printf("WRITELN\n");
                break;
            case TAC_FPRINT:
                printf("FPRINT %s\n", curr->arg1);
                break;
            case TAC_INT_TO_FLOAT:
                printf("%s = (float) %s\n", curr->result, curr->arg1);
                break;
            case TAC_FLOAT_TO_INT:
                printf("%s = (int) %s\n", curr->result, curr->arg1);
                break;
            /* End new print cases */
            case TAC_DECL_ARRAY:
                printf("DECL_ARRAY %s[%s]\n", curr->result, curr->arg1);
                break;
            case TAC_STORE:
                printf("%s[%s] = %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_LOAD:
                printf("%s = %s[%s]\n", curr->result, curr->arg1, curr->arg2);
                break;
            /* Other ops */
            case TAC_LABEL:
                printf("LABEL %s\n", curr->result);
                break;
            case TAC_PARAM:
                printf("PARAM %s\n", curr->arg1);
                break;
            case TAC_CALL:
                printf("%s = CALL %s, %d\n", curr->result, curr->arg1, curr->paramCount);
                break;
            case TAC_RETURN:
                if (curr->arg1) printf("RETURN %s\n", curr->arg1);
                else printf("RETURN\n");
                break;
            case TAC_FUNC_BEGIN:
                printf("FUNC_BEGIN %s\n", curr->result);
                break;
            case TAC_FUNC_END:
                printf("FUNC_END %s\n", curr->result);
                break;
            case TAC_EQ:
                printf("%s = %s == %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_NE:
                printf("%s = %s != %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_LT:
                printf("%s = %s < %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_LE:
                printf("%s = %s <= %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_GT:
                printf("%s = %s > %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_GE:
                printf("%s = %s >= %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_IF_FALSE:
                printf("IF_FALSE %s GOTO %s\n", curr->arg1, curr->result);
                break;
            case TAC_GOTO:
                printf("GOTO %s\n", curr->result);
                break;
            case TAC_AND:
                printf("%s = %s && %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_OR:
                printf("%s = %s || %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_NOT:
                printf("%s = !%s\n", curr->result, curr->arg1);
                break;
            default:
                printf("Unknown Op (Optimized)\n");
                break;
        }
        curr = curr->next;
    }
}

