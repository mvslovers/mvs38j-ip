/* ctci_ioread.c - ctcio read process */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>
#include <q.h>
#include <ctc.h>			// general CTC headers
#include <ctci.h>			// CTCI headers
#include <mvsio.h>			// MVS I/O control blocks
#include <config.h>			// Config
#include <ctcread.h>			// ctci_read, ctci_ioread interface

#define	DEBUG
#include <debug.h>

#undef	DEBUG_DUMP_BUFFER

/*------------------------------------------------------------------------
 *  ctci_ioread - 	CTC I/O read process
 *		Interfaces with CTC Channel End Appendage
 *		Interfaces with ctci_read
 *		Activated by iohook when ctcecb POSTed
 *------------------------------------------------------------------------
 */

//  When we receive control after ctcinit resumes us, the CTC read
//  semaphore is owned by ctcinit.  We signal the read semaphore 
//  once we finish our initialization, allowing ctci_read to begin
//  satisfying Xinu read() requests for the CTCI device

// -----------------  MVS-Xinu CTC I/O Overview  ------------------------------

//  We ask MVS to perform our CTC I/O via the MVSCTC assembler program;
//  MVSCTC will issue MVS EXCP to initiate the I/O, and returns to us
//  Shortly thereafter, we suspend ourselves to await I/O completion
//  If there's no ready Xinu work, resched will dispatch the nulluser process
//  which will eventually issue MVS WAIT against (perhaps) several ECBs,
//  one of which is ctcecb
//  With that action, MVS-Xinu has surrendered control of the cpu to MVS38j,
//  which runs other MVS work 
//  MVS-Xinu does NOT busy wait; busy waiting is _very_ rude in MVS
//  ... time passes ... eventually the I/O completes (channel end)
//  Upon CTC device channel end, our channel end appendage IGG019X8 (CHE)
//  will receive control from MVS IOS [MVS knew we had a channel end appendage
//  because we told it so in our Data Management DCB control block]
//  CHE will leave an <I/O complete> footprint for iohook, and issue MVS POST 
//  against the ctcecb (one of perhaps several which nulluser may wait upon)
//  MVS POST awakens the nulluser process (if it's waiting); resched shortly
//  thereafter call resched to look for ready Xinu work 
//  Resched calls iohook, who will see that CHE has marked our I/O complete
//  and readies our process; when we're the highest priority work (usually)
//  we get restarted by resched following the suspend call
//  Once we've verified the I/O is good, we hang the buffer up for ctci_read
//  to find and we start the next read (we might block if no buffers)
//  Thus, when Xinu read() calls ctci_read (via devsw) if everything has gone
//  well, there's a nice fresh buffer already waiting to satisfy the caller's
//  read request


