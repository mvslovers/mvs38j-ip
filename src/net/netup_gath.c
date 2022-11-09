/* netup_gath.c - netup_gath */

#include <conf.h>
#include <kernel.h>
#include <sleep.h>
#include <network.h>
#include <snmp.h>
#include <tcptimer.h>
#include <proc.h>
#include <date.h>
#include <q.h>
#include <config.h>	// MVS-Xinu configuration

#define	NOGATE_SLEEP	0	// seconds to sleep to gather routes
	// when no gateway (was 30); since MVS-Xinu uses a point-to-
	// point link and doesn't currently offer gateway services
	// there doesn't seem to be much point in waiting for something
	// to arrive that never will

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  netup_gath	Initialize network interfaces 
 *		Gather IP addresses, subnet mask, and DNS names
 *		Called by netstart
 *------------------------------------------------------------------------
 */

void netup_gath() {

	IPaddr	ifip;
	char	bufip[IP_MAXDOT];
	char	junk2[128];
	int	i;


	trace("NETROUTE gathering routes\n");
	if ( (!Config.megate) && (NOGATE_SLEEP > 0) ) {
		trace("NETROUTE sleeping %d seconds to get routes\n", NOGATE_SLEEP);
		sleep(NOGATE_SLEEP);
		trace("NETROUTE awoken; routes gathered, currently\n");
		rtdump(ALL_ROUTES);
	} else
		trace("NETROUTE MVS-Xinu point-to-point; not sleeping to gather routes\n");
	trace("\n");


	/* get addrs & names */

	trace("NETUP_GATH gather interfaces' IP address, subnet mask, and DNS name\n");
	for (i=0; i<Net.nif; ++i) {

		if (i == NI_LOCAL) {
			trace("NETUP_GATH skipping NI_LOCAL\n");
			nifdump("NETUP_GATH NI_LOCAL", &nif[i], 0);
			continue;
		}
		if (nif[i].ni_state != NIS_UP) {
			trace("NETUP_GATH skipping DOWN interface %d\n", i);
			continue;
		}

	//  Retrieve interface's IP address

		if ( ! nif[i].ni_ivalid) {
			trace("NETUP_GATH getting IP address for interface %d\n", i);
			ifip = getiaddr(i);	// find interface's IP addr
		}
		if (nif[i].ni_ivalid)
			trace("NETUP_GATH interface %d IP %s\n", i, ip2dot(bufip, nif[i].ni_ip));

	//  Retrieve subnet mask

		if ( ! nif[i].ni_svalid) {
			trace("NETUP_GATH getting subnet mask for interface %d\n", i);
			trace("NETUP_GATH icmp MASKRQ\n");
			icmp(ICT_MASKRQ, 0, nif[i].ni_brc, 0, 0);
			recvtim(30);	/* wait for an answer */
		}
		if (nif[i].ni_svalid)
			trace("NETUP_GATH interface %d mask %08x\n", i, ip2dot(bufip, nif[i].ni_subnet));

	//  Retrieve host DNS name

		if ( ! nif[i].ni_nvalid) {
			trace("NETUP_GATH getting host DNS name for interface %d\n", i);
			getiname(i, junk2);
		}
		if (nif[i].ni_nvalid)
			trace("NETUP_GATH interface %d DNS name %s\n", i, nif[i].ni_name);

		if ( (!nif[i].ni_ivalid) || (!nif[i].ni_svalid) || (!nif[i].ni_nvalid) ) {
			trace("NETUP_GATH ****** WARNING ****** interface %d incomplete\n", i);
			nifdump("NETUP_GATH interface incomplete", &nif[i], 1);
		}
	}
	trace("NETUP_GATH addresses and names gathered\n"); 
}



