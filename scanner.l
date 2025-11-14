%{
/* LEXICAL ANALYZER (SCANNER)
 * This is the first phase of compilation - breaking input into tokens
 * Flex generates a scanner from these rules that reads character by character
 */
#include <stdio.h>
#include <stdlib.h>
#include "parser.tab.h"  /* Token definitions from Bison */
%}

/* Flex options to suppress warnings for unused functions */
%option nounput
%option noinput

%%

"int"           { return INT; }
"float"         { return FLOAT; }
"func"          { return FUNC; }
"print"         { return PRINT; }
"write"         { return WRITE; }
"writeln"       { return WRITELN; }
"return"        { return RETURN; }
"if"            { return IF; }
"when"          { return IF; }
"else"          { return ELSE; }
"while"         { return WHILE; }

[a-zA-Z_][a-zA-Z0-9_]* { 
    yylval.str = strdup(yytext);
    return ID; 
}

[0-9][0-9]*\.[0-9][0-9]* { 
    yylval.fnum = atof(yytext); 
    return FLOAT_LITERAL; 
}

'.'             { 
    yylval.num = (int)yytext[1]; 
    return NUM; 
}

[0-9][0-9]* { 
    yylval.num = atoi(yytext);
    return NUM; 
}

"+"             { return '+'; }
"-"             { return '-'; }
"*"             { return '*'; }
"/"             { return '/'; }
"="             { return '='; }
";"             { return ';'; }
","             { return ','; }
"("             { return '('; }
")"             { return ')'; }
"["             { return '['; }
"]"             { return ']'; }
"{"             { return '{'; }
"}"             { return '}'; }
"=="            { return EQ; }
"!="            { return NE; }
"<"             { return LT; }
"<="            { return LE; }
">"             { return GT; }
">="            { return GE; }
"&&"            { return AND; }
"||"            { return OR; }
"!"             { return NOT; }

[ \t\n]         { }

.               { printf("Lexical Error: Unknown character '%c'\n", *yytext); }

%%

/* Required by Flex - signals end of input */
int yywrap() {
    return 1;  /* 1 means no more input files */
}

