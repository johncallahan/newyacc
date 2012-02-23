%{

extern char *bcharno(),*echarno();
extern char *blineno(),*elineno();
extern char *bcwline(),*ecwline();

%}

%tag ECHOL ECHOC
%token ATOK BTOK CTOK DTOK ETOK FTOK GTOK

%start alist

%%

alist	:	CTOK
		[ (ECHOL) #1() "(" blineno() "," bcwline() ","
		  elineno() "," ecwline() ")\n" ]
		[ (ECHOC) #1() "(" bcharno() "," echarno() ")\n" ]
	|	DTOK ATOK alist BTOK
		[ (ECHOL) #3 "-" #1() "-" #2() "-" #3() "-" #4() "-"
		  "(" blineno() "," bcwline() "," elineno() ","
		  ecwline() ")\n" ]
		[ (ECHOC) #3 "-" #1() "-" #2() "-" #3() "-" #4() "-"
		  "(" bcharno() "," echarno() ")\n" ]
	|	ETOK ATOK alist BTOK
		[ (ECHOL) #3 "-" #1() "-" #2() "-" #3() "-" #4() "-"
		  "(" blineno() "," bcwline() "," elineno() ","
		  ecwline() ")\n" ]
		[ (ECHOC) #3 "-" #1() "-" #2() "-" #3() "-" #4() "-"
		  "(" bcharno() "," echarno() ")\n" ]
	|	other ATOK alist BTOK
		[ (ECHOL) #3 "-" #1() "-" #2() "-" #3() "-" #4() "-"
		  "(" blineno() "," bcwline() "," elineno() ","
		  ecwline() ")\n" ]
		[ (ECHOC) #3 "-" #1() "-" #2() "-" #3() "-" #4() "-"
		  "(" bcharno() "," echarno() ")\n" ]
	;

other	:	/* empty */
	|	FTOK
	|	GTOK
	;

%%

#include "lex.yy.c"