PROCESS ctci_ioread() {

	ctblk		*pctblk;
	ctdev		*pctdev;
	ctioblk		*pctioblk;
	ctchalf		*pconfig;
	mvsctcdcb	*pdcb;
	void		*piob = NULL;
	void		*pecb;
	ctche		*pctche;
	void		*pbuf, *pdata, *pvoid;
	int		i, pid;
	int		memlen;
	int		readlen;

	int		iostat;
	ctreadstat	*pstat;			// shared with ctci_read
	int		bufndx = 0;		// buffer # to use (stat index)


	trace("\nCTCI_IOread ******** STARTED ********\n\n");

//  Find the CTCI read device which we will service
//  Our pid was stored in the ctdev by ctci_init

	pid = getpid();
	pctdev = NULL;
	for (i = 0; i < Neth; i++) {
		pctblk = &ctcdev[i];
		if (pid == pctblk->read.iopid) {
			pctdev = &pctblk->read;
			break;
		}
	}
	if (pctdev == NULL)
		panic("CTCI_IOread ctdev locate");

	trace("CTCI_IOread pid %d pctblk 0x%x, pctdev 0x%x cuu %x\n", 
		pid, pctblk, pctdev, pctdev->cuu);

//  Allocate control blocks associated with the ctdev (ordered as in memory):
//  	CTCHE requires no initialization, it's just a workarea for IGG019X8
//		(debug: ctche.cheid is set to CTCHE, which differs from the
//      	value IGG019X8 sets)
//  	CTIOBLK requires initialization, including asking MVSCTC to fill 
//  		in the MVS I/O control blocks
//  	CTREADSTAT has its eyecatcher initialized

	memlen = 		sizeof(ctche) + 
				sizeof(ctioblk) + 
				sizeof(ctreadstat);
	pctche = (void *)getmem(memlen);
	if (pctche == (void *)SYSERR)
		panic("CTCI_IOread getmem");
	trace("CTCI_IOread allocated 0x%x length %d 0x%x: che, ctioblk, ctreadstat\n",
		pctche, memlen, memlen);

	pctioblk = (void *) ( (char *)pctche   + sizeof(ctche) );
	pstat    = (void *) ( (char *)pctioblk + sizeof(ctioblk) );

	memset(pctche, 0x00,	sizeof(ctche) + 
				sizeof(ctioblk) + 
				sizeof(ctreadstat) );

	pctdev->chework = pctche;
	strncpy(pctche->cheid, "CTCHE", sizeof(pctche->cheid));

	pctdev->ctioblk = pctioblk;
	strncpy(pctioblk->eye, "CTIOBLK", sizeof(pctioblk->eye));
	pctioblk->pecb = &pctdev->ecb;
	pctioblk->pctdev = pctdev;

	pstat->avail = 0;					// ctcread starts at 0th pointer
	strncpy(pstat->eye, "CTREADSTAT", sizeof(pstat->eye));
	pctdev->ioword = pstat;					// tell ctcread where

	trace("CTCI_IOread pctche 0x%x length %d 0x%x\n", 
		pctche, sizeof(ctche), sizeof(ctche));
	trace("CTCI_IOread pctioblk 0x%x length %d 0x%x\n", 
		pctioblk, sizeof(ctioblk), sizeof(ctioblk));
	trace("CTCI_IOread pstat 0x%x length %d 0x%x\n", 
		pstat, sizeof(ctreadstat), sizeof(ctreadstat));

//  Copy MVS CTC DCB to ctioblk, fill in ddname, open DCB

	pdcb = mvsctc(CTC_OPCOPYR, pctioblk);
	pconfig = pctdev->config;
	pvoid = (char *)pdcb + 40;
	memcpy((char *)pvoid, pconfig->ddname, sizeof(pconfig->ddname));
	
	pdcb = mvsctc(CTC_OPOPENR, pctioblk);
	if (pdcb == NULL) {
		snap("CTCI_IOread DCB open fail DCB; ctioblk", 
			(void *)pctioblk, sizeof(ctioblk));
		panic("CTCI_IOread open failed");
	}
	piob = &pctioblk->dcb;
	pecb = &pctdev->ecb;
	pctioblk->pecb = pecb;
	trace("CTCI_IOread opened DCB pdcb 0x%x piob 0x%x pecb 0x%x\n", 
		pdcb, piob, pecb);

//	snap("CTCI_IOread ctchalf", (void *)pconfig, sizeof(ctchalf));
//	snap("CTCI_IOread ctblk", (void *)pctblk, sizeof(ctblk));
	snap("CTCI_IOread ctdev", (void *)pctdev, sizeof(ctdev));
	snap("CTCI_IOread ctche", (void *)pctche, sizeof(ctche));
	snap("CTCI_IOread ctioblk", (void *)pctioblk, sizeof(ctioblk));
//	snap("CTCI_IOread readstat", (void *)pstat, sizeof(ctreadstat));

//  Create buffer pool

	readlen = CTC_BUFLEN;
	if (CTC_BUFLEN > MVSMAXIOSIZE)
		readlen = MVSMAXIOSIZE;				// max MVS38j I/O length
	pctioblk->buflen = readlen;

	pctioblk->poolid = mkpool(readlen, CTC_READBUFS);
	if (pctioblk->poolid == SYSERR)
		panic("CTCI_IOread mkpool");

	signal(pctdev->sem);					// device ready for action

//  The CTC read process fills all available buffers, so that hopefully when
//  ctcread is called there will be a filled buffer already available
//  We do this until the CTC device is shutdown

	pbuf = NULL;						// acquire buffer below
	while (pctblk->shutdown == FALSE) {

//		snap("CTCI_IOread stat", pstat, sizeof(ctreadstat));

		if (pbuf == NULL) {
			pbuf = getbuf(pctioblk->poolid);	// block until buffer free
			if (pbuf == (void *)SYSERR)
				panic("CTCI_IOread getbuf");
		}

		wait(pctdev->sem);				// get exclusive control of device

		if (pstat->buf[bufndx] != NULL) {

			//  The buffer pointer should have been cleared by ctci_read
			//  after it finished using the buffer;  something is wrong
			//  if getbuf succeeded but the buffer pointer is non-zero

			snap(" CTCI_IOread panic: stat", (void *)pstat, sizeof(ctreadstat));
			panic("CTCI_IOread logic error; bufndx %d\n", bufndx);
		} else {
			pstat->len[bufndx] = 0;			// no data in buffer
			pstat->buf[bufndx] = pbuf;		// ctcread finished with slot, use it
		}

		//  Initiate the read operation

		pctdev->flag1 = CTIOPEND;			// indicate I/O pending
		pctioblk->buf = pbuf;				// current read buffer
		dbug(memset(pbuf, 0xcc, readlen));

		trace("CTCI_IOread buffer 0x%x ****** READ ISSUED ******\n", pbuf);

		piob = mvsctc(CTC_OPREAD, pctioblk);		// issue EXCP for write CCW

		signal(pctdev->sem);				// free device

		trace("CTCI_IOread waiting\n");

		suspend(pid);					// wait for iohook to awaken us

		trace("\nCTCI_IOread pid %d ******** AWOKEN ******** ; ctcecb 0x%x cuu %x\n\n", 
			pid, ctcecb, pctdev->cuu);

		//  Read complete, examine MVS I/O status
		//  If the I/O looks good, add the read buffer to ctreadstat

		iostat = ctcpostread(pctblk);			// see how I/O went
		if (iostat < 0)
			panic("CTCI_IOread I/O ERROR\n");

		trace("CTCI_IOread I/O status: GOOD, %d bytes read 0x%x\n", 
			iostat, pbuf);
		wait(pctdev->sem);				// get exclusive control of device
		pstat->len[bufndx] = iostat;			// length of data read
		signal(pctdev->sem);				// free device
		pbuf = NULL;					// buffer hung for ctci_read
		if (pctdev->userpid != BADPID)	 		// process waiting for buffer?
			resume(pctdev->userpid);		// tell ctcread buffer filled
		bufndx++;
		if (bufndx == CTC_READBUFS) 
			bufndx = 0;				// wrap to pstat->buf[0]
#ifdef	DEBUG_DUMP_BUFFER
		snap("CTCI_IOread buffer", pbuf, iostat.datalen);
#endif

	} /* while not shutdown */

//  Close CTC DCB
//  FIXME there is a mkpool, but no freepool (memory leak)

	trace("CTCI_IOread closing DCB 0x%x\n", pctioblk->dcb);
	mvsctc(CTC_OPCLOSER, pctioblk);
	freemem(pctche, memlen);
	trace("CTCI_IOread exit; OK\n");
	return OK;

}

