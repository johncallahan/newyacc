%{

#include <stdio.h>
#include <ctype.h>

int regs[26];
int base;

%}

%tag PREFIX POSTFIX INFIX
%start list
%token DIGIT LETTER
%left '|'
%left '&'
%left '+' '-'
%left '*' '/' '%'
%left UMINUS

%%

list	:	/* empty */
	|	list stat '\n'
			{ yylineno++; }
			[ (PREFIX) #1 #2 "\n" ]
			[ (INFIX) #1 #2 "\n" ]
			[ (POSTFIX) #1 #2 "\n" ]
	|	list error '\n'
			{ yyerrok; }
			[ (INFIX) #1 ]
	;

stat	:	expr
			{ printf("%d\n",$1); }
	|	LETTER '=' expr
			{ regs[$1] = $3; }
	;

expr	:	'(' expr ')'
			{ $$ = $2; }
			[ (PREFIX) #2 ]
			[ (POSTFIX) #2 ]
	|	expr '+' expr
			{ $$ = $1 + $3; }
			[ (PREFIX) "(+ " #1 " " #3 ")" ]
			[ (POSTFIX) "(" #3 " " #1 " +)" ]
	|	expr '-' expr
			{ $$ = $1 - $3; }
			[ (PREFIX) "(- " #1 " " #3 ")" ]
			[ (POSTFIX) "(" #1 " " #3 " -)" ]
	|	expr '*' expr
			{ $$ = $1 * $3; }
			[ (PREFIX) "(* " #1 " " #3 ")" ]
			[ (POSTFIX) "(" #1 " " #3 " *)" ]
	|	expr '/' expr
			{ $$ = $1 / $3; }
			[ (PREFIX) "(/ " #1 " " #3 ")" ]
			[ (POSTFIX) "(" #1 " " #3 " /)" ]
	|	expr '%' expr
			{ $$ = $1 % $3; }
			[ (PREFIX) "(% " #1 " " #3 ")" ]
			[ (POSTFIX) "(" #1 " " #3 " %)" ]
	|	expr '&' expr
			{ $$ = $1 & $3; }
			[ (PREFIX) "(& " #1 " " #3 ")" ]
			[ (POSTFIX) "(" #1 " " #3 " &)" ]
	|	expr '|' expr
			{ $$ = $1 | $3; }
			[ (PREFIX) "(| " #1 " " #3 ")" ]
			[ (POSTFIX) "(" #1 " " #3 " |)" ]
	|	'-' expr %prec UMINUS
			{ $$ = - $2; }
			[ (PREFIX)"(-" #2 ")" ]
			[ (POSTFIX) "(-" #2 ")" ]
	|	LETTER
			{ $$ = regs[$1]; }
	|	number
	;

number	:	DIGIT
			{ $$ = $1;
			  base = ($1==0) ? 8 : 10; }
	|	number DIGIT
			{ $$ = base * $1 + $2; }
	;

%%

char yytext[2];

yylex()
{
  int c;

  while((c=getchar())==' ');
  yytext[0] = c; yytext[1] = '\0';
  if(islower(c)) {
    yylval=c-'a';
    return(LETTER);
  }
  if(isdigit(c)) {
    yylval=c-'0';
    return(DIGIT);
  }
  return(c);
}
