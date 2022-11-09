/* ctci_iowrite.c - CTCI I/O write process */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <ctc.h>				// general CTC headers
#include <ctci.h>				// CTCI headers
#include <mvsio.h>				// MVS I/O control blocks
#include <config.h>				// Config
#include <ctcwrite.h>				// data shared with ctciowrite
#include <string.h>				// Systems/C S/370 quick string functions

#define	DEBUG
#include <debug.h>

#define	DEBUG_IPSHOW				// display IP frames written
#define	DEBUG_DUMPBUF				// snap buffer before write
	
/*------------------------------------------------------------------------
 *  ctci_iowrite - 	CTC I/O write process
 *		Interfaces with CTC Channel End Appendage
 *		Activated by iohook when ctcecb POSTed
 *------------------------------------------------------------------------
 */

//  When we receive control after ctci_init resumes us, the CTC write
//  semaphore is owned by ctcinit.  We release the CTC write semaphore 
//  once we finish our initialization.
//  The CTCI write buffer that we present to the CTCI device is laid 
//  out the same as the CTCI read buffer.  
//  See ctci.h for CTCI buffer notes
//  See ctci_ioread for an overview of CTC I/O

PROCESS ctci_iowrite() {

	Bool		owndev = TRUE;		// we own CTC write device at entry
	void		*pbuf, *pvoid;
	ctblk		*pctblk;
	ctdev		*pctdev;
	ctioblk		*pctioblk;
	mvsiob		*piob;
	mvsctcdcb	*pdcb;
	void		*pecb;
	ctche		*pctche;
	ctchalf		*pconfig;
	ctcihdr		*pcih;
	ctcseghdr	*pseg;			// addr of CTCI buffer segment
	struct ip	*pip;			// addr of IP frame
	int		i, pid, writelen, memlen;
	int		retry = CTC_RETRY;	// # times to retry write
	Bool		iogood;			// buffer blessed (TRUE) or condemned (FALSE)
	ctwritestat	*pstat;			// shared with ctcwrite


	trace("\nCTCI_IOwrite ******** STARTED ********\n\n");

//  Find the CTCI write device which we will service
//  Our pid was stored in the ctdev by ctci_init
	pid = getpid();
	pctdev = NULL;
	for (i = 0; i < Neth; i++) {
		pctblk = &ctcdev[i];
		if (pid == pctblk->write.iopid) {
			pctdev = &pctblk->write;
			break;
		}
	}
	if (pctdev == NULL)
		panic("CTCI_IOwrite ctdev locate");

	trace("CTCI_IOwrite pid %d pctblk 0x%x, pctdev 0x%x cuu %x\n", 
		pid, pctblk, pctdev, pctdev->cuu);

//  Allocate control blocks associated with the ctdev (ordered as in memory):
//  	CTCHE requires no initialization, it's just a workarea for IGG019X8
//		(debug: ctche.cheid is set to CTCHE, which differs from the
//      	value IGG019X8 sets)
//  	CTIOBLK requires initialization, including asking MVSCTC to fill 
//  		in the MVS I/O control blocks
//  	CTWRITESTAT has its eyecatcher initialized & pidsleep set

	memlen = 		sizeof(ctche) + 
				sizeof(ctioblk) + 
				sizeof(ctwritestat);
	pctche = (void *)getmem(memlen);
	if (pctche == (void *)SYSERR)
		panic("CTCI_IOwrite getmem");
	trace("CTCI_IOwrite allocated 0x%x length %d 0x%x: che, ctioblk, ctwritestat\n",
		pctche, memlen, memlen);

	pctioblk = (void *) ( (char *)pctche   + sizeof(ctche) );
	pstat    = (void *) ( (char *)pctioblk + sizeof(ctioblk) );

	memset(pctche, 0x00,	sizeof(ctche) + 
				sizeof(ctioblk) + 
				sizeof(ctwritestat) );

	pctdev->chework = pctche;
	strncpy(pctche->cheid, "CTCHE", sizeof(pctche->cheid));

	pctdev->ctioblk = pctioblk;
	strncpy(pctioblk->eye, "CTIOBLK", sizeof(pctioblk->eye));
	pctioblk->pecb = &pctdev->ecb;
	pctioblk->pctdev = pctdev;

	strncpy(pstat->eye, "CTWRITESTAT", sizeof(pstat->eye));
	pctdev->ioword = pstat;					// tell ctci_write where
	pstat->pidsleep = BADPID;				// not sleeping

	trace("CTCI_IOwrite pctche 0x%x length %d 0x%x\n", 
		pctche, sizeof(ctche), sizeof(ctche));
	trace("CTCI_IOwrite pctioblk 0x%x length %d 0x%x\n", 
		pctioblk, sizeof(ctioblk), sizeof(ctioblk));
	trace("CTCI_IOwrite pstat 0x%x length %d 0x%x\n", 
		pstat, sizeof(ctwritestat), sizeof(ctwritestat));

//  Copy MVS CTC DCB to ctioblk, fill in ddname, open DCB

	pdcb = mvsctc(CTC_OPCOPYW, pctioblk);
	pconfig = pctdev->config;
	pvoid = (char *)pdcb + 40;
	memcpy((char *)pvoid, pconfig->ddname, sizeof(pconfig->ddname));
	
	pdcb = mvsctc(CTC_OPOPENW, pctioblk);
	if (pdcb == NULL) {
		snap("CTCI_IOwrite DCB open fail DCB; ctioblk", 
			(void *)pctioblk, sizeof(ctioblk));
		panic("CTCI_IOwrite open failed");
	}
	trace("CTCI_IOwrite open OK\n");
	piob = &pctioblk->iob;
	pecb = &pctdev->ecb;
	pctioblk->pecb = pecb;
	trace("CTCI_IOwrite opened DCB pdcb 0x%x piob 0x%x pecb 0x%x\n", 
		pdcb, piob, pecb);

//	snap("CTCI_IOwrite ctchalf", (void *)pconfig, sizeof(ctchalf));
//	snap("CTCI_IOwrite ctblk", (void *)pctblk, sizeof(ctblk));
	snap("CTCI_IOwrite ctdev", (void *)pctdev, sizeof(ctdev));
	snap("CTCI_IOwrite ctche", (void *)pctche, sizeof(ctche));
	snap("CTCI_IOwrite ctioblk", (void *)pctioblk, sizeof(ctioblk));
//	snap("CTCI_IOwrite writestat", (void *)pstat, sizeof(ctwritestat));

//  Create buffer pool

	pctioblk->buflen = CTC_BUFLEN;
	if (CTC_BUFLEN > MVSMAXIOSIZE)
		pctioblk->buflen = MVSMAXIOSIZE;		// max MVS38j I/O length

	pctioblk->poolid = mkpool(pctioblk->buflen, CTC_WRITEBUFS);
	if (pctioblk->poolid == SYSERR)
		panic("CTCI_IOwrite mkpool");

	signal(pctdev->sem); owndev = FALSE;			// device ready for action

//  We attempt to stay current as we write the output buffers queued by ctci_write
//  Once we have written all the buffers ctci_write queued, we attempt to steal a buffer
//  from ctci_write, until nothing remains to be written; whereupon we go to sleep until
//  awoken by ctci_write

	pbuf = NULL;						// haven't found buffer to write
	while (pctblk->shutdown == FALSE) {

		//  If we don't already own the CTC write device, get control of it

		if (!owndev)
			wait(pctdev->sem); owndev = TRUE;	// get control of device

		//  Attempt to find a buffer to write

		trace("CTCI_IOwrite searching for buffer\n");
//		snap("CTCI_IOwrite searching for buffer; ctwritestat", 
//			(void *)pstat, sizeof(ctwritestat));

		if (pbuf == NULL)				// we might be retrying write
			pbuf = pstat->filled[0];		// retrieve FIFO buf ptr

		if (pbuf == NULL) {				// try completely full buffer list first
			if (pstat->fillbuf != NULL) {		// can we steal one from ctci_write?
				pbuf = pstat->fillbuf;		// @ ctci_write buffer
				pstat->fillbuf = NULL;		// stole ctci_write's buffer
			}
		} else {

			//  Remove top ptr from filled[], move rest of array up one, clear last ptr

			pbuf = pstat->filled[0];
			memcpy(&pstat->filled[0], &pstat->filled[1], (CTC_WRITEBUFS - 1) * sizeof(void *));
			pstat->filled[ CTC_WRITEBUFS - 1 ] = NULL;
		}

		//  If no buffer is available, tell ctci_write we're going to sleep
		//  (suspend ourselves) until ctci_write awakens us to process the
		//  new buffer ctci_write just hung in writestat

		if (pbuf == NULL) {
			trace("CTCI_IOwrite no buffer available\n");
			pstat->pidsleep = pid;			// tell ctci_write who we are
			signal(pctdev->sem); owndev = FALSE;	// release device
			suspend(pid);				// wait for filled buffer
			wait(pctdev->sem); owndev = TRUE;	// seize device ownership
			pstat->pidsleep = BADPID;		// we're no long waiting
			continue;				// go look for a buffer again
		}

#ifdef	DEBUG_DUMPBUF
		snap("CTCI_IOwrite buffer", pbuf, 138);
#endif

		//  We now have a buffer to write
		//  Calculate the length of the write & initiate write operation

		pcih = (void *)pbuf;
		writelen = pcih->offset + sizeof(pcih->offset);

		trace("CTCI_IOwrite ****** WRITE ISSUED ****** "
			"pbuf 0x%x writelen %d pcih->offset %d\n", 
			pbuf, writelen, pcih->offset);

		pctdev->flag1 = CTIOPEND;			// indicate I/O pending
		pctioblk->buf = pbuf;				// current buffer
		pctioblk->buflen = writelen;			// length of write CCW

		piob = mvsctc(CTC_OPWRITE, pctioblk);		// start the write

		//  Await completion of the write

		signal(pctdev->sem); owndev = FALSE;		// release device

#ifdef	DEBUG_IPSHOW
		//  We can examine buffer without owning device; mvsctc doesn't modify buffer
		pseg = (void *)
			((BYTE *)pcih + sizeof(ctcihdr));	// @ 1st CTCI buffer segment
								// or 0x0000 in empty block
		while (pseg->seglen) {				// @ 0x0000 ctcihdr when thru
			pip = (void *)
				((BYTE *)pseg + sizeof(ctcseghdr));
			ipshowhdr(pip, NULL);			// display IP frame
			pseg = (void *)
				((BYTE *)pseg + pseg->seglen);	// @ next segment
		}
#endif

		trace("CTCI_IOwrite waiting for write I/O completion\n");
		suspend(pid);				// wait for iohook to awaken us
		trace("CTCI_IOwrite awoken following write I/O; ctcecb 0x%x\n", ctcecb);

		//  Write complete, examine MVS I/O status

		wait(pctdev->sem); owndev = TRUE;		// get device ownership
		iogood = ctcpostwrite(pctblk);			// returns TRUE when write OK

		trace("CTCI_IOwrite I/O status: %s\n", iogood ? "GOOD" : "BAD");

		//  Write went OK, or we've exceeded retries: discard buffer

		if ( (iogood) || (--retry < 0) ) {
			freebuf(pbuf);				// free buffer for ctci_write to fill
			pbuf = NULL;				// tell top of loop already have buffer
			retry = CTC_RETRY;			// next write begins with fresh retry count
			continue;				// go look for another buffer to write
		}

		//  Retry the write until we've exahusted the retry count (pbuf preserved)

		trace("CTCI_IOwrite retrying write\n");

	} /* while not shutdown */

//  Close CTC DCB
//  FIXME there is a mkpool, but no freepool (memory leak)

	trace("CTCI_IOwrite closing DCB 0x%x\n", pctioblk->dcb);
	mvsctc(CTC_OPCLOSEW, pctioblk);				// close CTC device
	freemem(pctche, memlen);				// release memory associated with device
	if (owndev)
		signal(pctdev->sem);				// release device ownership
	trace("CTCI_IOwrite exit; OK\n");
	return OK;
}


