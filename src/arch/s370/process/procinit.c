/* procinit.c - procinit */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <string.h>	// ANSI C

/*------------------------------------------------------------------------
 *  procinit  --  initialize process management
 *------------------------------------------------------------------------
 */
void
procinit()
{
	int	i, j;
	struct	pentry	*pptr;

	numproc = 1;			// sysinit() running, don't allow
					// xdone() until sysinit() complete

	nextproc = NPROC-1;

	for (i=0 ; i<NPROC ; i++)	/* initialize process table */
		proctab[i].pstate = PRFREE;

	pptr = &proctab[NULLPROC];	/* initialize null process entry */
	memset(pptr, 0, sizeof(struct pentry));
	pptr->pstate = PRCURR;
	for (j=0; j<7; j++)
		pptr->pname[j] = "prnull"[j];
	pptr->paddr = (WORD) nulluser;
	pptr->pargs = 0;
	pptr->pprio = 0;
	pptr->pflag = PEFCREATE | PEFINIT;	// prevent resched from creating, 
						// initializing existing process
	currpid = NULLPROC;
//	snap("PROCINIT nulluser pentry", (void *)pptr, sizeof(struct pentry));

	rdyhead = newqueue();		/* initialize ready list */
	rdytail = 1 + rdyhead;

} /* procinit */


