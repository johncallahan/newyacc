#include "nyacc.h"
#include "nymem.h"

#define	TRUE 1
#define	FALSE 0

extern int ny_debug;
extern struct ny_RhsItem *ny_tstack;	/* terminal stack used by yacc */
extern struct ny_RhsItem *ny_pstack;	/* internal nyacc stack */
extern struct ny_Node *ny_cnode;	/* current node in parse tree */
extern struct ny_RStack *ny_rstack;
extern struct ny_CNStack *ny_cnstack;
extern struct ny_ArgStack *ny_astack;

nyprint(f,msk,type,filter)
int (*f)();
long unsigned msk;
ny_Selector type;
ny_Boolean filter;
{
  ny__traverse(f,ny_pstack->item.tree,msk,type,filter);
}

ny__traverse(f,node,msk,type,filter)
int (*f)();
struct ny_Node *node;
long unsigned msk;
ny_Selector type;
ny_Boolean filter;
{
  struct ny_Translation *t = NIL;
  struct ny_Translation *z = NIL;
	struct ny_RhsItem *p = NIL;

  for(t=node->translations;t != NIL;t = t->next) {
    if(t->mask == 0) z = t;
    if(t->mask == 0 && msk == 0) {
      ny__dotrans(f,z->order,node,msk,type,filter,TRUE);
      return;
    }
    if(t->mask & msk) {
      ny__dotrans(f,t->order,node,msk,type,filter,TRUE);
      return;
    }
  }
  if(t == NIL && z != NIL) {
    ny__dotrans(f,z->order,node,msk,type,filter,FALSE);
  }
}

ny__dotrans(f,order,node,msk,type,filter,matched)
int (*f)();
struct ny_Order *order;
struct ny_Node *node;
long unsigned msk;
ny_Selector type;
ny_Boolean filter;
ny_Boolean matched;
{
  if(order == NIL) return;
  ny__dotrans(f,order->next,node,msk,type,filter,matched);
  switch(order->op) {
    case ny_item:
      ny__doitem(f,order,node,msk,type,filter,matched); break;
    case ny_string:
      if(matched || type == ny_openr) ny__dostring(f,order->u.str,filter); break;
    case ny_assign:
      if(matched || type == ny_openr) ny__doassign(order,node); break;
    case ny_concat:
      if(matched || type == ny_openr) ny__doconcat(order,node); break;
    case ny_reference:
      ny__doref(f,order,msk,type,filter,matched); break;
    case ny_udf:
      ny__dofunc(f,order,node,msk,type,filter,matched); break;
    case ny_beginr:
      ny__dobeg(); break;
    case ny_endr:
      ny__doend(); break;
    case ny_declr:
      ny__dodecl(order->reg); break;
    case ny_comma:
      ny__docomma(); break;
    case ny_cond:
      ny__docond(f,order,node,msk,type,filter,matched); break;
    case ny_noop: break;
  }
}

ny__doconcat(order,node)
struct ny_Order *order;
struct ny_Node *node;
{
  struct ny_Register *temp,*n,*t,*p;

  temp = NEW_REG();
  if(order->op2 == ny_reference) {
    t = *order->u.reg2;
    temp->op = t->op;
    temp->elem = t->elem;
    temp->str = t->str;
    temp->tt = t->tt;
    temp->type = t->type;
    temp->filtered = t->filtered;
    temp->mask = t->mask;
    temp->node = t->node;
    t = t->next;
    p = temp;
    while(t != NIL) {
      if(p->next == NIL) {
	p->next = NEW_REG();
      }
      p->next->op = t->op;
      p->next->elem = t->elem;
      p->next->str = t->str;
      p->next->tt = t->tt;
      p->next->type = t->type;
      p->next->mask = t->mask;
      p->next->filtered = t->filtered;
      p->next->node = t->node;
      t = t->next;
      p = p->next;
    }
  } else {
    temp->op = order->op2;
    temp->elem = order->elem;
    temp->str = order->u.str;
    temp->tt = order->u.st->tt;
    temp->type = order->u.st->stype;
    temp->filtered = order->filterStrings;
    temp->mask = order->u.st->tmask;
    temp->next = NIL;
    temp->node = node;
  }
  n = *order->reg;
  while(n->next != NIL) n = n->next;
  n->next = temp;
}

