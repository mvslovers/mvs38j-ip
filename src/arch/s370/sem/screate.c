/* screate.c - screate */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>

#undef	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 * screate  --  create and initialize a semaphore, returning its id
 *		See sem.h for semaphore notes
 *------------------------------------------------------------------------
 */
SYSCALL screate(unsigned count) {
	STATWORD ps;    
	int	sem;

	disable(ps);
	sem = newsem();
	if (sem == SYSERR) {
		sema_stat.failures++;
		restore(ps);
		trace("SCREATE newsem SYSERR\n");
		return(SYSERR);
	}

	semaph[sem].semcnt = count;

	/* sqhead and sqtail were initialized at system startup */

	sema_stat.inuse++;
	if (sema_stat.inuse > sema_stat.maxuse)
		sema_stat.maxuse = sema_stat.inuse;

	restore(ps);
	trace("SCREATE created semaphore %d; initial semcnt %d\n", sem, count);
	return sem;
}


