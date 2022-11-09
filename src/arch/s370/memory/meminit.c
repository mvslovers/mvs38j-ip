/* meminit.c - meminit */

#include <conf.h>
#include <kernel.h>
#include <mem.h>

/*------------------------------------------------------------------------
 *  meminit  -	initialize memory management
 *		assumes we're called in problem state
 *------------------------------------------------------------------------
 */
void meminit() {
	int		len;
	void		*p;
	mblock		*mptr;

	kprintf("MEMINIT entered\n");
	len = MVSPAGESIZE * MVSPAGES;		// # bytes to allocate
	p = mvsallocmem(len);			// allocate MVS memory (subpool 0)
	maxaddr = (char *)p + len - 1;
	memlist.mnext = (void *)p;		// @ mblock in allocated storage
	memlist.mlen = 0;			// memlist is only anchor, has no memory
	mptr = (mblock *)p;
	mptr->mnext = 0;
	mptr->mlen = len;
	kprintf("MEMINIT exited\n");

} /* meminit */


