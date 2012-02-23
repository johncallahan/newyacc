%{

#include <stdio.h>
#include "nymem.h"

#define YYMAXDEPTH 250
#define LONGALLONES -1
#define FALSE 0
#define TRUE 1

  /* extern char *malloc(); */
extern FILE *ny_out;

struct ny_displayitem {
  struct ny_displayitem *left,*right;
  char *tok;
  long mask;
};

typedef enum { ny_assign, ny_concat } ny_AssignType;
typedef enum { ny_empty, ny_terminal, ny_nonTerminal } ny_TokenType;
typedef enum { ny_passThru, ny_newMask } ny_TraversalType;
typedef enum {
	ny_normal = 0,
	ny_readunion = 1,
	ny_readdefs = 2,
	ny_readaction = 3,
	ny_readtail = 4
} ny_LexModeType;
typedef enum { ny_selective=0, ny_openr=1 } ny_TreeType;

ny_LexModeType ny_mode=ny_normal;
int ny_rhscount;
char ny_bounce=1, ny_action=0, ny_codeFound=0, ny_white=0, ny_displays=0;
long ny_curdisplay=0, ny_curmask=0, ny_gdisplay=1, ny_inasg=0, ny_incall=0;
long sf_incall[YYMAXDEPTH],sf_incallp=0;
long ny_nargs[YYMAXDEPTH],ny_nargsp=0;
ny_TreeType ny_ptype=ny_openr;
char *ny_ritem,*ny_callp;
ny_AssignType ny_asg=ny_assign;
ny_TraversalType ny_nmask=ny_passThru;

%}

%token	IDENTIFIER
%token	C_IDENTIFIER

%token	START UNION LCURL RCURL TOKEN TAG TAB TABINC TABDEC
%token	RWORD NUMBER CONCAT ASSIGN
%token	LSQUIGGLY RSQUIGGLY STAR
%token	MARK PREC ERROR LITERAL VBAR SEMICOLON
%token	IF THEN ELSE EQUAL NOTEQUAL ANDAND OROR NOT

%token	STRING DEFSBODY TAILBODY ACTIONBODY UNIONBODY SELECTIVEITEM OPENITEM

%start spec

%union {
	char *str;
	int num;
}

%type <str> STRING IDENTIFIER C_IDENTIFIER LITERAL refitem
%type <num> SELECTIVEITEM OPENITEM masks fitem sitem
%type <num> maskl display


%%

spec	:	defs MARK
		{ if(!ny_codeFound) {
		    fprintf(ny_out,"%%{\n\ntypedef enum {ny_empty,ny_terminal");
		    fprintf(ny_out,",ny_nonTerminal} ny_TokenType;\n\n%%}\n\n"); }
		  fprintf(ny_out,"%%%%"); }
		rules tail
	;

tail	:	/* empty */
	|	MARK
		{ fprintf(ny_out,"%%%%");
		  ny_mode = ny_readtail; }
		TAILBODY
		{ ny_PrintDisplays(); YYACCEPT; }
	;

defs	:	/* empty */
	|	def defs { ny_bounce = FALSE; }
	;

def	:	START IDENTIFIER
	|	UNION LSQUIGGLY { fprintf(ny_out,"{"); ny_mode = ny_readunion; }
		UNIONBODY { ny_mode = ny_normal; }
		RSQUIGGLY { fprintf(ny_out,"}"); }
	|	LCURL
		{ fprintf(ny_out,"%%{\n\ntypedef enum {ny_empty,ny_terminal");
		  fprintf(ny_out,",ny_nonTerminal} ny_TokenType;\n\n");
		  ny_mode = ny_readdefs; }
		DEFSBODY { ny_mode = ny_normal; }
		RCURL { ny_codeFound=TRUE; fprintf(ny_out,"%%}"); }
	|	rword tag nlist
		{ ny_displays=FALSE; }
	;

rword	:	TOKEN
	|	TAG { ny_displays = TRUE; }
	|	RWORD
	;

tag	:	/* empty */
	|	'<' IDENTIFIER '>'
	;

nlist	:	nmno
	|	nmno nlist
	|	nmno ',' nlist
	;

nmno	:	LITERAL
		{ fprintf(ny_out,"%s'",$1); }
	|	STRING
		{ fprintf(ny_out,"\"%s\"",$1); }
	|	IDENTIFIER
	|	IDENTIFIER NUMBER
	;

rules	:	C_IDENTIFIER
		{ ny_rhscount = 0;
		  fprintf(ny_out,"%s",$1);
		}
		rbody prec
	|	rules rule
	;

