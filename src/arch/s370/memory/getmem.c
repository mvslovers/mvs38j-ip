/* getmem.c - getmem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 * getmem  --  allocate heap storage, returning lowest WORD address
 *------------------------------------------------------------------------
 */
WORD	*getmem(unsigned int nbytes) {

	mblock	*p;		// free block being examined
	mblock	*q;		// prior block
	mblock	*leftover;


	memlock();
	if (nbytes == 0 || 
	    memlist.mnext == (struct mblock *) NULL) {
		trace("GETMEM syserr\n");
		dbug(memstate(NULL));
		memunlock();
		return( (WORD *)SYSERR);
	}
	nbytes = (unsigned int) roundmb(nbytes);
	for (q= &memlist,p=memlist.mnext ;
	     p != (struct mblock *) NULL ;
	     q=p,p=p->mnext)
		if ( p->mlen == nbytes) {
			q->mnext = p->mnext;
			memunlock();
			return( (WORD *)p );
		} else if ( p->mlen > nbytes ) {
			leftover = (struct mblock *)( (unsigned)p + nbytes );
			q->mnext = leftover;
			leftover->mnext = p->mnext;
			leftover->mlen = p->mlen - nbytes;
			memunlock();
			return( (WORD *)p );
		}
	memunlock();
	trace("GETMEM syserr\n");
	dbug(memstate(NULL));
	return( (WORD *)SYSERR );
}



