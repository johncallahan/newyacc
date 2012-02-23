#include <stdio.h>
#include "nytags.h"

myprint(s)
char *s;
{
  if(s != 0) fputs(s,stdout);
}


main()
{
  if (yyparse() == 1) { exit(1); }
  nyprint(myprint,REVERSE,NY_OPEN,NY_NORMAL);
}

yyerror()
{
  return(1);
}
