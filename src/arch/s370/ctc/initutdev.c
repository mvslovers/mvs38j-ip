/* ue_init.c - ue_init */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <ue.h>
#include <q.h>

#ifndef	MVS38J
	#include <i386.h>
#endif

#undef	DEBUG
#include <debug.h>

static int	ramsize[4] = { 8192, 16384, 32768, 65536 };

static	currpage;	/* current register page	*/

/*------------------------------------------------------------------------
 * initutdev	
 *		Code borrowed from ue_init (startup initialization of 
 *		SMC Ultra Ethernet device)
 *------------------------------------------------------------------------
 */

//  initdev is an attempt by MVS-Xinu to fake out portions of PC-Xinu
//  which depend upon struct utdev (anchored in devsw.devioblk)
//  Only enough of utdev is filled in to make things work
//  The definition of ue[] was moved to gblether.c

//  This nonsense is mostly only needed because I didn't want to
//  learn flex/lex, yacc, and whatever other tools the Xinu config
//  process uses.  So instead of a clean CTC device, we have a CTC
//  device that tries to wedge itself into an ethernet environment.
//  It's probably for the best, since PC-Xinu seems to be heavily
//  dependent upon ethernet.  Hopefully it'll work out OK.

int initutdev(struct devsw *pdev) {

	int mbase;
	struct utdev	*pud;
	unsigned char	tmp, rcon;
	unsigned short	*p;
	unsigned int	fn, i, ue_nrx, ue_int();


	trace("INITUTDEV enter; pdev 0x%x\n", pdev);
	pud = &ue[pdev->dvminor];
	pdev->dvioblk = (char *) pud;
	pud->ud_pdev = pdev;
	pud->ud_outq = newq(ETOUTQSZ, QF_NOWAIT);
	pud->ud_ifnum = -1;
	pud->ud_descr = "Hercules CTCI";
	pud->ud_xpending = 0;

#ifndef	MVS38J
	for (i=0; i<6 ; ++i) {
		pud->ud_paddr[i] = inb(pdev->dvcsr + EADDR + i);
		pud->ud_bcast[i] = ~0;
	}

	tmp = inb(pdev->dvcsr + HWR);
	outb(pdev->dvcsr + HWR, tmp | HWR_SWH);
	pud->ud_iomem = inb(pdev->dvcsr + RAR);
	pud->ud_iosize = ramsize[(pud->ud_iomem >> 4) & 3];
	pud->ud_iomem = ((pud->ud_iomem & 0x40) << 17) |
		(pud->ud_iomem & 0x0f)<<13 | 0xC0000;
	uewr(pdev->dvcsr, GCR, uerd(pdev->dvcsr, GCR) | GCR_LIT);
#endif

	kprintf("INITUTDEV ue%d: %02x:%02x:%02x:%02x:%02x:%02x, bid %x\n",
		pdev->dvminor,
		pud->ud_paddr[0] & 0xff,
		pud->ud_paddr[1] & 0xff,
		pud->ud_paddr[2] & 0xff,
		pud->ud_paddr[3] & 0xff,
		pud->ud_paddr[4] & 0xff,
		pud->ud_paddr[5] & 0xff,
#ifndef	MVS38J
		inb(pdev->dvcsr + BOARDID)
#else
		0
#endif
		);
//	snap("INITUTDEV ue", (void *)pud, sizeof(struct utdev));

#ifndef	MVS38J
	kprintf("          irq %d iomem %X (%d K)\n", 
		pdev->dvivec - IRQBASE,
		pud->ud_iomem, pud->ud_iosize/1024);

	/* disable ROM address mapping */
	uewr(pdev->dvcsr, BIO, 0x30);

	/* enable host access to interface memory */
	outb(pdev->dvcsr + CR, CR_MENB);


	currpage = 0;
	UEPAGE(pdev->dvcsr, currpage);	/* sync currpage w/ chip */
	for (i=0; i<EP_ALEN; ++i)
		uewr(pdev->dvcsr, STA0+i, pud->ud_paddr[i]);

	uewr(pdev->dvcsr, W_DCON, DCON_BUS16);

	/* clear interface memory */

	p = (unsigned short *)pud->ud_iomem;
	for (i=0; i<pud->ud_iosize / 2; ++i, ++p) {
		*p = 0;
		if (*p)
			kprintf("%X not zero but %x\n", p, *p);
	}
	/* set up receive ring */
	p=(unsigned short *)pud->ud_iomem;
	pud->ud_rmin = (int)p;
	ue_nrx = (pud->ud_iosize / 256) - 6;	/* io size - 1 xmit pack */
	pud->ud_xmaddr = ue_nrx;
	pud->ud_rmax = (int)p + (ue_nrx << 8);
	for (fn=1; fn < ue_nrx; ++fn) {
		*p = fn << 8;
		p += 128;	/* 128 shorts, 256 bytes */
	}

	mbase = 0;	/* start of receive buffers */
	uewr(pdev->dvcsr, W_RSTART, mbase);	/* RSTART = 0 */
	uewr(pdev->dvcsr, W_RSTOP, mbase + ue_nrx); /* RSTOP */

	uewr(pdev->dvcsr, BOUND, mbase + ue_nrx - 1);	/* BOUND = RSTOP-1 */
	uewr(pdev->dvcsr, CURR, mbase);
	pud->ud_nextbuf = mbase;

	uewr(pdev->dvcsr, NEXT, mbase);
	uewr(pdev->dvcsr, RADDH, mbase);
	uewr(pdev->dvcsr, RADDL, mbase);

	uewr(pdev->dvcsr, W_INTMASK, 0x7f);

	set_evec(pdev->dvivec, ue_int);

	rcon = RCON_BROAD | RCON_RCA;
	#ifdef	MULTICAST
	rcon |= RCON_GROUP;
	#endif	/* MULTICAST */
	#ifdef	NETMON
	/*	rcon |= RCON_PROM; */
	#endif	/* NETMON */
	uewr(pdev->dvcsr, W_RCON, rcon);

	/* enable the device */

	uewr(pdev->dvcsr, ICR, ICR_EIL);
	tmp = uerd(pdev->dvcsr, CMD);
	tmp |= CMD_STA;
	tmp &= ~CMD_STP;
	uewr(pdev->dvcsr, CMD, tmp);
#endif

//	snap("INITUTDEV utdev at exit", (void *)pud, sizeof(struct utdev));
	trace("INITUTDEV exit\n");
	return OK;
}




