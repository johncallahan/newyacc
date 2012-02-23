#include "nyacc.h"
#include "nymem.h"

#define	TRUE 1
#define	FALSE 0

extern struct ny_Order *ny_ops_pop();
extern void ny_ops_push();

extern int ny_debug;
extern struct ny_RhsItem *ny_tstack;	/* terminal stack used by yacc */
extern struct ny_RhsItem *ny_pstack;	/* internal nyacc stack */
extern struct ny_Node *ny_cnode;	/* current node in parse tree */
extern struct ny_RStack *ny_rstack;
extern struct ny_CNStack *ny_cnstack;
extern struct ny_ArgStack *ny_astack;

struct ny_OStack *ny_condStack = NIL;
struct ny_OStack *ny_expStack = NIL;

ny__cond()
{
  struct ny_Order *temp;

  temp = ny_cnode->translations->order;
  ny_cnode->translations->order = NEW_ORDER();
  ny_cnode->translations->order->u.br = NEW_BRANCH();
  ny_cnode->translations->order->next = temp;
  ny_cnode->translations->order->op = ny_cond;
  ny_ops_push(&ny_condStack,ny_cnode->translations->order);
  ny_cnode->translations->order = NIL;
}

ny__then()
{
  /*
   * take the condition ordering out of the current node
   * attach it to the top of the condition stack's
   * condition
   *
   */
  struct ny_Order *topos;
  topos = ny_ops_pop(&ny_condStack);
  topos->u.br->condition = ny_cnode->translations->order;
  ny_ops_push(&ny_condStack,topos);
  ny_cnode->translations->order = NIL;
}

ny__endthen()
{
  /*
   * take the order found in the current node and
   * attach it to the top of the condition stack's
   * then branch
   *
   */
  struct ny_Order *topos;
  topos = ny_ops_pop(&ny_condStack);
  topos->u.br->ifbranch = ny_cnode->translations->order;
  topos->u.br->elsebranch = NIL;
  ny_ops_push(&ny_condStack,topos);
  ny_cnode->translations->order = NIL;
}

ny__endelse()
{
  /*
   * take the order found in the current node and
   * attach it to the top of the condition stack's
   * else branch
   *
   */
  struct ny_Order *topos;
  topos = ny_ops_pop(&ny_condStack);
  topos->u.br->elsebranch = ny_cnode->translations->order;
  ny_ops_push(&ny_condStack,topos);
  ny_cnode->translations->order = NIL;
}

ny__endcond()
{
  /*
   * the condition order, the then order and optionally
   * an else order should be on the top of the condition
   * stack - pop the stack and set up the node appropriately
   *
   */
  struct ny_Order *topos;
  topos = ny_ops_pop(&ny_condStack);
  ny_cnode->translations->order = topos;
}

ny__begbexp(op)
unsigned op;
{
  /*
   * the first arg to the binary equality operator is in
   * the current node.  Put it in the ifbranch.  Push this
   * node on the expression stack
   *
   */
  struct ny_Order *temp;
  temp = ny_cnode->translations->order;
  ny_cnode->translations->order = NEW_ORDER();
  ny_cnode->translations->order->u.br = NEW_BRANCH();
  ny_cnode->translations->order->next = NIL;
  ny_cnode->translations->order->op =
    (ny_Operation)((unsigned)op+(unsigned)ny_equal);
  ny_cnode->translations->order->u.br->ifbranch = temp;
  ny_ops_push(&ny_expStack,ny_cnode->translations->order);
  ny_cnode->translations->order = NIL;
}

ny__endbexp()
{
  struct ny_Order *topos;
  topos = ny_ops_pop(&ny_expStack);
  topos->u.br->elsebranch = ny_cnode->translations->order;
  ny_cnode->translations->order = topos;
}

ny__not(order)
struct ny_Order *order;
{
  if(order == NIL) order = ny_cnode->translations->order;
  switch(order->op) {
    case ny_equal: order->op = ny_notequal; break;
    case ny_notequal: order->op = ny_equal; break;
    case ny_andand: order->op = ny_oror; ny__not(order->u.br->ifbranch);
      ny__not(order->u.br->elsebranch);
    case ny_oror: order->op = ny_andand; ny__not(order->u.br->ifbranch);
      ny__not(order->u.br->elsebranch);
    }
}
