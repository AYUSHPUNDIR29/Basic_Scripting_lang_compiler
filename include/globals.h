#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdbool.h>
#include "ast.h"    // Contains ASTNode definition
#include "semantic.h" // Contains SymbolTable definition

// Global flag to indicate if a syntax error has occurred
extern bool has_syntax_error;
extern bool has_semantic_error;

// Global root of the AST
extern ASTNode *root;

// Global symbol table
extern SymbolTable *table;

#endif // GLOBALS_H
