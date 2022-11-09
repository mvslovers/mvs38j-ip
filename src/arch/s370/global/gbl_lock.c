/* gbl_lock.c - define lock manager global data area */

#include <conf.h>
#include <kernel.h>
#include <lock.h>

//  The following locks are used to keep the dispatcher from
//  taking control away from kernel code while it is in
//  the process of updating MVS-Xinu system global data
//  such as process queues, memory control blocks, etc.

//  This implementation is used in lieu of disabling/enabling
//  the system, which would needlessly interfere with standard
//  MVS operation.  Thus, the disable(), enable(), and restore()
//  system calls are no longer used in MVS-Xinu.

#ifdef DEFINE_EXTERN
	LOCK	locks[NLOCKS];
#endif


