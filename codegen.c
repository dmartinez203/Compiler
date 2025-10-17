#include <stdio.h>
#include <stdlib.h>
#include "codegen.h"
#include "symtab.h"

FILE* output;
int tempReg = 0;

int getNextTemp() {
    int reg = tempReg++;
    if (tempReg > 7) tempReg = 0;  // Reuse $t0-$t7
    return reg;
}

void genExpr(ASTNode* node) {
    if (!node) return;
    
    switch(node->type) {
        case NODE_NUM:
            fprintf(output, "    li $t%d, %d\n", getNextTemp(), node->data.num);
            break;
            
        case NODE_VAR: {
            int offset = getVarOffset(node->data.name);
            if (offset == -1) {
                fprintf(stderr, "Error: Variable %s not declared\n", node->data.name);
                exit(1);
            }
            fprintf(output, "    lw $t%d, %d($sp)\n", getNextTemp(), offset);
            break;
        }
        
        case NODE_BINOP:
            genExpr(node->data.binop.left);
            int leftReg = tempReg - 1;
            genExpr(node->data.binop.right);
            int rightReg = tempReg - 1;
            if (node->data.binop.op == '+') {
                fprintf(output, "    add $t%d, $t%d, $t%d\n", leftReg, leftReg, rightReg);
            } else if (node->data.binop.op == '-') {
                fprintf(output, "    sub $t%d, $t%d, $t%d\n", leftReg, leftReg, rightReg);
            } else if (node->data.binop.op == '*') {
                // use mult and mflo to get product
                fprintf(output, "    mult $t%d, $t%d\n", leftReg, rightReg);
                fprintf(output, "    mflo $t%d\n", leftReg);
            } else if (node->data.binop.op == '/') {
                // use div and mflo to get quotient (integer division)
                fprintf(output, "    div $t%d, $t%d\n", leftReg, rightReg);
                fprintf(output, "    mflo $t%d\n", leftReg);
            }
            tempReg = leftReg + 1;
            break;
            
        default:
            break;
    }
}

void genStmt(ASTNode* node) {
    if (!node) return;
    
    switch(node->type) {
        case NODE_DECL: {
            int offset = addVar(node->data.name);
            if (offset == -1) {
                fprintf(stderr, "Error: Variable %s already declared\n", node->data.name);
                exit(1);
            }
            fprintf(output, "    # Declared %s at offset %d\n", node->data.name, offset);
            break;
        }
        
        case NODE_ASSIGN: {
            int offset = getVarOffset(node->data.assign.var);
            if (offset == -1) {
                fprintf(stderr, "Error: Variable %s not declared\n", node->data.assign.var);
                exit(1);
            }
            genExpr(node->data.assign.value);
            fprintf(output, "    sw $t%d, %d($sp)\n", tempReg - 1, offset);
            tempReg = 0;
            break;
        }
        
        case NODE_PRINT:
            genExpr(node->data.expr);
            fprintf(output, "    # Print integer\n");
            fprintf(output, "    move $a0, $t%d\n", tempReg - 1);
            fprintf(output, "    li $v0, 1\n");
            fprintf(output, "    syscall\n");
            fprintf(output, "    # Print newline\n");
            fprintf(output, "    li $v0, 11\n");
            fprintf(output, "    li $a0, 10\n");
            fprintf(output, "    syscall\n");
            tempReg = 0;
            break;
            
        case NODE_STMT_LIST:
            genStmt(node->data.stmtlist.stmt);
            genStmt(node->data.stmtlist.next);
            break;
        case NODE_FUNC_DECL: {
            /* Emit function label */
            fprintf(output, "%s:\n", node->data.func_decl.name);
            /* Prologue: save return address and frame (minimal) */
            fprintf(output, "    addi $sp, $sp, -8\n");
            fprintf(output, "    sw $ra, 4($sp)\n");
            fprintf(output, "    sw $fp, 0($sp)\n");
            /* Set new frame pointer */
            fprintf(output, "    addi $fp, $sp, 8\n");
            /* Body */
            genStmt(node->data.func_decl.body);
            /* If function has return expression node, evaluate and move to $v0 */
            if (node->data.func_decl.ret) {
                genStmt(node->data.func_decl.ret);
                /* return expression expects its value in $tX (tempReg-1) */
                fprintf(output, "    move $v0, $t%d\n", tempReg - 1);
                tempReg = 0;
            }
            /* Epilogue: restore frame and return */
            fprintf(output, "    lw $fp, 0($sp)\n");
            fprintf(output, "    lw $ra, 4($sp)\n");
            fprintf(output, "    addi $sp, $sp, 8\n");
            fprintf(output, "    jr $ra\n");
            break;
        }
        case NODE_FUNC_CALL: {
            /* Evaluate arguments (minimal: no args or args evaluated into $t regs) */
            ASTNode* a = node->data.func_call.args;
            int argCount = 0;
            while (a) {
                genExpr(a->data.arg_list.expr);
                /* Push arg to stack */
                fprintf(output, "    addi $sp, $sp, -4\n");
                fprintf(output, "    sw $t%d, 0($sp)\n", tempReg - 1);
                tempReg = 0;
                argCount++;
                a = a->data.arg_list.next;
            }
            /* Call function */
            fprintf(output, "    jal %s\n", node->data.func_call.name);
            /* Pop arguments off stack */
            if (argCount > 0) {
                fprintf(output, "    addi $sp, $sp, %d\n", argCount * 4);
            }
            /* After call, return value is in $v0, move to temp reg */
            fprintf(output, "    move $t%d, $v0\n", getNextTemp());
            break;
        }
        case NODE_RETURN: {
            /* Evaluate return expression */
            genExpr(node->data.return_expr);
            /* Move result to $v0 and jump to epilogue by jr $ra (caller handles cleanup) */
            fprintf(output, "    move $v0, $t%d\n", tempReg - 1);
            tempReg = 0;
            /* Return from function */
            fprintf(output, "    jr $ra\n");
            break;
        }
            
        default:
            break;
    }
}

void generateMIPS(ASTNode* root, const char* filename) {
    output = fopen(filename, "w");
    if (!output) {
        fprintf(stderr, "Cannot open output file %s\n", filename);
        exit(1);
    }
    
    // Initialize symbol table
    initSymTab();
    
    // MIPS program header
    fprintf(output, ".data\n");
    fprintf(output, "\n.text\n");
    fprintf(output, ".globl main\n");
    fprintf(output, "main:\n");
    
    // Allocate stack space (max 100 variables * 4 bytes)
    fprintf(output, "    # Allocate stack space\n");
    fprintf(output, "    addi $sp, $sp, -400\n\n");
    
    // Generate code for statements
    genStmt(root);
    
    // Program exit
    fprintf(output, "\n    # Exit program\n");
    fprintf(output, "    addi $sp, $sp, 400\n");
    fprintf(output, "    li $v0, 10\n");
    fprintf(output, "    syscall\n");
    
    fclose(output);
}