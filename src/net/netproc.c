/* netproc.c - netproc */

#include <conf.h>
#include <kernel.h>
#include <network.h>

#define	DEBUG
#include <debug.h>		// dbug, snap, trace, etc.

#define	DEBUG_FLUSH		// bail on syserr

/*------------------------------------------------------------------------
 *  netproc	Read inbound packets from MVS-Xinu CTC device
 *------------------------------------------------------------------------
 */

//  Netproc is similar to netin as described in Comer's book (vol2).
//  For some reason, I can't find the netin source code?!?
//  Netproc basically lays around trying to read IP frames from
//  the CTC device, and passes them to ni_in in a similar fashion
//  to the PC-Xinu ether/*demux routines.

PROCESS netproc() {


	int	rc;
	int	ifnum = NI_PRIMARY;	// primary network interface
  struct ep	*pep;
  struct nif	*pnif = &nif[ifnum];

	trace("NETPROC starting\n");

//  The MVS-Xinu CTC device doesn't require an open or close

	while (TRUE) {
		pep = getbuf(Net.netpool);
		rc = read(ETHER, pep, MAXNETBUF);
		if (rc == SYSERR) {
			trace("NETPROC read syserr\n");
#ifdef	DEBUG_FLUSH
			trace("NETPROC option FLUSH\n");
			shutxinu = TRUE;
			return(SYSERR);
#else
			continue;
#endif
		}
		ni_in(pnif, pep, pep->ep_len);
	}
}

