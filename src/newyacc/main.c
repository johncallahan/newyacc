#include <stdio.h>
#include "nymem.h"

unsigned ny_sbuf_size = 0;
char *ny_sbuf = 0;

#define	YACC "/usr/bin/yacc"
#define	TEMPFILE "./ny.temp.y"
#define TRUE 1
#define FALSE 0

FILE *ny_out;
extern FILE *yyin,*yyout;
/* extern char *malloc(); */

ny_usage()
{
  fprintf(stderr,"usage: newyacc [-f infile] [-lds] [-Y yaccpath]\n");
}

main(argc,argv)
int argc;
char **argv;
{
  char *xcute, *yaccs=NULL;
  int i,temp;
  char yaccDebug=FALSE,leave=FALSE;
  FILE *f,*fopen();

  if(argc < 2) {
    ny_usage(); exit(-1);
  }
  f = stdin;
  for(i=1;i < argc;i++) {
    if(argv[i][0] == '-') {
      switch(argv[i][1]) {
        case 'l': leave = !leave; break;
        case 's': f = stdin; break;
        case 'f': if(argc > i) {
          if((f = fopen(argv[++i],"r+")) == NULL) {
            fprintf(stderr,"can't open %s\n",argv[1]);
            exit(-1);
          }
        } break;
        case 'd': yaccDebug = !yaccDebug; break;
        case 'Y': if(argc > i) {
					temp = strlen(argv[++i])+1;
          yaccs = MALLOC(temp);
          strcpy(yaccs,argv[i]);
          break;
        } else {
          ny_usage(); exit(-1);
        }
      }
    }
  }
  if((ny_out = fopen(TEMPFILE,"w+")) == NULL) {
    fprintf(stderr,"can't open temporary file\n");
    exit(-1);
  }
  yyin = f;
  yyout = ny_out;
  if(yyparse()) { fclose(ny_out); exit(-1); }
  fclose(ny_out);
  fclose(f);
  if(yaccs == NULL) yaccs=YACC;
  xcute = MALLOC(strlen(yaccs)+1+strlen(TEMPFILE)+5);
  strcpy(xcute,yaccs);
  strcat(xcute," ");
  if(yaccDebug) strcat(xcute,"-vd ");
  strcat(xcute,TEMPFILE);
  if(yaccDebug) fprintf(stdout,"Executing %s\n",xcute); fflush(stdout);
  system(xcute);
  if(!leave) unlink(TEMPFILE);
  exit(0);
}
