/* inithost.c - inithost */

//  OBSOLETE  I butchered this, and now there's nothing left
//  OBSOLETE  Same deal with initgate
//  OBSOLETE  Code split between netstart and netup_nif

#include <conf.h>
#include <kernel.h>
#include <network.h>

#define	DEBUG
#include <debug.h>

#if Noth > 0
	int	getsim();
	static	int	itod[] = { NI_LOCAL, ETHER, OTHER1, OTHER2 };
	static	IPaddr	iftoip[] = { {0, 0, 0, 0}, {0, 0, 0, 0},
			{130, 10, 59, 0}, {209, 6, 36, 0} };
#else
	static	int	itod[] = { NIF_LOOPDEV, ETHER };
#endif

/*------------------------------------------------------------------------
 * inithost	initialize a gateway's interface structures
 *		(not a gateway)
 *------------------------------------------------------------------------
 */

int inithost(void) {

	int	i, host;
	void	*pnif;

	trace("INITHOST entered\n");

#if	Noth > 0
	host = getsim();
#endif

	trace("INITHOST initializing network interfaces\n");
	for (i = 0; i < Net.nif; ++i) {

		pnif = &nif[i];
		trace("INITHOST initializing interface %d 0x%x\n", i, pnif);

		/* start with everything 0 */
		memset(&nif[i], 0, sizeof(nif[i]));

		nif[i].ni_state = NIS_DOWN;
		nif[i].ni_ivalid = FALSE;
		nif[i].ni_nvalid = FALSE;
		nif[i].ni_svalid = FALSE;
		nif[i].ni_dev = itod[i];

		switch(nif[i].ni_dev) {

#ifdef OTHER1
			case OTHER1:
			case OTHER2:
				trace("INITHOST interface %d type OTHER\n", i);
				if (nif[i].ni_dev != host) {
					nif[i].ni_state = NIS_DOWN;
					nif[i].ni_admstate = NIS_DOWN;
					continue;
				}
				ofaceinit(i);
				break;
#endif /* OTHER1 */

			case NIF_LOOPDEV:
				trace("INITHOST interface %d type LOOPBACK\n", i);
				loopinit(i);
				break;

			case ETHER:
				trace("INITHOST interface %d type ETHER\n", i);
				efaceinit(i);
				break;

			default:
				trace("INITHOST unknown type %d; interface %d; IGNORED\n", 
					nif[i].ni_dev, i);
				break;
		};
	}

	trace("INITHOST exit; OK\n");
	return OK;
}



