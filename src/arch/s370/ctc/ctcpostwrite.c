/* ctcpostwrite.c - ctcio post-read */

//  TODO  Examine MVS I/O write status (POST code, etc.)

#include <conf.h>
#include <kernel.h>
#include <ctc.h>
#include <mvsio.h>		// IOB

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  ctcpostwrite 	Examine I/O status after CTC write
 *	Returns TRUE when I/O OK, else FALSE
 *------------------------------------------------------------------------
 */

Bool ctcpostwrite(ctblk *pctblk) {

	ctdev		*pctdev;
	ctioblk		*pctioblk;
	mvsiob		*piob;
	mvsctcdcb	*pdcb;
	ccw0		*pccw;
	int		reqlen, reslen, writelen;


	pctdev = &pctblk->write;			// @ write half of CTC
	pctioblk = pctdev->ctioblk;			// @ write CTC ctioblk
	piob = &pctioblk->iob;				// @ write CTC IOB
	pdcb = &pctioblk->dcb;				// @ write CTC DCB
	pccw = &pctioblk->ccw;				// @ write CTC CCW

//  It never seems likely that the IOB CSW residual != zero on a write

	trace("CTCPOSTwrite ccw 0x%08x; cmd 0x%02x addr 0x%08x flags 0x%02x count 0x%04x\n",
		(int)pccw & 0xffffff, 
		(int)pccw->req.cmd & 0xff, 
		(int)pccw->req.addr & 0xffffff, 
		(int)pccw->flag & 0xff, 
		(int)pccw->count & 0xffff);

	reqlen = pccw->count & 0xffff;			// I/O length from write CCW
	reslen = piob->residual;			// CSW residual length
	writelen = reqlen - reslen;			// length of write fulfilled
	trace("CTCPOSTwrite requested %d 0x%x residual %d 0x%x; write data length %d 0x%x\n",
		reqlen, reqlen, reslen, reslen, writelen, writelen);

	if (piob->ecb.cc == 0x7f)
		return TRUE;				// I/O completed normally

	trace("CTCPOSTwrite iob 0x%x dcb 0x%x ccw 0x%x\n", piob, pdcb, pccw);
	snap("CTCPOSTwrite ctioblk", (void *)pctioblk, sizeof(ctioblk));
	snap("CTCPOSTwrite ctdev", (void *)pctdev, sizeof(ctdev));
	trace("CTCPOSTwrite post code 0x%x\n", piob->ecb.cc);

	return FALSE;					// I/O error
}

