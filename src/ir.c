#include "ir.h"
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// --- Globals for IR Generation ---
static int temp_count = 0;
static int label_count = 0;

// Simple hash for binary expressions
#define MAX_EXPR_CACHE 128
static struct {
    IROp op;
    char left[32];
    char right[32];
    char temp[16];
    bool used;
} expr_cache[MAX_EXPR_CACHE];
static int expr_cache_count = 0;

// --- Helper Functions ---
char* new_temp() {
    char* temp = malloc(16);
    sprintf(temp, "t%d", temp_count++);
    return temp;
}

char* new_label() {
    char* label = malloc(16);
    sprintf(label, "L%d", label_count++);
    return label;
}

// Deep copy for IROperand
IROperand deep_copy_operand(const IROperand* src) {
    IROperand dest = *src;
    if ((src->type == OP_VARIABLE || src->type == OP_TEMP || src->type == OP_LABEL) && src->name) {
        dest.name = strdup(src->name);
    }
    return dest;
}

IRInstruction* create_ir_instruction(IROp op, int line_number) {
    IRInstruction* instr = (IRInstruction*)calloc(1, sizeof(IRInstruction));
    instr->op = op;
    instr->line_number = line_number;
    return instr;
}

IRInstruction* append_ir(IRInstruction* a, IRInstruction* b) {
    if (!a) return b;
    if (!b) return a;
    IRInstruction* current = a;
    while (current->next) {
        current = current->next;
    }
    current->next = b;
    return a;
}

// --- Forward Declarations for Recursive Generation ---
IRInstruction* generate_ir_for_expression(ASTNode* node, IROperand* result_operand);

// --- Main IR Generation Logic ---
IRInstruction* generate_ir_for_statement(ASTNode* node) {
    if (!node) return NULL;

    switch (node->type) {
        case AST_VAR_DECL:
        case AST_ASSIGNMENT: {
            IROperand rhs_operand;
            IRInstruction* expr_code = generate_ir_for_expression(node->var_decl.value, &rhs_operand);

            IRInstruction* assign_code = create_ir_instruction(IR_ASSIGN, node->line_number);
            assign_code->result.type = OP_VARIABLE;
            assign_code->result.name = strdup(node->type == AST_VAR_DECL ? node->var_decl.name : node->assignment.name);
            assign_code->arg1 = rhs_operand;

            return append_ir(expr_code, assign_code);
        }
        case AST_IF: {
            IROperand condition_operand;
            IRInstruction* cond_code = generate_ir_for_expression(node->if_stmt.condition, &condition_operand);

            char* true_label_name = new_label();
            char* end_label_name = new_label();

            IRInstruction* if_goto_code = create_ir_instruction(IR_IF_GOTO, node->line_number);
            if_goto_code->arg1 = condition_operand;
            if_goto_code->result.type = OP_LABEL;
            if_goto_code->result.name = true_label_name;
            
            IRInstruction* then_code = generate_ir_for_statement(node->if_stmt.then_branch);
            
            IRInstruction* true_label_code = create_ir_instruction(IR_LABEL, node->line_number);
            true_label_code->result.type = OP_LABEL;
            true_label_code->result.name = strdup(true_label_name);
            
            IRInstruction* end_label_code = create_ir_instruction(IR_LABEL, node->line_number);
            end_label_code->result.type = OP_LABEL;
            end_label_code->result.name = end_label_name;

            IRInstruction* final_code = cond_code;
            final_code = append_ir(final_code, if_goto_code);
            final_code = append_ir(final_code, true_label_code);
            final_code = append_ir(final_code, then_code);
            final_code = append_ir(final_code, end_label_code);
            
            return final_code;
        }
        case AST_STATEMENT_LIST: {
            IRInstruction* stmt_code = generate_ir_for_statement(node->stmt_list.stmt);
            IRInstruction* next_code = generate_ir_for_statement(node->stmt_list.next);
            return append_ir(stmt_code, next_code);
        }
        default:
            // Handle other statements like function calls if they can be standalone
            return NULL;
    }
}

static const char* find_expr_cache(IROp op, const char* left, const char* right) {
    for (int i = 0; i < expr_cache_count; ++i) {
        if (expr_cache[i].used && expr_cache[i].op == op &&
            strcmp(expr_cache[i].left, left) == 0 && strcmp(expr_cache[i].right, right) == 0) {
            return expr_cache[i].temp;
        }
    }
    return NULL;
}

static void add_expr_cache(IROp op, const char* left, const char* right, const char* temp) {
    if (expr_cache_count < MAX_EXPR_CACHE) {
        expr_cache[expr_cache_count].op = op;
        strncpy(expr_cache[expr_cache_count].left, left, 31);
        strncpy(expr_cache[expr_cache_count].right, right, 31);
        strncpy(expr_cache[expr_cache_count].temp, temp, 15);
        expr_cache[expr_cache_count].used = true;
        expr_cache_count++;
    }
}

