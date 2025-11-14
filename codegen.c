/*
 * CODE GENERATOR
 *
 * This file translates the Three-Address Code (TAC) list into MIPS assembly.
 * It is the final stage of the compiler.
 *
 * It reads from the `optimizedList` (defined in tac.c) and outputs MIPS
 * assembly code to the specified output file.
 *
 * Key features:
 * - Reads from TAC, not the AST.
 * - Manages MIPS integer ($t0-$t9) and float ($f0-$f31) registers.
 * - Allocates stack space for all variables (respecting 4-byte alignment).
 * - Handles all integer and float operations and type conversions.
 * - Stores float literals in the .data section.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "codegen.h"
#include "symtab.h"
#include "tac.h"

FILE* output; // Output file (e.g., "output.s")
extern TACList optimizedList; // The TAC list from tac.c

// Simple register management for temporary values
int intReg = 0;   // Current integer temp register ($t0-$t9)
int floatReg = 0; // Current float temp register ($f0-$f11)
int labelCounter = 0; // For unique label generation

// List to store float literals for the .data section
typedef struct FloatLiteral {
    char* label;
    char* value;
    struct FloatLiteral* next;
} FloatLiteral;

FloatLiteral* floatList = NULL;
int floatLabelCount = 0;

/* --- Register Management --- */

// Get the next available integer temp register (e.g., "$t0")
char* nextIntReg() {
    char* reg = malloc(10); // UPDATED: Increased size from 4 to 10
    if (intReg > 9) {
        printf("Warning: Ran out of integer temp registers. Reusing $t0.\n");
        intReg = 0; // Wrap around (very simple spill)
    }
    sprintf(reg, "$t%d", intReg++);
    return reg;
}

// Get the next available float temp register (e.g., "$f0")
char* nextFloatReg() {
    char* reg = malloc(10); // UPDATED: Increased size from 4 to 10
    if (floatReg > 11) {
        printf("Warning: Ran out of float temp registers. Reusing $f0.\n");
        floatReg = 0; // Wrap around
    }
    // Use even-numbered registers for single-precision
    sprintf(reg, "$f%d", floatReg); 
    floatReg += 2; // Increment by 2 for single-precision pairs
    return reg;
}

// Reset register counters (e.g., at the end of a statement)
void resetRegs() {
    intReg = 0;
    floatReg = 0;
}

/* --- Argument Loading Helpers --- */

// Checks if a string is a number (int or float)
int isNumeric(const char* s) {
    if (s == NULL) return 0;
    char* end;
    strtod(s, &end); // Try to parse as a double
    return *end == '\0'; // If end of string is reached, it's numeric
}

// Generates MIPS to load an integer value into a register
// Value can be a literal ("123") or a variable name ("x")
char* load_int_arg(char* arg) {
    char* reg = nextIntReg();
    if (isdigit(arg[0]) || (arg[0] == '-' && isdigit(arg[1]))) {
        // It's an integer literal
        fprintf(output, "    li %s, %s\n", reg, arg);
    } else {
        // It's a variable. Load from stack
        int offset = getVarOffset(arg);
        fprintf(output, "    lw %s, %d($sp)\n", reg, offset);
    }
    return reg;
}

// Generates MIPS to load a float value into a register
// Value can be a literal ("3.14") or a variable name ("f")
char* load_float_arg(char* arg) {
    char* reg = nextFloatReg();
    if (isNumeric(arg)) {
        // It's a float literal. Find its label in the .data section
        FloatLiteral* f = floatList;
        while (f) {
            if (strcmp(f->value, arg) == 0) {
                fprintf(output, "    l.s %s, %s\n", reg, f->label);
                return reg;
            }
            f = f->next;
        }
        // Should not happen if pre-scan worked
        fprintf(output, "    # ERROR: Float literal %s not found!\n", arg);
    } else {
        // It's a variable. Load from stack
        int offset = getVarOffset(arg);
        fprintf(output, "    l.s %s, %d($sp)\n", reg, offset);
    }
    return reg;
}

