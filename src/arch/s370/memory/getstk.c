/* getstk.c - getstk */

#include <conf.h>
#include <kernel.h>
#include <mem.h>

/*------------------------------------------------------------------------
 * getstk  --  allocate stack memory, returning address of topmost WORD
 *------------------------------------------------------------------------
 */
WORD *getstk(unsigned int nbytes) {
	mblock	*p, *q;	/* q follows p along memlist		*/
	mblock	*fits, *fitsq;
	WORD	len;

	if (nbytes == 0) {
		return( (WORD *)SYSERR );
	}
	memlock();
	nbytes = (unsigned int) roundew(nbytes);
	fits = (struct mblock *) NULL;
	q = &memlist;
	for (p = q->mnext ; p !=(struct mblock *) NULL ; q = p,p = p->mnext)
		if ( p->mlen >= nbytes) {
			fitsq = q;
			fits = p;
		}
	if (fits == (struct mblock *) NULL) {
		memunlock();
		return( (WORD *)SYSERR );
	}
	if (nbytes == (len = fits->mlen) ) {
		fitsq->mnext = fits->mnext;
	} else {
		fits->mlen -= nbytes;
	}
	fits = (struct mblock *) ((WORD) fits + len - sizeof(WORD));
	*((WORD *) fits) = nbytes;
	memunlock();
	return( (WORD *) fits);
}

