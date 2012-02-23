#include <stdio.h>
#include "nytags.h"

extern int yylineno;
extern char *malloc();

myprint(s)
char *s;
{
  if(s != 0) fputs(s,stdout);
}

main(argc,argv)
int argc;
char **argv;
{
  int subtree = -1,i;
  unsigned tt;

  if(yyparse() == 1) exit(-1);
  nyprint(myprint,ECHOL,0,1);
}

yyerror(s)
char *s;
{
  fprintf(stderr,s); fprintf(stderr,"\n"); fflush(stderr);
}
