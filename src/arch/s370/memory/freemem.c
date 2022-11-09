/* freemem.c - freemem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>

/*------------------------------------------------------------------------
 *  freemem  --  free a memory block, returning it to memlist
 *------------------------------------------------------------------------
 */
SYSCALL freemem(void *block0, unsigned size) {
	mblock *block = (mblock *)block0;
	STATWORD ps;    
	mblock	*p, *q;
	unsigned top;

	if (size==0 || (unsigned)block>(unsigned)maxaddr
	    || ((unsigned)block)<((unsigned) memlist.mnext))
		return(SYSERR);
	size = (unsigned)roundmb(size);
	memlock();
	for( p=memlist.mnext,q= &memlist;
	     p != (struct mblock *) NULL && p < block ;
	     q=p,p=p->mnext )
		;
	if ((top=q->mlen+(unsigned)q)>(unsigned)block && q!= &memlist ||
	    p!=NULL && (size+(unsigned)block) > (unsigned)p ) {
		memunlock();
		return(SYSERR);
	}
	if ( q!= &memlist && top == (unsigned)block )
			q->mlen += size;
	else {
		block->mlen = size;
		block->mnext = p;
		q->mnext = block;
		q = block;
	}
	if ( (unsigned)( q->mlen + (unsigned)q ) == (unsigned)p) {
		q->mlen += p->mlen;
		q->mnext = p->mnext;
	}
	memunlock();
	return(OK);
}


