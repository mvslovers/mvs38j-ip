/* userret.c - userret */

#include <conf.h>
#include <kernel.h>

/*------------------------------------------------------------------------
 * userret  --  entered when a process exits by return
 *------------------------------------------------------------------------
 */
userret()
{
	kprintf("USERRET - kill pid %d - complete\n", getpid());
	killpid( getpid() );
}


