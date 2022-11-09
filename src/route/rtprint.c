/* rtprint.c - rtprint */

#include <conf.h>
#include <kernel.h>
#include <sleep.h>
#include <network.h>

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  rtprint  -  print info for one routing table entry
 *------------------------------------------------------------------------
 */

void	rtprint(struct route *prt, char *msg) {

	char	bufnet[IP_MAXDOT];
	char	bufgw[IP_MAXDOT];

	if (prt == (struct route *)NULL) {
		//       prt    next   dest   key    mask   gw
		kprintf("%-*.*s %-*.*s %-*.*s %-*.*s %-*.*s %-*.*s "
			"metric intf ttl  refs  use func-history <<%s>>\n",
			6, 6, "route",				// struct route S/370 address 
			6, 6, "next",
			IP_MAXDOT, IP_MAXDOT, "dest", 
			3, 3, "/n",				// number 1 bits in mask
			8, 8, "mask",
			IP_MAXDOT, IP_MAXDOT, "gateway",
			msg);
	} else {
		//       prt    next      key   mask gw
		kprintf("%06x %06x %-*.*s /%02d %08X %-*.*s %4d    %1d  %4d %4d %4d  %s %s\n",
			prt,
			prt->rt_next,
			IP_MAXDOT, IP_MAXDOT, ip2dot(bufnet, prt->rt_net),
			prt->rt_key & 0xffff,
			prt->rt_mask,
			IP_MAXDOT, IP_MAXDOT, ip2dot(bufgw,prt->rt_gw),
			prt->rt_metric,
			prt->rt_ifnum,
			prt->rt_ttl,
			prt->rt_refcnt,
			prt->rt_usecnt,
			prt->rt_func,
			msg);
	}
	return;
}