// Generates MIPS to store an integer register value into a variable
void store_int_result(char* reg, char* resultVar) {
    int offset = getVarOffset(resultVar);
    fprintf(output, "    sw %s, %d($sp)\n", reg, offset);
    free(reg); // Free register name string
}

// Generates MIPS to store a float register value into a variable
void store_float_result(char* reg, char* resultVar) {
    int offset = getVarOffset(resultVar);
    fprintf(output, "    s.s %s, %d($sp)\n", reg, offset);
    free(reg); // Free register name string
}


/* --- Helper function to register float literals --- */
static void findFloatLiteral(char* arg) {
    if (arg && !isVarDeclared(arg) && (strchr(arg, '.') || strchr(arg, 'e'))) {
        // Check if this literal is already in our list
        FloatLiteral* f = floatList;
        int found = 0;
        while (f) {
            if (strcmp(f->value, arg) == 0) {
                found = 1;
                break;
            }
            f = f->next;
        }
        
        // If not found, add it
        if (!found) {
            FloatLiteral* newLit = malloc(sizeof(FloatLiteral));
            newLit->label = malloc(10);
            sprintf(newLit->label, "fl%d", floatLabelCount++);
            newLit->value = strdup(arg);
            newLit->next = floatList;
            floatList = newLit;
        }
    }
}

/* --- Main Code Generation Function --- */

