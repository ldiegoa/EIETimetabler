/**
 @file BatchLang.y
 @brief This file contains the actual parser definition for the EasyLocal++
 language for batch experiences.
 
 @author Andrea Schaerf (schaerf@uniud.it)
 @author Luca Di Gaspero (digasper@dimi.uniud.it)
 
 @version: 0.2
 @date 18 Jan 2002
*/ 

/* this is a chunk of C/C++ code that is inserted before the bison parser code,
   it consists of all header inclusion and the definition of global variables
   that are needed for exchanging information between the lexical analyzer and
   the actual bison parser */

%{
#include <iostream>
#include <cstdlib> // for free(void*)
#include <cstdio>
#include <string>
#include <list>
#include <limits.h>

#include <FlexLexer.h>
#include "EasyLocal.h"

using namespace easylocal;

#define YYERROR_VERBOSE

extern "C++" int yylex();
extern unsigned int yycolno;
extern int yylineno;
AbstractTester* main_tester;

/* this is the function for error handling/notify/recovery, for our purposes
   it simply writes an error info */
int yyerror(const char* s) {
	printf("At line %d column %d %s\n", yylineno, yycolno, s);
  return -1;
}

struct ix_parbox {
  unsigned int i;
  ParameterBox* pb;
};

%}

/* Bison declarations */

%union {
  std::string* char_string;
  unsigned long natural;
  double real;
  ParameterBox* par_box;
}

%token INSTANCE "Instance"
%token OUTPUT_PREFIX "Output prefix"
%token LOG_FILE "Log file"
%token PLOT_PREFIX "Plot prefix"
%token TRIALS "Trials"
%token RUNNER "Runner"
%token TABU_SEARCH "tabu search"
%token HILL_CLIMBING "hill climbing"
%token SIMULATED_ANNEALING "simulated annealing"
%token MAX_ITERATION "max iteration"
%token MAX_IDLE_ITERATION "max idle iteration"
%token MIN_TABU_TENURE "min tabu tenure"
%token MAX_TABU_TENURE "max tabu tenure"
%token START_TEMPERATURE "start temperature"
%token COOLING_RATE "cooling rate"
%token NEIGHBORS_SAMPLED   "neighbors sampled"
%token BBLOCK "{"
%token EBLOCK "}"
%token ESTMT ";"
%token <char_string> IDENTIFIER "identifier"
%token <char_string> STRING "string"
%token <natural> NATURAL "natural"
%token <real> REAL "real"

%type <par_box> hc_parameters
%type <par_box> ts_parameters
%type <par_box> sa_parameters

%start instances

%% 

/* Grammar rules */

instances:      /* the empty file matches this rule */
|
                instance instances
;

/* basically a problem instance has associated an indentifier and a solver */

instance: 	INSTANCE STRING
{ 
  /* write on the output stream some header data */
  //  write_header($2);
  /* get the solver from the tester class */
  main_tester->LoadInstance(*$2);
}
                solver
;

solver:          BBLOCK
				   other_parameters
                   runners
                 EBLOCK
{
  main_tester->StartSolver();
}
;

other_parameters:  solver_trials file_parameters | file_parameters solver_trials;

file_parameters:   log_file output_and_plot | output_and_plot log_file;

output_and_plot:   output_file_prefix plot_file_prefix
					   | plot_file_prefix output_file_prefix

solver_trials: /* one trial solver */
{
  main_tester->SetSolverTrials(1);
} 
|          TRIALS NATURAL ESTMT /* multi trial solver */
{
  main_tester->SetSolverTrials($2);
}
;

log_file: /* no log file, write on stderr */
{}
|		   LOG_FILE STRING ESTMT /* log file name */
{
	main_tester->SetLogFile(*$2);
	delete $2;
}

output_file_prefix: /* no output */
{}
|          OUTPUT_PREFIX STRING ESTMT /* output prefix */
{ 
  main_tester->SetOutputPrefix(*$2); 
  delete $2;
}
;

plot_file_prefix: /* no plot */
{}
|          PLOT_PREFIX STRING ESTMT /* plot prefix */
{ 
  main_tester->SetPlotPrefix(*$2); 
  delete $2;
}
;


runners:   runner
|          runner runners
;