ny__doassign(order,node)
struct ny_Order *order;
struct ny_Node *node;
{
  struct ny_Register *t,*p;
  if(order->op2 == ny_reference) {
    t = *order->u.reg2;
    (*order->reg)->op = t->op;
    (*order->reg)->elem = t->elem;
    (*order->reg)->str = t->str;
    (*order->reg)->tt = t->tt;
    (*order->reg)->type = t->type;
    (*order->reg)->mask = t->mask;
    (*order->reg)->filtered = t->filtered;
    (*order->reg)->node = t->node;
    t = t->next;
    p = *order->reg;
    while(t != NIL) {
      if(p->next == NIL) {
	p->next = NEW_REG();
      }
      p->next->op = t->op;
      p->next->elem = t->elem;
      p->next->str = t->str;
      p->next->tt = t->tt;
      p->next->type = t->type;
      p->next->mask = t->mask;
      p->next->filtered = t->filtered;
      p->next->node = t->node;
      t = t->next;
      p = p->next;
    }
  } else {
    (*order->reg)->op = order->op2;
    (*order->reg)->elem = order->elem;
    (*order->reg)->str = order->u.str;
    (*order->reg)->tt = order->u.st->tt;
    (*order->reg)->type = order->u.st->stype;
    (*order->reg)->mask = order->u.st->tmask;
    (*order->reg)->filtered = order->filterStrings;
    (*order->reg)->node = node;
    (*order->reg)->next = NIL;
  }
}

ny__doref(f,order,msk,type,filter,matched)
int (*f)();
struct ny_Order *order;
long unsigned msk;
ny_Selector type;
ny_Boolean filter;
ny_Boolean matched;
{
  struct ny_Order o;
  struct ny_subtree_t st;
  struct ny_Register *t;

  t = *order->reg;
  o.u.st = &st;
  while(t != NIL) {
    o.elem = t->elem;
    o.u.st->tmask = t->mask;
    o.u.st->stype = t->type;
    o.u.st->tt = t->tt;
    o.filterStrings = t->filtered;
    switch(t->op) {
      case ny_item:
        ny__doitem(f,&o,t->node,msk,type,filter,matched);
        break;
      case ny_string:
        if(matched || type == ny_openr) ny__dostring(f,t->str,
          (filter) ? 1 : t->filtered);
        break;
      case ny_udf:
        ny__dofunc(f,&o,t->node,msk,type,filter,matched); break;
      case ny_noop: break;
    }
    t = t->next;
  }
}

ny__doitem(f,order,node,msk,type,filter,matched)
int (*f)();
struct ny_Order *order;
struct ny_Node *node;
long unsigned msk;
ny_Selector type;
ny_Boolean filter;
ny_Boolean matched;
{
  struct ny_RhsItem *t;
  unsigned i;

  if(order->elem < 1 || order->elem > node->nchildren) return;
  for(i=0,t=node->children;i < node->nchildren-order->elem;i++,t = t->next);
  switch(t->type) {
    case ny_terminal:
      if(type == ny_openr ||
       (matched && order->u.st->tt != ny_passThru && order->u.st->stype == ny_openr))
        ny__dostring(f,t->item.str,filter); break;
    case ny_nonTerminal:
      ny__traverse(f,t->item.tree,
        (order->u.st->tt == ny_passThru) ? msk : order->u.st->tmask,
        (type == ny_selective && order->u.st->tt == ny_passThru) ? type : order->u.st->stype,
	(filter) ? 1 : order->filterStrings); break;
  }
}

ny__dostring(f,str,filter)
int (*f)();
char *str;
ny_Boolean filter;
{
  if(filter) {
    char *ns;
    unsigned sl,i,j=0;

    sl = strlen(str);
    ns = NEW_STR(sl*2+1);
    for(i=0;i < sl;i++) {
      if(str[i] == '"') ns[j++] = '\\';
      if(str[i] == '\\') ns[j++] = '\\';
      ns[j++] = str[i];
    }
    ns[j] = '\0';
    (*f)(ns);
    FREE_M(ns);
  } else (*f)(str);
}

ny__decl(reg)
struct ny_Register **reg;
{
  struct ny_Order *temp;

