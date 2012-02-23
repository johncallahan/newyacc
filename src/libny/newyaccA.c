#include "nyacc.h"
#include "nymem.h"

long yylineno=1,yycharno=0,yytok=0,yybline=0;

unsigned ny_sbuf_size = 0;
char *ny_sbuf = NIL;

#define	TRUE 1
#define	FALSE 0

extern long yylastok;

int ny_debug=0;
struct ny_RhsItem *ny_tstack = NIL;	/* terminal stack used by yacc */
struct ny_RhsItem *ny_pstack = NIL;	/* internal nyacc stack */
struct ny_Node *ny_cnode = NIL;	/* current node in parse tree */
struct ny_RStack *ny_rstack = NIL;
struct ny_CNStack *ny_cnstack = NIL;
struct ny_ArgStack *ny_astack = NIL;

nyinit()
{
  ny_tstack = NIL; ny_pstack = NIL; ny_cnode = NIL;
  ny_rstack = NIL; ny_cnstack = NIL; ny_astack = NIL;
}

/*
 * called internally by yacc when reading a token
 * from the lexical analyzer
 *
 */
nypushs(s)
char *s;
{
  struct ny_RhsItem *nitem;

  nitem = NEW_RHSITEM();
  nitem->item.str = NEW_STR(strlen(s)+1);
  strcpy(nitem->item.str,s);
  nitem->type = ny_terminal;
	nitem->blineno = nitem->elineno = yylineno;
	nitem->bcwline = yytok - yybline;
	nitem->bcharno = yytok;
	nitem->ecwline = yycharno - yybline;
  nitem->echarno = yycharno;
  nitem->next = ny_tstack;
  ny_tstack = nitem;
}

/*
 * called by yacc to internally pop terminals
 * off terminal/token stack when shifting
 *
 */
nyipop()
{
  struct ny_RhsItem *temp;

  temp = ny_tstack;
  if(temp != NIL) ny_tstack = ny_tstack->next;
  if(temp != NIL) temp->next = ny_pstack;
  if(temp != NIL) ny_pstack = temp;
}

ny__orderp(stackitem,msk,tt,type,filtered)
unsigned stackitem;
long unsigned msk;
ny_TraversalType tt;
ny_Selector type;
ny_Boolean filtered;
{
  struct ny_Order *temp;

  temp = ny_cnode->translations->order;
  ny_cnode->translations->order = NEW_ORDER();
  ny_cnode->translations->order->u.st = NEW_SUBTREE();
  ny_cnode->translations->order->next = temp;
  ny_cnode->translations->order->elem = stackitem;
  ny_cnode->translations->order->u.st->stype = type;
  ny_cnode->translations->order->u.st->tmask = msk;
  ny_cnode->translations->order->u.st->tt = tt;
  ny_cnode->translations->order->filterStrings = filtered;
  ny_cnode->translations->order->op = ny_item;
}

ny__refer(ref)
struct ny_Register **ref;
{
  struct ny_Order *temp;

  temp = ny_cnode->translations->order;
  ny_cnode->translations->order = NEW_ORDER();
  ny_cnode->translations->order->next = temp;
  ny_cnode->translations->order->reg = ref;
  ny_cnode->translations->order->op = ny_reference;
}

ny__assignr(ref1,ref2)
struct ny_Register **ref1,**ref2;
{
  struct ny_Order *temp;

  temp = ny_cnode->translations->order;
  ny_cnode->translations->order = NEW_ORDER();
  ny_cnode->translations->order->next = temp;
  ny_cnode->translations->order->reg = ref1;
  ny_cnode->translations->order->u.reg2 = ref2;
  ny_cnode->translations->order->op = ny_assign;
  ny_cnode->translations->order->op2 = ny_reference;
}

ny__concatr(ref1,ref2)
struct ny_Register **ref1,**ref2;
{
  struct ny_Order *temp;

  temp = ny_cnode->translations->order;
  ny_cnode->translations->order = NEW_ORDER();
  ny_cnode->translations->order->next = temp;
  ny_cnode->translations->order->reg = ref1;
  ny_cnode->translations->order->u.reg2 = ref2;
  ny_cnode->translations->order->op = ny_concat;
  ny_cnode->translations->order->op2 = ny_reference;
}

ny__assignp(ref,stackitem,msk,tt,type,filtered)
struct ny_Register **ref;
unsigned stackitem;
long unsigned msk;
ny_TraversalType tt;
ny_Selector type;
ny_Boolean filtered;
{
  struct ny_Order *temp;

  temp = ny_cnode->translations->order;
  ny_cnode->translations->order = NEW_ORDER();
  ny_cnode->translations->order->u.st = NEW_SUBTREE();
  ny_cnode->translations->order->u.st->tt = tt;
  ny_cnode->translations->order->u.st->tmask = msk;
  ny_cnode->translations->order->u.st->stype = type;
  ny_cnode->translations->order->filterStrings = filtered;
  ny_cnode->translations->order->next = temp;
  ny_cnode->translations->order->reg = ref;
  ny_cnode->translations->order->elem = stackitem;
  ny_cnode->translations->order->op = ny_assign;
  ny_cnode->translations->order->op2 = ny_item;
}

