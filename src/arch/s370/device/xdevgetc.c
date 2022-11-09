/* xdevgetc.c - xdevgetc */

// Renamed getc.c to xdevgetc.c to avoid Standard C name collision

#include <conf.h>
#include <kernel.h>
#include <io.h>

/*------------------------------------------------------------------------
 *  xdevgetc  -  get one character from a Xinu device
 *------------------------------------------------------------------------
 */

SYSCALL	xdevgetc(int descrp) {

	struct	devsw	*devptr;

	if (isbaddev(descrp) )
		return(SYSERR);
	devptr = &devtab[descrp];
        return( (*devptr->dvgetc)(devptr) );
}