rule	:	C_IDENTIFIER
		{ fprintf(ny_out,"%s",$1); }
		rbody prec
	|	VBAR
		{ if(!ny_action) { ny_action = TRUE; fprintf(ny_out," { "); }
		  fprintf(ny_out," ny__squish(%d,%d); } |",ny_rhscount,ny_white); ny_white = FALSE;
		  ny_action = FALSE; ny_rhscount = 0;
		}
		rbody prec
	;

rbody	:	/* empty */
	|	rbody
		{ if(ny_action) { fprintf(ny_out," } "); ny_action = FALSE; }
		  ny_rhscount++;
		}
		IDENTIFIER
		{ fprintf(ny_out,"%s",$3); }
	|	rbody
		{ if(ny_action) { fprintf(ny_out," } "); ny_action = FALSE; }
		  ny_rhscount++;
		}
		LITERAL
		{ fprintf(ny_out,"%s'",$3); }
	|	rbody
		{ if(ny_action) { fprintf(ny_out," } "); ny_action = FALSE; }
		  ny_rhscount++;
		}
		STRING
		{ fprintf(ny_out,"\"%s\"",$3); }
	|	rbody act
	;

act	:	LSQUIGGLY
		{ fprintf(ny_out,"{"); ny_mode = ny_readaction; }
		ACTIONBODY
		{ ny_mode = ny_normal; }
		RSQUIGGLY
		{ ny_action = TRUE; }
	;

transl	:	/* empty */
		{ if(!ny_action) fprintf(ny_out," { ");
		  ny_action = TRUE;
		}
	|	transl trans
	;

trans	:	'[' display
		{ if(!ny_action) fprintf(ny_out," { ");
		  ny_action = TRUE; ny_curmask = $2;
		  fprintf(ny_out," ny__trans(%ld);",ny_curmask);
		}
		scope ']'
	;

display	:	/* empty */
		{ ny_white = TRUE; $$ = 0; }
	|	'(' masks ')'
		{ if($2 == 0) ny_white = TRUE; $$ = $2; }
	;

scope	:	odecls tbody
		{ fprintf(ny_out," ny__end(); "); }
	;

odecls	:	/* empty */
		{ fprintf(ny_out," ny__beg(); "); }
	|	odecls '!' IDENTIFIER
		{ fprintf(ny_out," ny__decl(&%s); ",$3); }
	;

argl	:	/* empty */
	|	tbitem { ny_nargs[ny_nargsp]++; }
	|	argl ',' { fprintf(ny_out," ny__comma(); "); } tbitem { ny_nargs[ny_nargsp]++; }
	;

tbody	:	/* empty */
	|	tbody LSQUIGGLY scope RSQUIGGLY
	|	tbody assign
	|	tbody cond
	|	tbody tbitem
	;

cond	:	ifpref
		{ fprintf(ny_out," ny__endcond(); "); }
	|	ifpref ELSE LSQUIGGLY scope RSQUIGGLY
		{ fprintf(ny_out," ny__endelse (); ny__endcond(); "); }
	;

ifpref	:	IF
		{ fprintf(ny_out," ny__cond(); "); }
		bexp
		{ fprintf(ny_out," ny__then(); "); }
		THEN LSQUIGGLY scope RSQUIGGLY
		{ fprintf(ny_out," ny__endthen(); "); }
	;

bexp	:	tbitem EQUAL
		{ fprintf(ny_out, "ny__begbexp(0); "); }
		tbitem
		{ fprintf(ny_out, "ny__endbexp(); "); }
	|	tbitem NOTEQUAL
		{ fprintf(ny_out, "ny__begbexp(1); "); }
		tbitem
		{ fprintf(ny_out, "ny__endbexp(); "); }
/*	|	bexp ANDAND
		{ fprintf(ny_out, "ny__begbexp(2); "); }
		bexp
		{ fprintf(ny_out, "ny__endbexp(); "); }
	|	bexp OROR
		{ fprintf(ny_out, "ny__begbexp(3); "); }
		bexp
		{ fprintf(ny_out, "ny__endbexp(); "); }
	|	NOT bexp
		{ fprintf(ny_out, "ny__not(NIL); "); } */
	|	'(' bexp ')'
	;

assign	:	refitem { ny_ritem = $1; } ASSIGN
		{ ny_inasg = TRUE; ny_callp = " ny__assign"; }
		exp { ny_inasg = FALSE; }
	|	refitem { ny_ritem = $1; } CONCAT
		{ ny_inasg = TRUE; ny_callp = " ny__concat"; }
		exp { ny_inasg = FALSE; }
	;