IRInstruction* generate_ir_for_expression(ASTNode* node, IROperand* result_operand) {
    if (!node) return NULL;

    // The result of ANY expression goes into a new temporary. This ensures true TAC.
    // result_operand->type = OP_TEMP;
    // result_operand->name = new_temp();

    switch(node->type) {
        case AST_NUMBER: {
            result_operand->type = OP_TEMP;
            result_operand->name = new_temp();
            IRInstruction* code = create_ir_instruction(IR_ASSIGN, node->line_number);
            code->result = *result_operand;
            code->arg1.type = OP_CONSTANT;
            code->arg1.constant = node->number.value;
            return code; 
        }
        case AST_IDENTIFIER: {
            result_operand->type = OP_TEMP;
            result_operand->name = new_temp();
            IRInstruction* code = create_ir_instruction(IR_ASSIGN, node->line_number);
            code->result = *result_operand;
            code->arg1.type = OP_VARIABLE;
            code->arg1.name = strdup(node->identifier.name);
            return code;
        }
        case AST_BINARY_OP: {
            IROperand op1, op2;
            IRInstruction* code1 = generate_ir_for_expression(node->binary.left, &op1);
            IRInstruction* code2 = generate_ir_for_expression(node->binary.right, &op2);

            char* op_str = (char*)node->binary.op;
            IROp op_type = IR_ASSIGN;
            if (strcmp(op_str, "+") == 0) op_type = IR_ADD;
            else if (strcmp(op_str, "-") == 0) op_type = IR_SUB;
            else if (strcmp(op_str, "*") == 0) op_type = IR_MUL;
            else if (strcmp(op_str, "/") == 0) op_type = IR_DIV;

            // Memoization: check if this expression was already computed
            const char* cached = find_expr_cache(op_type, op1.name, op2.name);
            if (cached) {
                result_operand->type = OP_TEMP;
                result_operand->name = strdup(cached);
                return append_ir(code1, code2); // No new instruction needed
            }

            result_operand->type = OP_TEMP;
            result_operand->name = new_temp();
            add_expr_cache(op_type, op1.name, op2.name, result_operand->name);

            IRInstruction* code3 = create_ir_instruction(op_type, node->line_number);
            code3->result = *result_operand;
            code3->arg1 = deep_copy_operand(&op1);
            code3->arg2 = deep_copy_operand(&op2);
            return append_ir(append_ir(code1, code2), code3);
        }
        default:
            return NULL;
    }
}

IRInstruction* generate_ir(ASTNode* node) {
    temp_count = 0;
    label_count = 0;
    expr_cache_count = 0; // Reset cache for each compilation
    return generate_ir_for_statement(node);
}

// --- Printing and Freeing ---
void print_operand(IROperand op) {
    switch (op.type) {
        case OP_CONSTANT: printf("%d", op.constant); break;
        case OP_VARIABLE:
        case OP_TEMP:
        case OP_LABEL:
            if (op.name) printf("%s", op.name);
            break;
        case OP_EMPTY: break;
    }
}

void print_ir(IRInstruction* head) {
    for (IRInstruction* current = head; current; current = current->next) {
        if (current->op == IR_LABEL) {
            print_operand(current->result);
            printf(":");
        } else {
            printf("    "); // Indent instructions
            switch (current->op) {
                case IR_ASSIGN:
                    print_operand(current->result);
                    printf(" = ");
                    print_operand(current->arg1);
                    break;
                case IR_ADD:
                case IR_SUB:
                case IR_MUL:
                case IR_DIV:
                    print_operand(current->result);
                    printf(" = ");
                    print_operand(current->arg1);
                    printf(" %c ", current->op == IR_ADD ? '+' : current->op == IR_SUB ? '-' : current->op == IR_MUL ? '*' : '/');
                    print_operand(current->arg2);
                    break;
                case IR_GOTO:
                    printf("goto ");
                    print_operand(current->result);
                    break;
                case IR_IF_GOTO:
                    printf("if ");
                    print_operand(current->arg1);
                    printf(" goto ");
                    print_operand(current->result);
                    break;
                default:
                    printf("Unsupported IR op for printing");
            }
        }
        printf("\n");
    }
}

void free_ir(IRInstruction* head) {
    IRInstruction* current = head;
    while (current) {
        IRInstruction* next = current->next;
        if(current->result.type == OP_VARIABLE || current->result.type == OP_TEMP || current->result.type == OP_LABEL) {
            if(current->result.name) free(current->result.name);
        }
        if(current->arg1.type == OP_VARIABLE || current->arg1.type == OP_TEMP || current->arg1.type == OP_LABEL) {
            if(current->arg1.name) free(current->arg1.name);
        }
        if(current->arg2.type == OP_VARIABLE || current->arg2.type == OP_TEMP || current->arg2.type == OP_LABEL) {
            if(current->arg2.name) free(current->arg2.name);
        }
        free(current);
        current = next;
    }
}