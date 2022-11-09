/* rtfree.c - rtfree */

#include <conf.h>
#include <kernel.h>
#include <network.h>

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  rtfree  -  remove one reference to a route
 *------------------------------------------------------------------------
 */

int	rtfree(struct route *prt) {

	char	bufcall[FUNCSTR_BUFLEN];


	if (!Route.ri_valid) {
		trace("RTFREE syserr; routing state invalid 0x%x\n");
		return SYSERR;
	}
	dbug(funcstr(bufcall, MY_CALLER));

	wait(Route.ri_mutex);
	RTFREE(prt);
	signal(Route.ri_mutex);

	trace("RTFREE exit <<%s>> OK; route 0x%x\n", bufcall, prt);
	return OK;
}


