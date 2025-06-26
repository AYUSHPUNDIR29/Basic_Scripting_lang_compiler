#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include <stdbool.h>

// Define types for variables
typedef enum {
    TYPE_INT,
    TYPE_STRING,
    TYPE_UNKNOWN
} VarType;

typedef struct Symbol {
    const char* name;
    VarType type;        // Add type information
    int line_number;     // Line number where symbol is defined
    struct Symbol* next;
} Symbol;

typedef struct SymbolTable {
    Symbol* variables;
    Symbol* functions;
} SymbolTable;

// Error reporting structure
typedef struct {
    bool has_error;
    int line_number;
    char* message;
} SemanticError;

// Global error state
extern SemanticError last_semantic_error;

// Function declarations
SymbolTable* create_symbol_table(void);
void free_symbol_table(SymbolTable* table);
bool check_semantics(ASTNode* node, SymbolTable* table);
bool analyze_ast(ASTNode* root, SymbolTable* table);  // Alias for check_semantics
void print_symbol_table(SymbolTable* table);  // Print symbol table contents
void report_semantic_error(const char* message, const char* name, int line_number);
SemanticError get_last_semantic_error(void);
void clear_semantic_error(void);

// Type checking functions
VarType get_expression_type(ASTNode* node, SymbolTable* table);
bool check_type_compatibility(VarType expected, VarType actual, const char* context, int line_number);
void set_variable_type(SymbolTable* table, const char* name, VarType type);
VarType get_variable_type(SymbolTable* table, const char* name);

#endif