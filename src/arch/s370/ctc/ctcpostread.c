/* ctcpostread.c - ctcio post-read */

//  TODO  Examine MVS I/O read status (POST code, etc.)

#include <conf.h>
#include <kernel.h>
#include <ctc.h>
#include <mvsio.h>		// IOB

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  ctcpostread 	Examine I/O status after CTC read
 *	Returns -1 when I/O error, else length of data read
 *------------------------------------------------------------------------
 */

int	ctcpostread(ctblk *pctblk) {

	ctdev		*pctdev;
	ctioblk		*pctioblk;
	mvsiob		*piob;
	mvsctcdcb	*pdcb;
	ccw0		*pccw;
	int		reqlen, reslen, readlen;


	pctdev = &pctblk->read;				// @ read half of CTC
	pctioblk = pctdev->ctioblk;			// @ read CTC ctioblk
	piob = &pctioblk->iob;				// @ read CTC IOB
	pdcb = &pctioblk->dcb;				// @ read CTC DCB
	pccw = &pctioblk->ccw;				// @ read CTC CCW

	trace("CTCPOSTread ccw 0x%08x; cmd 0x%02x addr 0x%08x flags 0x%02x count 0x%04x\n",
		(int)pccw & 0xffffff, 
		(int)pccw->req.cmd & 0xff, 
		(int)pccw->req.addr & 0xffffff, 
		(int)pccw->flag & 0xff, 
		(int)pccw->count & 0xffff);

	reqlen = pccw->count & 0xffff;			// I/O length from read CCW
	reslen = piob->residual;			// CSW residual length
	readlen = reqlen - reslen;			// length of read fulfilled
	trace("CTCPOSTread requested %d 0x%x residual %d 0x%x; read data length %d 0x%x\n",
		reqlen, reqlen, reslen, reslen, readlen, readlen);

	if (piob->ecb.cc == 0x7f) {
		pctioblk->iolen = readlen;		// tell caller how much read
		return readlen;				// I/O completed normally
	}
	pctioblk->iolen = -1;				// tell caller I/O error
	trace("CTCPOSTread post code 0x%x\n", piob->ecb.cc);
	trace("CTCPOSTread iob 0x%x dcb 0x%x ccw 0x%x\n", piob, pdcb, pccw);
	snap("CTCPOSTread iob", (void *)piob, sizeof(mvsiob));
	snap("CTCPOSTread ctioblk", (void *)pctioblk, sizeof(ctioblk));
	snap("CTCPOSTread ctdev", (void *)pctdev, sizeof(ctdev));
	return -1;					// return I/O error
}

