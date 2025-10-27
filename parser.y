%{
/* SYNTAX ANALYZER (PARSER)
 * This is the second phase of compilation - checking grammar rules
 * Bison generates a parser that builds an Abstract Syntax Tree (AST)
 * The parser uses tokens from the scanner to verify syntax is correct
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* External declarations for lexer interface */
extern int yylex();      /* Get next token from scanner */
extern int yyparse();    /* Parse the entire input */
extern FILE* yyin;       /* Input file handle */

void yyerror(const char* s);  /* Error handling function */
ASTNode* root = NULL;          /* Root of the Abstract Syntax Tree */
%}

%debug

/* SEMANTIC VALUES UNION
 * Defines possible types for tokens and grammar symbols
 * This allows different grammar rules to return different data types
 */
%union {
    int num;                /* For integer literals */
    double fnum;            /* For float literals */
    char* str;              /* For identifiers */
    struct ASTNode* node;   /* For AST nodes */
}

/* TOKEN DECLARATIONS with their semantic value types */
%token <num> NUM        /* Number token carries an integer value */
%token <fnum> FNUM      /* Float token carries a double value */
%token <str> ID         /* Identifier token carries a string */
%token INT FLOAT PRINT RETURN FUNC /* Keywords have no semantic value */

/* NON-TERMINAL TYPES - Define what type each grammar rule returns */
%type <node> program translation_unit top_item stmt_list stmt decl assign expr primary print_stmt func_decl return_stmt param_list arg_list

/* OPERATOR PRECEDENCE AND ASSOCIATIVITY */
%left '+'  /* Addition is left-associative: a+b+c = (a+b)+c */
%left '*' '/' /* Multiply and divide (higher precedence than + and -) */

%%

/* GRAMMAR RULES - Define the structure of our language */

/* PROGRAM RULE - Entry point of our grammar */
program:
    translation_unit {
        /* The program is a sequence of top-level items (functions or statements) */
        root = $1;
    }
    ;

/* STATEMENT LIST - Handles multiple statements */
stmt_list:
    stmt { 
        /* Base case: single statement */
        $$ = $1;  /* Pass the statement up as-is */
    }
    | stmt_list stmt { 
        /* Recursive case: list followed by another statement */
        $$ = createStmtList($1, $2);  /* Build linked list of statements */
    }
    ;

/* TOP-LEVEL ITEM LIST - translation_unit consists of either statements or functions */
translation_unit:
    top_item { $$ = $1; }
    | translation_unit top_item { $$ = createStmtList($1, $2); }
    ;

top_item:
    stmt { $$ = $1; }
    | func_decl { $$ = $1; }
    ;

/* STATEMENT TYPES - The three kinds of statements we support */
stmt:
    decl        /* Variable declaration */
    | assign    /* Assignment statement */
    | print_stmt /* Print statement */
    | return_stmt /* Return statement (inside functions) */
    ;

/* DECLARATION RULE - "int x;" or "int arr[10];" */
decl:
    INT ID ';' { 
        /* Create declaration node for a regular variable */
        $$ = createDecl($2);
        free($2);
    }
    | FLOAT ID ';' {
        /* Create declaration node for a float variable */
        $$ = createDeclFloat($2);
        free($2);
    }
    | INT ID '[' NUM ']' ';' { 
        /* Create declaration node for an array */
        $$ = createArrayDecl($2, $4); /* NOTE: New AST function needed */
        free($2);
    }
    ;

/* ASSIGNMENT RULE - "x = expr;" or "arr[i] = expr;" */
assign:
    ID '=' expr ';' { 
        /* Create assignment node for a regular variable */
        $$ = createAssign($1, $3);
        free($1);
    }
    | ID '[' expr ']' '=' expr ';' { 
        /* Create assignment node for an array element */
        $$ = createArrayAssign($1, $3, $6); /* NOTE: New AST function needed */
        free($1);
    }
    ;
    
/* EXPRESSION RULES - Build expression trees */
expr:
    primary { $$ = $1; }
    | expr '+' expr { $$ = createBinOp('+', $1, $3); }
    | expr '-' expr { $$ = createBinOp('-', $1, $3); }
    | expr '*' expr { $$ = createBinOp('*', $1, $3); }
    | expr '/' expr { $$ = createBinOp('/', $1, $3); }
    ;

primary:
    NUM { $$ = createNum($1); }
    | FNUM { $$ = createFloat($1); }
    | ID { $$ = createVar($1); free($1); }
    | ID '[' expr ']' { $$ = createArrayAccess($1, $3); free($1); }
    | ID '(' ')' { $$ = createFuncCall($1, NULL); free($1); }
    | ID '(' arg_list ')' { $$ = createFuncCall($1, $3); free($1); }
    | '(' expr ')' { $$ = $2; }
    ;
arg_list:
    expr { $$ = createArgList($1); }
    | arg_list ',' expr { $$ = appendArg($1, $3); }
    ;
/* PRINT STATEMENT - "print(expr);" */
print_stmt:
    PRINT '(' expr ')' ';' { 
        /* Create print node with expression to print */
        $$ = createPrint($3);  /* $3 is the expression inside parens */
    }
    ;

/* FUNCTION DECLARATION (minimal) */
func_decl:
    FUNC ID '(' ')' '{' stmt_list '}' {
        /* The function body (stmt_list) may include a return statement */
        $$ = createFuncDecl($2, NULL, $6, NULL);
        free($2);
    }
    | FUNC ID '(' param_list ')' '{' stmt_list '}' {
        $$ = createFuncDecl($2, $4, $7, NULL);
        free($2);
    }
    ;

param_list:
    INT ID { $$ = createParamList($2, TYPE_INT); free($2); }
    | FLOAT ID { $$ = createParamList($2, TYPE_FLOAT); free($2); }
    | param_list ',' INT ID { $$ = appendParam($1, $4, TYPE_INT); free($4); }
    | param_list ',' FLOAT ID { $$ = appendParam($1, $4, TYPE_FLOAT); free($4); }
    ;

return_stmt:
    RETURN expr ';' { $$ = createReturn($2); }
    ;

%%

/* ERROR HANDLING - Called by Bison when syntax error detected */
void yyerror(const char* s) {
    fprintf(stderr, "Syntax Error: %s\n", s);
}