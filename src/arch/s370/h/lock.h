/* lock.h - MVS lock manager; see deflock.c, mvslock.c, mvsunlock.c */

#ifndef H_LOCK_H
#define H_LOCK_H

//  Notes:  The nulluser process may hold no locks, since its pid = 0,
//  which means the lock is free.

struct	lock {
	int		owner;		// pid of process holding lock or zero
	int		count;		// # times pid has seized this lock
	long long	time;		// time pid seized lock (STCK)
};

#define	LOCK	struct lock
#define	NLOCKS	5
#define MAXLOCK NLOCKS-1
#define	LOCKNULL	0x01234567	// special nulluser pid value

enum	lockflagenum {			// lock.flag values
	LFUNUSED	= 0		// no values currently assigned
};

enum	kenum {				// lock manager parms
	KMEM		= 0,		// memory
	KPROC		= 1,		// process
	KQUEUE		= 2,		// queue
	KSEM		= 3,		// semaphore
	KCLOCK		= 4		// clock
};

//  The locks in locks[NLOCKS] are referred to as k-locks in the doc

#ifdef DEFINE_EXTERN
extern	LOCK	locks[NLOCKS];
#endif

void	mvslock(int, char *);		// kenum lock to seize
void	mvsunlock(int, char *);		// kenum lock to release

#endif /* H_LOCK_H */





