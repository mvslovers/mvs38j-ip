/* initgate.c - initgate */

#include <conf.h>
#include <kernel.h>
#include <bufpool.h>
#include <network.h>

/* static IP addresses, if used */

static	IPaddr	iftoip[] = { 0, 0, 0x800A0B00, 0x800A0C00 };

#ifdef	OTHER1
	static	int	itod[] = { NI_LOCAL, ETHER, OTHER1, OTHER2 };
	void ofaceinit(unsigned);
#else
	static	int	itod[] = { -1, ETHER };
#endif

/*------------------------------------------------------------------------
 * ofaceinit	initialize an Othernet interface
 *------------------------------------------------------------------------
 */

#if	Noth > 0
void ofaceinit(unsigned ifn) {

	struct otblk	*otptr;
	struct etblk	*etptr;
	int		i;

	nif[ifn].ni_ip = iftoip[ifn];

	otptr = (struct otblk *)devtab[nif[ifn].ni_dev].dvioblk;
	otptr->ot_intf = ifn;

	nif[ifn].ni_descr = otptr->ot_descr;
	nif[ifn].ni_mtype = 1;	/* RFC 1156, "other" :-)	*/
	nif[ifn].ni_speed = 400000000; /* ~25Mz 32 bit mem copy :-)	*/
	nif[ifn].ni_maxreasm = MAXLRGBUF;

	/* fill in physical net addresses */
	memcpy(nif[ifn].ni_hwa.ha_addr, otptr->ot_paddr, EP_ALEN);
	memcpy(nif[ifn].ni_hwb.ha_addr, otptr->ot_baddr, EP_ALEN);
	nif[ifn].ni_hwa.ha_len = nif[ifn].ni_hwb.ha_len = EP_ALEN;

	/* set the mtu */

	if (nif[ifn].ni_dev == OTHER2)
		nif[ifn].ni_mtu = SMALLMTU;
	else
		nif[ifn].ni_mtu = EP_DLEN;

	etptr = (struct etblk *)devtab[otptr->ot_pdev].dvioblk;
	nif[ifn].ni_outq = etptr->etoutq;

	/* net num comes from ether addr */

FIX ME FIX ME
	nif[ifn].ni_ip[IP_ALEN-1] = etptr->etpaddr[EP_ALEN-1];
	nif[ifn].ni_net = netnum(nif[ifn].ni_ip);
	nif[ifn].ni_ivalid = TRUE;

	/* set the mask (same for both) */

	setmask(ifn, 0xffffff00);

	/* host */
	rtadd(nif[ifn].ni_ip, ip_maskall, nif[ifn].ni_ip, 0,
		NI_LOCAL, RT_INF);
	/* broadcast (all 0's, no subnet) */
	rtadd(nif[ifn].ni_net, ip_maskall, nif[ifn].ni_ip, 0,
		NI_LOCAL, RT_INF);
}
#endif	/* Noth > 0 */


