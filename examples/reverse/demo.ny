%tag REVERSE
%token ITEM

%start tlist

%%

tlist	:	list
		[ (REVERSE) #1 "\n" ]
	;

list	:	list ITEM
		[ (REVERSE) #2 " " #1 ]
	|	ITEM
		[ (REVERSE) #1 " " ]
	;

%%

#include "lex.yy.c"

