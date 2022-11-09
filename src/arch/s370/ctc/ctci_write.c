/* ctci_write.c - ctci_write */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>
#include <ip.h>
#include <q.h>
#include <ctc.h>
#include <ctci.h>
#include <string.h>			// Systems/C S/370 quick string functions
#include <ctcwrite.h>			// data shared with ctciowrite

#define DEBUG
#include <debug.h>

#undef	DEBUG_DUMPBUF			// dump caller's buffer on entry
#define	DEBUG_CLEARBUF			// clear fresh buffers
#undef	DEBUG_STACK			// show savearea chain info
#undef	DEBUG_SNAPSEG			// show segment at exit

/*------------------------------------------------------------------------
 *  ctwfresh	acquire and initialize a fresh CTCI write buffer
 *		or return NULL
 *------------------------------------------------------------------------
 */

//  The caller must control the CTC write device when we're called;
//  the CTC write device will be controlled when we return
//  If we need a fresh buffer, we release the write device in case ctciowrite
//  needs to drain a buffer and freebuf it so we can have one

//  Upon successful return, the buffer is initialized to contains both CTCIHDRs
//  and the device will again be owned by the caller's process; fillbuf updated.
//  Upon failure, the device is free and NULL is returned

void *ctwfresh(ctwritestat *pstat, ctdev *pcd) {

	ctcihdr	*pcih;				// @ buffer = 1st ctcihdr
	ctcihdr	*plast;
	ctioblk	*pctioblk = pcd->ctioblk;


	signal(pcd->sem);			// release device (we might block)

	pcih = (void *)getbuf(pctioblk->poolid);	// acquire fresh buffer
	if (pcih == (void *) SYSERR) {
		trace("CTWFRESH syserr; getbuf\n");
		return NULL;
	}

#ifdef	DEBUG_CLEARBUF
	memset((void *)pcih, 0x00, CTC_BUFLEN);	// clear buf (which coincidentally
						// also builds the final ctcihdr)
#else
	plast = (void *) ((BYTE *)pcih + sizeof(ctcihdr));
	plast->offset = CTCI_BH_FINAL;		// final block header
#endif

	pcih->offset = sizeof(ctcihdr);		// 1st ctcihdr = offset to last ctcihdr

	wait(pcd->sem);				// acquire device

	pstat->fillbuf = pcih;			// @ 1st CTCIHDR in buffer ctci_write is filling

	trace("CTWFRESH exit; buffer 0x%x\n", pcih);
	return pcih;				// @ init'd buffer
}

/*------------------------------------------------------------------------
 *  ctci_write	queue a single IP frame for writing to MVS-Xinu CTC device
 *------------------------------------------------------------------------
 */

//	devptr	ptr to struct devsw, MVS-Xinu device definition
//	p2	ptr to struct ep:
//		ethernet header, ethernet frame, IP frame
//	lenparm	the length of "something"
//		It was more trouble to find out what "something" meant
//		than to just use the length from the IP frame, so that
//		is how this code does it; hence, len parm ignored

