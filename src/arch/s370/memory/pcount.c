/* pcount.c - pcount */

#include <conf.h>
#include <kernel.h>
#include <mark.h>
#include <ports.h>
#include <sem.h>

/*------------------------------------------------------------------------
 *  pcount  --  return the count of current messages in a port
 *------------------------------------------------------------------------
 */
SYSCALL	pcount(int portid) {
	STATWORD ps;    
	int	scnt;
	int	count;
	struct	pt	*ptptr;

	memlock();
	if ( isbadport(portid) ||
#ifdef	MEMMARK
		unmarked(ptmark) ||
#endif
		(ptptr= &ports[portid])->ptstate != PTALLOC ) {
			memunlock();
			return(SYSERR);
	}
	count = scount(ptptr->ptrsem);
	if ( (scnt=scount(ptptr->ptssem)) < 0 )
		count -= scnt;			/* add number waiting	*/
	memunlock();
	return(count);
}

