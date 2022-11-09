/* efaceinit.c - efaceinit */

#include <conf.h>
#include <kernel.h>
#include <bufpool.h>
#include <network.h>
#include <config.h>

#define	DEBUG
#include <debug.h>

#define	RARP_HACK		// undefined: get IPaddr via RARP
				// defined: bypass RARP

static	unsigned char	ctcihwa[EP_ALEN] = {		// Bogus CTCI ethernet addr
		0x12, 0x34, 0x45, 0x67, 0x89, 0xab };

static	unsigned char	ctcihwb[EP_ALEN] = {		// Ethernet broadcast addr
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

/*------------------------------------------------------------------------
 * efaceinit	initialize Ethernet interfaces NI_PRIMARY
 *		called from netup_nif
 *	By the time we're called, ctci_init has run (assuming the
 *	device class definition in looks like this (note -i ctci_init):
 *
 * eth:
 *	on CTC	-i ctci_init	-o noopen	-c noclose
 *		-r ctci_read	-w ctci_write	-s noseek
 *		-n nocntl	-g nogetc	-p noputc
 *		-iint noiint	-oint nooint
 *------------------------------------------------------------------------
 */

void efaceinit(unsigned ifn) {

	netif		*pnif;
	struct devsw	*pdevsw;
	struct utdev	*pud;
	int		dvnum;		// Xinu device #
	int		xdev;		// Xinu minor device #
	IPaddr		junk;
	IPaddr		mask;
	void		*ptemp;


	pnif = &nif[ifn];
	dvnum = pnif->ni_dev;
	pdevsw = &devtab[dvnum];
	xdev = pdevsw->dvminor;
	trace("\nEFACEINIT ******** enter; iface %d, device %d minor %d\n", ifn, dvnum, xdev);
//	snap("EFACEINIT devtab entry", (void *)pdevsw, sizeof(struct devsw));

	//  I'm not really sure what a utdev represents, and at least for
	//  what's currently in the source tree it doesn't seem to be
	//  all that important.  Nevertheless, we take a shot at setting
	//  up the control block.  Might have some impact on RARP, which
	//  is not supported with our current CTCI devices; if we switch
	//  to using LCS (possibly), it might have some bearing.

	pud = (struct utdev *)pdevsw->dvioblk;
	if (pud == NULL) {
		trace("EFACEINIT initializing utdev\n");
		initutdev(pdevsw);
		pud = (struct utdev *)pdevsw->dvioblk;
	}

	//  See initutdev, but I don't think these are correct
	//  It's not important for our current CTCI support
	memcpy(pnif->ni_hwa.ha_addr, pud->ud_paddr, EP_ALEN);
	memcpy(pnif->ni_hwb.ha_addr, pud->ud_bcast, EP_ALEN);

	pnif->ni_descr = pud->ud_descr;
	pnif->ni_outq = pud->ud_outq;
	pud->ud_ifnum = ifn;

	nif[ifn].ni_mtype = 6;			/* RFC 1156, Ethernet CSMA/CD	*/
	nif[ifn].ni_speed = 10000000;		/* bits per second		*/
	nif[ifn].ni_maxreasm = MAXLRGBUF;
	nif[ifn].ni_hwa.ha_len = EP_ALEN;
	nif[ifn].ni_hwb.ha_len = EP_ALEN;
	nif[ifn].ni_mtu = EP_DLEN;
	nif[ifn].ni_mcast = ethmcast;

	nif[ifn].ni_ipinq = newq(NI_INQSZ, QF_NOWAIT);
	nif[ifn].ni_hwtype = AR_HARDWARE;		/* for ARP */

#ifdef	RARP_HACK

	/* TEMPORARY HACK to avoid RARP +-DLS */
	// MVS-Xinu also wants it this way for our CTCI devices
	// which don't support ethernet frames (only IP frames)
	// hence no ARP/RARP is possible with CTCI

	trace("\nEFACEINIT begin RARP hack\n\n");

	pnif->ni_ip = Config.nif[ifn].ip;	// get interface IP from Config
	pnif->ni_ivalid = TRUE;			// interface IP addr valid

	pnif->ni_net = netnum(pnif->ni_ip);	// get subnet mask
	mask = netmask(pnif->ni_ip);		// get default mask our net
	setmask(ifn, mask);			// set ni_mask, routes

// MVS-Xinu: the non-gateway routes are set up the way we want them in setmask.  
// We don't install a default route for our point-to-point CTCI link,
// that's done in netup_rt
// FIXME Gateway support is a secondary item for MVS-Xinu, so this may
// FIXME bear some later examination which is why this code is still here
// FIXME Version 2: We need to create a NI_LOCAL route for our host IP,
// FIXME Version 2: or inbound IP frames get sent directly back out...
// FIXME Version 2: or something.  Still fooling around with it....

	trace("\nEFACEINIT ******** NI_LOCAL host route ********\n\n");

	/* host route, NI_LOCAL interface ???? */
	trace("EFACEINIT adding NI_LOCAL host route\n");
	rtadd(nif[ifn].ni_ip, ip_maskall, nif[ifn].ni_ip, 0,
		NI_LOCAL, RT_INF);
	trace("\n\n");

#if 0	// This gets overlaid later by setmask, so we don't bother creating it
	/* non subnetted route */
	trace("EFACEINIT adding NI_LOCAL subnetted broadcast route\n");
	rtadd(nif[ifn].ni_net, ip_maskall, nif[ifn].ni_ip, 0,
		NI_LOCAL, RT_INF);
#endif

	dbug(rtdump(ALL_ROUTES));	// see how things look

	trace("EFACEINIT end RARP hack\n");
	trace("EFACEINIT CTCI hack NI_PRIMARY **\n");

	// Overwrites Eaddr's from utdev
	ptemp = (void *)ctcihwa;
	pnif->ni_hwa.ha_len = EP_ALEN;
	memcpy(pnif->ni_hwa.ha_addr, ptemp, EP_ALEN);

	ptemp = (void *)ctcihwb;
	pnif->ni_hwb.ha_len = EP_ALEN;
	memcpy(pnif->ni_hwb.ha_addr, ptemp, EP_ALEN);

	pnif->ni_state = NIS_UP;
	pnif->ni_admstate = NIS_UP;
	trace("EFACEINIT CTCI hack iface %d UP **\n", ifn);
#else
	//  Current MVS-Xinu status:  Neither RARP nor ARP can work with
	//  CTCI adapters (IP frames only); we might want to switch to LCS
	//  adapter which can handle ethernet frames

	panic("EFACEINIT no RARP hack; CTCI requires");
#endif	// RARP_HACK

	nifdump("EFACEINIT exit", &nif[ifn], NODUMP_NIF);	// display struct netif
	trace("\nEFACEINIT ********** exit; iface %d\n", ifn);
	return;
}





