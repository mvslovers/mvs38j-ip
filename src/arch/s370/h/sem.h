/* sem.h - isbadsem */

#ifndef	H_SEM_H
#define H_SEM_H

#ifndef	NSEM
#define	NSEM		50	/* number of semaphores, if not defined	*/
#endif

#define	SFREE	'\01'		/* this semaphore is free		*/
#define	SUSED	'\02'		/* this semaphore is used		*/

#define	signal(x)	xsignal(x)	// remap name; Systems/C signal() name collision

//  Semaphore count used as follows:
//	Initially set in screate; number of resources available
//	Decremented by wait(); returns immediately while semcnt non-negative, else waits
//	Incremented by signal(), which marks first waiting process ready

struct	sentry	{		/* semaphore table entry		*/
	char	sstate;		/* the state SFREE or SUSED		*/
	int	semcnt;		/* count for this semaphore		*/
	int	sqhead;		/* q index of head of list		*/
	int	sqtail;		/* q index of tail of list		*/
};

struct sema_stat {		// semaphore statistics
	int	inuse;
	int	maxuse;
	int	failures;
};

#ifdef DEFINE_EXTERN
extern	struct	sentry	semaph[];
extern	struct sema_stat sema_stat;
extern	int	nextsem;
#endif /* DEFINE_EXTERN */

#define	isbadsem(s)	(s<0 || s>=NSEM)
#define	semwillwait(s)	(scount(s) <= 0)	// TRUE = issuing wait() will block

SYSCALL newsem();		// allocate an unused semaphore and return its index
SYSCALL scount(int);		// return a semaphore count
SYSCALL screate(unsigned);	// create and initialize a semaphore, returning its id
SYSCALL sdelete(int);		// delete a semaphore by releasing its table entry
SYSCALL xsignal(int);		// signal a semaphore, releasing one waiting process
SYSCALL signaln(int, int);	// signal a semaphore n times
SYSCALL sreset(int, int);	// reset the count and queue of a semaphore
SYSCALL wait(int);		// make current process wait on a semaphore

#endif /* H_SEM_H */





