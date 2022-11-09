/* freebuf.c - freebuf */

#include <conf.h>
#include <kernel.h>
#include <mark.h>
#include <bufpool.h>
#include <sem.h>

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  freebuf  --  free a buffer that was allocated from a pool by getbuf
 *------------------------------------------------------------------------
 */

int freebuf(void *buf0) {

	STATWORD	ps;    
	int		*buf = (int *)buf0;
	int		poolid;
	dbug(char	bufcall[FUNCSTR_BUFLEN]);


	dbug(funcstr(bufcall, MY_CALLER));

#ifdef	MEMMARK
	if ( unmarked(bpmark) ) {
		trace("FREEBUF syserr; unmarked\n");
		BUFPOOL_SYSERR(SYSERR);
	}
#endif
	poolid = *(--buf);
	if (poolid<0 || poolid>=nbpools) {
		trace("FREEBUF syserr; poolid\n");
		BUFPOOL_SYSERR(SYSERR);
	}

	memlock();
	*buf = (int) bptab[poolid].bpnext;
	bptab[poolid].bpnext = (char *) buf;
	memunlock();
	signal(bptab[poolid].bpsem);

	trace("FREEBUF pid %d freed buffer 0x%x size %d poolid %d <<%s>>\n", 
		getpid(), buf0, bptab[poolid].bpsize, poolid, bufcall);
	return(OK);
}




