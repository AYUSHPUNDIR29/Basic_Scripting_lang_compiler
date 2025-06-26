#include "codegen.h"
#include "ir.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// This is a forward declaration, as print_operand is in ir.c but useful for debugging here
void print_operand(IROperand op);

// --- Main Code Generation Logic ---

void generate_code(IRInstruction* head) {
    for (IRInstruction* current = head; current; current = current->next) {
        switch (current->op) {
            case IR_ASSIGN:
                printf("    MOV R1, ");
                if (current->arg1.type == OP_CONSTANT) {
                    printf("#");
                }
                print_operand(current->arg1);
                printf("\n");
                printf("    MOV ");
                print_operand(current->result);
                printf(", R1\n");
                break;

            case IR_ADD:
            case IR_SUB:
            case IR_MUL:
            case IR_DIV: {
                char* op_str = "ADD";
                if (current->op == IR_SUB) op_str = "SUB";
                else if (current->op == IR_MUL) op_str = "MUL";
                else if (current->op == IR_DIV) op_str = "DIV";
                
                printf("    MOV R1, ");
                if (current->arg1.type == OP_CONSTANT) printf("#");
                print_operand(current->arg1);
                printf("\n");
                printf("    MOV R2, ");
                if (current->arg2.type == OP_CONSTANT) printf("#");
                print_operand(current->arg2);
                printf("\n");
                printf("    %s R1, R2\n", op_str);
                printf("    MOV ");
                print_operand(current->result);
                printf(", R1\n");
                break;
            }
            case IR_LABEL:
                print_operand(current->result);
                printf(":\n");
                break;
            case IR_GOTO:
                printf("    JMP ");
                print_operand(current->result);
                printf("\n");
                break;
            case IR_IF_GOTO:
                printf("    CMP ");
                print_operand(current->arg1);
                printf(", #0\n");
                printf("    JNE ");
                print_operand(current->result);
                printf("\n");
                break;
            default:
                break;
        }
    }
} 