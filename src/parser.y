 %{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "ast.h"
#include "semantic.h"

bool check_semantics(ASTNode* root, SymbolTable* table);

extern int yylex();
extern int yyparse();
extern int line_num;
extern ASTNode* root;  // Declare root as external
extern ASTNode* ast_root;  // Declare ast_root as external
void yyerror(const char* msg);
%}
%union {
    int int_val;
    char* string_val;
    struct ASTNode* ast;
}

%token UNKNOWN
%token <string_val> IDENTIFIER
%token <int_val> NUMBER
%token <string_val> STRING

%token LET FUNCTION IF ELSE FOR TAKE PUBLISH
%token EQ NEQ LEQ GEQ AND OR
%token ASSIGN LT GT PLUS MINUS MUL DIV NOT
%token LPAREN RPAREN LBRACE RBRACE COMMA SEMICOLON

%type <ast> program statement expression primary term factor unary logical_or logical_and equality comparison
%type <ast> variable_declaration assignment if_statement for_loop function_declaration function_call input_statement publish_statement
%type <ast> identifier_list argument_list statement_list for_init

%%  

program
    : statement_list {
        root = $1;
        ast_root = $1;  // Set ast_root to the same value as root
        $$ = $1;  // Return the root node
    }
    ;

statement_list
    : statement {
        // If this is a variable declaration, add it to the front of the list
        if ($1->type == AST_VAR_DECL) {
            $$ = create_stmt_list_node($1, NULL, line_num);
        } else {
            // For other statements, add them to the end
            $$ = create_stmt_list_node($1, NULL, line_num);
        }
    }
    | statement_list statement {
        if ($2->type == AST_VAR_DECL) {
            // For variable declarations, add to the front
            $$ = create_stmt_list_node($2, $1, line_num);
        } else {
            // For other statements, add to the end
            ASTNode* current = $1;
            while (current->stmt_list.next != NULL) {
                current = current->stmt_list.next;
            }
            current->stmt_list.next = create_stmt_list_node($2, NULL, line_num);
            $$ = $1;
        }
    }
    ;

statement
    : variable_declaration SEMICOLON   { $$ = $1; }
    | assignment SEMICOLON             { $$ = $1; }
    | if_statement                     { $$ = $1; }
    | for_loop                         { $$ = $1; }
    | function_declaration             { $$ = $1; }
    | function_call SEMICOLON          { $$ = $1; }
    | input_statement SEMICOLON        { $$ = $1; }
    | publish_statement SEMICOLON      { $$ = $1; }
    ;

variable_declaration
    : LET IDENTIFIER ASSIGN expression { $$ = create_var_decl_node($2, $4, line_num); }
    ;

assignment
    : IDENTIFIER ASSIGN expression     { $$ = create_assignment_node($1, $3, line_num); }
    ;

if_statement
    : IF LPAREN expression RPAREN LBRACE statement_list RBRACE
        { $$ = create_if_node($3, $6, NULL, line_num); }
    | IF LPAREN expression RPAREN LBRACE statement_list RBRACE ELSE LBRACE statement_list RBRACE
        { $$ = create_if_node($3, $6, $10, line_num); }
    ;

for_init
    : variable_declaration { $$ = $1; }
    | assignment { $$ = $1; }
    ;

for_loop
    : FOR LPAREN for_init SEMICOLON expression SEMICOLON assignment RPAREN LBRACE statement_list RBRACE
        { $$ = create_for_node($3, $5, $7, $10, line_num); }
    ;

function_declaration
    : FUNCTION IDENTIFIER LPAREN identifier_list RPAREN LBRACE statement_list RBRACE
        { $$ = create_func_decl_node($2, $4, $7, line_num); }
    | FUNCTION IDENTIFIER LPAREN RPAREN LBRACE statement_list RBRACE
        { $$ = create_func_decl_node($2, NULL, $6, line_num); }
    ;

function_call
    : IDENTIFIER LPAREN argument_list RPAREN
        { $$ = create_func_call_node($1, $3, line_num); }
    | IDENTIFIER LPAREN RPAREN
        { $$ = create_func_call_node($1, NULL, line_num); }
    ;

input_statement
    : TAKE IDENTIFIER { $$ = create_input_node($2, line_num); }
    ;

publish_statement
    : PUBLISH LPAREN expression RPAREN { $$ = create_publish_node($3, line_num); }
    ;

identifier_list
    : IDENTIFIER                         { $$ = create_identifier_list($1, NULL, line_num); }
    | identifier_list COMMA IDENTIFIER  { $$ = create_identifier_list($3, $1, line_num); }
    ;

argument_list
    : expression                        { $$ = create_expr_list_node($1, NULL, line_num); }
    | argument_list COMMA expression    { $$ = create_expr_list_node($3, $1, line_num); }
    ;

expression
    : logical_or { $$ = $1; }
    ;

logical_or
    : logical_and                       { $$ = $1; }
    | logical_or OR logical_and         { $$ = create_binary_node("||", $1, $3, line_num); }
    ;

logical_and
    : equality                          { $$ = $1; }
    | logical_and AND equality          { $$ = create_binary_node("&&", $1, $3, line_num); }
    ;

equality
    : comparison                        { $$ = $1; }
    | equality EQ comparison            { $$ = create_binary_node("==", $1, $3, line_num); }
    | equality NEQ comparison           { $$ = create_binary_node("!=", $1, $3, line_num); }
    ;

comparison
    : term                              { $$ = $1; }
    | comparison LT term                { $$ = create_binary_node("<", $1, $3, line_num); }
    | comparison LEQ term               { $$ = create_binary_node("<=", $1, $3, line_num); }
    | comparison GT term                { $$ = create_binary_node(">", $1, $3, line_num); }
    | comparison GEQ term               { $$ = create_binary_node(">=", $1, $3, line_num); }
    ;

term
    : factor                            { $$ = $1; }
    | term PLUS factor                  { $$ = create_binary_node("+", $1, $3, line_num); }
    | term MINUS factor                 { $$ = create_binary_node("-", $1, $3, line_num); }
    ;

factor
    : unary                             { $$ = $1; }
    | factor MUL unary                  { $$ = create_binary_node("*", $1, $3, line_num); }
    | factor DIV unary                  { $$ = create_binary_node("/", $1, $3, line_num); }
    ;

unary
    : NOT unary                         { $$ = create_unary_node("!", $2, line_num); }
    | MINUS unary                       { $$ = create_unary_node("-", $2, line_num); }
    | primary                           { $$ = $1; }
    ;

primary
    : NUMBER                            { $$ = create_number_node($1, line_num); }
    | STRING                            { $$ = create_string_node($1, line_num); }
    | IDENTIFIER                        { $$ = create_identifier_node($1, line_num); }
    | function_call                     { $$ = $1; }
    | LPAREN expression RPAREN          { $$ = $2; }
    ;

%%  // End grammar

void yyerror(const char* msg) {
    printf("Syntax Error at line %d: %s\n", line_num, msg);
    has_syntax_error = 1;  // Set the syntax error flag
}
