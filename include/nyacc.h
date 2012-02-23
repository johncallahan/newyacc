#ifndef NULL
#include <stdio.h>
#endif

#define NIL 0

/*
 * nyacc structures for building the traversable
 * parse tree
 *
 */
 
typedef char ny_Boolean;
typedef enum { ny_terminal, ny_nonTerminal } ny_NodeType;
typedef enum { ny_item, ny_string, ny_assign, ny_concat, ny_beginr, ny_declr, ny_endr,
		 ny_reference, ny_udf, ny_comma, ny_noop, ny_cond, ny_equal, ny_notequal,
		 ny_andand, ny_oror } ny_Operation;
typedef enum { ny_selective, ny_openr } ny_Selector;
typedef enum { ny_passThru, ny_newMask } ny_TraversalType;
typedef long unsigned ny_Mask;

struct ny_Node;
struct ny_Register;

struct ny_subtree_t {
  ny_Mask tmask;
  ny_TraversalType tt;
  ny_Selector stype;
};

struct ny_func_t {
  char *(*f)();
  struct ny_Order *args;
};

struct ny_cond_t {
  struct ny_Order *ifbranch,*elsebranch;
  struct ny_Order *condition;
};

struct ny_Order {
  ny_Operation op,op2;
  ny_Boolean filterStrings;
  union{
    char *str;
    struct ny_subtree_t *st;
    struct ny_func_t *func;
    struct ny_cond_t *br;
    struct ny_Register **reg2;
  } u;
  unsigned elem;
  struct ny_Register **reg;
  struct ny_Order *next;
};

struct ny_Translation {
  ny_Mask mask;
  struct ny_Order *order;
  struct ny_Translation *next;
};

struct ny_RhsItem {
  ny_NodeType type;
  long blineno,elineno,bcwline,ecwline;
  long int bcharno,echarno;
  union {
    char *str;
    struct ny_Node *tree;
  } item;
  struct ny_RhsItem *next;
};

struct ny_Node {
  struct ny_Translation *translations;
  unsigned nchildren;
  struct ny_RhsItem *children;
};

struct ny_Register {
  ny_Operation op;
  unsigned elem;
  ny_Mask mask;
  ny_Selector type;
  ny_TraversalType tt;
  ny_Boolean filtered;
  char *str;
  struct ny_Node *node;
  struct ny_Register *next;
};

struct ny_RStackElem {
  struct ny_Register *reg,**myaddress;
  struct ny_RStackElem *next;
};

struct ny_RStack {
  struct ny_RStackElem *top;
  struct ny_RStack *next;
};

struct ny_CNStack {
  struct ny_Node *node;
  struct ny_CNStack *next;
};

struct ny_OStack {
  struct ny_Order *order;
  struct ny_OStack *next;
};

struct ny_ArgStack {
  struct ny_Node *node;
  unsigned nargs, carg, maxargs;
  char **arguments;
  unsigned *arglens;
  struct ny_ArgStack *next;
};
