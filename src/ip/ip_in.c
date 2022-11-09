/* ip_in.c - ip_in */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <proc.h>

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  ip_in - IP input function
 *------------------------------------------------------------------------
 */

int ip_in(struct netif *pni, struct ep *pep) {

	struct	ip	*pip = (struct ip *)pep->ep_data;
	void		*msg;


	trace("\nIP_IN enter; pni 0x%x ep 0x%x pip 0x%x\n", 
		(void *)pni, (void *)pep, (void *)pip);
	dbug(ipdump(pip, 0));

	IpInReceives++;

	if (enq(pni->ni_ipinq, pep, pip->ip_tos & IP_PREC) < 0) {
		IpInDiscards++;
		trace("IP_IN discarded (note new return SYSERR)\n");
		freebuf(pep);
		return(SYSERR);
	}

	msg = pni - &nif[0];
	trace("IP_IN sending ippid %d msg 0x%x (interface index)\n", ippid, msg);
	send(ippid, msg);

	trace("IP_IN exit; OK\n");
	return OK;
}