exp	:	tbitem
	|	exp '+' { ny_callp = " ny__concat"; } tbitem
	;

tbitem	:	STRING
		{ if(ny_inasg && !ny_incall) fprintf(ny_out,"%ss(&%s,\"%s\"); ",ny_callp,ny_ritem,$1);
		  else fprintf(ny_out,"ny__orders(\"%s\"); ",$1); }
	|	IDENTIFIER
		{ ny_nargs[++ny_nargsp] = 0;
		  sf_incall[sf_incallp++] = ny_incall;
		  fprintf(ny_out," ny__begf(); "); ny_incall = TRUE; }
		'(' argl ')'
		{ ny_incall = sf_incall[--sf_incallp];
		  if(ny_inasg && !ny_incall)
		    fprintf(ny_out,"%sf(%ld,&%s,%s); ",ny_callp,ny_nargs[ny_nargsp],ny_ritem,$1);
		  else fprintf(ny_out,"ny__orderf(%ld,%s); ",ny_nargs[ny_nargsp],$1);
		  ny_nargsp--;
		}
	|	sitem
		{ if(ny_inasg && !ny_incall) fprintf(ny_out,"%sp(&%s,%d,%ld,%d,%d,0); ",
		    ny_callp,ny_ritem,$1,ny_curdisplay,ny_nmask,ny_ptype);
		  else fprintf(ny_out,"ny__orderp(%d,%ld,%d,%d,0); ",
		    $1,ny_curdisplay,ny_nmask,ny_ptype); }
	|	fitem
		{ if(ny_inasg && !ny_incall) fprintf(ny_out,"%sp(&%s,%d,%ld,%d,%d,1); ",
		    ny_callp,ny_ritem,$1,ny_curdisplay,ny_nmask,ny_ptype);
                  else fprintf(ny_out,"ny__orderp(%d,%ld,%d,%d,1); ",
		    $1,ny_curdisplay,ny_nmask,ny_ptype); }
	|	refitem
		{ if(ny_inasg && !ny_incall) fprintf(ny_out,"%sr(&%s,&%s); ",ny_callp,ny_ritem,$1);
		  else fprintf(ny_out," ny__refer(&%s); ",$1); }
	;

refitem	:	'@' IDENTIFIER
		{ $$ = $2; }
	;

fitem	:	'\\' sitem
		{ $$ = $2; }
	;

sitem	:	SELECTIVEITEM
		{ $$ = $1; ny_curdisplay = 0; ny_nmask = ny_passThru;
		  ny_ptype = ny_selective; }
	|	OPENITEM
		{ $$ = $1; ny_curdisplay = 0; ny_nmask = ny_passThru; ny_ptype = ny_openr; }
	|	SELECTIVEITEM '(' masks ')'
		{ $$ = $1; ny_curdisplay = $3; ny_nmask = ny_newMask;
		  ny_ptype = ny_selective; }
	|	OPENITEM '(' masks ')'
		{ $$ = $1; ny_curdisplay = $3; ny_nmask = ny_newMask; ny_ptype = ny_openr; }
	;

masks	:	/* empty */ { $$ = 0; }
	|	STAR { $$ = LONGALLONES; }
	|	maskl { $$ = $1; }
	;

maskl	:	IDENTIFIER { $$ = ny_GetDisplay($1); }
	|	maskl ',' IDENTIFIER
		{ $$ = ny_GetDisplay($3) | $1; }
	;
	
prec	:	transl
	|	PREC
	|	PREC act transl
	|	prec SEMICOLON
		{ if(!ny_action) { ny_action = TRUE; fprintf(ny_out," { "); }
		  fprintf(ny_out," ny__squish(%d,%d); } ;",ny_rhscount,ny_white);
		  ny_white = FALSE; ny_action = FALSE; ny_rhscount = 0;
		}
	;

%%

#include "headerlex.yy.c"
#define	TRUE	1
#define	FALSE	0

yylex()
{
  switch(ny_mode) {
    case ny_normal: return(headeryylex()); break;
    case ny_readunion:
      if (ny__echo("}","{")) return(UNIONBODY); break;
    case ny_readdefs:
      if (ny__echo("%}",NULL)) return(DEFSBODY); break;
    case ny_readaction:
      if (ny__echo("}","{")) return(ACTIONBODY); break;
    case ny_readtail:
      if (ny__echo(NULL,NULL)) return(TAILBODY); break;
    default: return(ERROR);
  }
  return(ERROR);
}

