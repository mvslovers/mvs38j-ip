/* ipredirect.c - ipredirect */

#include <conf.h>
#include <kernel.h>
#include <network.h>

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  ipredirect  -  send redirects, if needed
 *------------------------------------------------------------------------
 */

void	ipredirect(struct ep *pep, unsigned ifn, struct route *prt) {

	struct	ip	*pip = (struct ip *)pep->ep_data;
	struct	route	*tprt;
	int		rdtype, isonehop;
	IPaddr		nmask;	/* network part's mask			*/


	if (ifn == NI_LOCAL || ifn != prt->rt_ifnum) {
		trace("IPREDIRECT exit; interface %d\n", ifn);
		return;
	}

	tprt = rtget(pip->ip_src, RTF_LOCAL);
	dbug(rtdump(tprt));
	if (!tprt) {
		trace("IPREDIRECT exit; source route (locally generated traffic) NULL\n");
		return;
	}

	isonehop = tprt->rt_metric == 0;
	rtfree(tprt);
	if (!isonehop)
		return;

	/* got one... */

	panic("IPREDIRECT has redirect FIXME\n");
	dbug(ipdump(pip, 0));

	nmask = netmask(prt->rt_net);	/* get the default net mask	*/
	if (prt->rt_mask == nmask)
		rdtype = ICC_NETRD;
	else
		rdtype = ICC_HOSTRD;

	icmp(ICT_REDIRECT, rdtype, pip->ip_src, pep, (void *)prt->rt_gw);
}

