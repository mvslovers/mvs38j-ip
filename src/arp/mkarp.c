/* mkarp.c - mkarp */

//  Extracted from rarpsend - jmm

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <proc.h>

#define	DEBUG
#include <debug.h>

#define	DEBUG_BUFFER

/*------------------------------------------------------------------------
 *  mkarp  -  allocate and fill in an ARP or RARP packet
 *------------------------------------------------------------------------
 */

//  spa		Sender's protocol (IP) address
//  tpa		Target's protocol (IP) address

struct ep *mkarp(int ifn, short type, short op, IPaddr spa, IPaddr tpa) {

	register struct	arp	*parp;
	struct	ep		*pep;


	trace("MKARP entered; ifn %d type 0x%x op %d IPaddr spa 0x%x IPaddr tpa 0x%x\n",
		ifn, (int) type, op, spa, tpa);
	pep = (struct ep *) getbuf(Net.netpool);
	if ((int)pep == SYSERR) {
		trace("MKARP syserr; getbuf\n");
		return (struct ep *)SYSERR;
	}
	memcpy(pep->ep_dst, nif[ifn].ni_hwb.ha_addr, EP_ALEN);
	pep->ep_order = ~0;
	pep->ep_type = type;
	parp = (struct arp *)pep->ep_data;
	parp->ar_hwtype = hs2net(AR_HARDWARE);
	parp->ar_prtype = hs2net(EPT_IP);
	parp->ar_hwlen = EP_ALEN;
	parp->ar_prlen = IP_ALEN;
	parp->ar_op = hs2net(op);
	memcpy(SHA(parp), nif[ifn].ni_hwa.ha_addr, EP_ALEN);
	memcpy(SPA(parp), &spa, IP_ALEN);
	memcpy(THA(parp), nif[ifn].ni_hwa.ha_addr, EP_ALEN);
	memcpy(TPA(parp), &tpa, IP_ALEN);

#ifdef	DEBUG_BUFFER
	snap("MKARP ARP/RARP prototype buffer (struct ep, struct arp)", pep, 
		EP_HLEN + sizeof(struct arp) + (parp->ar_hwlen * 2) + (parp->ar_prlen * 2));
#endif

	trace("MKARP exit; pep 0x%x\n", pep);
	return pep;
}


