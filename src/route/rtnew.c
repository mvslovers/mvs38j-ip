/* rtnew.c - rtnew */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <string.h>		// Standard C

#undef	DEBUG
#include <debug.h>

#undef	DEBUG_SNAPNEW		// defined = snap newly created route
#define	DEBUG_WARN_DISCARD	// defined = warn when route discarded
				// due to no struct route buffers available

/*------------------------------------------------------------------------
 *  rtnew  -  create a route structure
 *------------------------------------------------------------------------
 */

struct route *rtnew(IPaddr net, IPaddr mask, IPaddr gw, unsigned metric, unsigned ifnum, unsigned ttl) {

	struct	route	*prt;
	dbug(char	bufnet[IP_MAXDOT]);
	dbug(char	bufgw[IP_MAXDOT]);
	dbug(char	bufcall[FUNCSTR_BUFLEN]);


	dbug(ip2dot(bufnet, net));
	dbug(ip2dot(bufgw, gw));
	dbug(funcstr(bufcall, MY_CALLER));
	trace("RTNEW enter <<%s>>; IP %s mask 0x%x gw %s metric %d iface %d TTL %d\n",
		bufcall, bufnet, mask, bufgw, metric, ifnum, ttl);

	prt = (struct route *)getbuf(Route.ri_bpool);
	if (prt == (struct route *)SYSERR) {
		IpRoutingDiscards++;

#ifdef	DEBUG_WARN_DISCARD
		kprintf("RTNEW getbuf syserr; route discarded\n");
		kprintf("RTADD possible config error: RT_BPSIZE %d too small?\n", RT_BPSIZE);
		dbug(rtdump(ALL_ROUTES));
#endif

		return (struct route *)SYSERR;
	}

	memset(prt, 0x00, sizeof(struct route));

	prt->rt_net = net;
	prt->rt_mask = mask;
	prt->rt_gw = gw;
	prt->rt_metric = metric;
	prt->rt_ifnum = ifnum;
	prt->rt_ttl = ttl;
	prt->rt_refcnt = 1;	/* our caller */
	prt->rt_usecnt = 0;
	prt->rt_next = NULL;
	funchist(prt->rt_func, sizeof(prt->rt_func), NULL);

#ifdef	DEBUG_SNAPNEW
	snap("RTNEW new route", (void *)prt, sizeof(struct route));
#endif

	trace("RTNEW exit; created struct route 0x%x\n", prt);
	return prt;
}