runner:    RUNNER HILL_CLIMBING STRING 
{
  // add runner of type hill climbing to the solver
  int status_value = main_tester->AddRunnerToSolver(*$3, "Hill Climbing");
  if (status_value  == RUNNER_NOT_FOUND)
    {
      std::string s(*$3);
      s = "runner `" + s + "' not found error";
      yyerror(s.c_str());
      YYABORT;
    }
  else if (status_value == RUNNER_TYPE_MISMATCH)
    {
      std::string s(*$3);
      s = "runner `" + s + "' type mismatch";
      yyerror(s.c_str());
      YYABORT;
    }
}
           BBLOCK
             hc_parameters
{
  main_tester->SetRunningParameters(*$3, "Hill Climbing", *$6);
}
           EBLOCK
|          RUNNER TABU_SEARCH STRING 
{
  // add runner of type tabu search to the solver
  int status_value = main_tester->AddRunnerToSolver(*$3, "Tabu Search");
  if (status_value  == RUNNER_NOT_FOUND)
    {
      std::string s(*$3);
      s = "runner `" + s + "' not found error";
      yyerror(s.c_str());
      YYABORT;
    }
  else if (status_value == RUNNER_TYPE_MISMATCH)
    {
      std::string s(*$3);
      s = "runner `" + s + "' type mismatch";
      yyerror(s.c_str());
      YYABORT;
    }
}
           BBLOCK
             ts_parameters
{
  main_tester->SetRunningParameters(*$3, "Tabu Search", *$6);
}
           EBLOCK
|          RUNNER SIMULATED_ANNEALING STRING 
{
  // add runner of type simulated annealing to the solver
  int status_value = main_tester->AddRunnerToSolver(*$3, "Simulated Annealing");
  if (status_value  == RUNNER_NOT_FOUND)
    {
      std::string s(*$3);
      s = "runner `" + s + "' not found error";
      yyerror(s.c_str());
      YYABORT;
    }
  else if (status_value == RUNNER_TYPE_MISMATCH)
    {
      std::string s(*$3);
      s = "runner `" + s + "' type mismatch";
      yyerror(s.c_str());
      YYABORT;
    }
}
           BBLOCK
             sa_parameters
{
  main_tester->SetRunningParameters(*$3, "Simulated Annealing", *$6);
}
           EBLOCK
; /* remember to add other runner types */

hc_parameters:  MAX_IDLE_ITERATION NATURAL ESTMT
{
  $$ = new ParameterBox;
  $$->Put("max idle iteration", $2);  
  $$->Put("max iteration", ULONG_MAX);  
}
|               MAX_IDLE_ITERATION NATURAL ESTMT
                MAX_ITERATION NATURAL ESTMT
{
  $$ = new ParameterBox;
  $$->Put("max idle iteration", $2);
  $$->Put("max iteration", $5);  
}
;              

ts_parameters:  MIN_TABU_TENURE NATURAL ESTMT
                MAX_TABU_TENURE NATURAL ESTMT
                MAX_IDLE_ITERATION NATURAL ESTMT
{
  $$ = new ParameterBox;
  $$->Put("min tenure", (unsigned int)$2);
  $$->Put("max tenure", (unsigned int)$5);
  $$->Put("max idle iteration", $8);  
  $$->Put("max iteration", ULONG_MAX);  
}
|               MIN_TABU_TENURE NATURAL ESTMT
                MAX_TABU_TENURE NATURAL ESTMT
                MAX_IDLE_ITERATION NATURAL ESTMT
                MAX_ITERATION NATURAL ESTMT
{
  $$ = new ParameterBox;
  $$->Put("min tenure", (unsigned int)$2);
  $$->Put("max tenure", (unsigned int)$5);
  $$->Put("max idle iteration", $8);  
  $$->Put("max iteration", $11);
}
;

sa_parameters:  START_TEMPERATURE REAL ESTMT
                COOLING_RATE REAL ESTMT
                NEIGHBORS_SAMPLED NATURAL ESTMT
{
  $$ = new ParameterBox;
  $$->Put("start temperature", $2);
  $$->Put("cooling rate", $5);
  $$->Put("neighbors sampled", (unsigned int)$8);
  $$->Put("max iteration", ULONG_MAX);
}
|               START_TEMPERATURE REAL ESTMT
                COOLING_RATE REAL ESTMT
                NEIGHBORS_SAMPLED NATURAL ESTMT
                MAX_ITERATION NATURAL ESTMT
{
  $$ = new ParameterBox;
  $$->Put("start temperature", $2);
  $$->Put("cooling rate", $5);
  $$->Put("neighbors sampled", (unsigned int)$8);
  $$->Put("max iteration", $11);
}
;

%%
