#ifndef TOKEN_DISPLAY_H
#define TOKEN_DISPLAY_H

#include "parser.tab.h"

// Function to display token information
void display_token(int token, const char* text, int line);

// Function to initialize token display
void init_token_display(void);

// Function to finalize token display
void finalize_token_display(void);

#endif // TOKEN_DISPLAY_H 