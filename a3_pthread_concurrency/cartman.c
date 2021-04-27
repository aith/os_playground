
#include "cartman.h"

/*
 * Callback when CART on TRACK arrives at JUNCTION in preparation for
 * crossing a critical section of track.
 */
void arrive(unsigned int cart, enum track track, enum junction junction) 
{
}

/*
 * Start the CART Manager. 
 *
 * Return is optional, i.e. entering an infinite loop is allowed, but not
 * recommended. Some implementations will do nothing, most will initialise
 * necessary concurrency primitives.
 */
void cartman(unsigned int tracks) 
{
  // Remove, only here to fail code warning test
  int foo = 1024;
}