  temp = ny_cnode->translations->order;
  ny_cnode->translations->order = NEW_ORDER();
  ny_cnode->translations->order->next = temp;
  ny_cnode->translations->order->reg = reg;
  ny_cnode->translations->order->op = ny_declr;
}

ny__beg()
{
  struct ny_Order *temp;

  temp = ny_cnode->translations->order;
  ny_cnode->translations->order = NEW_ORDER();
  ny_cnode->translations->order->next = temp;
  ny_cnode->translations->order->op = ny_beginr;
}

ny__end()
{
  struct ny_Order *temp;

  temp = ny_cnode->translations->order;
  ny_cnode->translations->order = NEW_ORDER();
  ny_cnode->translations->order->next = temp;
  ny_cnode->translations->order->op = ny_endr;
}

ny__dodecl(reg)
struct ny_Register **reg;
{
  struct ny_RStackElem *temp;

  if(reg != NIL) {
    temp = ny_rstack->top;
    ny_rstack->top = NEW_RSE();
    ny_rstack->top->reg = *reg;
    ny_rstack->top->myaddress = reg;
    ny_rstack->top->next = temp;
  }
  *reg = NEW_REG();
  (*reg)->op = ny_noop;
  (*reg)->next = NIL;
  (*reg)->node = NIL;
}

ny__dobeg()
{
  struct ny_RStack *temp;

  temp = ny_rstack;
  ny_rstack = NEW_RS();
  ny_rstack->next = temp;
  ny_rstack->top = NIL;
}

ny__doend()
{
  struct ny_RStack *rs;
  struct ny_RStackElem *rse,*temp;

  if(ny_rstack == NIL) return;
  rse = ny_rstack->top;
  while(rse != NIL) {
    *rse->myaddress = rse->reg;
    temp = rse;
    rse = rse->next;
    FREE_M(temp);
  }
  rs = ny_rstack;
  ny_rstack = ny_rstack->next;
  FREE_M(rs);
}

ny__orderf(n,f)
unsigned n;
char *(*f)();
{
  struct ny_Node *temp;
  struct ny_CNStack *t;
  struct ny_Order *tord;

  temp = ny_cnode;
  t = ny_cnstack;
  ny_cnode = ny_cnstack->node;
  ny_cnstack = ny_cnstack->next;
  FREE_CNSTACK(t);
  tord = ny_cnode->translations->order;
  ny_cnode->translations->order = NEW_ORDER();
  ny_cnode->translations->order->u.func = NEW_FUNC();
  ny_cnode->translations->order->next = tord;
  ny_cnode->translations->order->u.func->f = f;
  ny_cnode->translations->order->u.func->args = temp->translations->order;
  ny_cnode->translations->order->op = ny_udf;
}

ny__assignf(n,ref,f)
unsigned n;
struct ny_Register **ref;
char *(*f)();
{
  struct ny_Node *temp;
  struct ny_CNStack *t;
  struct ny_Order *tord;

  temp = ny_cnode;
  t = ny_cnstack;
  ny_cnode = ny_cnstack->node;
  ny_cnstack = ny_cnstack->next;
  FREE_CNSTACK(t);
  tord = ny_cnode->translations->order;
  ny_cnode->translations->order = NEW_ORDER();
  ny_cnode->translations->order->u.func = NEW_FUNC();
  ny_cnode->translations->order->next = tord;
  ny_cnode->translations->order->reg = ref;
  ny_cnode->translations->order->u.func->f = f;
  ny_cnode->translations->order->u.func->args = temp->translations->order;
  ny_cnode->translations->order->op = ny_assign;
  ny_cnode->translations->order->op2 = ny_udf;
}

ny__begf()
{
  struct ny_CNStack *temp;

  temp = ny_cnstack;
  ny_cnstack = NEW_CNS();
  ny_cnstack->node = ny_cnode;
  ny_cnstack->next = temp;
  ny_cnode = NEW_NODE();
  ny_cnode->nchildren = 0;
  ny_cnode->children = NIL;
  ny_cnode->translations = NEW_TRANS();
  ny_cnode->translations->mask = 0;
  ny_cnode->translations->order = NIL;
  ny_cnode->translations->next = NIL;
}

