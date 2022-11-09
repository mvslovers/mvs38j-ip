/* xdevseek.c - xdevseek */

// Renamed seek.c to xdevseek.c to avoid Standard C name collision

#include <conf.h>
#include <kernel.h>
#include <io.h>

/*------------------------------------------------------------------------
 *  xdevseek  --  position a Xinu device 
 *		 (very common special case of control)
 *------------------------------------------------------------------------
 */

int xdevseek(int descrp, long pos) {

	struct	devsw	*devptr;

	if (isbaddev(descrp) )
		return(SYSERR);
	devptr = &devtab[descrp];
        return( (*devptr->dvseek)(devptr,pos) );
}

