#include <stdio.h>
#include "nytags.h"

myprint(s)
char *s;
{
  fputs(s,stdout);
}

help()
{
  printf("\n\tcalc - a simple calculator\n\n");
  printf("\tcalc          will evaluate expressions and echo infix   at EOF\n");
  printf("\tcalc prefix   will evaluate expressions and echo prefix  at EOF\n");
  printf("\tcalc postfix  will evaluate expressions and echo postfix at EOF\n");
  printf("\tcalc help     prints this message\n\n");
  printf("\tcontrol-D terminates.\n\n");
  exit(0);
}

main(argc,argv)
int argc;
char **argv;
{
  long mask;

  mask = INFIX;
  if(argc > 1) {
    if(strcmp(argv[1],"postfix") == 0) mask = POSTFIX;
    else if(strcmp(argv[1],"prefix") == 0) mask = PREFIX;
    else if(strcmp(argv[1],"help") == 0) help();
  }
  if(yyparse() == 1) exit(-1);
  nyprint(myprint,mask,NY_OPEN,NY_NORMAL);
  fflush(stdout);
}

yyerror()
{
  printf("SYNTAX ERROR\n");
}
