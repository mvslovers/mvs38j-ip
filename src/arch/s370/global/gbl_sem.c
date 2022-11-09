/* gbl_sem.c - define semaphore global data area */

#include <conf.h>
#include <kernel.h>
#include <sem.h>

int	nextsem;		/* next sempahore slot to use in screate*/
struct	sentry	semaph[NSEM];	/* semaphore table			*/ 
struct	sema_stat sema_stat;	// semaphore statistics


