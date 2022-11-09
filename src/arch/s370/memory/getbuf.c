/* getbuf.c - getbuf */

#include <conf.h>
#include <kernel.h>
#include <mark.h>
#include <bufpool.h>
#include <sem.h>

#define	DEBUG
#include <debug.h>

#undef	DEBUG_WAIT		// defined = issue msg before blocking wait()

#undef	DEBUG_ROUTE		// defined = stupid routing debug code
#ifdef	DEBUG_ROUTE
	#include <route.h>
#endif

/*------------------------------------------------------------------------
 *  getbuf  --  get a buffer from a buffer pool
 *------------------------------------------------------------------------
 */

void	*getbuf(unsigned poolid) {

	STATWORD	ps;    
	int		*buf, inuse;
	dbug(char	bufcall[FUNCSTR_BUFLEN]);


	dbug(funcstr(bufcall, MY_CALLER));
//	trace("GETBUF enter; poolid %d\n", poolid);

#ifdef	MEMMARK
	if (unmarked(bpmark)) {
		trace("GETBUF initializing buffer pools\n");
		poolinit();
	}
#endif
	if (poolid >= nbpools) {
		trace("GETBUF syserr; poolid >= nbpools\n");
		BUFPOOL_SYSERR((void *) SYSERR);
	}

#ifdef	DEBUG_WAIT
	if (semwillwait(bptab[poolid].bpsem)) 
		trace("GETBUF waiting for poolid %d sem\n", poolid);
#endif

	wait(bptab[poolid].bpsem);
//	trace("GETBUF semaphore owned; poolid %d\n", poolid);

#ifdef	DEBUG_ROUTE
	if (	(Route.ri_valid == TRUE) && 
		(poolid == Route.ri_bpool) &&
		(Route.ri_mutex != 0)
		) {
		trace("GETBUF poolid %d Route.ri_bpool %d\n", poolid, Route.ri_bpool);
		snap("GETBUF Route", (void *)&Route, sizeof(struct rtinfo));
		snap("GETBUF bptab for poolid", (void *)&bptab[poolid], sizeof(struct bpool));
		snap("GETBUF buffer pool", (void *)bptab[poolid].bpbase,
			bptab[poolid].bptotal * (bptab[poolid].bpsize + sizeof(int)) );
	}
#endif

	memlock();
	inuse = bptab[poolid].bptotal - scount(bptab[poolid].bpsem);
	if (inuse > bptab[poolid].bpmaxused)
		bptab[poolid].bpmaxused = inuse;

	buf = (int *) bptab[poolid].bpnext;
//	trace("GETBUF new buffer prefix 0x%x, next 0x%x\n", buf, *buf);
	bptab[poolid].bpnext = (char *) *buf;
	memunlock();

//	trace("GETBUF storing poolid 0x%x\n", buf);
	*buf++ = poolid;

	trace("GETBUF exit; currpid %d %s poolid %d size %d buffer 0x%x <<%s>>\n", 
		getpid(), proctab[getpid()].pname, poolid, 
		bptab[poolid].bpsize, buf, bufcall);
	return( (void *) buf );
}

/*------------------------------------------------------------------------
 * nbgetbuf - a non-blocking version of getbuf
 *------------------------------------------------------------------------
 */
int *nbgetbuf(int poolid) {
	STATWORD ps;    
	int	*buf;
	int	inuse;

#ifdef	MEMMARK
	if ( unmarked(bpmark) ) {
		trace("NBGETBUF syserr; unmarked\n");
		BUFPOOL_SYSERR((int *) SYSERR);
	}
#endif
	if (poolid<0 || poolid>=nbpools) {
		trace("NBGETBUF syserr; poolid\n");
		BUFPOOL_SYSERR((int *) SYSERR);
	}
	memlock();
	if (scount(bptab[poolid].bpsem) <= 0) {
		memunlock();
		return 0;
	}
	wait(bptab[poolid].bpsem);	/* shouldn't block */
	inuse = bptab[poolid].bptotal - scount(bptab[poolid].bpsem);
	if (inuse > bptab[poolid].bpmaxused)
		bptab[poolid].bpmaxused = inuse;
	buf = (int *) bptab[poolid].bpnext;
	bptab[poolid].bpnext = (char *) *buf;
	memunlock();
	*buf++ = poolid;
	return( (int *) buf );
}









