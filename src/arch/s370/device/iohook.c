/* iohook.c - MVS-specific CTC I/O support */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <ctc.h>
#include <string.h>	// Standard C header

#define	DEBUG
#include <debug.h>
#undef	DEBUG_SNAP_CTDEV
#undef	DEBUG_PROC

/*------------------------------------------------------------------------
 * iohook - MVS-Xinu resched intersection with Channel End Appendage
 *------------------------------------------------------------------------
 */

void iohook() {

	ctdev	*pct;
	xgd	*pxgd;

//	trace("IOHOOK enter\n");
	pxgd = findxgd();
	pct = pxgd->ctclink;
	while (pct) {
		if (pct->flag1 & CTIOCOMPLETE) {

#ifdef	DEBUG_SNAP_CTDEV
			snap("IOHOOK ctdev with I/O complete", 
				pct, sizeof(ctdev));
#endif

			//  Before we lose control, turn off our flags
			//  so we don't reprocess the same I/O

			pct->flag1 &= (255 - (CTIOPEND | CTIOCOMPLETE));
#ifdef	DEBUG_PROC
			procstate(NPROC, "IOHOOK pre-resume");
			trace("IOHOOK awakening CTCIO process %d\n",
				pct->iopid);
#endif
			resume(pct->iopid);
		}
		pct = pct->next;
	}
//	trace("IOHOOK exit\n");

} /* iohook */