int ctci_write(struct devsw *devptr, char *p2, unsigned lenparm) {

	ctwritestat	*pstat;				// shared with ctciowrite
	struct ep	*pep = (void *)p2;
	struct ip	*pip;
	ctblk		*pct;
	ctdev		*pcd;
	ctcihdr		*pcih;
	ctcihdr		*pfinal;
	struct ctcseghdr *pseg;
	void		*pbuf;
	int		i, free, iplen;


#if CTC_WRITEBUFS < 3				// minumum acceptable # buffers
	#error CTCI_write config error; CTC_WRITEBUFS inadequate
#endif

	trace("CTCI_write enter; devptr 0x%x, buffer 0x%x, len %d\n",
		devptr, p2, lenparm);

#ifdef	DEBUG_STACK
	stackstate(NULL, "CTCI_write ENTRY");
#endif

	pip = (void *)pep->ep_data;		// @ IP frame
	iplen = pip->ip_len;			// IP frame length in bytes

#ifdef	DEBUG_DUMPBUF
	pdump(pep);
	snap("CTCI_write IP frame", (void *)pip, iplen);
#endif

//  Assure the CTC pair device is initialized

	pct = &ctcdev[devptr->dvminor];		// @ CTC ctblk
	if (pct->ctcinit != TRUE) {
		panic("CTCI_write CTC not initialized\n");
	}
	pcd = &pct->write;			// @ CTC write device

	wait(pcd->sem);				// get control of CTC write device

//  Either locate a buffer we already began to fill on a prior call, 
//  or acquire a fresh buffer into which to place the caller's data

	pstat = (void *)pcd->ioword;		// find ctciowrite ctwritestat
	trace("CTCI_write ioword 0x%x = CTWRITESTAT\n", (void *)pstat);

	pcih = (void *)pstat->fillbuf;
	if (pcih == NULL) {
		pcih = ctwfresh(pstat, pcd);	// acquire & init new buffer
		if (pcih == NULL) {
			signal(pcd->sem);	// release CTCI write device
			return SYSERR;		// ctwfresh issued syserr msg
		}
	}

//  If there's insufficient room in the current buffer for the caller's data
//  indicate current buffer full and acquire another
//  Calculate the address into which the new segment header is to be placed

	free = CTC_BUFLEN;			// buffer size
	free -= pcih->offset;			// bytes remaining in buffer
	free -= sizeof(ctcseghdr);		// need room for segment hdr
	free -= sizeof(ctcihdr);		// leave room for last ctcihdr
	if (free < iplen) {
		for (i = 0; i < CTC_WRITEBUFS; i++) {
			if (pstat->filled[i] == NULL) {		// queue buffer FIFO
				pstat->filled[i] = pcih;	// available to be written
				break;
			}
		}
		pcih = ctwfresh(pstat, pcd);		// acquire & init fresh buffer
		if (pcih == NULL) {
			signal(pcd->sem);		// release CTCI write device
			return SYSERR;
		}
		free = CTC_BUFLEN - (2 * sizeof(ctcihdr)) - sizeof(ctcseghdr);
		if (free < iplen) {
			trace("CTCWRITE syserr; new buff size %d too small %d\n",
				CTC_BUFLEN, iplen);
			signal(pcd->sem);		// release CTCI write device
			return SYSERR;
		}
		pseg = (void *)((BYTE *)pcih + sizeof(ctcihdr));
	} else {
		pseg = (void *)((BYTE *)pcih + pcih->offset);
	}
	trace("CTCI_WRITE pseg 0x%x\n");

//  We now have a buffer which will hold the caller's data & overhead
//  Create a CTCI segment header (overlaying the old final ctcihdr), copy the 
//  caller's data, and create the final CTCIHDR

	pseg->seglen = iplen + sizeof(ctcseghdr);
	pseg->frametype = CTCI_SH_FRAMTYP;		// IPv4 frame type
	pseg->reserved = CTCI_SH_RESV;			// reserved field

	pbuf = (void *) ((BYTE *)pseg + sizeof(ctcseghdr));
	memcpy(pbuf, pip, iplen);

	pfinal = (void *) ((BYTE *)pbuf + iplen);		// @ final ctcihdr
	pcih->offset = 
		(UHWORD) ((BYTE *)pfinal - (BYTE *)pcih);	// offset to final ctcihdr

#ifdef	DEBUG_SNAPSEG
	snap("CTCI_WRITE segment", pseg, pseg->seglen);
	ipshowhdr(pip, NULL);
#endif

#ifndef	DEBUG_CLEARBUF
	pfinal->offset = CTCI_BH_FINAL;			// final block header
#endif

//  Save status from this call so we can pick up where we left off
//  on the next call.  This consists of keeping pstat.fillbuf current
//  (which ctwfresh did for us), and updating pcih->offset which we
//  just did (above; right after copying the caller's buffer).

	if (pstat->pidsleep != BADPID) 
		ready(pstat->pidsleep, RESCHNO);	// awaken ctciowrite
	signal(pcd->sem);				// release CTCI write device
	procstate(NPROC, "CTCI_WRITE after CTCI device released");
	trace("CTCI_WRITE exit; return %d\n", lenparm);
	return(lenparm);
}





