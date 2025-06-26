#include "ast.h"
#include <stdlib.h>
#include <string.h>

static ASTNode* alloc_node(ASTNodeType type) {
ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
node->type = type;
return node;
}

ASTNode* create_var_decl_node(const char* name, ASTNode* value, int line_number) {
ASTNode* node = alloc_node(AST_VAR_DECL);
node->line_number = line_number;
node->var_decl.name = strdup(name);
node->var_decl.value = value;
return node;
}

ASTNode* create_assignment_node(const char* name, ASTNode* value, int line_number) {
ASTNode* node = alloc_node(AST_ASSIGNMENT);
node->line_number = line_number;
node->assignment.name = strdup(name);
node->assignment.value = value;
return node;
}

ASTNode* create_if_node(ASTNode* cond, ASTNode* then_branch, ASTNode* else_branch, int line_number) {
ASTNode* node = alloc_node(AST_IF);
node->line_number = line_number;
node->if_stmt.condition = cond;
node->if_stmt.then_branch = then_branch;
node->if_stmt.else_branch = else_branch;
return node;
}

ASTNode* create_for_node(ASTNode* init, ASTNode* cond, ASTNode* post, ASTNode* body, int line_number) {
ASTNode* node = alloc_node(AST_FOR);
node->line_number = line_number;
node->for_loop.init = init;
node->for_loop.cond = cond;
node->for_loop.post = post;
node->for_loop.body = body;
return node;
}

ASTNode* create_func_decl_node(const char* name, ASTNode* params, ASTNode* body, int line_number) {
ASTNode* node = alloc_node(AST_FUNCTION_DECL);
node->line_number = line_number;
node->func_decl.name = strdup(name);
node->func_decl.params = params;
node->func_decl.body = body;
return node;
}

ASTNode* create_func_call_node(const char* name, ASTNode* args, int line_number) {
ASTNode* node = alloc_node(AST_FUNCTION_CALL);
node->line_number = line_number;
node->func_call.name = strdup(name);
node->func_call.args = args;
return node;
}

ASTNode* create_input_node(const char* name, int line_number) {
ASTNode* node = alloc_node(AST_INPUT);
node->line_number = line_number;
node->input.name = strdup(name);
return node;
}

ASTNode* create_publish_node(ASTNode* value, int line_number) {
ASTNode* node = alloc_node(AST_PUBLISH);
node->line_number = line_number;
node->publish.value = value;
return node;
}

ASTNode* create_binary_node(const char* op, ASTNode* left, ASTNode* right, int line_number) {
ASTNode* node = alloc_node(AST_BINARY_OP);
node->line_number = line_number;
node->binary.op = strdup(op);
node->binary.left = left;
node->binary.right = right;
return node;
}

ASTNode* create_unary_node(const char* op, ASTNode* operand, int line_number) {
ASTNode* node = alloc_node(AST_UNARY_OP);
node->line_number = line_number;
node->unary.op = strdup(op);
node->unary.operand = operand;
return node;
}

ASTNode* create_number_node(int value, int line_number) {
ASTNode* node = alloc_node(AST_NUMBER);
node->line_number = line_number;
node->number.value = value;
return node;
}

ASTNode* create_string_node(const char* value, int line_number) {
ASTNode* node = alloc_node(AST_STRING);
node->line_number = line_number;
node->string.value = strdup(value);
return node;
}

ASTNode* create_identifier_node(const char* name, int line_number) {
ASTNode* node = alloc_node(AST_IDENTIFIER);
node->line_number = line_number;
node->identifier.name = strdup(name);
return node;
}

ASTNode* create_stmt_list_node(ASTNode* stmt, ASTNode* next, int line_number) {
ASTNode* node = alloc_node(AST_STATEMENT_LIST);
node->line_number = line_number;
node->stmt_list.stmt = stmt;
node->stmt_list.next = next;
return node;
}

ASTNode* create_expr_list_node(ASTNode* expr, ASTNode* next, int line_number) {
ASTNode* node = alloc_node(AST_EXPR_LIST);
node->line_number = line_number;
node->expr_list.expr = expr;
node->expr_list.next = next;
return node;
}

ASTNode* create_identifier_list(const char* id, ASTNode* next, int line_number) {
ASTNode* node = alloc_node(AST_ID_LIST);
node->line_number = line_number;
node->id_list.id = strdup(id);
node->id_list.next = next;
return node;
}

