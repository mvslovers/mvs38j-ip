/* rtadd.c - rtadd */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>

#define	DEBUG
#include <debug.h>

#define	DEBUG_FUNC_HIST		// defined = save function call history
				// in rt_func

#undef	DEBUG_RTDUMP		// dump all routes @ entry & exit

/*------------------------------------------------------------------------
 *  rtadd  -  add a route to the routing table
 *------------------------------------------------------------------------
 */

int rtadd(IPaddr net, IPaddr mask, IPaddr gw, unsigned metric,
	unsigned intf, unsigned ttl) {

	struct	route	*prt, *srt, *prev, *tmprt;
	Bool		isdup;
	int		i, hv, key;
	IPaddr		maskbit;
	dbug(char	bufnet[IP_MAXDOT]);
	dbug(char	bufgw[IP_MAXDOT]);


	dbug(ip2dot(bufnet, net));
	dbug(ip2dot(bufgw, gw));
	trace("RTADD enter; IP %s mask 0x%x gw %s metric %d iface %d TTL %d\n",
		bufnet, mask, bufgw, metric, intf, ttl);

	if (!Route.ri_valid) {
		trace("RTADD routing not init'd; init'ing\n");
		rtinit();
	}

#ifdef	DEBUG_RTDUMP
	trace("RTADD routes at entry\n");
	dbug(rtdump(ALL_ROUTES));
#endif

	prt = rtnew(net, mask, gw, metric, intf, ttl);		// alloc & init route buffer
	if (prt == (struct route *)SYSERR) {
		trace("RTADD syserr; rtnew\n");
		return SYSERR;
	}

#ifdef	DEBUG_FUNC_HIST
	funchist(prt->rt_func, sizeof(prt->rt_func), NULL);	// save function call history
#endif

	/* compute the queue sort key for this route */
	// It's the number of 1 bits in the mask, so more-specific routes
	// appear before less-specific routines in the rttable chain

	key = 0;
	maskbit = 1;						// bit to examine
	for (i = 0; i < (sizeof(IPaddr) * BITS_PER_OCTET); i++) {
		if (mask & maskbit) key++;			// accumulate # 1 bits
		maskbit = maskbit << 1;				// shift test bit over 1
	}
	prt->rt_key = key;

	wait(Route.ri_mutex);

	/* special case for default routes */

	if (net == RT_DEFAULT) {
		if (Route.ri_default) {
			trace("RTADD exit; OK, previous default 0x%x removed\n",
				Route.ri_default);
			RTFREE(Route.ri_default);
		}
		Route.ri_default = prt;
		signal(Route.ri_mutex);
		trace("RTADD exit; OK, default route 0x%x installed\n", prt);
		return OK;
	}

	//  Scan rttable for a duplicate route, setting srt & prev for later

	prev = NULL;
	hv = rthash(net);					// calculate net hash
	isdup = FALSE;
	for (srt = rttable[hv]; srt; srt = srt->rt_next) {
		if (prt->rt_key > srt->rt_key)
			break;
		if (srt->rt_net == prt->rt_net &&
	    	    srt->rt_mask == prt->rt_mask) {
			isdup = TRUE;
			break;
		}
		prev = srt;
	}

	// Handle a duplicate route

	if (isdup) {
		trace("RTADD duplicate routes; caller 0x%x dup 0x%x\n", prt, srt);
		if (srt->rt_gw == prt->rt_gw) {
			/* just update the existing route */
#ifdef	RIP
			if (dorip) {
				srt->rt_ttl = ttl;
				if (srt->rt_metric != metric) {
					if (metric == RTM_INF)
						srt->rt_ttl = RIPZTIME;
					send(rippid, 0);
				}
			}
#endif	/* RIP */
			srt->rt_metric = metric;
			RTFREE(prt);
			signal(Route.ri_mutex);
			trace("RTADD exit; updated existing route 0x%x "
				"metric %d (same gateway); discarded 0x%x\n", 
				srt, metric, prt);
			return OK;
		}
		/* else, someone else has a route there... */
		if (srt->rt_metric <= prt->rt_metric) {
			/* metric no better; drop the new one */
			RTFREE(prt);
			signal(Route.ri_mutex);
			trace("RTADD exit; duplicate, new dropped (new metric no better)\n");
			return OK;
		}
#ifdef	RIP
		else if (dorip)
			send(rippid, 0);
#endif	/* RIP */
		tmprt = srt;
		srt = srt->rt_next;
		RTFREE(tmprt);
		trace("RTADD discarded old route 0x%x\n", srt);
	}

	//  Not a duplicate, if RIP enabled & active rip requested, give it to RIP

#ifdef	RIP
	else if (dorip)
		send(rippid, 0);
#endif	/* RIP */

	//  Add the new route, chaining it in as needed

	prt->rt_next = srt;
	if (prev)
		prev->rt_next = prt;
	else
		rttable[hv] = prt;

	signal(Route.ri_mutex);

#ifdef	DEBUG_RTDUMP
	trace("RTADD all routes at exit\n");
	dbug(rtdump(ALL_ROUTES));
#else
	dbug(rtdump(prt));
#endif
	trace("RTADD exit; OK\n");
	return OK;
}