void generateMIPS(const char* filename) {
    output = fopen(filename, "w");
    if (!output) {
        fprintf(stderr, "Cannot open output file %s\n", filename);
        exit(1);
    }

    // Initialize symbol table to calculate stack size
    // Note: We re-init the symtab to be independent of other phases
    // In a real compiler, this data would be passed via a structure
    initSymTab();
    
    // --- Phase 1: Pre-scan TAC for float literals and stack size ---
    
    int stackSize = 0;
    TACInstr* curr = optimizedList.head;
    
    while (curr) {
        // Add declarations to symtab to calculate total stack size
        if (curr->op == TAC_DECL) {
            addVar(curr->result, TYPE_INT);
            stackSize += 4;
        } else if (curr->op == TAC_DECL_FLOAT) {
            addVar(curr->result, TYPE_FLOAT);
            stackSize += 4;
        } else if (curr->op == TAC_DECL_ARRAY) {
            int size = atoi(curr->arg1);
            addArray(curr->result, TYPE_INT, size); // TODO: Assumes int array
            stackSize += size * 4;
        }

        // Check all args of all ops for float literals
        findFloatLiteral(curr->arg1);
        findFloatLiteral(curr->arg2);
        
        curr = curr->next;
    }
    
    // Ensure stack size is word-aligned (though it should be)
    if (stackSize % 4 != 0) {
        stackSize += (4 - (stackSize % 4));
    }
    // Add a buffer for safety (e.g., saving $ra)
    stackSize += 8;


    // --- Phase 2: Generate MIPS ---
    
    // MIPS program header
    fprintf(output, ".data\n");
    fprintf(output, "newline: .asciiz \"\\n\"\n");
    
    // Write all float literals
    FloatLiteral* f = floatList;
    while(f) {
        fprintf(output, "%s: .float %s\n", f->label, f->value);
        f = f->next;
    }
    
    fprintf(output, "\n.text\n");
    fprintf(output, ".globl main\n");
    fprintf(output, "main:\n");
    
    // Allocate stack space
    fprintf(output, "    # Allocate stack space (dynamic based on var count)\n");
    fprintf(output, "    addi $sp, $sp, -%d\n", stackSize);
    
    // --- Process TAC list ---
    curr = optimizedList.head;
    while (curr) {
        resetRegs(); // Reset temp register counts for each statement
        
        switch(curr->op) {
            case TAC_DECL:
            case TAC_DECL_FLOAT:
            case TAC_DECL_ARRAY:
                // Declarations are handled in pre-scan for stack space
                // No MIPS instructions needed here.
                fprintf(output, "    # (Declaration: %s)\n", curr->result);
                break;

            // --- Integer Arithmetic ---
            case TAC_ADD: {
                char* r1 = load_int_arg(curr->arg1);
                char* r2 = load_int_arg(curr->arg2);
                char* res = nextIntReg();
                fprintf(output, "    add %s, %s, %s\n", res, r1, r2);
                store_int_result(res, curr->result);
                free(r1); free(r2);
                break;
            }
            case TAC_SUB: {
                char* r1 = load_int_arg(curr->arg1);
                char* r2 = load_int_arg(curr->arg2);
                char* res = nextIntReg();
                fprintf(output, "    sub %s, %s, %s\n", res, r1, r2);
                store_int_result(res, curr->result);
                free(r1); free(r2);
                break;
            }
            case TAC_MUL: {
                char* r1 = load_int_arg(curr->arg1);
                char* r2 = load_int_arg(curr->arg2);
                char* res = nextIntReg();
                fprintf(output, "    mult %s, %s\n", r1, r2);
                fprintf(output, "    mflo %s\n", res);
                store_int_result(res, curr->result);
                free(r1); free(r2);
                break;
            }
            case TAC_DIV: {
                char* r1 = load_int_arg(curr->arg1);
                char* r2 = load_int_arg(curr->arg2);
                char* res = nextIntReg();
                fprintf(output, "    div %s, %s\n", r1, r2);
                fprintf(output, "    mflo %s\n", res);
                store_int_result(res, curr->result);
                free(r1); free(r2);
                break;
            }

            // --- Float Arithmetic ---
            case TAC_FADD: {
                char* r1 = load_float_arg(curr->arg1);
                char* r2 = load_float_arg(curr->arg2);
                char* res = nextFloatReg();
                fprintf(output, "    add.s %s, %s, %s\n", res, r1, r2);
                store_float_result(res, curr->result);
                free(r1); free(r2);
                break;
            }
            case TAC_FSUB: {
                char* r1 = load_float_arg(curr->arg1);
                char* r2 = load_float_arg(curr->arg2);
                char* res = nextFloatReg();
                fprintf(output, "    sub.s %s, %s, %s\n", res, r1, r2);
                store_float_result(res, curr->result);
                free(r1); free(r2);
                break;
            }
            case TAC_FMUL: {
                char* r1 = load_float_arg(curr->arg1);
                char* r2 = load_float_arg(curr->arg2);
                char* res = nextFloatReg();
                fprintf(output, "    mul.s %s, %s, %s\n", res, r1, r2);
                store_float_result(res, curr->result);
                free(r1); free(r2);
                break;
            }
            case TAC_FDIV: {
                char* r1 = load_float_arg(curr->arg1);
                char* r2 = load_float_arg(curr->arg2);
                char* res = nextFloatReg();
                fprintf(output, "    div.s %s, %s, %s\n", res, r1, r2);
                store_float_result(res, curr->result);
                free(r1); free(r2);
                break;
            }

            // --- Type Conversions ---
            case TAC_INT_TO_FLOAT: {
                char* r1 = load_int_arg(curr->arg1);
                char* res = nextFloatReg();
                fprintf(output, "    mtc1 %s, %s\n", r1, res);
                fprintf(output, "    cvt.s.w %s, %s\n", res, res);
                store_float_result(res, curr->result);
                free(r1);
                break;
            }
            case TAC_FLOAT_TO_INT: {
                char* r1 = load_float_arg(curr->arg1);
                char* res = nextIntReg();
                // We need a temp float reg for truncation
                char* tempFloat = nextFloatReg();
                fprintf(output, "    trunc.w.s %s, %s\n", tempFloat, r1);
                fprintf(output, "    mfc1 %s, %s\n", res, tempFloat);
                store_int_result(res, curr->result);
                free(r1); free(tempFloat);
                break;
            }

            // --- Assignment ---
            case TAC_ASSIGN: {
                // Check type of variable to see what kind of load we need
                DataType varType = getVarType(curr->result);
                if (varType == TYPE_INT) {
                    char* r1 = load_int_arg(curr->arg1);
                    store_int_result(r1, curr->result);
                } else {
                    char* r1 = load_float_arg(curr->arg1);
                    store_float_result(r1, curr->result);
                }
                break;
            }
            
            // --- Print Statements ---
            case TAC_PRINT: { // Print Integer
                char* r1 = load_int_arg(curr->arg1);
                fprintf(output, "    move $a0, %s\n", r1);
                fprintf(output, "    li $v0, 1\n");
                fprintf(output, "    syscall\n");
                free(r1);
                // Print newline
                fprintf(output, "    la $a0, newline\n");
                fprintf(output, "    li $v0, 4\n");
                fprintf(output, "    syscall\n");
                break;
            }
            case TAC_WRITE: { // Write Integer or Character (no newline)
                char* r1 = load_int_arg(curr->arg1);
                fprintf(output, "    move $a0, %s\n", r1);
                // Check if it's a character (< 256) or integer
                fprintf(output, "    li $t9, 256\n");
                fprintf(output, "    blt $a0, $t9, write_char_%d\n", labelCounter);
                // Print as integer
                fprintf(output, "    li $v0, 1\n");
                fprintf(output, "    syscall\n");
                fprintf(output, "    j write_done_%d\n", labelCounter);
                fprintf(output, "write_char_%d:\n", labelCounter);
                // Print as character
                fprintf(output, "    li $v0, 11\n");
                fprintf(output, "    syscall\n");
                fprintf(output, "write_done_%d:\n", labelCounter);
                labelCounter++;
                free(r1);
                break;
            }
            case TAC_WRITELN: { // Write newline
                fprintf(output, "    la $a0, newline\n");
                fprintf(output, "    li $v0, 4\n");
                fprintf(output, "    syscall\n");
                break;
            }
            case TAC_FPRINT: { // Print Float
                char* r1 = load_float_arg(curr->arg1);
                fprintf(output, "    mov.s $f12, %s\n", r1); // Syscall arg is $f12
                fprintf(output, "    li $v0, 2\n");
                fprintf(output, "    syscall\n");
                free(r1);
                // Print newline
                fprintf(output, "    la $a0, newline\n");
                fprintf(output, "    li $v0, 4\n");
                fprintf(output, "    syscall\n");
                break;
            }

            // --- Relational Operations ---
            case TAC_EQ: {
                char* r1 = load_int_arg(curr->arg1);
                char* r2 = load_int_arg(curr->arg2);
                char* res = nextIntReg();
                fprintf(output, "    seq %s, %s, %s\n", res, r1, r2);
                store_int_result(res, curr->result);
                free(r1); free(r2);
                break;
            }
            case TAC_NE: {
                char* r1 = load_int_arg(curr->arg1);
                char* r2 = load_int_arg(curr->arg2);
                char* res = nextIntReg();
                fprintf(output, "    sne %s, %s, %s\n", res, r1, r2);
                store_int_result(res, curr->result);
                free(r1); free(r2);
                break;
            }
            case TAC_LT: {
                char* r1 = load_int_arg(curr->arg1);
                char* r2 = load_int_arg(curr->arg2);
                char* res = nextIntReg();
                fprintf(output, "    slt %s, %s, %s\n", res, r1, r2);
                store_int_result(res, curr->result);
                free(r1); free(r2);
                break;
            }
            case TAC_LE: {
                char* r1 = load_int_arg(curr->arg1);
                char* r2 = load_int_arg(curr->arg2);
                char* res = nextIntReg();
                fprintf(output, "    sle %s, %s, %s\n", res, r1, r2);
                store_int_result(res, curr->result);
                free(r1); free(r2);
                break;
            }
            case TAC_GT: {
                char* r1 = load_int_arg(curr->arg1);
                char* r2 = load_int_arg(curr->arg2);
                char* res = nextIntReg();
                fprintf(output, "    sgt %s, %s, %s\n", res, r1, r2);
                store_int_result(res, curr->result);
                free(r1); free(r2);
                break;
            }
            case TAC_GE: {
                char* r1 = load_int_arg(curr->arg1);
                char* r2 = load_int_arg(curr->arg2);
                char* res = nextIntReg();
                fprintf(output, "    sge %s, %s, %s\n", res, r1, r2);
                store_int_result(res, curr->result);
                free(r1); free(r2);
                break;
            }
            
            // --- Logical Operations ---
            case TAC_AND: {
                char* r1 = load_int_arg(curr->arg1);
                char* r2 = load_int_arg(curr->arg2);
                char* res = nextIntReg();
                fprintf(output, "    and %s, %s, %s\n", res, r1, r2);
                // Normalize to 0 or 1 (boolean)
                fprintf(output, "    sltu %s, $zero, %s\n", res, res);
                store_int_result(res, curr->result);
                free(r1); free(r2);
                break;
            }
            case TAC_OR: {
                char* r1 = load_int_arg(curr->arg1);
                char* r2 = load_int_arg(curr->arg2);
                char* res = nextIntReg();
                fprintf(output, "    or %s, %s, %s\n", res, r1, r2);
                // Normalize to 0 or 1 (boolean)
                fprintf(output, "    sltu %s, $zero, %s\n", res, res);
                store_int_result(res, curr->result);
                free(r1); free(r2);
                break;
            }
            case TAC_NOT: {
                char* r1 = load_int_arg(curr->arg1);
                char* res = nextIntReg();
                fprintf(output, "    seq %s, %s, $zero\n", res, r1);
                store_int_result(res, curr->result);
                free(r1);
                break;
            }
            
            // --- Control Flow ---
            case TAC_IF_FALSE: {
                char* r1 = load_int_arg(curr->arg1);
                fprintf(output, "    beqz %s, %s\n", r1, curr->result);
                free(r1);
                break;
            }
            case TAC_GOTO:
                fprintf(output, "    j %s\n", curr->result);
                break;
            
            // --- Array Operations ---
            case TAC_STORE: { // arr[index] = value
                // Load index
                char* idx = load_int_arg(curr->arg1);
                // Scale by 4 (word size)
                char* scaled = nextIntReg();
                fprintf(output, "    sll %s, %s, 2\n", scaled, idx);
                // Get base address
                int baseOffset = getVarOffset(curr->result);
                char* base = nextIntReg();
                fprintf(output, "    addi %s, $sp, %d\n", base, baseOffset);
                // Add scaled index
                char* addr = nextIntReg();
                fprintf(output, "    add %s, %s, %s\n", addr, base, scaled);
                // Load value and store
                DataType arrType = getVarType(curr->result);
                if (arrType == TYPE_FLOAT) {
                    char* val = load_float_arg(curr->arg2);
                    fprintf(output, "    s.s %s, 0(%s)\n", val, addr);
                    free(val);
                } else {
                    char* val = load_int_arg(curr->arg2);
                    fprintf(output, "    sw %s, 0(%s)\n", val, addr);
                    free(val);
                }
                free(idx); free(scaled); free(base); free(addr);
                break;
            }
            case TAC_LOAD: { // result = arr[index]
                // Load index
                char* idx = load_int_arg(curr->arg2);
                // Scale by 4
                char* scaled = nextIntReg();
                fprintf(output, "    sll %s, %s, 2\n", scaled, idx);
                // Get base address
                int baseOffset = getVarOffset(curr->arg1);
                char* base = nextIntReg();
                fprintf(output, "    addi %s, $sp, %d\n", base, baseOffset);
                // Add scaled index
                char* addr = nextIntReg();
                fprintf(output, "    add %s, %s, %s\n", addr, base, scaled);
                // Load value
                DataType arrType = getVarType(curr->arg1);
                if (arrType == TYPE_FLOAT) {
                    char* res = nextFloatReg();
                    fprintf(output, "    l.s %s, 0(%s)\n", res, addr);
                    store_float_result(res, curr->result);
                } else {
                    char* res = nextIntReg();
                    fprintf(output, "    lw %s, 0(%s)\n", res, addr);
                    store_int_result(res, curr->result);
                }
                free(idx); free(scaled); free(base); free(addr);
                break;
            }
            
            // --- Labels ---
            case TAC_LABEL:
                fprintf(output, "%s:\n", curr->result);
                break;
                
            // --- Function Operations ---
            case TAC_FUNC_BEGIN:
                fprintf(output, "\n# Function: %s\n", curr->result);
                fprintf(output, "# (Function stack frame management omitted for simplicity)\n");
                break;
                
            case TAC_FUNC_END:
                fprintf(output, "# End of function %s\n\n", curr->result);
                break;
                
            case TAC_PARAM: {
                // For simplicity, push parameters onto stack
                // In a real implementation, first 4 would go in $a0-$a3
                DataType paramType = TYPE_INT; // Default
                if (isVarDeclared(curr->arg1)) {
                    paramType = getVarType(curr->arg1);
                }
                
                if (paramType == TYPE_FLOAT || strchr(curr->arg1, '.')) {
                    char* r = load_float_arg(curr->arg1);
                    fprintf(output, "    addi $sp, $sp, -4\n");
                    fprintf(output, "    s.s %s, 0($sp)\n", r);
                    free(r);
                } else {
                    char* r = load_int_arg(curr->arg1);
                    fprintf(output, "    addi $sp, $sp, -4\n");
                    fprintf(output, "    sw %s, 0($sp)\n", r);
                    free(r);
                }
                break;
            }
                
            case TAC_CALL: {
                fprintf(output, "    # Save return address\n");
                fprintf(output, "    addi $sp, $sp, -4\n");
                fprintf(output, "    sw $ra, 0($sp)\n");
                fprintf(output, "    # Call function %s with %d params\n", curr->arg1, curr->paramCount);
                fprintf(output, "    jal %s\n", curr->arg1);
                fprintf(output, "    # Restore return address\n");
                fprintf(output, "    lw $ra, 0($sp)\n");
                fprintf(output, "    addi $sp, $sp, 4\n");
                fprintf(output, "    # Clean up %d parameters\n", curr->paramCount);
                fprintf(output, "    addi $sp, $sp, %d\n", curr->paramCount * 4);
                fprintf(output, "    # Store return value\n");
                // Assume function returns int in $v0
                int offset = getVarOffset(curr->result);
                fprintf(output, "    sw $v0, %d($sp)\n", offset);
                break;
            }
                
            case TAC_RETURN: {
                if (curr->arg1) {
                    fprintf(output, "    # Return value\n");
                    char* r = load_int_arg(curr->arg1);
                    fprintf(output, "    move $v0, %s\n", r);
                    free(r);
                }
                fprintf(output, "    jr $ra\n");
                break;
            }
                
            default:
                break;
        }
        curr = curr->next;
    }
    
    // Program exit
    fprintf(output, "\n    # Exit program\n");
    fprintf(output, "    addi $sp, $sp, %d\n", stackSize);
    fprintf(output, "    li $v0, 10\n");
    fprintf(output, "    syscall\n");
    
    fclose(output);
    
    // Clean up float literal list
    FloatLiteral* f_curr = floatList;
    while(f_curr) {
        FloatLiteral* f_next = f_curr->next;
        free(f_curr->label);
        free(f_curr->value);
        free(f_curr);
        f_curr = f_next;
    }
    floatList = NULL;
}


