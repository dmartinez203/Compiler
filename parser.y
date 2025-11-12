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
#include "symtab.h" /* NEW: Include for data types */

/* External declarations for lexer interface */
extern int yylex();      /* Get next token from scanner */
extern int yyparse();    /* Parse the entire input */
extern FILE* yyin;       /* Input file handle */
extern int yydebug;      /* NEW: Fix for multiple definition error */

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
    double fnum;            /* NEW: For float literals */
    char* str;              /* For identifiers */
    struct ASTNode* node;   /* For AST nodes */
}

/* TOKEN DECLARATIONS with their semantic value types */
%token <num> NUM                 /* Number token carries an integer value */
%token <fnum> FLOAT_LITERAL    /* NEW: Float literal token */
%token <str> ID                  /* Identifier token carries a string */
%token INT FLOAT PRINT IF ELSE
%token EQ NE LT LE GT GE
%token AND OR NOT
%token RETURN FUNC /* Keywords have no semantic value */

/* NON-TERMINAL TYPES - Define what type each grammar rule returns */
%type <node> program translation_unit top_item stmt_list stmt decl assign expr primary print_stmt func_decl return_stmt if_stmt condition logical_expr

/* OPERATOR PRECEDENCE AND ASSOCIATIVITY */
%right '!'
%left OR
%left AND
%left EQ NE
%left LT LE GT GE
%left '+' '-'
%left '*' '/'

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

/* STATEMENT TYPES - The kinds of statements we support */
stmt:
    decl        /* Variable declaration */
    | assign    /* Assignment statement */
    | print_stmt /* Print statement */
    | return_stmt /* Return statement (inside functions) */
    | if_stmt   /* If statement */
    | expr ';'  /* Expression statement (e.g., function calls) */
    ;

/* DECLARATION RULE - "int x;" or "int arr[10];" */
decl:
    INT ID ';' { 
        /* UPDATED: Pass type to createDecl */
        $$ = createDecl(TYPE_INT, $2);
        free($2);
    }
    | FLOAT ID ';' {
        /* NEW: float x; */
        $$ = createDecl(TYPE_FLOAT, $2);
        free($2);
    }
    | INT ID '[' NUM ']' ';' { 
        /* UPDATED: Pass type to createArrayDecl */
        $$ = createArrayDecl(TYPE_INT, $2, $4);
        free($2);
    }
    | FLOAT ID '[' NUM ']' ';' {
        /* NEW: float arr[10]; */
        $$ = createArrayDecl(TYPE_FLOAT, $2, $4);
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
        $$ = createArrayAssign($1, $3, $6);
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
    | FLOAT_LITERAL { $$ = createFloatNum($1); } /* NEW: Float literal */
    | ID { $$ = createVar($1); free($1); }
    | ID '[' expr ']' { $$ = createArrayAccess($1, $3); free($1); }
    | ID '(' ')' { $$ = createFuncCall($1, NULL); free($1); }
    | '(' expr ')' { $$ = $2; }
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
    ;

return_stmt:
    RETURN expr ';' { $$ = createReturn($2); }
    ;

/* IF STATEMENT - supports if and if-else */
if_stmt:
    IF '(' logical_expr ')' '{' stmt_list '}' {
        $$ = createIf($3, $6, NULL);
    }
    | IF '(' logical_expr ')' '{' stmt_list '}' ELSE '{' stmt_list '}' {
        $$ = createIf($3, $6, $10);
    }
    ;

/* LOGICAL EXPRESSIONS - Boolean operations with proper precedence */
logical_expr:
    condition { $$ = $1; }
    | logical_expr AND logical_expr { $$ = createLogicOp("&&", $1, $3); }
    | logical_expr OR logical_expr { $$ = createLogicOp("||", $1, $3); }
    | NOT logical_expr { $$ = createUnaryOp("!", $2); }
    | '(' logical_expr ')' { $$ = $2; }
    ;

/* CONDITION - relational expressions */
condition:
    expr EQ expr { $$ = createRelOp("==", $1, $3); }
    | expr NE expr { $$ = createRelOp("!=", $1, $3); }
    | expr LT expr { $$ = createRelOp("<", $1, $3); }
    | expr LE expr { $$ = createRelOp("<=", $1, $3); }
    | expr GT expr { $$ = createRelOp(">", $1, $3); }
    | expr GE expr { $$ = createRelOp(">=", $1, $3); }
    ;

%%

/* ERROR HANDLING - Called by Bison when syntax error detected */
void yyerror(const char* s) {
    fprintf(stderr, "Syntax Error: %s\n", s);
}


