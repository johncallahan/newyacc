#include "nyacc.h"
#include "nymem.h"

void
ny_ops_push(ostack,order)
struct ny_OStack **ostack;
struct ny_Order *order;
{
  struct ny_OStack *temp;
  temp = *ostack;
  *ostack = NEW_OS();
  (*ostack)->order = order;
  (*ostack)->next = temp;
}

struct ny_Order
*ny_ops_pop(ostack)
struct ny_OStack **ostack;
{
  struct ny_Order *result;
  result = (*ostack)->order;
  *ostack = (*ostack)->next;
  return(result);
}
