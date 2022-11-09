/* netup_nif.c - netup_nif */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <ctc.h>
#include <config.h>	// MVS-Xinu configuration

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
 *  netup_nif	Initialize network interfaces
 *		Called by netstart
 *------------------------------------------------------------------------
 */

int	netup_nif() {

	int		i, host;
	void		*pnif;
	fignif		*pfignif;
	ctblk		*pctblk;
	struct devsw	*pdevsw;


	trace("\nNETUP_NIF enter; initializing network interfaces\n");
	for (i = 0; i < Net.nif; ++i) {

		pnif = &nif[i];

		/* start with everything 0 */
		memset(&nif[i], 0, sizeof(nif[i]));

		nif[i].ni_state = NIS_DOWN;
		nif[i].ni_admstate = NIS_UP;
		nif[i].ni_ivalid = FALSE;
		nif[i].ni_nvalid = FALSE;
		nif[i].ni_svalid = FALSE;

		pfignif = &Config.nif[i];		// @ nif's fignif
		if (!pfignif->enabled) {
			trace("NETUP_NIF skipping disabled interface %d 0x%x\n", i, pnif);
			continue;		// ignore disabled interfaces
		}

		switch(pfignif->type) {

			case NIFT_LOOP:
				trace("NETUP_NIF init iface %d type LOOPBACK, pnif 0x%x\n", i, pnif);
				//  no Xinu device associated with loopback
				loopinit(i);
				break;

			case NIFT_CTCI:
			case NIFT_LCS:
				trace("NETUP_NIF init iface %d type ETHER/CTCI/LCS 0x%x\n", i, pnif);
				pctblk = &ctcdev[i - 1];	// @ ctblk
				pdevsw = pctblk->pdevsw;	// @ devtab entry
				nif[i].ni_dev = pdevsw->dvnum;	// Xinu device number
				efaceinit(i);
				break;

			default:
				trace("NETUP_NIF unknown type %d; interface %d; IGNORED\n", 
					nif[i].ni_dev, i);
				break;
		};
	}

	trace("NETUP_NIF routes at network interface init completion\n");
	rtdump(ALL_ROUTES);
	trace("NETUP_NIF exit; OK\n");
	return OK;
}

