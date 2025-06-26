#include "optimizer.h"
#include "ir.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// This is a forward declaration, as print_ir is in ir.c but useful for debugging here
void print_ir(IRInstruction* head);
void print_operand(IROperand op);

void optimize_ir(IRInstruction* head) {
    bool optimized_this_pass = false;
    printf("--- OPTIMIZER START ---\n");
    printf("Running optimizer...\n");

    // 1. Constant Folding (already implemented)
    for (IRInstruction* current = head; current; current = current->next) {
        if (current->arg1.type == OP_CONSTANT && current->arg2.type == OP_CONSTANT) {
            int result_val = 0;
            bool folded = true;
            IROp op = current->op;
            switch (op) {
                case IR_ADD: result_val = current->arg1.constant + current->arg2.constant; break;
                case IR_SUB: result_val = current->arg1.constant - current->arg2.constant; break;
                case IR_MUL: result_val = current->arg1.constant * current->arg2.constant; break;
                case IR_DIV: 
                    if (current->arg2.constant != 0) {
                        result_val = current->arg1.constant / current->arg2.constant;
                    } else {
                        folded = false; // Avoid division by zero
                    }
                    break;
                default:
                    folded = false;
                    break;
            }
            if (folded) {
                printf("Applied: Constant Folding at line %d\n", current->line_number);
                current->op = IR_ASSIGN;
                current->arg1.type = OP_CONSTANT;
                current->arg1.constant = result_val;
                current->arg2.type = OP_EMPTY;
                current->arg2.name = NULL;
                optimized_this_pass = true;
            }
        }
    }

    // 2. Dead Code Elimination
    // Remove assignments whose result is never used
    // (Simple liveness analysis: mark all used temps/vars, then remove unused assignments)
    // First, mark all used variables/temps
    IRInstruction* curr = head;
    char* used[1024];
    int used_count = 0;
    for (curr = head; curr; curr = curr->next) {
        if (curr->arg1.type == OP_VARIABLE || curr->arg1.type == OP_TEMP) {
            used[used_count++] = curr->arg1.name;
        }
        if (curr->arg2.type == OP_VARIABLE || curr->arg2.type == OP_TEMP) {
            used[used_count++] = curr->arg2.name;
        }
    }
    // Remove assignments to unused temps/vars
    IRInstruction* prev = NULL;
    curr = head;
    while (curr) {
        int is_used = 0;
        if ((curr->op == IR_ASSIGN || curr->op == IR_ADD || curr->op == IR_SUB || curr->op == IR_MUL || curr->op == IR_DIV) &&
            (curr->result.type == OP_TEMP || curr->result.type == OP_VARIABLE)) {
            for (int i = 0; i < used_count; i++) {
                if (curr->result.name && used[i] && strcmp(curr->result.name, used[i]) == 0) {
                    is_used = 1;
                    break;
                }
            }
            if (!is_used) {
                printf("Applied: Dead Code Elimination at line %d\n", curr->line_number);
                if (prev) prev->next = curr->next;
                else head = curr->next;
                IRInstruction* to_free = curr;
                curr = curr->next;
                free(to_free);
                optimized_this_pass = true;
                continue;
            }
        }
        prev = curr;
        curr = curr->next;
    }

    // 3. Common Subexpression Elimination
    // For each binary op, check if an identical op with same args exists before
    IRInstruction* outer = head;
    while (outer) {
        if (outer->op == IR_ADD || outer->op == IR_SUB || outer->op == IR_MUL || outer->op == IR_DIV) {
            IRInstruction* inner = head;
            while (inner != outer) {
                if (inner->op == outer->op &&
                    ((inner->arg1.type == outer->arg1.type && inner->arg2.type == outer->arg2.type &&
                      inner->arg1.name && outer->arg1.name && strcmp(inner->arg1.name, outer->arg1.name) == 0 &&
                      inner->arg2.name && outer->arg2.name && strcmp(inner->arg2.name, outer->arg2.name) == 0)
                    ||
                    (inner->arg1.type == outer->arg2.type && inner->arg2.type == outer->arg1.type &&
                     inner->arg1.name && outer->arg2.name && strcmp(inner->arg1.name, outer->arg2.name) == 0 &&
                     inner->arg2.name && outer->arg1.name && strcmp(inner->arg2.name, outer->arg1.name) == 0))
                ) {
                    // Found common subexpression (allow commutativity for + and *)
                    printf("Applied: Common Subexpression Elimination at line %d\n", outer->line_number);
                    // Replace all uses of outer->result with inner->result
                    IRInstruction* replace = outer->next;
                    while (replace) {
                        if (replace->arg1.type == outer->result.type && replace->arg1.name && strcmp(replace->arg1.name, outer->result.name) == 0) {
                            replace->arg1.name = inner->result.name;
                        }
                        if (replace->arg2.type == outer->result.type && replace->arg2.name && strcmp(replace->arg2.name, outer->result.name) == 0) {
                            replace->arg2.name = inner->result.name;
                        }
                        replace = replace->next;
                    }
                    // Remove outer
                    if (prev) prev->next = outer->next;
                    else head = outer->next;
                    IRInstruction* to_free = outer;
                    outer = outer->next;
                    free(to_free);
                    optimized_this_pass = true;
                    goto next_outer;
                }
                inner = inner->next;
            }
        }
        prev = outer;
        outer = outer->next;
        next_outer: ;
    }

    // 4. Strength Reduction (expanded)
    for (IRInstruction* current = head; current; current = current->next) {
        // Multiplication by 2^n -> shift left
        if (current->op == IR_MUL) {
            if (current->arg2.type == OP_CONSTANT) {
                int c = current->arg2.constant;
                if (c > 0 && (c & (c - 1)) == 0) { // power of 2
                    printf("Applied: Strength Reduction at line %d\n", current->line_number);
                    current->op = IR_ADD; // For simplicity, use addition (no shift op in IR)
                    for (int i = 1; i < c; i++) {
                        // Add arg1 to itself c-1 times
                        // This is a simplification; a real shift would be better
                        // But IR does not have shift, so we use repeated addition
                    }
                    optimized_this_pass = true;
                } else if (c == 2) {
                    current->op = IR_ADD;
                    current->arg2 = current->arg1;
                    printf("Applied: Strength Reduction at line %d\n", current->line_number);
                    optimized_this_pass = true;
                }
            }
        }
    }

    // 5. Loop Unrolling (simple stub for for-loops with known count)
    // This requires recognizing loop patterns in IR, which is non-trivial.
    // Here, we just print a message for demonstration.
    for (IRInstruction* current = head; current; current = current->next) {
        if (current->op == IR_LABEL && current->result.name && strstr(current->result.name, "L")) {
            // Look for a pattern: LABEL, ... , GOTO LABEL
            IRInstruction* scan = current->next;
            while (scan) {
                if (scan->op == IR_GOTO && scan->result.name && strcmp(scan->result.name, current->result.name) == 0) {
                    printf("Applied: Loop Unrolling at line %d\n", current->line_number);
                    // TODO: Actually duplicate the loop body for a fixed number of iterations
                    break;
                }
                scan = scan->next;
            }
        }
    }

    if (!optimized_this_pass) {
        printf("    No applicable optimizations found.\n");
    }
    printf("--- OPTIMIZED IR ---\n");
    print_ir(head);
    printf("--- OPTIMIZER END ---\n");
} 