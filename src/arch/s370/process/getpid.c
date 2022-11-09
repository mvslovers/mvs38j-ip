/* getpid.c - getpid */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

#ifdef	MVS38J
	#undef	getpid		// undo <proc.h> definition
#endif

/*------------------------------------------------------------------------
 * getpid  --  get the process id of currently executing process
 *------------------------------------------------------------------------
 */

SYSCALL getpid() {

	return(currpid);
}


