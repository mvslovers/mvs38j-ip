/* qinit.c - qinit */

#include <conf.h>
#include <kernel.h>
#include <q.h>

/*------------------------------------------------------------------------
 *  qinit  --  initialize queue management
 *------------------------------------------------------------------------
 */
void
queueinit()
{

	nextqueue = NPROC;		/* q[0...NPROC-1] are processes */

} /* qinit */

 


