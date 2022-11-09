/* netwrite.c - netwrite */

#include <conf.h>
#include <kernel.h>
#include <network.h>

#define	DEBUG
#include <debug.h>

#ifdef	OSPF
	#define	DEBUG_PDUMP_OSPF
#endif

/*------------------------------------------------------------------------
 * netwrite - write a packet on an interface, using ARP if needed
 *------------------------------------------------------------------------
 */

int netwrite(struct netif *pni, struct ep *pep, unsigned len) {

	struct	arpentry 	*pae;
	STATWORD		ps;
	struct ip 		*pip = (struct ip *)pep->ep_data;
	int			rv;


	trace("NETWRITE enter; pni 0x%x %s pep 0x%x len %d\n", 
		pni, pni == &nif[NI_LOCAL] ? "LOCAL" : "PRIMARY", pep, len);

	if (pni->ni_state != NIS_UP) {
		trace("NETWRITE syserr; interface not up, discarded\n");
		freebuf(pep);
		return SYSERR;
	}
	pep->ep_len = len;

#ifdef	DEBUG_PDUMP_OSPF
	if (pni != &nif[NI_LOCAL]) {
		if (pip->ip_proto == IPT_OSPF) {
			struct ospf *po = (struct ospf *)pip->ip_data;
/*			if (po->ospf_type != T_HELLO) { */
			{
				kprintf("netwrite(pep %X, len %d)\n", pep, len);
				pdump(pep);
			}
		}
	}
#endif

//  Give the packet to a local process, handle broadcast, or continue on

	if (pni == &nif[NI_LOCAL]) {
		trace("NETWRITE passing local_out packet 0x%x\n", pep);
		return local_out(pep);
	} else if (isbrc(pep->ep_nexthop)) {
		memcpy(pep->ep_dst, pni->ni_hwb.ha_addr, EP_ALEN);
		trace("NETWRITE write broadcast\n");
		write(pni->ni_dev, pep, len);
		return OK;
	}

	/* else, look up the protocol address ... */

//  Write packet to device or ????

	trace("NETWRITE looking up protocol address\n");
	disable(ps);
	pae = arpfind((u_char *)&pep->ep_nexthop, pep->ep_type, pni);
	if (pae && pae->ae_state == AS_RESOLVED) {
		memcpy(pep->ep_dst, pae->ae_hwa, pae->ae_hwlen);
		restore(ps);
		trace("NETWRITE writing pni 0x%x (device %d) pep 0x%x len %d\n", 
			pni, pni->ni_dev, pep, len);
		rv = write(pni->ni_dev, pep, len);
		return rv;
	}
	if (IP_CLASSD(pep->ep_nexthop)) {
		restore(ps);
		trace("NETWRITE syserr; class D\n");
		return SYSERR;
	}
	if (pae == 0) {
		pae = arpalloc();
		pae->ae_hwtype = AR_HARDWARE;
		pae->ae_prtype = EPT_IP;
		pae->ae_hwlen = EP_ALEN;
		pae->ae_prlen = IP_ALEN;
		pae->ae_pni = pni;
		pae->ae_queue = EMPTY;
		memcpy(pae->ae_pra, &pep->ep_nexthop, pae->ae_prlen);
		pae->ae_attempts = 0;
		pae->ae_ttl = ARP_RESEND;
		arpsend(pae);
	}
	if (pae->ae_queue == EMPTY)
		pae->ae_queue = newq(ARP_QSIZE, QF_NOWAIT);
	if (enq(pae->ae_queue, pep, 0) < 0)
		freebuf(pep);
	restore(ps);
	return OK;
}