void print_ast(ASTNode* node, int indent) {
    if (!node) return;

    // Print indentation
    for (int i = 0; i < indent; i++) printf("|   ");

    switch (node->type) {
        case AST_VAR_DECL:
            printf("Variable Declaration: %s\n", node->var_decl.name);
            for (int i = 0; i < indent; i++) printf("|   ");
            printf("___ Value: ");
            print_ast(node->var_decl.value, indent + 1);
            break;
        case AST_ASSIGNMENT:
            printf("Assignment: %s\n", node->assignment.name);
            for (int i = 0; i < indent; i++) printf("|   ");
            printf("___ Value: ");
            print_ast(node->assignment.value, indent + 1);
            break;
        case AST_IF:
            printf("If Statement\n");
            for (int i = 0; i < indent; i++) printf("|   ");
            printf("___ Condition: ");
            print_ast(node->if_stmt.condition, indent + 1);
            for (int i = 0; i < indent; i++) printf("|   ");
            printf("___ Then Branch:\n");
            print_ast(node->if_stmt.then_branch, indent + 2);
            if (node->if_stmt.else_branch) {
                for (int i = 0; i < indent; i++) printf("|   ");
                printf("___ Else Branch:\n");
                print_ast(node->if_stmt.else_branch, indent + 2);
            }
            break;
        case AST_FOR:
            printf("For Loop\n");
            for (int i = 0; i < indent; i++) printf("|   ");
            printf("___ Initialization: ");
            print_ast(node->for_loop.init, indent + 1);
            for (int i = 0; i < indent; i++) printf("|   ");
            printf("___ Condition: ");
            print_ast(node->for_loop.cond, indent + 1);
            for (int i = 0; i < indent; i++) printf("|   ");
            printf("___ Update: ");
            print_ast(node->for_loop.post, indent + 1);
            for (int i = 0; i < indent; i++) printf("|   ");
            printf("___ Body:\n");
            print_ast(node->for_loop.body, indent + 2);
            break;
        case AST_FUNCTION_DECL:
            printf("Function: %s\n", node->func_decl.name);
            for (int i = 0; i < indent; i++) printf("|   ");
            printf("___ Parameters: ");
            print_ast(node->func_decl.params, indent + 1);
            for (int i = 0; i < indent; i++) printf("|   ");
            printf("___ Body:\n");
            print_ast(node->func_decl.body, indent + 2);
            break;
        case AST_FUNCTION_CALL:
            printf("Call: %s\n", node->func_call.name);
            for (int i = 0; i < indent; i++) printf("|   ");
            printf("___ Arguments: ");
            print_ast(node->func_call.args, indent + 1);
            break;
        case AST_INPUT:
            printf("Input: %s\n", node->input.name);
            break;
        case AST_PUBLISH:
            printf("Publish\n");
            for (int i = 0; i < indent; i++) printf("|   ");
            printf("___ Value: ");
            print_ast(node->publish.value, indent + 1);
            break;
        case AST_BINARY_OP:
            printf("Binary Operation: %s\n", node->binary.op);
            for (int i = 0; i < indent; i++) printf("|   ");
            printf("___ Left: ");
            print_ast(node->binary.left, indent + 1);
            for (int i = 0; i < indent; i++) printf("|   ");
            printf("___ Right: ");
            print_ast(node->binary.right, indent + 1);
            break;
        case AST_UNARY_OP:
            printf("Unary Operation: %s\n", node->unary.op);
            for (int i = 0; i < indent; i++) printf("|   ");
            printf("___ Operand: ");
            print_ast(node->unary.operand, indent + 1);
            break;
        case AST_NUMBER:
            printf("Number: %d\n", node->number.value);
            break;
        case AST_STRING:
            printf("String: \"%s\"\n", node->string.value);
            break;
        case AST_IDENTIFIER:
            printf("Identifier: %s\n", node->identifier.name);
            break;
        case AST_STATEMENT_LIST:
            print_ast(node->stmt_list.stmt, indent);
            if (node->stmt_list.next) {
                print_ast(node->stmt_list.next, indent);
            }
            break;
        case AST_EXPR_LIST:
            if (indent == 0) printf("Arguments:\n");
            for (int i = 0; i < indent; i++) printf("|   ");
            printf("___ ");
            print_ast(node->expr_list.expr, indent + 1);
            if (node->expr_list.next) {
                print_ast(node->expr_list.next, indent);
            }
            break;
        case AST_ID_LIST:
            printf("Identifier List\n");
            for (int i = 0; i < indent; i++) printf("|   ");
            printf("___ Identifier: %s\n", node->id_list.id);
            if (node->id_list.next) {
                for (int i = 0; i < indent; i++) printf("|   ");
                printf("___ Next: ");
                print_ast(node->id_list.next, indent + 1);
            }
            break;
        default:
            printf("Unknown Node Type\n");
            break;
    }
}

void free_ast(ASTNode* node) {
    if (!node) return;

    // Recursively free child nodes based on node type
    switch (node->type) {
        case AST_VAR_DECL:
            free((char*)node->var_decl.name);
            free_ast(node->var_decl.value);
            break;

        case AST_ASSIGNMENT:
            free((char*)node->assignment.name);
            free_ast(node->assignment.value);
            break;

        case AST_IF:
            free_ast(node->if_stmt.condition);
            free_ast(node->if_stmt.then_branch);
            free_ast(node->if_stmt.else_branch);
            break;

        case AST_FOR:
            free_ast(node->for_loop.init);
            free_ast(node->for_loop.cond);
            free_ast(node->for_loop.post);
            free_ast(node->for_loop.body);
            break;

        case AST_FUNCTION_DECL:
            free((char*)node->func_decl.name);
            free_ast(node->func_decl.params);
            free_ast(node->func_decl.body);
            break;

        case AST_FUNCTION_CALL:
            free((char*)node->func_call.name);
            free_ast(node->func_call.args);
            break;

        case AST_INPUT:
            free((char*)node->input.name);
            break;

        case AST_PUBLISH:
            free_ast(node->publish.value);
            break;

        case AST_BINARY_OP:
            free((char*)node->binary.op);
            free_ast(node->binary.left);
            free_ast(node->binary.right);
            break;

        case AST_UNARY_OP:
            free((char*)node->unary.op);
            free_ast(node->unary.operand);
            break;

        case AST_STRING:
            free(node->string.value);
            break;

        case AST_IDENTIFIER:
            free((char*)node->identifier.name);
            break;

        case AST_STATEMENT_LIST:
            free_ast(node->stmt_list.stmt);
            free_ast(node->stmt_list.next);
            break;

        case AST_EXPR_LIST:
            free_ast(node->expr_list.expr);
            free_ast(node->expr_list.next);
            break;

        case AST_ID_LIST:
            free((char*)node->id_list.id);
            free_ast(node->id_list.next);
            break;

        default:
            break;
    }

    // Finally, free the node itself
    free(node);
}