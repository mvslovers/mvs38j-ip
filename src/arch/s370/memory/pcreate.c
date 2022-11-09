/* pcreate.c - pcreate */

#include <conf.h>
#include <kernel.h>
#include <mark.h>
#include <ports.h>
#include <sem.h>

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  pcreate  --  create a port that allows "count" outstanding messages
 *------------------------------------------------------------------------
 */
SYSCALL pcreate(unsigned int count) {
	int	i, p;
	struct	pt	*ptptr;

	trace("PCREATE entered; count %d\n", count);
	callinfo("PCREATE");
	memlock();
#ifdef	MEMMARK
	if (mark(ptmark) == OK)
		pinit(MAXMSGS);
#endif
	for (i=0 ; i<NPORTS ; i++) {
		if ( (p=ptnextp--) <= 0)
			ptnextp = NPORTS - 1;
		if ( (ptptr= &ports[p])->ptstate == PTFREE) {
			ptptr->ptstate = PTALLOC;
			ptptr->ptssem = screate(count);
			ptptr->ptrsem = screate(0);
			ptptr->pthead = ptptr->pttail = (struct ptnode *)NULL;
			ptptr->ptseq++;
			ptptr->ptmaxcnt = count;
			memunlock();
			return(p);
		}
	}
	memunlock();
	trace("PCREATE syserr\n");
	return(SYSERR);
}


