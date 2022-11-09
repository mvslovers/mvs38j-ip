/* xdevwrite.c - xdevwrite */

// Renamed write.c to xdevwrite.c to avoid Standard C name collision

#include <conf.h>
#include <kernel.h>
#include <io.h>

#define DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  xdevwrite  -  write 1 or more bytes to a Xinu device
 *------------------------------------------------------------------------
 */

int xdevwrite(int descrp, const void *buff, unsigned int count) {

	struct	devsw	*pdev;
	int		rv;

	trace("XDEVWRITE enter; dev 0x%x buf 0x%x count %d\n", descrp, buff, count);
	if (isbaddev(descrp) ) {
		trace("XDEVWRITE syserr; isbaddev\n");
		return SYSERR;
	}
	pdev = &devtab[descrp];
	rv = (*pdev->dvwrite)(pdev, buff, count);

	trace("XDEVWRITE exit; return %d\n", rv);
	return rv;
}




