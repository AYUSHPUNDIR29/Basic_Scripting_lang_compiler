#include "globals.h"
#include <stddef.h> // For NULL

// Define and initialize global variables
bool has_syntax_error = false;
bool has_semantic_error = false;
ASTNode *root = NULL;
SymbolTable *table = NULL;

// Initialize global variables
ASTNode* ast_root = NULL;

// Define the global syntax error flag
int has_semantic_error_int = 0; 