ny__concatf(n,ref,f)
unsigned n;
struct ny_Register **ref;
int (*f)();
{
  struct ny_Node *temp;
  struct ny_CNStack *t;
  struct ny_Order *tord;

  temp = ny_cnode;
  t = ny_cnstack;
  ny_cnode = ny_cnstack->node;
  ny_cnstack = ny_cnstack->next;
  FREE_CNSTACK(t);
  tord = ny_cnode->translations->order;
  ny_cnode->translations->order = NEW_ORDER();
  ny_cnode->translations->order->u.func = NEW_FUNC();
  ny_cnode->translations->order->next = tord;
  ny_cnode->translations->order->reg = ref;
  ny_cnode->translations->order->u.func->args = temp->translations->order;
  ny_cnode->translations->order->op = ny_concat;
  ny_cnode->translations->order->op2 = ny_udf;  
}

ny__collect(str)
char *str;
{
  unsigned slen,lsf;

  slen = (str != NIL) ? strlen(str) : 0 ;
  lsf = (ny_astack->arguments[ny_astack->carg] != NIL)
    ? strlen(ny_astack->arguments[ny_astack->carg]) : 0 ;
  if(ny_astack->carg >= ny_astack->maxargs) {
    ny_astack->arguments =
      (char **)RE_STR(ny_astack->arguments,
        sizeof(char *)*(ny_astack->maxargs+10));
    ny_astack->arglens =
      (unsigned *)RE_STR(ny_astack->arglens,
        sizeof(int)*(ny_astack->maxargs+10));
    ny_astack->maxargs += 10;
  }
  if(slen+lsf > ny_astack->arglens[ny_astack->carg]) {
    ny_astack->arglens[ny_astack->carg] += slen+10;
    if(ny_astack->arguments[ny_astack->carg] == NIL) {
      ny_astack->arguments[ny_astack->carg] =
        NEW_STR(ny_astack->arglens[ny_astack->carg]);
      ny_astack->arguments[ny_astack->carg][0] = '\0';
    } else ny_astack->arguments[ny_astack->carg] =
      RE_STR(ny_astack->arguments[ny_astack->carg],
        ny_astack->arglens[ny_astack->carg]);
  }
  if(slen > 0) strcat(ny_astack->arguments[ny_astack->carg],str);
  if(ny_astack->nargs == 0) ny_astack->nargs++;
}

ny__uncollect()
{
  struct ny_ArgStack *temp;
  unsigned i;

  for(i=0;i <= ny_astack->carg;i++) {
    FREE_M(ny_astack->arguments[i]);
    ny_astack->arguments[i] = NIL;
  }
  FREE_M(ny_astack->arguments);
  FREE_M(ny_astack->arglens);
  temp = ny_astack;
  ny_astack = ny_astack->next;
  FREE_ASTACK(temp);
}

ny__docomma()
{
  ny_astack->carg++; ny_astack->nargs++;
}

/*
 * do each argument according to the current environment,
 * collect the results into strings, call the client
 * function which returns a string and "print it" with
 * the argument function f.
 *
 */
ny__dofunc(f,order,node,msk,type,filter,matched)
int (*f)();
struct ny_Order *order;
struct ny_Node *node;
long unsigned msk;
ny_Selector type;
ny_Boolean filter;
ny_Boolean matched;
{
  struct ny_ArgStack *temp;
  char *str;
  unsigned i;

  temp = ny_astack;
  ny_astack = NEW_ARGS();
  ny_astack->next = temp;
  ny_astack->node = node;
  ny_astack->carg = 0;
  ny_astack->maxargs = 10;
  ny_astack->nargs = 0;
  ny_astack->arguments = (char **)NEW_STR(sizeof(char *)*10);
  ny_astack->arglens = (unsigned *)NEW_STR(sizeof(int)*10);
  for(i=0;i < 10;i++) {
    ny_astack->arguments[i] = NIL;
    ny_astack->arglens[i] = 0;
  }
  ny__dotrans(ny__collect,order->u.func->args,node,msk,type,filter,matched);
  str = (*order->u.func->f)(ny_astack->nargs,ny_astack->arguments);
  ny__uncollect();
  (*f)(str);
  FREE_M(str);
}

