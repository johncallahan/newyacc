#include "nyacc.h"
#include "nymem.h"

int ny_numtabs=0;

char
*tabdecr()
{
  ny_numtabs--;
  return("");
}

char
*tabincr()
{
  ny_numtabs++;
  return("");
}

char
*tab()
{
  char *str;
  unsigned i = 0;

  str = NEW_STR(ny_numtabs+1);
  for(i=0;i < ny_numtabs;i++) str[i] = ' ';
  str[i] = '\0';
  return(str);
}

extern struct ny_ArgStack *ny_astack;

char
*blineno()
{
  int i,count;
  char *temp;
  struct ny_RhsItem *p;

  temp = NEW_STR(10);
  if(ny_astack->node->children != NIL) {
    count = ny_astack->node->nchildren;
again:
    for(p=ny_astack->node->children,i=1;i < count;i++) {
      p=p->next;
    }
    if((count > 0) && (p->blineno < 0)) { count--; goto again; }
    sprintf(temp,"%d",p->blineno);
  } else sprintf(temp,"%d",0);
  return(temp);
}

char
*elineno()
{
  char *temp;
  struct ny_RhsItem *p;

  temp = NEW_STR(10);
  if(ny_astack->node->children != NIL) {
    p = ny_astack->node->children;
    while((p->next != NIL) && (p->elineno < 0)) p = p->next;
    sprintf(temp,"%d",p->elineno);
  }
  return(temp);
}

char
*bcharno()
{
  int i,count;
  char *temp;
  struct ny_RhsItem *p;

  temp = NEW_STR(10);
  if(ny_astack->node->children != NIL) {
    count = ny_astack->node->nchildren;
again:
    for(p=ny_astack->node->children,i=1;i < count;i++) {
      p=p->next;
    }
    if((count > 0) && (p->bcharno < 0)) { count--; goto again; }
    sprintf(temp,"%d",p->bcharno);
  } else sprintf(temp,"%d",0);
  return(temp);
}

char
*echarno()
{
  char *temp;
  struct ny_RhsItem *p;

  temp = NEW_STR(10);
  if(ny_astack->node->children != NIL) {
    p = ny_astack->node->children;
    while((p->next != NIL) && (p->echarno < 0)) p = p->next;
    sprintf(temp,"%d",p->echarno);
  }
  return(temp);
}

char
*bcwline()
{
  int i,count;
  char *temp;
  struct ny_RhsItem *p;

  temp = NEW_STR(10);
  if(ny_astack->node->children != NIL) {
    count = ny_astack->node->nchildren;
again:
    for(p=ny_astack->node->children,i=1;i < count;i++) {
      p=p->next;
    }
    if((count > 0) && (p->bcwline < 0)) { count--; goto again; }
    sprintf(temp,"%d",p->bcwline);
  } else sprintf(temp,"%d",0);
  return(temp);
}

char
*ecwline()
{
  char *temp;
  struct ny_RhsItem *p;

  temp = NEW_STR(10);
  if(ny_astack->node->children != NIL) {
    p = ny_astack->node->children;
    while((p->next != NIL) && (p->ecwline < 0)) p = p->next;
    sprintf(temp,"%d",p->ecwline);
  }
  return(temp);
}
