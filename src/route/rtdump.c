/* rtdump.c - rtdump */

#include <conf.h>
#include <kernel.h>
#include <sleep.h>
#include <network.h>

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  rtdump  -  dump the routing table to the console
 *		When pr == NULL, dump whole table
 *		Else just dump single pr entry
 *------------------------------------------------------------------------
 */

//  Route dump, being a debug tool, is called from many places in MVS-Xinu.
//  Some of those places already hold the routing mutex, which would cause
//  us to wait forever if we attempted to gain control of it.
//  So we don't.

void	rtdump(struct route *pr) {

	struct route *srt;
	int	i;
	char	bufcall[FUNCSTR_BUFLEN] = "";
	char	bufndx[8] = "";

	if (!Route.ri_valid) {
		trace("RTDUMP initializing routes\n");
		rtinit();
	}

	dbug(funcstr(bufcall, MY_CALLER));		// who called me?
	rtprint((struct route *)NULL, bufcall);		// print heading

	if (pr != ALL_ROUTES) {				// print single entry
		rtprint(pr, "");
		return;
	}

	for(i = 0; i < RT_TSIZE; ++i) {
		for (srt = rttable[i]; (srt); srt = srt->rt_next) {
			sprintf(bufndx, "hash=%d", i);
			rtprint(srt, bufndx);
		}
	}

	if (Route.ri_default)
		rtprint(Route.ri_default, "-> default");
	else
		kprintf("no default route\n");
	trace("\n");
}

