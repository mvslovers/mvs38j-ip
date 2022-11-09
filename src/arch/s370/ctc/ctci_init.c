/* ctci_init.c - ctci_init */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <proc.h>
#include <q.h>
#include <bufpool.h>
#include <ctc.h>				// general CTC headers
#include <ctci.h>				// CTCI headers
#include <xgd.h>
#include <config.h>				// MVS-Xinu configuration

#define	DEBUG
#include <debug.h>

#undef	DEBUG_CHAIN			// defined = run xgd.ctclink chain

/*------------------------------------------------------------------------
 *  ctci_init  -  initialize MVS CTC I/O device and buffers
 *		The MVS-Xinu configuration file will only provide
 *		one device definition for each CTC pair; we "split"
 *		this definition into two CTDEVs each of which is
 *		assigned to a MVS CTC device.
 *		The low (even) cuu is the read side, the next sequential
 *		cuu (odd) the write side.
 *------------------------------------------------------------------------
 */

int ctci_init(struct devsw *devptr) {

	xgd		*pxgd;
	ctblk		*pctblk;
	ctdev		*pctdev;
	ctdev		*prun;
	ctdev		*plast;
	ctdev		*pread;
	char		bufproc[PNMLEN];			// pentry.pname
	int		pid;
	int		poolid;
	int		xdev, xnif;
	dbug(char	bufcall[FUNCSTR_BUFLEN]);


	dbug(funcstr(bufcall, MY_CALLER));
	trace("CTCI_INIT enter, devsw 0x%x <<%s>>\n", devptr, bufcall);

//  The Xinu "minor device number" can vary from 0 to (Neth - 1)

	xdev = devptr->dvminor;
	xnif = xdev + 1;					// account for NI_LOCAL
	trace("CTCI_INIT minor CTC device number %d iface %d\n", 
		xdev, xnif);

	pctblk = &ctcdev[xdev];
	memset(pctblk, 0x00, sizeof(ctblk));

	pctblk->shutdown = FALSE;				// not shutting CTC down
	pctblk->pdevsw = devptr;				// back @ devsw entry

//  If the network interface is disabled, we're done
//  We can't look at netif fields, since the network interfaces haven't been
//  initialized yet; check Config instead

	if ( !Config.nif[xnif].enabled) {
		trace("CTCI_INIT iface %d disabled; short init\n", xnif);
		pctblk->ctcinit = TRUE;				// CTC init complete
		return OK;
	}
	trace("CTCI_INIT iface %d enabled\n", xnif);

//  Chain the CTDEVs off xgd.ctclink, so IGG019X8 can find them

	pxgd = findxgd();
	pread = &pctblk->read;					// @ read ctdev
	pread->next = &pctblk->write;				// point read @ write

	mvskey0();						// heads up: key 0
	if (pxgd->ctclink == NULL) {				// first ctdev being added?
		pxgd->ctclink = pread;				// add first two ctdevs
	} else {
		plast = pxgd->ctclink;				// @ first ctdev (pair exists)
		while (prun = plast->next)			// run ctdev chain to end
			plast = prun;				// @ ctdev
		plast->next = pread;				// add two ctdevs at end of chain
	}
	mvskey8();						// back to safer storage key

#ifdef	DEBUG_CHAIN
	prun = pxgd->ctclink;
	trace("CTCI_INIT CHAIN xgd 0x%x\n", pxgd);
	while (prun) {
		trace("CTCI_INIT CHAIN ctdev 0x%x\n", prun);
		prun = prun->next;
	}
#endif

//  Since we use buffer pools, which can cause a process to block, separate
//  processes are started for read and write for each CTC pair
//  We leave the process' pid in the ctdev so it can find the device
//  which the process is to service

//  First, the read initialization:

	pctdev = (void *)&pctblk->read;				// @ read half of ctc
	strncpy(pctdev->eye, "CTDEV", sizeof(pctdev->eye));	// set ctdev eyecatcher
	pctdev->sem = screate(1);				// create read semaphore
	wait(pctdev->sem);					// serialize startup
	pctdev->userpid = BADPID;				// no active reads
	pctdev->cuu = Config.nif[xnif].ctc.read.cuu;		// read device cuu
	pctdev->config = &Config.nif[xnif].ctc.read;		// @ Config ctchalf
	snprintf(bufproc, sizeof(bufproc),
		"%s %03x", 
		CTC_PROC_READ,
		Config.nif[xnif].ctc.read.cuu);
	pid = create(ctci_ioread, CTC_STK, CTC_PRI, 		// create CTCI read process
		bufproc, 0);
	pctdev->iopid = pid;					// ctcio pid
	trace("CTCI_INIT started pid %d %s\n", 
		pid, proctab[pid].pname);

//  Now, the write initialization
	pctdev = (void *)&pctblk->write;			// @ read write of ctc
	strncpy(pctdev->eye, "CTDEV", sizeof(pctdev->eye));	// set ctdev eyecatcher
	pctdev->sem = screate(1);				// create write semaphore
	wait(pctdev->sem);					// serialize startup
	pctdev->userpid = BADPID;				// no active writes
	pctdev->cuu = Config.nif[xnif].ctc.write.cuu;		// write cuu
	pctdev->config = &Config.nif[xnif].ctc.write;		// @ Config ctchalf
	snprintf(bufproc, sizeof(bufproc),
		"%s %03x", 
		CTC_PROC_WRITE,
		Config.nif[xnif].ctc.write.cuu);
	pid = create(ctci_iowrite, CTC_STK, CTC_PRI, 		// create CTCI write process
		bufproc, 0);
	pctdev->iopid = pid;					// ctcio pid
	trace("CTCI_INIT started pid %d %s\n", 
		pid, proctab[pid].pname);

//  Start the CTC I/O processes

	ctcecb = 0;						// clear CTC I/O process ECB
	pctblk->ctcinit = TRUE;					// CTC init complete
	resume(pctblk->read.iopid);
	resume(pctblk->write.iopid);				// run CTC I/O processes
	trace("CTCI_INIT ctcio* processes resumed\n");

	snap("CTCI_INIT initialized ctblk", (void *)pctblk, sizeof(ctblk));
	trace("CTCI_INIT exit; OK\n");
	return OK;

}



