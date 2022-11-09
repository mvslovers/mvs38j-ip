/* rarpsend.c - rarpsend */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <proc.h>

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  rarpsend  -  broadcast a RARP packet to obtain my IP address
 *------------------------------------------------------------------------
 */

int	rarpsend(int ifn) {

	STATWORD		ps;
	struct	netif		*pni = &nif[ifn];
	struct	ep		*pep;
	int			i, mypid, resp;
	IPaddr			junk = 0; /* arg to mkarp; never used */


	trace("RARPSEND enter; ifn %d\n", ifn);
	mypid = getpid();
	for (i=0; i<ARP_MAXRETRY; ++i) {
		trace("RARP constructing RARP request; type EPT_RARP op RA_REQUEST\n");
		pep = mkarp(ifn, EPT_RARP, RA_REQUEST, junk, junk);
		if ((int)pep == SYSERR) {
			trace("RARPSEND syserr; mkarp\n");
			break;
		}
		disable(ps);
		rarppid = mypid;
		restore(ps);
		recvclr();
		trace("RARPSEND pid %d writing ifn %d pep 0x%x len %d\n", 
			mypid, ifn, pep, EP_MINLEN);
		write(pni->ni_dev, pep, EP_MINLEN);
		/* ARP_RESEND is in secs, recvtim uses 1/10's of secs	*/
		trace("RARPSEND waiting for RARP response\n");
		resp = recvtim(10*ARP_RESEND<<i);
		if (resp != TIMEOUT) {
			trace("RARPSEND ifn %d RARP response received before timeout\n", ifn);
			/* host route */
			rtadd(pni->ni_ip, ip_maskall, pni->ni_ip, 0,
				NI_LOCAL, RT_INF);

			/* non-subnetted route */
			rtadd(pni->ni_net, ip_maskall, pni->ni_ip, 0,
				NI_LOCAL, RT_INF);

			return OK;
		} else {
			trace("RARPSEND ifn %d RARP response not received before timeout\n", ifn);
		}
	}
	panic("RARPSEND panic; no response to RARP");
	return SYSERR;
}

