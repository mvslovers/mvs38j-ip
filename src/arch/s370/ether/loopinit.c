/* loopinit.c - loopinit */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <config.h>

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 * loopinit	initialize loopback adapter
 *		Called by inithost, initgate
 *		Both callers have already done some initialization for us
 *------------------------------------------------------------------------
 */

int loopinit(int iface) {

	struct netif	*pnif;
	char		bufip[IP_MAXDOT];


	pnif = &nif[iface];
	trace("LOOPINIT init LOOPBACK iface %d pnif 0x%08x\n", iface, pnif);

	if (iface != NI_LOCAL)
		panic("LOOPINIT iface %d not NI_LOCAL\n", iface);

	if (Config.nif[iface].type == NIFT_LOOP) {
		pnif->ni_ip = Config.nif[iface].ip;
		pnif->ni_ivalid = TRUE;
	} else
		panic("LOOPINIT config network interface %d not loopback\n", iface);

	/* maxbuf - ether header - CRC - nexthop */
	pnif->ni_mtu = MAXLRGBUF - EP_HLEN - EP_CRC - IP_ALEN;

	pnif->ni_ipinq = newq(NI_INQSZ, QF_NOWAIT);
	pnif->ni_hwtype = AR_HARDWARE;		/* for ARP */
	pnif->ni_descr = (void *)&loopback_text;

	if (Config.nif[iface].enabled)
		pnif->ni_state = NIS_UP;

	nifdump("LOOPINIT", pnif, NODUMP_NIF);

	trace("LOOPINIT exit; OK\n");
	return OK;
}

