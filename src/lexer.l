%{
#include "parser.tab.h"
#include "token_display.h"
#include <stdlib.h>
#include <string.h>

int line_num = 1;
%}

%option noinput nounput
%option noyywrap
%x COMMENT
%x SINGLE_LINE_COMMENT

%%

"//" { BEGIN(SINGLE_LINE_COMMENT); }
<SINGLE_LINE_COMMENT>\n { line_num++; BEGIN(INITIAL); }
<SINGLE_LINE_COMMENT>. { /* ignore single line comment content */ }

"/*" { BEGIN(COMMENT); }
<COMMENT>"*/" { BEGIN(INITIAL); }
<COMMENT>\n { line_num++; }
<COMMENT>. { /* ignore multi-line comment content */ }

"let" { display_token(LET, yytext, line_num); return LET; }
"function" { display_token(FUNCTION, yytext, line_num); return FUNCTION; }
"if" { display_token(IF, yytext, line_num); return IF; }
"else" { display_token(ELSE, yytext, line_num); return ELSE; }
"for" { display_token(FOR, yytext, line_num); return FOR; }
"take" { display_token(TAKE, yytext, line_num); return TAKE; }
"publish" { display_token(PUBLISH, yytext, line_num); return PUBLISH; }

"==" { display_token(EQ, yytext, line_num); return EQ; }
"!=" { display_token(NEQ, yytext, line_num); return NEQ; }
"<=" { display_token(LEQ, yytext, line_num); return LEQ; }
">=" { display_token(GEQ, yytext, line_num); return GEQ; }
"&&" { display_token(AND, yytext, line_num); return AND; }
"||" { display_token(OR, yytext, line_num); return OR; }

"=" { display_token(ASSIGN, yytext, line_num); return ASSIGN; }
"<" { display_token(LT, yytext, line_num); return LT; }
">" { display_token(GT, yytext, line_num); return GT; }
"+" { display_token(PLUS, yytext, line_num); return PLUS; }
"-" { display_token(MINUS, yytext, line_num); return MINUS; }
"*" { display_token(MUL, yytext, line_num); return MUL; }
"/" { display_token(DIV, yytext, line_num); return DIV; }
"!" { display_token(NOT, yytext, line_num); return NOT; }

"(" { display_token(LPAREN, yytext, line_num); return LPAREN; }
")" { display_token(RPAREN, yytext, line_num); return RPAREN; }
"{" { display_token(LBRACE, yytext, line_num); return LBRACE; }
"}" { display_token(RBRACE, yytext, line_num); return RBRACE; }
"," { display_token(COMMA, yytext, line_num); return COMMA; }
";" { display_token(SEMICOLON, yytext, line_num); return SEMICOLON; }

[0-9]+ {
    yylval.int_val = atoi(yytext);
    display_token(NUMBER, yytext, line_num);
    return NUMBER;
}

\"([^"\\\\]|\\\\.)*\" {
    char* str = strdup(yytext);
    str[strlen(str) - 1] = '\0';
    memmove(str, str + 1, strlen(str));
    
    char* src = str;
    char* dst = str;
    while (*src) {
        if (*src == '\\') {
            src++;
            switch (*src) {
                case 'n': *dst++ = '\n'; break;
                case 't': *dst++ = '\t'; break;
                case 'r': *dst++ = '\r'; break;
                case '\\': *dst++ = '\\'; break;
                case '\"': *dst++ = '\"'; break;
                default: *dst++ = *src; break;
            }
        } else {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';
    
    yylval.string_val = str;
    display_token(STRING, yytext, line_num);
    return STRING;
}


[a-zA-Z_][a-zA-Z0-9_]* {
    yylval.string_val = strdup(yytext);
    display_token(IDENTIFIER, yytext, line_num);
    return IDENTIFIER;
}

[ \t\r]+ { /* skip whitespace */ }
\n { line_num++; }

. {
    display_token(UNKNOWN, yytext, line_num);
    printf("Unknown character at line %d: %s\n", line_num, yytext);
    return UNKNOWN;
}

%%

void reset_lexer(void) {
    line_num = 1;
    BEGIN INITIAL;
} 