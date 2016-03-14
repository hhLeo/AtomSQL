/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

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

#ifndef YY_YY_BISON_TAB_H_INCLUDED
# define YY_YY_BISON_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    CREATE = 258,
    DROP = 259,
    USE = 260,
    SHOW = 261,
    SELECT = 262,
    DESC = 263,
    INSERT = 264,
    DELETE = 265,
    UPDATE = 266,
    QUIT = 267,
    TABLES = 268,
    DATABASE = 269,
    TABLE = 270,
    INDEX = 271,
    SET = 272,
    WHERE = 273,
    FROM = 274,
    ORDER = 275,
    GROUP = 276,
    CHECK = 277,
    BY = 278,
    INTO = 279,
    VALUES = 280,
    AS = 281,
    DISTINCT = 282,
    ALL = 283,
    PRIMARY = 284,
    KEY = 285,
    ASC = 286,
    IN = 287,
    AND = 288,
    OR = 289,
    EXCEPT = 290,
    INTERSECT = 291,
    UNION = 292,
    NOT = 293,
    TOKEN_NULL = 294,
    MAX = 295,
    MIN = 296,
    SUM = 297,
    AVG = 298,
    COUNT = 299,
    EQU = 300,
    NEQ = 301,
    LSS = 302,
    GTR = 303,
    LEQ = 304,
    GEQ = 305,
    CONCAT = 306,
    IS = 307,
    LIKE = 308,
    T_EOF = 309,
    PLUS = 310,
    MINUS = 311,
    MULTIPLY = 312,
    DIVIDE = 313,
    INTEGER = 314,
    CHAR = 315,
    FLOAT = 316,
    VARCHAR = 317,
    INT_LITERAL = 318,
    FLOAT_LITERAL = 319,
    STRING_LITERAL = 320,
    IDENTIFIER = 321,
    T_SHELL_CMD = 322,
    CONTAC = 323
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 59 "bison.y" /* yacc.c:1909  */

	int ival;
	float rval;
	char *sval;
	bool bval;
	NODE *n;

#line 131 "bison.tab.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_BISON_TAB_H_INCLUDED  */
