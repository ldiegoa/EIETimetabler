/**
 @file BatchLang.lex
 @brief This file contains the lexical analyzer definition for the EasyLocal++
 language for batch experiences.
 
 @author Andrea Schaerf (schaerf@uniud.it)
 @author Luca Di Gaspero (digasper@dimi.uniud.it)
 
 @version: 0.2
 @date 18 Jan 2002
*/ 

/* this is a chunk of C code that is inserted before the lexical analyzer code,
   it consists of all header inclusion and the definition of global variables
   that are needed for exchanging information between the lexical analyzer and
   the actual bison parser */
%{
#include <string>
#include "EasyLocal.h"
using namespace std;
using std::string;
using namespace easylocal;
#include "ExpSpec.tab.h"

  
/* the YY_USER_ACTION defines the default action that will be taken when a
   new lexical token has recognized */

#define YY_USER_ACTION yycolno += yyleng;

/* the yyparse function is an extern function provided by the bison parser
   which simply start to parse a file */
extern "C++" int yyparse(void);
/* yyerror is a bison function for error notifying */
extern "C++" int yyerror(const char* s);

char* trimdelims(const char*);
char* copy(const char*);

unsigned int yycolno = 0; /* the column number */
int comment_caller; /* this variable contains the environment from which the comment is called */
int string_caller; /* this variable contains the environment from which the string is called */
char* foo; 
%}

/* now we have to define some options; among these we may define some lexical
   environments for top-down parsing (problem, solver, runner, wm, comment).
*/

%option noyywrap
%option yylineno
%option nostdinit
%option never-interactive
%x instance
%x runner
%x comment

/* here we define some shortcuts for lexical categories */

bblock "{"
eblock "}"
estmt ";"
dash "-"
dot "."
digit [0-9]
natural {digit}+
float {digit}*{dot}{digit}+
integer [-+]?{digit}+
id [A-Za-z]([A-Za-z0-9_]*{dash}*)*
string \"[^\n"]+\"

%%

<*>"/*"                comment_caller = YYSTATE;  BEGIN(comment); 
<comment>{
  [^*\n]*              /* eat anything but a `*' */       
  "*"+[^"*/"\n]*       /* eat the `*' not followed by a `/' */
  "*"+"/"              BEGIN(comment_caller); /* restart from last environment */
}

"Instance"             BEGIN(instance); return INSTANCE; 

<instance>{
  "Log file:"          return LOG_FILE;
  "Output prefix:"     return OUTPUT_PREFIX;
  "Plot prefix:"       return PLOT_PREFIX;
  "Trials:"            return TRIALS;
  "Runner"             BEGIN(runner); return RUNNER;
  {eblock}             {  
                         /* end block, go back to the initial state */
                         BEGIN(INITIAL); return EBLOCK;
                       } 
}

<runner>{
  "hill climbing"      return HILL_CLIMBING;
  "tabu search"        return TABU_SEARCH;
  "simulated annealing" return SIMULATED_ANNEALING;
  "max iteration:"     return MAX_ITERATION;
  "max idle iteration:" return MAX_IDLE_ITERATION;
  "max tabu tenure:"   return MAX_TABU_TENURE;
  "min tabu tenure:"   return MIN_TABU_TENURE;
  "start temperature:" return START_TEMPERATURE;
  "cooling rate:"      return COOLING_RATE; 
  "neighbors sampled:" return NEIGHBORS_SAMPLED;
  {eblock}             {
                         /* end block, go back to the previous state */ 
                         BEGIN(instance); return EBLOCK; 
                       }
}

<*>{bblock}            return BBLOCK;  /* beginning block */
<*>{estmt}             return ESTMT;   /* end of statement `;' */

<*>{string}            yylval.char_string = new string(trimdelims(yytext)); return STRING;
<*>{natural}           yylval.natural = strtoul(yytext,&foo,0); return NATURAL; 
<*>{float}             yylval.real = strtod(yytext,&foo); return REAL;
<*>{id}                yylval.char_string =  new string(yytext); return IDENTIFIER; 

<*>[ \r]               /* skips the blanks and the carriage returns */
<*>[\t]                {
                         /* skips the tabs and update the column count */ 
                         yycolno += 7;                         
                       }
<*>[\n]                {
                         /* skipping the newline involves the update 
						    of the column number */
                         yycolno = 0;
                       }


<*>.                   { /* overrules the default rule that echos all the 
						    unmatched input raising a syntax error message 
							as output */
                         yyerror("syntax error");
					     yyterminate();
                       }

%%

char* trimdelims(const char* t) {
  int l = strlen(t);
  char* s = new char[l];
  for (int i = 0; i < l-2; i++)
    s[i] = t[i+1];
  s[l-2] = '\0';
  return s;
}

char* copy(const char* t) {
  int l = strlen(t);
  char* s = new char[l+1];
  for (int i = 0; i < l; i++)
    s[i] = t[i];
  s[l] = '\0';
  return s;
}
