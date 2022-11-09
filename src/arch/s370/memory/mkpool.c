/* mkpool.c - mkpool */

#include <conf.h>
#include <kernel.h>
#include <mark.h>
#include <bufpool.h>
#include <sem.h>

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  mkpool  --  allocate memory for a buffer pool and link together
 *------------------------------------------------------------------------
 */
int mkpool(int bufsiz, int numbufs) {

	STATWORD	ps;    
	int		poolid;
	char		*where = NULL;
	dbug(char	bufcall[FUNCSTR_BUFLEN]);


	dbug(funcstr(bufcall, MY_CALLER));
	trace("MKPOOL entered; %d buffers, bufsize %d 0x%x <<%s>>\n", 
		numbufs, bufsiz, bufsiz, bufcall);

#ifdef	MEMMARK
	if ( unmarked(bpmark) )
		poolinit();
#endif

	memlock();
	if (bufsiz < BPMINB 
	    || bufsiz > BPMAXB
	    || numbufs < 1 
	    || numbufs > BPMAXN
	    || nbpools >= NBPOOLS
	    || (where = (char *) getmem((bufsiz+sizeof(int))*numbufs)) == (char *) SYSERR) {
		memunlock();
		trace("MKPOOL syserr <<%s>>\n"
		      "MKPOOL BPMINB %d BPMAXB %d BPMAXN %d NBPOOLS %d nbpools %d where 0x%x\n",
			bufcall, BPMINB, BPMAXB, BPMAXN, NBPOOLS, nbpools, where);
		BUFPOOL_SYSERR(SYSERR);
	}
	poolid = nbpools++;
	bptab[poolid].bpbase = where;		// beginning of buffer pool storage
	bptab[poolid].bptotal = numbufs;
	bptab[poolid].bpmaxused = 0;
	bptab[poolid].bpnext = where;
	bptab[poolid].bpsize = bufsiz;
	bptab[poolid].bpsem = screate(numbufs);
	bufsiz += sizeof(int);

	for (numbufs-- ; numbufs>0 ; numbufs--, where+=bufsiz)
		*( (int *) where ) = (int)(where + bufsiz);
	*( (int *) where) = (int) NULL;

	memunlock();
	trace("MKPOOL exit; created pool %d; %d buffers, bufsize %d 0x%x <<%s>>\n", 
		poolid, bptab[poolid].bptotal, bufsiz, bufsiz, bufcall);
	return(poolid);
}



