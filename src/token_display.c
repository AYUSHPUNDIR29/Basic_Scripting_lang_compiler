#include "token_display.h"
#include "parser.tab.h"  // Include this to get token definitions
#include <stdio.h>
#include <string.h>

// Structure to track token categories
typedef struct {
    int count;
    const char* name;
} TokenCategory;

static TokenCategory categories[] = {
    {0, "Keywords"},
    {0, "Operators"},
    {0, "Punctuations"},
    {0, "Identifiers"},
    {0, "Literals"},
    {0, NULL}  // Terminator
};

// Map token values to their descriptive names and categories
static const char* get_token_name(int token, int* category) {
    switch (token) {
        // Keywords
        case LET: *category = 0; return "let";
        case FUNCTION: *category = 0; return "function";
        case IF: *category = 0; return "if";
        case ELSE: *category = 0; return "else";
        case FOR: *category = 0; return "for";
        case TAKE: *category = 0; return "take";
        case PUBLISH: *category = 0; return "publish";

        // Comparison Operators
        case EQ: *category = 1; return "==";
        case NEQ: *category = 1; return "!=";
        case LEQ: *category = 1; return "<=";
        case GEQ: *category = 1; return ">=";
        case LT: *category = 1; return "<";
        case GT: *category = 1; return ">";

        // Logical Operators
        case AND: *category = 1; return "&&";
        case OR: *category = 1; return "||";
        case NOT: *category = 1; return "!";

        // Arithmetic Operators
        case PLUS: *category = 1; return "+";
        case MINUS: *category = 1; return "-";
        case MUL: *category = 1; return "*";
        case DIV: *category = 1; return "/";

        // Assignment
        case ASSIGN: *category = 1; return "=";

        // Punctuation
        case LPAREN: *category = 2; return "(";
        case RPAREN: *category = 2; return ")";
        case LBRACE: *category = 2; return "{";
        case RBRACE: *category = 2; return "}";
        case COMMA: *category = 2; return ",";
        case SEMICOLON: *category = 2; return ";";

        // Literals and Identifiers
        case NUMBER: *category = 4; return "number";
        case STRING: *category = 4; return "string";
        case IDENTIFIER: *category = 3; return "identifier";
        default: *category = -1; return "unknown";
    }
}

void init_token_display(void) {
    printf("\n=== Lexical Analysis ===\n");
    // Reset category counts
    for (int i = 0; categories[i].name != NULL; i++) {
        categories[i].count = 0;
    }
}

void display_token(int token, const char* text, int line) {
    (void)line;  // Explicitly mark parameter as unused if we don't need it
    int category;
    const char* token_name = get_token_name(token, &category);
    
    if (category >= 0 && category < 5) {
        categories[category].count++;
        
        // Only print the first occurrence of each category
        if (categories[category].count == 1) {
            printf("\n%s:\n", categories[category].name);
        }
        
        // Print token with appropriate formatting
        switch (token) {
            case IDENTIFIER:
                printf("  %s: %s\n", token_name, text);
                break;
            case NUMBER:
                printf("  %s: %s\n", token_name, text);
                break;
            case STRING:
                printf("  %s: %s\n", token_name, text);
                break;
            default:
                printf("  %s\n", token_name);
                break;
        }
    }
}

void finalize_token_display(void) {
    printf("\nToken Statistics:\n");
    for (int i = 0; categories[i].name != NULL; i++) {
        printf("%s: %d tokens\n", categories[i].name, categories[i].count);
    }
    printf("\n=== End of Lexical Analysis ===\n\n");
} 