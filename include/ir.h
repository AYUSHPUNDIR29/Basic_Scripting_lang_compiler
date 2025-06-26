#ifndef IR_H
#define IR_H

#include "ast.h"

// Operations in our Intermediate Representation (Three-Address Code)
typedef enum {
    IR_ASSIGN,
    IR_ADD,
    IR_SUB,
    IR_MUL,
    IR_DIV,
    IR_LABEL,       // L1:
    IR_GOTO,        // goto L1
    IR_IF_GOTO,     // if t1 goto L1
    IR_PARAM,
    IR_CALL,
    IR_RETURN
} IROp;

// An operand in an IR instruction
typedef struct {
    enum {
        OP_EMPTY,
        OP_CONSTANT,
        OP_VARIABLE,
        OP_TEMP,
        OP_LABEL
    } type;
    union {
        int constant;
        char* name; // For variables, temps, and labels
    };
} IROperand;

// A single instruction in the IR
typedef struct IRInstruction {
    IROp op;
    int line_number; // Line in source code
    IROperand result;
    IROperand arg1;
    IROperand arg2;
    struct IRInstruction* next;
} IRInstruction;

// Function prototypes
IRInstruction* generate_ir(ASTNode* node);
void print_ir(IRInstruction* head);
void print_ir_instruction(IRInstruction* instruction);
void free_ir(IRInstruction* head);

#endif // IR_H 