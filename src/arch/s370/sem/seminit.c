/* seminit.c - seminit */

#include <conf.h>
#include <kernel.h>
#include <sem.h>

/*------------------------------------------------------------------------
 *  seminit  --  initialize semaphore management
 *------------------------------------------------------------------------
 */
void seminit() {
	int		i;
	struct sentry	*sptr;

	nextsem = NSEM-1;
	for (i=0; i<NSEM; i++) {
		sptr = &semaph[i];
		sptr->sstate = SFREE;
		sptr->sqtail = 1 + (sptr->sqhead = newqueue());
	}

} /* seminit */

