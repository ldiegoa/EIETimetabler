/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
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
     INSTANCE = 258,
     OUTPUT_PREFIX = 259,
     LOG_FILE = 260,
     PLOT_PREFIX = 261,
     TRIALS = 262,
     RUNNER = 263,
     TABU_SEARCH = 264,
     HILL_CLIMBING = 265,
     SIMULATED_ANNEALING = 266,
     MAX_ITERATION = 267,
     MAX_IDLE_ITERATION = 268,
     MIN_TABU_TENURE = 269,
     MAX_TABU_TENURE = 270,
     START_TEMPERATURE = 271,
     COOLING_RATE = 272,
     NEIGHBORS_SAMPLED = 273,
     BBLOCK = 274,
     EBLOCK = 275,
     ESTMT = 276,
     IDENTIFIER = 277,
     STRING = 278,
     NATURAL = 279,
     REAL = 280
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 2068 of yacc.c  */
#line 54 "ExpSpec.y"

  std::string* char_string;
  unsigned long natural;
  double real;
  ParameterBox* par_box;



/* Line 2068 of yacc.c  */
#line 84 "ExpSpec.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

