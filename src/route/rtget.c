/* rtget.c - rtget */

#include <conf.h>
#include <kernel.h>
#include <network.h>

#undef	DEBUG
#include <debug.h>

#undef	DEBUG_RTDUMP

/*------------------------------------------------------------------------
 *  rtget  -  get the route for a given IP destination
 *------------------------------------------------------------------------
 */
struct route *rtget(IPaddr dest, Bool local) {

	struct	route	*prt;
	int		hv;
	dbug(char	bufdest[IP_MAXDOT]);
	dbug(char	bufcall[FUNCSTR_BUFLEN]);

	dbug(ip2dot(bufdest, dest));
	dbug(funcstr(bufcall, MY_CALLER));
	trace("RTGET enter <<%s>>; dest %s iface %s\n",
		bufcall, bufdest, local ? "NI_LOCAL" : "NI_PRIMARY");

	if (!Route.ri_valid) {
		trace("RTGET initializing routes\n");
		rtinit();
	}

	trace("RTGET wait for routing mutex\n");
	wait(Route.ri_mutex);

	trace("RTGET calculate hash\n");
	hv = rthash(dest);

	dbug(rtdump(ALL_ROUTES));
	trace("RTGET scanning routes\n");
	for (prt = rttable[hv]; prt; prt = prt->rt_next) {
		if (prt->rt_ttl <= 0)
			continue;		/* route has expired */
		if (netmatch(dest, prt->rt_net, prt->rt_mask, local))
			if (prt->rt_metric < RTM_INF)
				break;
	}
	if (prt == 0)
		prt = Route.ri_default;	/* may be NULL too... */
	if (prt != 0 && prt->rt_metric >= RTM_INF)
		prt = 0;
	if (prt) {
		prt->rt_refcnt++;
		prt->rt_usecnt++;
	}

	signal(Route.ri_mutex);

	trace("RTGET selected route 0x%x for dest %s\n", (void *)prt, bufdest);
	dbug(rtdump(prt));

	trace("RTGET exit <<%s>>; dest %s route selected 0x%x\n", 
		bufcall, bufdest, (void *)prt);

#ifdef	DEBUG_RTDUMP
	snap("RTGET returned route", (void *)prt, sizeof(struct route));
#endif

	return prt;
}