ny__assigns(ref,str)
struct ny_Register **ref;
char *str;
{
  struct ny_Order *temp;

  temp = ny_cnode->translations->order;
  ny_cnode->translations->order = NEW_ORDER();
  ny_cnode->translations->order->next = temp;
  ny_cnode->translations->order->reg = ref;
  ny_cnode->translations->order->u.str = NEW_STR(strlen(str)+1);
  strcpy(ny_cnode->translations->order->u.str,str);
  ny_cnode->translations->order->op = ny_assign;
  ny_cnode->translations->order->op2 = ny_string;
}

ny__concatp(ref,stackitem,msk,tt,type,filtered)
struct ny_Register **ref;
unsigned stackitem;
long unsigned msk;
ny_TraversalType tt;
ny_Selector type;
ny_Boolean filtered;
{
  struct ny_Order *temp;

  temp = ny_cnode->translations->order;
  ny_cnode->translations->order = NEW_ORDER();
  ny_cnode->translations->order->u.st = NEW_SUBTREE();
  ny_cnode->translations->order->next = temp;
  ny_cnode->translations->order->u.st->tt = tt;
  ny_cnode->translations->order->u.st->tmask = msk;
  ny_cnode->translations->order->u.st->stype = type;
  ny_cnode->translations->order->filterStrings = filtered;
  ny_cnode->translations->order->reg = ref;
  ny_cnode->translations->order->elem = stackitem;
  ny_cnode->translations->order->op = ny_concat;
  ny_cnode->translations->order->op2 = ny_item;
}

ny__concats(ref,str)
struct ny_Register **ref;
char *str;
{
  struct ny_Order *temp;

  temp = ny_cnode->translations->order;
  ny_cnode->translations->order = NEW_ORDER();
  ny_cnode->translations->order->next = temp;
  ny_cnode->translations->order->reg = ref;
  ny_cnode->translations->order->u.str = NEW_STR(strlen(str)+1);
  strcpy(ny_cnode->translations->order->u.str,str);
  ny_cnode->translations->order->op = ny_concat;
  ny_cnode->translations->order->op2 = ny_string;
}

ny__orders(str)
char *str;
{
  struct ny_Order *temp;

  temp = ny_cnode->translations->order;
  ny_cnode->translations->order = NEW_ORDER();
  ny_cnode->translations->order->next = temp;
  ny_cnode->translations->order->u.str = MALLOC(strlen(str)+1);
  strcpy(ny_cnode->translations->order->u.str,str);
  ny_cnode->translations->order->op = ny_string;
}

ny__trans(msk)
long unsigned msk;
{
  struct ny_Translation *temp;
  
  if(ny_cnode == NIL) {
		ny_cnode = NEW_NODE();
    ny_cnode->translations = NIL;
    ny_cnode->children = NIL;
    ny_cnode->nchildren = 0;
  }
  temp = ny_cnode->translations;
	ny_cnode->translations = NEW_TRANS();
	ny_cnode->translations->order = NIL;
  ny_cnode->translations->next = temp;
  ny_cnode->translations->mask = msk;
}

ny__squish(nrhs,white)
unsigned nrhs;
ny_Boolean white;
{
  int count,finding;
  struct ny_RhsItem *temp,*p;
  unsigned i;
  
  if(ny_cnode == NIL) {
		ny_cnode = NEW_NODE();
    ny_cnode->translations = NIL;
    ny_cnode->children = NIL;
    ny_cnode->nchildren = 0;
  }
  temp = ny_pstack;
  ny_cnode->nchildren = nrhs;
  for(i=0;i < nrhs;i++) {
    if(temp != NIL) temp = temp->next;
    else break;
  }
  /* temp now top of new ny_pstack */
  ny_cnode->children = ny_pstack;
  ny_pstack = temp;
	temp = NEW_RHSITEM();
  temp->type = ny_nonTerminal;
  if((ny_cnode->children != NIL) && (nrhs > 0)) {
    count = nrhs;
    finding=1;
    while(finding) {
      for(p=ny_cnode->children,i=1;i < count;i++) p=p->next;
      if((count > 0) && (p->bcharno < 0)) count--;
      else finding=0;
    }
    temp->blineno = p->blineno;
    temp->elineno = ny_cnode->children->elineno;
    temp->bcharno = p->bcharno;
    temp->echarno = ny_cnode->children->echarno;
    temp->bcwline = p->bcwline;
    temp->ecwline = ny_cnode->children->ecwline;
  } else {
    temp->blineno = -1;
    temp->elineno = -1;
    temp->bcharno = -1;
    temp->echarno = -1;
    temp->bcwline = -1;
    temp->ecwline = -1;
  }
	temp->item.tree = ny_cnode;
	temp->next = ny_pstack;
	ny_cnode = NIL;
	ny_pstack = temp;
  if(!white) {
    struct ny_Translation *trans;
    struct ny_Order *order;

		trans = NEW_TRANS();
    trans->mask = 0;
    trans->next = ny_pstack->item.tree->translations;
    ny_pstack->item.tree->translations = trans;
		trans->order = NIL;
    for(i=0;i < ny_pstack->item.tree->nchildren;i++) {
      order = NEW_ORDER();
      order->u.st = NEW_SUBTREE();
      order->next = trans->order;
      trans->order = order;
      order->u.st->stype = ny_openr;
      order->op = ny_item;
      order->filterStrings = FALSE;
      order->u.st->tt = ny_passThru;
      order->elem = i+1;
    }
  }
}

ny__comma()
{
  struct ny_Order *temp;

  temp = ny_cnode->translations->order;
  ny_cnode->translations->order = NEW_ORDER();
  ny_cnode->translations->order->next = temp;
  ny_cnode->translations->order->op = ny_comma;
}
