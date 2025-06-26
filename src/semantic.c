#include "semantic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global error state
SemanticError last_semantic_error = {false, 0, NULL};

static Symbol* create_symbol(const char* name, VarType type, int line_number) {
    Symbol* sym = (Symbol*)malloc(sizeof(Symbol));
    sym->name = strdup(name);
    sym->type = type;
    sym->line_number = line_number;
    sym->next = NULL;
    return sym;
}

static bool symbol_exists(Symbol* list, const char* name) {
    while (list) {
        if (strcmp(list->name, name) == 0) return true;
        list = list->next;
    }
    return false;
}

static void add_symbol(Symbol** list, const char* name, VarType type, int line_number) {
    Symbol* sym = create_symbol(name, type, line_number);
    sym->next = *list;
    *list = sym;
}

void clear_semantic_error(void) {
    if (last_semantic_error.message) {
        free(last_semantic_error.message);
    }
    last_semantic_error.has_error = false;
    last_semantic_error.line_number = 0;
    last_semantic_error.message = NULL;
}

SemanticError get_last_semantic_error(void) {
    return last_semantic_error;
}

void report_semantic_error(const char* message, const char* name, int line_number) {
    clear_semantic_error();  // Clear any previous error
    
    // Format the error message
    char* full_message;
    if (name) {
        full_message = malloc(strlen(message) + strlen(name) + 50);  // Extra space for formatting
        sprintf(full_message, "Semantic Error at line %d: %s -> '%s'", line_number, message, name);
    } else {
        full_message = malloc(strlen(message) + 50);
        sprintf(full_message, "Semantic Error at line %d: %s", line_number, message);
    }
    
    last_semantic_error.has_error = true;
    last_semantic_error.line_number = line_number;
    last_semantic_error.message = full_message;
}

SymbolTable* create_symbol_table(void) {
    SymbolTable* table = (SymbolTable*)malloc(sizeof(SymbolTable));
    if (!table) {
        fprintf(stderr, "Error: Failed to allocate symbol table\n");
        return NULL;
    }

    // Initialize all fields to NULL
    table->variables = NULL;
    table->functions = NULL;

    // Clear any previous semantic errors
    clear_semantic_error();

    return table;
}

void free_symbol_table(SymbolTable* table) {
    if (!table) return;

    // Free all variables
    Symbol* cur = table->variables;
    while (cur) {
        Symbol* next = cur->next;
        if (cur->name) {
            free((char*)cur->name);
        }
        free(cur);
        cur = next;
    }

    // Free all functions
    cur = table->functions;
    while (cur) {
        Symbol* next = cur->next;
        if (cur->name) {
            free((char*)cur->name);
        }
        free(cur);
        cur = next;
    }

    free(table);
}

void set_variable_type(SymbolTable* table, const char* name, VarType type) {
    Symbol* sym = table->variables;
    while (sym) {
        if (strcmp(sym->name, name) == 0) {
            sym->type = type;
            return;
        }
        sym = sym->next;
    }
}

VarType get_variable_type(SymbolTable* table, const char* name) {
    Symbol* sym = table->variables;
    while (sym) {
        if (strcmp(sym->name, name) == 0) {
            return sym->type;
        }
        sym = sym->next;
    }
    return TYPE_UNKNOWN;
}

bool check_type_compatibility(VarType expected, VarType actual, const char* context, int line_number) {
    if (expected == TYPE_UNKNOWN || actual == TYPE_UNKNOWN) {
        return true;  // Allow unknown types during development
    }
    if (expected != actual) {
        char* type_names[] = {"integer", "string", "unknown"};
        char message[256];
        snprintf(message, sizeof(message), 
                "Type mismatch in %s: expected %s, got %s",
                context, type_names[expected], type_names[actual]);
        report_semantic_error(message, NULL, line_number);
        return false;
    }
    return true;
}

VarType get_expression_type(ASTNode* node, SymbolTable* table) {
    if (!node) return TYPE_UNKNOWN;

    switch (node->type) {
        case AST_NUMBER:
            return TYPE_INT;
        case AST_STRING:
            return TYPE_STRING;
        case AST_IDENTIFIER:
            return get_variable_type(table, node->identifier.name);
        case AST_BINARY_OP:
            // For arithmetic operations, result is integer
            if (strcmp(node->binary.op, "+") == 0 ||
                strcmp(node->binary.op, "-") == 0 ||
                strcmp(node->binary.op, "*") == 0 ||
                strcmp(node->binary.op, "/") == 0) {
                return TYPE_INT;
            }
            // For comparison and logical operations, result is integer (0 or 1)
            return TYPE_INT;
        case AST_UNARY_OP:
            return get_expression_type(node->unary.operand, table);
        default:
            return TYPE_UNKNOWN;
    }
}