int ny_seen = 0;
int ny_level = 0;
ny__ilookahead(str)
char *str;
{
  char c;

  if(str == NULL) return(0);
  if(str[0] == '\0') return(1);
  if(((c = input()) != EOF) && (c == str[0])) {
    if(ny__ilookahead(str+1)) {
      unput(c); return(1);
    } else unput(c); ny_seen++; return(0);
  } else if(c != EOF) {
    unput(c); ny_seen++; return(0);
  } else return(0);
}

ny__lookahead(c,str)
char c;
char *str;
{
  if(str == NULL) return(0);
  if(str[0] == '\0') return(1);
  if(ny_seen > 0) {
    ny_seen--; return(0);
  }
  if(c == str[0]) {
    return(ny__ilookahead(str+1));
  }
  return(0);
}

ny__echo(str,bal)
char *str,*bal;
{
  char ch,escapeon=FALSE,quoteon=FALSE,comment=FALSE,ans=FALSE;

  if((str != NULL) && (str[0] == '\0')) return(1);
  while((ch = input()) != 0) {
    ans = FALSE;
    if(!quoteon && !comment && (bal != NULL) && (ch == bal[0])) {
      ny_level++;
    }
    if(!quoteon && !comment && (ans = ny__lookahead(ch,str)) && (ny_level == 0)) {
      unput(ch); break;
    } else if(ans) {
      ny_level--;
    }
    if(escapeon) {
      escapeon = FALSE; fputc(ch,ny_out);
    } else if(str != NULL) {
      switch(ch) {
	case '/':
          if(!quoteon && !comment) {
            if((ch = input()) == '*') {
              comment = TRUE; fputc('/',ny_out);
            } else { unput(ch); ch = '/'; }
          }
          fputc(ch,ny_out);
          break;
        case '*':
          if(!quoteon && comment) {
            if((ch = input()) == '/') {
              comment = FALSE; fputc('*',ny_out);
            } else { unput(ch); ch = '*'; }
          }
          fputc(ch,ny_out);
          break;
        case '\\':
          if(quoteon && !comment) escapeon = TRUE;
          fputc(ch,ny_out);
          break;
        case '\"':
          if(!comment) quoteon = !quoteon;
          fputc(ch,ny_out);
          break;
        default: fputc(ch,ny_out);
      }
    } else fputc(ch,ny_out);
  }
  if(ch == 0 && str != NULL) return(0);
  return(1);
}

yyerror()
{
  /* fprintf(stderr,"newyacc: syntax error on line %d\n", yylineno); */
  fprintf(stderr,"newyacc: syntax error on line \n");
  exit(1);
}

struct ny_displayitem *ny_displaytree = NULL;

ny_AddDisplay(s,f)
char *s;
int f;
{
  int r;
  struct ny_displayitem *t,*l;

  t=ny_displaytree;l=ny_displaytree;
  fprintf(stderr,"Adding display tag %s\n",s);
  while(t != NULL && (r = strcmp(s,t->tok))) {
    l=t;
    if(r < 0) t = t->left;
    else t = t->right;
  }
  if(t == NULL) {
    t = (struct ny_displayitem *)malloc(sizeof(struct ny_displayitem));
    if((t->tok = malloc(strlen(s)+1)) == NULL) {
      fprintf(stderr,"can't allocate token space - abort!\n");
      return;
    }
    t->right=NULL;t->left=NULL;
    strcpy(t->tok,s);
    t->mask=ny_gdisplay; if(f > 0) t->mask |= 1;
    ny_gdisplay <<= 1;
    if(r < 0 && l != NULL) l->left=t;
    else if(l != NULL) l->right=t;
    else ny_displaytree=t;
  }
}

int ny_GetDisplay(s)
char *s;
{
  int r;
  struct ny_displayitem *t;

  for(t=ny_displaytree;t != NULL && (r = strcmp(s,t->tok));
    t= (r < 0) ? t->left : t->right);
  if(t==NULL) return(0);
  else return(t->mask);
}

ny_PrintDisplays()
{
  FILE *f;

  if((f=fopen("./nytags.h","w+")) == NULL) return(-1);
  ny_TraverseDisplays(f,ny_displaytree);
  fprintf(f,"#define NY_OPEN 1\n#define NY_SELECT 0\n");
  fprintf(f,"#define NY_FSTR 1\n#define NY_NORMAL 0\n");
  fclose(f);
}

ny_TraverseDisplays(f,t)
FILE *f;
struct ny_displayitem *t;
{
  if(t != NULL) {
    ny_TraverseDisplays(f,t->left);
    fprintf(f,"#define %s %ld\n",t->tok,t->mask);
    ny_TraverseDisplays(f,t->right);
  }
}
