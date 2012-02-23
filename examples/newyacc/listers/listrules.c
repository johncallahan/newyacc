#include <stdio.h>
#include "nytags.h"

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
  nyprint(myprint,ECOUNT,NY_SELECT,NY_NORMAL);
}

