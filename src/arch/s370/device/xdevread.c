/* xdevread.c - xdevread */

// Renamed read.c to xdevread.c to avoid Standard C name collision

#include <conf.h>
#include <kernel.h>
#include <io.h>

/*------------------------------------------------------------------------
 *  xdevread  -  read one or more bytes from a Xinu device
 *------------------------------------------------------------------------
 */

int xdevread(int descrp, void *buff, unsigned int count) {

	struct	devsw	*pdev;

	if (isbaddev(descrp) )
		return(SYSERR);
	pdev = &devtab[descrp];
	return (*pdev->dvread)(pdev, buff, count);
}


