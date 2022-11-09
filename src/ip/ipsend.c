/* ipsend.c - ipsend */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <proc.h>

#define	DEBUG
#include <debug.h>

#ifdef	MVS38J
	#define	SAVE_THIS_CONFUSING_CODE
#endif

static ipackid = 1;

/*------------------------------------------------------------------------
 *  ipsend  -  send an IP datagram to the specified address
 *------------------------------------------------------------------------
 */

int ipsend(IPaddr faddr, struct ep *pep, unsigned datalen, u_char proto, u_char tos, u_char ttl) {

	char		buffaddr[IP_MAXDOT];
	struct	ip	*pip;


	ip2dot(buffaddr, faddr);
	trace("IPSEND enter; faddr %s pep 0x%x datalen %d PROTO 0x%x TOS 0x%x TTL 0x%x\n",
		buffaddr, pep, datalen, proto, tos, ttl);

	pep->ep_type = EPT_IP;
	pep->ep_order |= EPO_IP|EPO_NET;

	pip = (struct ip *) pep->ep_data;
	pip->ip_verlen = (IP_VERSION<<4) | IP_MINHLEN;
	pip->ip_tos = tos;
	pip->ip_len = datalen+IP_HLEN(pip);
	pip->ip_id = ipackid++;
	pip->ip_fragoff = 0;
	pip->ip_ttl = ttl;
	pip->ip_proto = proto;
	pip->ip_dst = faddr;

	/*
	 * special case for ICMP, so source matches destination
	 * on multi-homed hosts.
	 */
	if (pip->ip_proto != IPT_ICMP)
		pip->ip_src = ip_anyaddr;

#ifndef	SAVE_THIS_CONFUSING_CODE

//  OK, I'm confused.  Perhaps this has something to do with wrapping the
//  IP frame back through the loopback adapter, but I don't get it.

	trace("IPSEND PC-Xinu queueing IP frame\n");
	if (enq(nif[NI_LOCAL].ni_ipinq, pep, 0) < 0) {
		freebuf(pep);
		IpOutDiscards++;
	}
	send(ippid, NI_LOCAL);		// curiously, currpid == ippid
#else

//  MVS-Xinu doesn't queue up outbound IP frames, it just sends them.
//  The device to which the data is written, the MVS-Xinu CTC device
//  (which acts as the NI_PRIMARY network interface), is given what I'm
//  guessing is the correct length on the write.  But it's just a
//  guess.  Not such a shabby guess, since in normal Xinu (not PC-Xinu)
//  the device driver write function is passed an ethernet packet.
//  PC-Xinu adds a struct ep in front of the ethernet frame, so it's a 
//  little hard (more trouble than it's worth) to decide what was intended.  
//  Besides, at least for the MVS-Xinu CTCI CTC device, the caller's
//  length is ignored.

	trace("IPSEND MVS-Xinu sending IP frame\n");
	write(ETHER, pep, pep->ep_len);
	trace("IPSEND MVS-Xinu IP frame sent\n");

#endif
	IpOutRequests++;

	trace("IPSEND exit; OK\n");
	return OK;
}





