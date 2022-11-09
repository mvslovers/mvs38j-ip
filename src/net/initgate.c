/* initgate.c - initgate */

//  TODO -- LOTS
//  TODO Compare inithost, call loopinit rather than doing it here
//  TODO remove buffer & semaphore creation; now in netstart
//  TODO maybe just throw this code away; its probably obsoleted by netup_nif

#include <conf.h>
#include <kernel.h>
#include <bufpool.h>
#include <network.h>

/* static IP addresses, if used */

static	IPaddr	iftoip[] = { 0, 0, 0x800A0B00, 0x800A0C00 };

#ifdef	OTHER1
	static	int	itod[] = { NI_LOCAL, ETHER, OTHER1, OTHER2 };
#else
	static	int	itod[] = { -1, ETHER };
#endif


/*------------------------------------------------------------------------
 * initgate  --  initialize a gateway's interface structures
 *------------------------------------------------------------------------
 */

int initgate(void) {

	int	i;

	Net.netpool = mkpool(MAXNETBUF, NETBUFS);
	Net.lrgpool = mkpool(MAXLRGBUF, LRGBUFS);
	Net.sema = screate(1);
	Net.nif = NIF;

#ifdef	IPADDR1
	iftoip[1] = dot2ip(IPADDR1);
#endif

	for (i=0; i<Net.nif; ++i) {
		/* start off with all 0's (esp. statistics) */
		memset(&nif[i], 0, sizeof(nif[i]));

		nif[i].ni_state = NIS_DOWN;
		nif[i].ni_admstate = NIS_UP;
		nif[i].ni_ivalid = nif[i].ni_nvalid = FALSE;
		nif[i].ni_svalid = FALSE;
		nif[i].ni_dev = itod[i];
		nif[i].ni_mcast = 0;
		if (i == NI_LOCAL) {
			/* maxbuf - ether header - CRC - nexthop */
			nif[i].ni_mtu = MAXLRGBUF-EP_HLEN-EP_CRC-IP_ALEN;
			nif[i].ni_ip = ip_anyaddr;
			continue;
		}

		switch(nif[i].ni_dev) {

#if	Noth > 0
		case OTHER1:
		case OTHER2:
			ofaceinit(i);
			break;
#endif	/* Noth > 0 */

		case ETHER:
			efaceinit(i);
			break;
		};
	}

	return OK;
}