bool check_semantics(ASTNode* node, SymbolTable* table) {
    if (!node) return true;

    switch (node->type) {
        case AST_VAR_DECL: {
            // Get the type of the initializing expression
            VarType expr_type = get_expression_type(node->var_decl.value, table);
            
            if (symbol_exists(table->variables, node->var_decl.name)) {
                report_semantic_error("Variable redeclared", node->var_decl.name, node->line_number);
                return false;
            }
            
            // Add variable with its type
            add_symbol(&table->variables, node->var_decl.name, expr_type, node->line_number);
            return check_semantics(node->var_decl.value, table);
        }

        case AST_ASSIGNMENT: {
            VarType var_type = get_variable_type(table, node->assignment.name);
            VarType expr_type = get_expression_type(node->assignment.value, table);
            
            if (!symbol_exists(table->variables, node->assignment.name)) {
                report_semantic_error("Assignment to undeclared variable", 
                                    node->assignment.name, node->line_number);
                return false;
            }
            
            if (!check_type_compatibility(var_type, expr_type, "assignment", node->line_number)) {
                return false;
            }
            return check_semantics(node->assignment.value, table);
        }

        case AST_INPUT: {
            if (!symbol_exists(table->variables, node->input.name)) {
                report_semantic_error("Input to undeclared variable", node->input.name, node->line_number);
                return false;
            }
            // Note: Actual type checking of input will happen at runtime
            return true;
        }

        case AST_PUBLISH: {
            // Allow publishing of any type, but track the type for runtime
            return check_semantics(node->publish.value, table);
        }

        case AST_IDENTIFIER:
            if (!symbol_exists(table->variables, node->identifier.name)) {
                report_semantic_error("Use of undeclared variable", node->identifier.name, node->line_number);
                return false;
            }
            return true;

        case AST_FUNCTION_DECL:
            if (symbol_exists(table->functions, node->func_decl.name)) {
                report_semantic_error("Function redeclared", node->func_decl.name, node->line_number);
                return false;
            }
            // Add function with TYPE_UNKNOWN as functions don't have a specific type
            add_symbol(&table->functions, node->func_decl.name, TYPE_UNKNOWN, node->line_number);

            {
                SymbolTable* localTable = create_symbol_table();

                // Register parameters with TYPE_UNKNOWN initially
                ASTNode* param = node->func_decl.params;
                while (param) {
                    if (symbol_exists(localTable->variables, param->id_list.id)) {
                        report_semantic_error("Duplicate parameter", param->id_list.id, node->line_number);
                        free_symbol_table(localTable);
                        return false;
                    }
                    // Add parameter with TYPE_UNKNOWN as we don't know its type yet
                    add_symbol(&localTable->variables, param->id_list.id, TYPE_UNKNOWN, node->line_number);
                    param = param->id_list.next;
                }

                bool result = check_semantics(node->func_decl.body, localTable);
                free_symbol_table(localTable);
                return result;
            }

        case AST_FUNCTION_CALL:
            if (!symbol_exists(table->functions, node->func_call.name)) {
                report_semantic_error("Call to undeclared function", node->func_call.name, node->line_number);
                return false;
            }
            return check_semantics(node->func_call.args, table);

        case AST_IF:
            return check_semantics(node->if_stmt.condition, table) &&
                   check_semantics(node->if_stmt.then_branch, table) &&
                   (!node->if_stmt.else_branch || check_semantics(node->if_stmt.else_branch, table));

        case AST_FOR:
            return check_semantics(node->for_loop.init, table) &&
                   check_semantics(node->for_loop.cond, table) &&
                   check_semantics(node->for_loop.post, table) &&
                   check_semantics(node->for_loop.body, table);

        case AST_STATEMENT_LIST:
            return check_semantics(node->stmt_list.stmt, table) &&
                   check_semantics(node->stmt_list.next, table);

        case AST_EXPR_LIST:
            return check_semantics(node->expr_list.expr, table) &&
                   check_semantics(node->expr_list.next, table);

        default:
            return true;
    }
}

// Alias for check_semantics
bool analyze_ast(ASTNode* root, SymbolTable* table) {
    return check_semantics(root, table);
}

// Print symbol table contents
void print_symbol_table(SymbolTable* table) {
    if (!table) return;

    printf("\n=== Symbol Table ===\n");
    
    // Print variables
    printf("Variables:\n");
    Symbol* var = table->variables;
    while (var) {
        const char* type_str = var->type == TYPE_INT ? "int" :
                              var->type == TYPE_STRING ? "string" : "unknown";
        printf("  %s (%s) at line %d\n", var->name, type_str, var->line_number);
        var = var->next;
    }
    
    // Print functions
    printf("\nFunctions:\n");
    Symbol* func = table->functions;
    while (func) {
        printf("  %s at line %d\n", func->name, func->line_number);
        func = func->next;
    }
    
    printf("=== End Symbol Table ===\n\n");
}