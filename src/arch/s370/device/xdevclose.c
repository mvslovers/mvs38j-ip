/* xdevclose.c - xdevclose */

//  Renamed to xdevclose to avoid Standard C name collision
//  Macro in io.h maps to new name

#include <conf.h>
#include <kernel.h>
#include <io.h>

/*------------------------------------------------------------------------
 *  xdevclose  -  close a Xinu device
 *------------------------------------------------------------------------
 */

SYSCALL xdevclose(int descrp) {

	struct	devsw	*devptr;

	if (isbaddev(descrp) )
		return(SYSERR);
	devptr = &devtab[descrp];
	return( (*devptr->dvclose)(devptr));
}


