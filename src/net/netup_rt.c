/* netup_rt.c - netup_rt */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <config.h>	// MVS-Xinu configuration

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  netup_rt	Establish initial MVS-Xinu network routes
 *------------------------------------------------------------------------
 */

int	netup_rt() {

	int	i;


	trace("NETUP_RT enter\n");
	if (Config.nif[NI_LOCAL].type == NIFT_LOOP) {
		RT_LOOPBACK = Config.nif[NI_LOCAL].ip;
	} else
		panic("NETUP_RT network interface %d not loopback\n", NI_LOCAL);

	trace("NETUP_RT add NI_LOCAL loopback route\n");
	rtadd(RT_LOOPBACK, ip_maskall, RT_LOOPBACK, RTM_NOHOPS, 
		NI_LOCAL, RT_INF);

//  The first default route we encounter on an enabled interface is the one we use

	for (i = 0; i < Neth; i++) {
		if (Config.nif[i].dfgw && Config.nif[i].enabled) {
			trace("\nNETUP_RT set default gateway\n");
			rtadd(RT_DEFAULT, RT_DEFAULT, Config.nif[i].gwip, RTM_INF - 1, 
				i, RT_INF);
		}
	}
	trace("NETUP_RT exit; OK\n");
	return OK;
}







