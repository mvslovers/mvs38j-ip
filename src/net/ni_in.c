/* ni_in.c - ni_in */

#include <conf.h>
#include <kernel.h>
#include <network.h>

#include <ospf.h>

#define	DEBUG
#include <debug.h>

#define	DEBUG_PROCSTATE

//	int arp_in(struct netif *, struct ep *);
//	int rarp_in(struct netif *, struct ep *);
//	int ip_in(struct netif *, struct ep *);

/*------------------------------------------------------------------------
 *  ni_in - network interface input function
 *------------------------------------------------------------------------
 */

int ni_in(struct netif *pni, struct ep *pep, unsigned len) {

	int	rv;


	trace("\n\nNI_IN ***** INCOMING PACKET ***** iface 0x%x packet 0x%x len %d 0x%x\n",
		(void *)pni, (void *)pep, len, len);

#ifdef	DEBUG_PROCSTATE
	procstate(NPROC, "ni_in");
#endif

	pep->ep_ifn = pni - &nif[0];	/* record originating intf # */

	pni->ni_ioctets += len;
	if (!memcmp(pni->ni_hwa.ha_addr, pep->ep_dst, EP_ALEN))
		pni->ni_iucast++;
	else
		pni->ni_inucast++;

	trace("NI_IN interface %d protocol %d\n", pep->ep_ifn, pep->ep_type);
	switch (pep->ep_type) {
		case EPT_ARP:	rv = arp_in(pni, pep);	break;
		case EPT_RARP:	rv = rarp_in(pni, pep);	break;
		case EPT_IP:	rv = ip_in(pni, pep);	break;
		default:
			pni->ni_iunkproto++;
			freebuf(pep);
			rv = OK;
	}
	trace("NI_IN exit; return %d\n", rv);
	return rv;
}




