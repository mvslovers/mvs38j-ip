/* xdevputc.c - xdevputc */

// Renamed putc.c to xdevputc.c to avoid Standard C name collision

#include <conf.h>
#include <kernel.h>
#include <io.h>

/*------------------------------------------------------------------------
 *  xdevputc  -  write a single character to a Xinu device
 *------------------------------------------------------------------------
 */

SYSCALL	xdevputc(int descrp, char ch) {

	struct	devsw	*devptr;
	
	if (isbaddev	(descrp) )
		return(SYSERR);
	devptr = &devtab[descrp];
	return(	(*devptr->dvputc)(devptr,ch) );
}

