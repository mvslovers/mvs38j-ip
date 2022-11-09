/* create.c - create */

// TODO scan proctab to attempt reuse of previously created environment
//	These entries have PEFINI set, and probably are in state PRDEAD

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <stdarg.h>

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */

SYSCALL create(PROCESS (*procaddr)(), int ssize, int priority, char *name, int nargs, ...) {

	va_list		ap;
	unsigned long	savsp, *pushsp;
	STATWORD 	ps;    
	int		pid;		/* stores new process id	*/
	struct	pentry	*pptr;		/* pointer to proc. table entry */
	int		i;
	unsigned long	*saddr, *uarg;	/* stack address		*/
	unsigned long   *getstk();
	int		INITRET();
	dbug(char	bufcall[FUNCSTR_BUFLEN]);


//	kprintf("CREATE entered for %s\n", name);
	if (priority < 1) {
		trace("CREATE syserr prio\n");
		return SYSERR;
	}
	if (ssize < MINSTK) ssize = MINSTK;
	ssize = (int) roundew(ssize);
	saddr = (unsigned long *)getstk(ssize);		// allocate process stack
	if (saddr == (unsigned long *)SYSERR) {
		trace("CREATE syserr getstk\n");
		return SYSERR;
	}
	pid=newpid();
	if (pid == SYSERR) {
		trace("CREATE syserr newpid\n");
		return SYSERR;
	}
	va_start(ap, nargs);
	disable(ps);
	numproc++;
	pptr = &proctab[pid];

	memset(pptr, 0x00, sizeof(pentry));

	mvsenv(ENVCREATE, pptr);

//	snap("CREATE envcreate r0:r15", &(pptr->gpr), sizeof(pptr->gpr));
	pptr->pstate = PRSUSP;
	pptr->ppid = pid;
	pptr->fildes[0] = 0;	/* stdin set to console */
	pptr->fildes[1] = 0;	/* stdout set to console */
	pptr->fildes[2] = 0;	/* stderr set to console */
	for (i=3; i < _NFILE; i++)	/* others set to unused */
		pptr->fildes[i] = FDFREE;
	for (i=0 ; i<PNMLEN && (int)(pptr->pname[i]=name[i])!=0 ; i++)
		;
	pptr->pprio = priority;
	pptr->psem = 0;
	pptr->phasmsg = FALSE;
	pptr->pnxtkin = BADPID;
	pptr->paddr = (long)procaddr;	// addr of proc init code

	restore(ps);
	va_end(ap);
	trace("CREATE complete; new pid %d 0x%x process %s <<%s>>\n", 
		pid, pid, pptr->pname, funcstr(bufcall, MY_CALLER));
	return pid;
}





