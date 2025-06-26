#include "globals.h"
#include "lexer.h"
#include "parser.tab.h"
#include "ast.h"
#include "semantic.h"
#include "ir.h"
#include "optimizer.h"
#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Global variables are defined in globals.c, and included via globals.h
extern FILE *yyin;
extern void reset_lexer(void);
extern SemanticError get_last_semantic_error(void);

void print_phase_header(const char *phase_name) {
    printf("==============================\n");
    printf(" Phase: %s\n", phase_name);
    printf("==============================\n");
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror(argv[1]);
            return 1;
        }
    } else {
        yyin = stdin;
    }

    // Initialize symbol table
    table = create_symbol_table();
    bool compilation_has_error = false;

    // 1. Lexical and Syntax Analysis
    print_phase_header("Lexical and Syntax Analysis");
    yyparse();
    reset_lexer(); // Reset for next potential stages if interactive

    if (has_syntax_error) {
        compilation_has_error = true;
    }

    // 2. Syntax Analysis (AST)
    if (!compilation_has_error) {
        if (root) {
            print_phase_header("Syntax Analysis (AST)");
            print_ast(root, 0);
        } else {
            printf("No AST generated.\n");
            compilation_has_error = true; // No AST is an error condition
        }
    }

    // 3. Semantic Analysis
    if (!compilation_has_error) {
        print_phase_header("Semantic Analysis");
        if (!check_semantics(root, table)) {
            compilation_has_error = true;
            SemanticError err = get_last_semantic_error();
            if (err.has_error && err.message) {
                printf("%s\n", err.message); // Print specific semantic error
            }
        } else {
            printf("✔ No semantic errors found.\n");
        }
    }

    IRInstruction* ir_code = NULL;
    // 4. Intermediate Representation (IR)
    if (!compilation_has_error) {
        print_phase_header("Intermediate Representation (IR)");
        ir_code = generate_ir(root);
        if (ir_code) {
            print_ir(ir_code);
        } else {
            printf("No IR generated.\n");
        }
    }
    
    // 5. Optimization
    if (!compilation_has_error && ir_code) {
        print_phase_header("Optimization");
        optimize_ir(ir_code);
    }
    
    // 6. Code Generation
    if (!compilation_has_error && ir_code) {
        print_phase_header("Code Generation");
        generate_code(ir_code);
    }

    if (yyin != stdin) {
        fclose(yyin);
    }
    // free_symbol_table(table);
    // free_ir(ir_code);
    // free_ast(root);

    return compilation_has_error ? 1 : 0;
} 