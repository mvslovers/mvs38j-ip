/* xdevinit.c - xdevinit */

//  Renamed init.c to xdevinit.c for Xinu device primitive name consistency

#include <conf.h>
#include <kernel.h>
#include <io.h>

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  xdevinit  -  initialize a Xinu device
 *------------------------------------------------------------------------
 */

int xdevinit(int descrp) {

	struct	devsw	*devptr;

	trace("XDEVINIT initializing device %d %s\n", 
		descrp, devtab[descrp].dvname);
	if (isbaddev(descrp) ) {
		trace("XDEVINIT return SYSERR\n");
		return(SYSERR);
	}
	devptr = &devtab[descrp];
        return( (devptr->dvinit)(devptr) );
}


