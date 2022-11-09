/* ipproc.c - ipproc */

#include <conf.h>
#include <kernel.h>
#include <network.h>

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  ipproc  -  handle an IP datagram coming in from the network
 *------------------------------------------------------------------------
 */

PROCESS ipproc(void) {

	struct	ep	*pep;
	struct	ip	*pip;
	struct	route	*pdstrt;	// destination route
	Bool		nonlocal;
	int		ifnum;
	short		sum;		// checksum
	dbug(char	bufsrc[IP_MAXDOT]);
	dbug(char	bufdst[IP_MAXDOT]);


	trace("IPPROC entered\n");
	ippid = getpid();		/* so others can find us	*/

	signal(Net.sema);		/* signal initialization done	*/

	while (shutxinu == FALSE) {

		trace("IPPROC currpid %d %s *** GETTING IP FRAME ***\n", 
			ippid, proctab[ippid].pname);

		pep = ipgetp(&ifnum);			// get an IP frame + overhead
		pip = (struct ip *)pep->ep_data;
		dbug(ip2dot(bufsrc, pip->ip_src));
		dbug(ip2dot(bufdst, pip->ip_dst));
		dbug(rtdump(ALL_ROUTES));

		trace("\nIPPROC pid %d %s *** RECEIVED IP FRAME *** ifnum %d pep 0x%x pip 0x%x\n", 
			ippid, proctab[ippid].pname, ifnum, pep, pip);
		dbug(ipdump(pip, 0));

		if ((pip->ip_verlen>>4) != IP_VERSION) {
			IpInHdrErrors++;
			trace("IPPROC incorrect IP version\n\n");
			freebuf(pep);
			continue;
		}
		if (IP_CLASSE(pip->ip_dst)) {
			IpInAddrErrors++;
			trace("IPPROC class E discarded\n\n");
			freebuf(pep);
			continue;
		}
		if (ifnum != NI_LOCAL) {
			sum = ipcksum((void *)pip);
			trace("IPPROC calculated cksum 0x%x packet cksum 0x%x\n",
				sum, pip->ip_cksum);
			if (sum != pip->ip_cksum) {
				IpInHdrErrors++;
				trace("IPPROC checksum ERROR; discarded\n\n");
				freebuf(pep);
				continue;
			} else
				trace("IPPROC checksum OK\n", sum);
			ipnet2h(pip);
			pep->ep_order |= EPO_IP;
		} else {
			trace("IPPROC ** NI_LOCAL %d\n", NI_LOCAL);
		}

		trace("IPPROC call rtget to select dest route\n");
		pdstrt = rtget(pip->ip_dst, (ifnum == NI_LOCAL));	// get dest route
		dbug(rtdump(pdstrt));

		if (pdstrt == NULL) {
			trace("IPPROC rtget selected NULL dest route\n");
			if (gateway) {
				iph2net(pip);
				pep->ep_order &= ~EPO_IP;
				trace("IPPROC destination %s unreachable\n", bufdst);
				icmp(ICT_DESTUR, ICC_NETUR,
						pip->ip_src, pep, 0);
			} else {
				IpOutNoRoutes++;
				trace("IPPROC IpOutNoRoutes; discarded dest %s\n\n", bufdst);
				freebuf(pep);
			}
			continue;
		}

		nonlocal = (ifnum != NI_LOCAL) && (pdstrt->rt_ifnum != NI_LOCAL);
		trace("IPPROC nonlocal %s ifnum %d dest iface %d\n",
			nonlocal ? "TRUE" : "FALSE", ifnum, pdstrt->rt_ifnum);
#ifndef	MVS38J
		if (!gateway && nonlocal) {
			IpInAddrErrors++;
			trace("IPPROC IpInAddrErrors; discarded\n\n");
			freebuf(pep);
			rtfree(pdstrt);
			continue;
		}
#endif
		if (nonlocal)
			IpForwDatagrams++;

		/* fill in src IP, if we're the sender */

		if (ifnum == NI_LOCAL) {
			if (pip->ip_src == ip_anyaddr) {
				trace("IPPROC what in the world is THIS ???\n");
				if (pdstrt->rt_ifnum == NI_LOCAL)
					pip->ip_src = pip->ip_dst;
				else
					pip->ip_src = nif[pdstrt->rt_ifnum].ni_ip;
			}
		} else if (--(pip->ip_ttl) == 0 &&
				pdstrt->rt_ifnum != NI_LOCAL) {
			IpInHdrErrors++;
			trace("IPPROC expired TTL\n\n");
			iph2net(pip);
			pep->ep_order &= ~EPO_IP;
			icmp(ICT_TIMEX, ICC_TIMEX, pip->ip_src, pep, 0);
			rtfree(pdstrt);
			continue;
		}

		trace("IPPROC handle directed broadcasts, redirect\n");
		ipdbc(ifnum, pep, pdstrt);		/* handle directed broadcasts	*/
		ipredirect(pep, ifnum, pdstrt);	/* do redirect, if needed	*/

		trace("IPPROC passing pep 0x%x to ipputp (%s hop) ifnum %d\n", 
			pep, pdstrt->rt_metric == 0 ? "direct" : "gw", pdstrt->rt_ifnum);

		if (pdstrt->rt_metric != 0)
			ipputp(pdstrt->rt_ifnum, pdstrt->rt_gw, pep);
		else
			ipputp(pdstrt->rt_ifnum, pip->ip_dst, pep);

		trace("IPPROC freeing route 0x%x\n", pdstrt);
		rtfree(pdstrt);

	} // while (shutxinu == FALSE)

}