ny_Boolean
ny__evalbexp(order,node,msk,type,filter,matched)
struct ny_Order *order;
struct ny_Node *node;
long unsigned msk;
ny_Selector type;
ny_Boolean filter;
ny_Boolean matched;
{
  struct ny_ArgStack *temp;
  int i;
  char *op1,*op2;
  ny_Boolean result;
  
  switch(order->op) {
  case ny_equal:
    temp = ny_astack;
    ny_astack = NEW_ARGS();
    ny_astack->next = temp;
    ny_astack->carg = 0;
    ny_astack->maxargs = 2;
    ny_astack->nargs = 0;
    ny_astack->arguments = (char **)NEW_STR(sizeof(char *)*2);
    ny_astack->arglens = (unsigned *)NEW_STR(sizeof(int)*2);
    for(i=0;i < 2;i++) {
      ny_astack->arguments[i] = NIL;
      ny_astack->arglens[i] = 0;
    }
    ny__dotrans(ny__collect,order->u.br->ifbranch,node,msk,type,filter,matched);
    if(ny_astack->arguments[0] != NIL) {
      op1 = NEW_STR(strlen(ny_astack->arguments[0])+1);
      strcpy(op1,ny_astack->arguments[0]);
    } else { op1 = NEW_STR(1); op1[0] = '\0'; }
    ny__docomma();
    ny__dotrans(ny__collect,order->u.br->elsebranch,node,msk,type,filter,matched);
    if(ny_astack->arguments[1] != NIL) {
      op2 = NEW_STR(strlen(ny_astack->arguments[1])+1);
      strcpy(op2,ny_astack->arguments[1]);
    } else { op2 = NEW_STR(1); op2[0] = '\0'; }
    ny__uncollect();
    if(strcmp(op1,op2) == 0) result = TRUE; else result = FALSE;
    FREE_M(op1); FREE_M(op2);
    break;
  case ny_notequal:
    temp = ny_astack;
    ny_astack = NEW_ARGS();
    ny_astack->next = temp;
    ny_astack->carg = 0;
    ny_astack->maxargs = 2;
    ny_astack->nargs = 0;
    ny_astack->arguments = (char **)NEW_STR(sizeof(char *)*2);
    ny_astack->arglens = (unsigned *)NEW_STR(sizeof(int)*2);
    for(i=0;i < 2;i++) {
      ny_astack->arguments[i] = NIL;
      ny_astack->arglens[i] = 0;
    }
    ny__dotrans(ny__collect,order->u.br->ifbranch,node,msk,type,filter,matched);
    if(ny_astack->arguments[0] != NIL) {
      op1 = NEW_STR(strlen(ny_astack->arguments[0])+1);
      strcpy(op1,ny_astack->arguments[0]);
    } else { op1 = NEW_STR(1); op1[0] = '\0'; }
    ny__docomma();
    ny__dotrans(ny__collect,order->u.br->elsebranch,node,msk,type,filter,matched);
    if(ny_astack->arguments[1] != NIL) {
      op2 = NEW_STR(strlen(ny_astack->arguments[1])+1);
      strcpy(op2,ny_astack->arguments[1]);
    } else { op2 = NEW_STR(1); op2[0] = '\0'; }
    ny__uncollect();
    if(strcmp(op1,op2) == 0) result = FALSE; else result = TRUE;
    FREE_M(op1); FREE_M(op2);
    break;
  case ny_andand:
    result = ny__evalbexp(order->u.br->ifbranch,node,msk,type,filter,matched)
      && ny__evalbexp(order->u.br->elsebranch,node,msk,type,filter,matched);
    break;
  case ny_oror:
    result = ny__evalbexp(order->u.br->ifbranch,node,msk,type,filter,matched)
      || ny__evalbexp(order->u.br->elsebranch,node,msk,type,filter,matched);
    break;
  }
  return(result);
}

ny__docond(f,order,node,msk,type,filter,matched)
int (*f)();
struct ny_Order *order;
struct ny_Node *node;
long unsigned msk;
ny_Selector type;
ny_Boolean filter;
ny_Boolean matched;
{
  /*
   * evaluate the boolean expression.  If TRUE, then perform
   * (traverse) the ifbranch, else the elsebranch.  Simple.
   *
   */
  if(ny__evalbexp(order->u.br->condition,node,msk,type,filter,matched))
    ny__dotrans(f,order->u.br->ifbranch,node,msk,type,filter,matched);
  else
    ny__dotrans(f,order->u.br->elsebranch,node,msk,type,filter,matched);
}
