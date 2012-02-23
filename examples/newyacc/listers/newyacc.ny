%{

typedef enum {
	ny_normal = 0,
	ny_readunion = 1,
	ny_readdefs = 2,
	ny_readaction = 3,
	ny_readtail = 4
} ny_LexModeType;

ny_LexModeType ny_mode=ny_normal;

char *lhs;

%}

%tag COUNT ECOUNT TRANS
%token	IDENTIFIER
%token	C_IDENTIFIER

%token	START UNION LCURL RCURL TOKEN TAG TAB TABINC TABDEC
%token	RWORD NUMBER CONCAT ASSIGN
%token	LSQUIGGLY RSQUIGGLY STAR
%token	MARK PREC ERROR LITERAL VBAR SEMICOLON
%token	IFR THENR ELSER EQUAL NOTEQUAL ANDAND OROR NOT

%token	STRING DEFSBODY TAILBODY ACTIONBODY UNIONBODY SELECTIVEITEM OPENITEM

%start spec

%%

spec	:	defs MARK
		rules tail
		[ (COUNT) !lhs #3 "\n" ]
		[ (ECOUNT) !lhs #3 "\n" ]
	;

tail	:	/* empty */
	|	MARK
		{ ny_mode = ny_readtail; }
		TAILBODY
		{ ny_mode = ny_normal; }
	;

defs	:	/* empty */
	|	def defs
	;

def	:	START IDENTIFIER
	|	UNION LSQUIGGLY { ny_mode = ny_readunion; }
		UNIONBODY { ny_mode = ny_normal; }
		RSQUIGGLY
	|	LCURL { ny_mode = ny_readdefs; }
		DEFSBODY { ny_mode = ny_normal; }
		RCURL
	|	rword tag nlist
	;


rword	:	TOKEN
	|	TAG
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
	|	IDENTIFIER
	|	IDENTIFIER NUMBER
	;

rules	:	C_IDENTIFIER
		rbody prec
		[ (COUNT) @lhs=#1() #1() ]
		[ (ECOUNT) @lhs=#1() #1() "\t" #2 " " ]
	|	rules rule
		[ (COUNT) #1 "\n" #2 ]
		[ (ECOUNT) #1 "\n" #2 ]
	;

rule	:	C_IDENTIFIER
		rbody prec
		[ (COUNT) @lhs=#1() #1() ]
		[ (ECOUNT) @lhs=#1() #1() "\t" #2 " " ]
	|	VBAR
		rbody prec
		[ (COUNT) @lhs ]
		[ (ECOUNT) @lhs "\t" #2 " " ]
	;

rbody	:	/* empty */
	|	rbody
		IDENTIFIER
		[ (ECOUNT) #1 #2() " " ]
	|	rbody
		LITERAL
		[ (ECOUNT) #1 #2() "' " ]
	|	rbody act
		[ (ECOUNT) #1 " " ]
	;

act	:	LSQUIGGLY
		{ ny_mode = ny_readaction; }
		ACTIONBODY
		{ ny_mode = ny_normal; }
		RSQUIGGLY
	;

transl	:	/* empty */
	|	transl trans
	;

trans	:	'[' display
		scope ']'
		[ (TRANS) "[ " #2() " ]\n" ]
	;

display	:	/* empty */
	|	'(' masks ')'
	;

scope	:	odecls tbody
	;

odecls	:	/* empty */
	|	odecls '!' IDENTIFIER
	;

argl	:	/* empty */
	|	tbitem
	|	argl ',' tbitem
	;

tbody	:	/* empty */
	|	tbody LSQUIGGLY scope RSQUIGGLY
	|	tbody assign
	|	tbody cond
	|	tbody tbitem
	;

cond	:	ifpref
	|	ifpref ELSER LSQUIGGLY scope RSQUIGGLY
	;

ifpref	:	IFR
		bexp
		THENR LSQUIGGLY scope RSQUIGGLY
	;


bexp	:	tbitem EQUAL
		tbitem
	|	tbitem NOTEQUAL
		tbitem
/*	|	bexp ANDAND
		bexp
	|	bexp OROR
		bexp
	|	NOT bexp */
	|	'(' bexp ')'
	;

assign	:	refitem ASSIGN
		exp
	|	refitem CONCAT
		exp
	;

exp	:	tbitem
	|	exp '+' tbitem
	;

tbitem	:	STRING
	|	IDENTIFIER
		'(' argl ')'
	|	sitem
	|	fitem
	|	refitem
	;

refitem	:	'@' IDENTIFIER
	;

fitem	:	'\\' sitem
	;

sitem	:	SELECTIVEITEM
	|	OPENITEM
	|	SELECTIVEITEM '(' masks ')'
	|	OPENITEM '(' masks ')'
	;

masks	:	/* empty */
	|	STAR
	|	maskl
	;

maskl	:	IDENTIFIER
	|	maskl ',' IDENTIFIER
	;
	
prec	:	transl
	|	PREC
	|	PREC act transl
	|	prec SEMICOLON
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
      escapeon = FALSE; 
    } else if(str != NULL) {
      switch(ch) {
	case '/':
          if(!quoteon && !comment) {
            if((ch = input()) == '*') {
              comment = TRUE; 
            } else { unput(ch); ch = '/'; }
          }
          break;
        case '*':
          if(!quoteon && comment) {
            if((ch = input()) == '/') {
              comment = FALSE;
            } else { unput(ch); ch = '*'; }
          }
          break;
        case '\\':
          if(quoteon && !comment) escapeon = TRUE;
          break;
        case '\"':
          if(!comment) quoteon = !quoteon;
          break;
      }
    }
  }
  if(ch == 0 && str != NULL) return(0);
  return(1);
}

yyerror()
{
  fprintf(stderr,"newyacc: syntax error on line %d\n", yylineno);
  exit(1);
}
