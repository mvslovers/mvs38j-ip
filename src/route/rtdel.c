/* rtdel.c - rtdel */

#include <conf.h>
#include <kernel.h>
#include <network.h>

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  rtdel  -  delete the route with the given net, mask
 *------------------------------------------------------------------------
 */

int	rtdel(IPaddr net, IPaddr mask) {

	struct	route	*prt, *prev;
	int		hv, i;
	char		bufip[IP_MAXDOT];
	char		bufnet[IP_MAXDOT];


	trace("RTDEL enter; net %s mask %s\n", 
		ip2dot(bufnet, net), 
		ip2dot(bufip, mask));

	if (!Route.ri_valid) {
		trace("RTDEL syserr\n");
		return SYSERR;
	}
	wait(Route.ri_mutex);
	if (Route.ri_default &&
	    net == Route.ri_default->rt_net) {
		RTFREE(Route.ri_default);
		Route.ri_default = 0;
		signal(Route.ri_mutex);
		trace("RTDEL exit; OK, dropped DEFAULT\n");
		return OK;
	}
	hv = rthash(net);

	prev = NULL;
	for (prt = rttable[hv]; prt; prt = prt->rt_next) {
		if (net == prt->rt_net &&
		    mask == prt->rt_mask)
			break;
		prev = prt;
	}
	if (prt == NULL) {
		signal(Route.ri_mutex);
		trace("RTDEL syserr\n");
		return SYSERR;
	}
	if (prev)
		prev->rt_next = prt->rt_next;
	else
		rttable[hv] = prt->rt_next;
	RTFREE(prt);
	signal(Route.ri_mutex);
	trace("RTDEL exit; OK\n");
	return OK;
}


