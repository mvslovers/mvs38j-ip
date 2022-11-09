/* ctci_read.c - ctci_read */

//  TODO	Review bogus struct ep initialization, with an eye to
//		possibly providing something besides IP frames at
//		some later (perhaps very later) time
//  TODO	Review FILL_MISSING_ETHADDRS

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>
#include <ether.h>
#include <q.h>
#include <ctc.h>
#include <ctci.h>
#include <mvsio.h>
#include <ctcread.h>	// ctcread, ctcioread interface

#define DEBUG
#include <debug.h>

#define	DEBUG_BUFFER
#undef	DEBUG_SHOWSEG

#define	FILL_MISSING_ETHADDR

//  Bogus struct ep initialization values

#define	CTCIFN		1				// CTC interface number
#define	CTCORDER	0				// "order" see ether.h
#define	CTCETHDST	"\011\022\033\044\055\066"	// bogus ethernet destination
#define	CTCETHSRC	"\111\122\133\144\155\166"	// bogus ethernet source
#define	CTCEPTYPE	EPT_IP				// ethernet frame type: IP

/*------------------------------------------------------------------------
 *  ctci_read	Retrieve IP frame from buffer filled by ctcioread
 *------------------------------------------------------------------------
 */

//	devptr	pointer to CTC device
//	p2	address of caller-supplied buffer
//	len	length of caller-supplied buffer

//  Notes:
//	Called by netproc
//	We simulate an ethernet packet header in the caller's
//	buffer, along with the rest of struct ep

int ctci_read(struct devsw *devptr, char *p2, unsigned len) {

	struct ep	*pep = (void *)p2;
	struct ep	epfake;
	ctblk		*pct;
	ctdev		*pcd;
	void		*piob;
	void		*pbuf;
	int		index, dl;
	int		ephlen = sizeof(struct ep) - sizeof(epfake.ep_data);
	ctreadstat	*pstat;
	ctcihdr		*pcih;
	ctcseghdr	*pseg;
	struct ip	*pip;			// IP frame pointer
	void		*plast;			// addr of final CTCIHDR in buffer
	ipcheat		cheat;			// handy IP info from ipshowhdr
	dbug(char	bufhist[80]);


	dbug(funchist(bufhist, sizeof(bufhist), NULL));		// call history
	trace("CTCI_read enter; callers: %s\n");
	trace("CTCI_read devptr 0x%x, buffer 0x%x, len %d\n", devptr, p2, len);

//  Assure the CTC device is already initialized

	pct = &ctcdev[devptr->dvminor];
	trace("CTCI_read pct 0x%x\n", (void *)pct);
	if (pct->ctcinit != TRUE) {
		panic("CTCI_read CTC not initialized\n");
	}
	pcd = &pct->read;				// @ CTC read device
	pstat = pcd->ioword;				// address of ctcioread stat
	trace("CTCI_read pcd 0x%x ioword 0x%x = CTREADSTAT\n", (void *)pcd, (void *)pstat);
	
	wait(pcd->sem);					// wait for device semaphore

//  Adjust the caller's buffer pointer to where we want the IP frame deposited.
//  Adjust the caller's length to account for struct ep overhead
//  Clear the area reserved for struct ep at front of caller's buffer
//  Fill in as much of struct ep as we can, even if it's bogus data

	p2 = (void *)&pep->ep_data;
	len -= ephlen;
	trace("CTCI_read fixup p2 0x%x len %d\n", p2, len);
	if (len <= 0) {
		trace("CTCI_read insufficient remaining buffer size %d\n", len);
		return(SYSERR);
	}
	memset((void *)pep, 0x00, ephlen);
	pep->ep_ifn = CTCIFN;
	pep->ep_order = CTCORDER;

#ifdef	FILL_MISSING_ETHADDRS
	if (pep->ep_dst == 0x000000)
		memcpy(&pep->ep_dst, CTCETHDST, sizeof(pep->ep_dst));
	if (pep->ep_dst == 0x000000)
		memcpy(&pep->ep_src, CTCETHSRC, sizeof(pep->ep_src));
#endif

	pep->ep_type = CTCEPTYPE;

//	snap("CTCI_read fake ethernet header", (void *)pep, ephlen);

//  Find the segment header for the next IP frame to be returned to the caller

	index = pstat->avail;				// stat buf[] & len[] index
	if (pstat->pseg != NULL) {			// already have buffer?

		//  Already have a buffer from a previous call, and it contains
		//  an IP frame which we can return to the caller

		pbuf = pstat->buf[index];		// old buffer
		pseg = pstat->pseg;			// next to be used frame
		trace("CTCI_read have buffer; index %d pbuf 0x%x pseg 0x%x\n", index, pbuf, pseg);
		snap("CTCI_read stat", (void *)pstat, sizeof(ctreadstat));

	} else {

		//  Locate a new buffer previously filled by ctcioread
		//  If none is present, wait for one

		if (pstat->len[index] == 0) {
			pcd->userpid = getpid();		// pid of waiting process
			signal(pcd->sem);			// release device
			suspend(pcd->userpid);			// suspend current process,
								// wait for buffer to appear
			pcd->userpid = BADPID;			// no pid waiting
			wait(pcd->sem);				// serialize device
			index = pstat->avail;			// refresh index
			if (pstat->len[index] == 0)
				panic("CTCI_read logic error 1");
		}
		pbuf = pstat->buf[index];			// address of filled buffer
		if (pbuf == NULL)
			panic("CTCI_read logic error 2");
		pseg = (ctcseghdr *) ( (unsigned char *)pbuf 
			+ sizeof(ctcihdr) );			// @ first segment
		trace("CTCI_read new buffer; pbuf 0x%x pseg 0x%x\n", pbuf, pseg);
	}

#ifdef	DEBUG_SHOWSEG
	snap("CTCI_read segment", pseg, pseg->seglen);
#endif

//  Return data to caller; if the IP frame won't fit return SYSERR
//  In case of SYSERR, the selected IP frame is left untouched

	pip = (struct ip *) ( (BYTE *)pseg 
		+ sizeof(ctcseghdr) );			// @ IP frame
	dl = pseg->seglen - sizeof(ctcseghdr);		// length of IP frame
	if (dl > len) {
		trace("CTCI_read buffer too small; dl %d len %d\n", dl, len);
		signal(pcd->sem);			// release device
		return(SYSERR);
	}
	pep->ep_len = dl + ephlen;			// update ethernet header with length

	memset(p2, 0x00, len);				// clear caller's buffer
	memcpy(p2, (void *)pip, dl);			// copy our buffer to caller's

#ifdef	DEBUG_BUFFER
	ipshowhdr(p2, &cheat);
	snap("CTCI_read buffer returned to caller", (void *)pep, pep->ep_len);
#endif

//  Set up for our next call
//  If we have just exhausted the data in the current buffer, free it 

	pseg += pseg->seglen;				// @ next segment's slot
	pstat->pseg = pseg;				// for next call

	plast = (ctcihdr *) ( (BYTE *)pbuf 
		+ ((ctcihdr *)pbuf)->offset );		// @ last CTCIHDR
	if ((void *)pseg >= (void *)plast) {
		pstat->buf[index] = NULL;		// finished with buffer
		pstat->len[index] = 0;			// finished with buffer
		pstat->pseg = NULL;			// no "next segment"
		index++;
		if (index == CTC_READBUFS)
			index = 0;			// wrap index for next read
		pstat->avail = index;
		freebuf(pbuf);				// release buffer
	}

//	snap("CTCI_read stat at exit", (void *)pstat, sizeof(ctreadstat));
	signal(pcd->sem);				// release device
	return(dl);
}







