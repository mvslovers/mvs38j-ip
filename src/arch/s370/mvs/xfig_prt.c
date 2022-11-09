/* xfig_prt.c - xfig_prt */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <config.h>	// the object of our desire
#include <string.h>	// Standard C header

#define	DEBUG
#include <debug.h>

#define	DEBUG_DUMPFIG	// defined = snap Config

/*------------------------------------------------------------------------
 * xfig_prt	Print Config
 *------------------------------------------------------------------------
 */

void	xfig_prt() {

	int	i;
	char	*ptxt;
	fignif	*pnif;
	char	bufip1[IP_MAXDOT];
	char	bufip2[IP_MAXDOT];
	char	bufip3[IP_MAXDOT];

#ifdef	DEBUG_DUMPFIG
	snap("Config", (void *)&Config, sizeof(Config));
#endif

	kprintf("MVS-Xinu gateway mode %sabled\n", 
		Config.megate ? "en" : "dis");

	kprintf("Server time %s rfs %s syslog %s\n",
		ip2dot(bufip1, Config.servtime), 
		ip2dot(bufip2, Config.servrfs), 
		ip2dot(bufip3, Config.servlog) );

	kprintf("DNS-1 %s DNS-2 %s\n",
		ip2dot(bufip1, Config.servdns[0]), 
		ip2dot(bufip2, Config.servdns[1]) );

	for (i = 0; i < MAX_IFACE; i++) {

		switch (Config.nif[i].type) {
			case	NIFT_LOOP:	ptxt = "LOOPBACK"; break;
			case	NIFT_CTCI:	ptxt = "CTCI"; break;
			case	NIFT_LCS:	ptxt = "LCS"; break;
			default:		ptxt = "UNKNOWN";
		}
		kprintf("iface %d %sabled type %s ip %s mask %s gw %s\n", 
			i, 
			Config.nif[i].enabled ? "en" : "dis", 
			ptxt, 
			ip2dot(bufip1, Config.nif[i].ip),
			ip2dot(bufip2, Config.nif[i].mask),
			ip2dot(bufip3, Config.nif[i].gwip) );

		if (i != 0) {		// loopback doesn't care about these

			kprintf("iface %d read ddname %s cuu %03x ", 
				i,
				Config.nif[i].ctc.read.ddname, 
				Config.nif[i].ctc.read.cuu );

			kprintf("write ddname %s cuu %03x\n", 
				Config.nif[i].ctc.write.ddname, 
				Config.nif[i].ctc.write.cuu );

			if (Config.nif[i].dfgw)
				kprintf("iface %d defaultgw %s\n",
					i,
					ip2dot(bufip1, Config.nif[i].gwip) );
		}
	}

	kprintf("\n");
	return;
}




