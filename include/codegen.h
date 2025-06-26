#ifndef CODEGEN_H
#define CODEGEN_H

#include "ir.h" // Depends on the IR structures

// Function to generate final code from IR
void generate_code(IRInstruction* head);

#endif // CODEGEN_H 