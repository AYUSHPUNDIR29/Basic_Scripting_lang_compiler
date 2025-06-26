
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     UNKNOWN = 258,
     IDENTIFIER = 259,
     NUMBER = 260,
     STRING = 261,
     LET = 262,
     FUNCTION = 263,
     IF = 264,
     ELSE = 265,
     FOR = 266,
     TAKE = 267,
     PUBLISH = 268,
     EQ = 269,
     NEQ = 270,
     LEQ = 271,
     GEQ = 272,
     AND = 273,
     OR = 274,
     ASSIGN = 275,
     LT = 276,
     GT = 277,
     PLUS = 278,
     MINUS = 279,
     MUL = 280,
     DIV = 281,
     NOT = 282,
     LPAREN = 283,
     RPAREN = 284,
     LBRACE = 285,
     RBRACE = 286,
     COMMA = 287,
     SEMICOLON = 288
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 18 "src/parser.y"

    int int_val;
    char* string_val;
    struct ASTNode* ast;



/* Line 1676 of yacc.c  */
#line 93 "src/parser.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


