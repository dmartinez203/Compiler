#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "tac.h"

TACList tacList;
TACList optimizedList;

void initTAC() {
    tacList.head = NULL;
    tacList.tail = NULL;
    tacList.tempCount = 0;
    optimizedList.head = NULL;
    optimizedList.tail = NULL;
}

char* newTemp() {
    char* temp = malloc(10);
    sprintf(temp, "t%d", tacList.tempCount++);
    return temp;
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

char* generateTACExpr(ASTNode* node) {
    if (!node) return NULL;
    
    switch(node->type) {
        case NODE_NUM: {
            char* temp = malloc(20);
            sprintf(temp, "%d", node->data.num);
            return temp;
        }
        case NODE_FNUM: {
            char* temp = malloc(32);
            sprintf(temp, "%g", node->data.fnum);
            return temp;
        }
        
        case NODE_VAR:
            return strdup(node->data.name);
        
        case NODE_BINOP: {
            char* left = generateTACExpr(node->data.binop.left);
            char* right = generateTACExpr(node->data.binop.right);
            char* temp = newTemp();
            
            if (node->data.binop.op == '+') {
                appendTAC(createTAC(TAC_ADD, left, right, temp));
            }
            else if (node->data.binop.op == '-') {
                appendTAC(createTAC(TAC_SUB, left, right, temp));
            }
            else if (node->data.binop.op == '*') {
                appendTAC(createTAC(TAC_MUL, left, right, temp));
            }
            else if (node->data.binop.op == '/') {
                appendTAC(createTAC(TAC_DIV, left, right, temp));
            }
            
            return temp;
        }
        
        case NODE_ARRAY_ACCESS: {
            char* index = generateTACExpr(node->data.array_access.index);
            char* temp = newTemp();
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
        
        default:
            return NULL;
    }
}

void generateTAC(ASTNode* node) {
    if (!node) return;
    
    switch(node->type) {
        case NODE_DECL:
            appendTAC(createTAC(TAC_DECL, NULL, NULL, node->data.name));
            break;

        case NODE_ARRAY_DECL: {
            char sizeStr[20];
            sprintf(sizeStr, "%d", node->data.array_decl.size);
            appendTAC(createTAC(TAC_DECL_ARRAY, strdup(sizeStr), NULL, node->data.array_decl.name));
            break;
        }
            
        case NODE_ASSIGN: {
            char* expr = generateTACExpr(node->data.assign.value);
            appendTAC(createTAC(TAC_ASSIGN, expr, NULL, node->data.assign.var));
            break;
        }

        case NODE_ARRAY_ASSIGN: {
            char* index = generateTACExpr(node->data.array_assign.index);
            char* value = generateTACExpr(node->data.array_assign.value);
            appendTAC(createTAC(TAC_STORE, index, value, node->data.array_assign.name));
            break;
        }
        
        case NODE_PRINT: {
            char* expr = generateTACExpr(node->data.expr);
            appendTAC(createTAC(TAC_PRINT, expr, NULL, NULL));
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
                appendTAC(createTAC(TAC_RETURN, ret, NULL, NULL));
            } else {
                appendTAC(createTAC(TAC_RETURN, NULL, NULL, NULL));
            }
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
                printf("           // Declare variable '%s'\n", curr->result);
                break;
            case TAC_ADD:
                printf("%s = %s + %s", curr->result, curr->arg1, curr->arg2);
                printf("     // Add: store result in %s\n", curr->result);
                break;
            case TAC_MUL:
                printf("%s = %s * %s", curr->result, curr->arg1, curr->arg2);
                printf("     // Multiply: store result in %s\n", curr->result);
                break;
            case TAC_DIV:
                printf("%s = %s / %s", curr->result, curr->arg1, curr->arg2);
                printf("     // Divide: store result in %s\n", curr->result);
                break;
            case TAC_SUB:
                printf("%s = %s - %s", curr->result, curr->arg1, curr->arg2);
                printf("     // Subtract: store result in %s\n", curr->result);
                break;
            case TAC_ASSIGN:
                printf("%s = %s", curr->result, curr->arg1);
                printf("           // Assign value to %s\n", curr->result);
                break;
            case TAC_PRINT:
                printf("PRINT %s", curr->arg1);
                printf("           // Output value of %s\n", curr->arg1);
                break;
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
            default:
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
            default: {
                /* Preserve unknown or unhandled ops by copying raw strings */
                newInstr = createTAC(curr->op, curr->arg1, curr->arg2, curr->result);
                newInstr->paramCount = curr->paramCount;
                break;
            }
            
            case TAC_STORE:
                newInstr = createTAC(TAC_STORE, curr->arg1, curr->arg2, curr->result);
                break;
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
            case TAC_ADD:
                printf("%s = %s + %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_MUL:
                printf("%s = %s * %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_DIV:
                printf("%s = %s / %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_SUB:
                printf("%s = %s - %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_ASSIGN:
                printf("%s = %s\n", curr->result, curr->arg1);
                break;
            case TAC_PRINT:
                printf("PRINT %s\n", curr->arg1);
                break;
            case TAC_DECL_ARRAY:
                printf("DECL_ARRAY %s[%s]\n", curr->result, curr->arg1);
                break;
            case TAC_STORE:
                printf("%s[%s] = %s\n", curr->result, curr->arg1, curr->arg2);
                break;
            case TAC_LOAD:
                printf("%s = %s[%s]\n", curr->result, curr->arg1, curr->arg2);
                break;
            default:
                break;
        }
        curr = curr->next;
    }
}