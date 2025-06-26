#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdbool.h>

typedef enum {
AST_PROGRAM,
AST_VAR_DECL,
AST_ASSIGNMENT,
AST_IF,
AST_FOR,
AST_FUNCTION_DECL,
AST_FUNCTION_CALL,
AST_INPUT,
AST_PUBLISH,
AST_BINARY_OP,
AST_UNARY_OP,
AST_NUMBER,
AST_STRING,
AST_IDENTIFIER,
AST_STATEMENT_LIST,
AST_EXPR_LIST,
AST_ID_LIST
} ASTNodeType;

typedef struct ASTNode {
ASTNodeType type;
int line_number;  // Line number where this node was created
union {
struct { struct ASTNode* left; struct ASTNode* right; const char* op; } binary;
struct { const char* op; struct ASTNode* operand; } unary;
struct { const char* name; struct ASTNode* value; } var_decl;
struct { const char* name; struct ASTNode* value; } assignment;
struct { struct ASTNode* condition; struct ASTNode* then_branch; struct ASTNode* else_branch; } if_stmt;
struct { struct ASTNode* init; struct ASTNode* cond; struct ASTNode* post; struct ASTNode* body; } for_loop;
struct { const char* name; struct ASTNode* params; struct ASTNode* body; } func_decl;
struct { const char* name; struct ASTNode* args; } func_call;
struct { const char* name; } input;
struct { struct ASTNode* value; } publish;
struct { int value; } number;
struct { char* value; } string;
struct { char* name; } identifier;
struct { struct ASTNode* stmt; struct ASTNode* next; } stmt_list;
struct { struct ASTNode* expr; struct ASTNode* next; } expr_list;
struct { char* id; struct ASTNode* next; } id_list;
};
} ASTNode;

ASTNode* create_var_decl_node(const char* name, ASTNode* value, int line_number);
ASTNode* create_assignment_node(const char* name, ASTNode* value, int line_number);
ASTNode* create_if_node(ASTNode* cond, ASTNode* then_branch, ASTNode* else_branch, int line_number);
ASTNode* create_for_node(ASTNode* init, ASTNode* cond, ASTNode* post, ASTNode* body, int line_number);
ASTNode* create_func_decl_node(const char* name, ASTNode* params, ASTNode* body, int line_number);
ASTNode* create_func_call_node(const char* name, ASTNode* args, int line_number);
ASTNode* create_input_node(const char* name, int line_number);
ASTNode* create_publish_node(ASTNode* value, int line_number);
ASTNode* create_binary_node(const char* op, ASTNode* left, ASTNode* right, int line_number);
ASTNode* create_unary_node(const char* op, ASTNode* operand, int line_number);
ASTNode* create_number_node(int value, int line_number);
ASTNode* create_string_node(const char* value, int line_number);
ASTNode* create_identifier_node(const char* name, int line_number);
ASTNode* create_stmt_list_node(ASTNode* stmt, ASTNode* next, int line_number);
ASTNode* create_expr_list_node(ASTNode* expr, ASTNode* next, int line_number);
ASTNode* create_identifier_list(const char* id, ASTNode* next, int line_number);

void print_ast(ASTNode* node, int indent);
void free_ast(ASTNode* node);

#endif