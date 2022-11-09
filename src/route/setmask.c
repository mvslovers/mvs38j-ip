/* setmask.c - setmask */

#include <conf.h>
#include <kernel.h>
#include <network.h>

//  PC-Xinu and MVS-Xinu differ here; see notes in "Install routes" section

#if	0					// TESTING TESTING TESTING
	#ifdef	MVS38J
		#define	SETMASK_DEVIATE	ifn	// MVS-Xinu defines as parm interface
	#else
		#define	SETMASK_DEVIATE	NI_LOCAL// PC-Xinu defines as loopback
	#endif
#else
	#define	SETMASK_DEVIATE NI_LOCAL	// TESTING TESTING TESTING
#endif

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  setmask - set the net mask for an interface, modify routes
 *------------------------------------------------------------------------
 */

int	setmask(unsigned ifn, IPaddr mask) {

	IPaddr		defmask;
	IPaddr		aobrc;		/* broadcast address */
	dbug(char	bufcall[FUNCSTR_BUFLEN]);
	dbug(char	bufip[IP_MAXDOT]);
	dbug(char	bufip2[IP_MAXDOT]);
	dbug(char	bufip3[IP_MAXDOT]);
	dbug(char	bufip4[IP_MAXDOT]);


	dbug(funcstr(bufcall, MY_CALLER));
	dbug(ip2dot(bufip, nif[ifn].ni_ip));
	trace("\nSETMASK enter; %s iface %d mask %x <<%s>>\n", 
		bufip, ifn, mask, bufcall);

	if (!nif[ifn].ni_ivalid)
		panic("SETMASK requires valid nif IP");

//  Remove previous routes for this interface

	if (nif[ifn].ni_svalid) {
		/* one set already-- fix things */
		trace("SETMASK deleting previous routes\n");
		rtdel(nif[ifn].ni_subnet, nif[ifn].ni_mask);
		rtdel(nif[ifn].ni_brc, ip_maskall);
		rtdel(nif[ifn].ni_subnet, ip_maskall);
		trace("SETMASK deleted previous routes\n");
	}

//  Set the interface's subnet and subnet mask

	nif[ifn].ni_subnet = nif[ifn].ni_ip & mask; 
	nif[ifn].ni_mask = mask;
	nif[ifn].ni_svalid = TRUE;	// interface subnet mask valid

//  Set broadcast IP address, either BSD style (host all 0s) 
//  or not (host all 1s)

	if (bsdbrc)
		nif[ifn].ni_brc = nif[ifn].ni_subnet;	// BSD broadcast
	else
		nif[ifn].ni_brc = nif[ifn].ni_subnet | (~mask);

	/* set network (not subnet) broadcast */

	defmask = netmask(nif[ifn].ni_ip);
	nif[ifn].ni_nbrc = nif[ifn].ni_ip | (~defmask);

	dbug(ip2dot(bufip2, nif[ifn].ni_subnet));
	dbug(ip2dot(bufip3, nif[ifn].ni_brc));
	dbug(ip2dot(bufip4, nif[ifn].ni_nbrc));
	trace("SETMASK subnet %s subnet broadcast %s net broadcast %s\n",
		bufip2, bufip3, bufip4);

//  Install routes

//  FIXME
//  MVS-Xinu: I'm at a bit of a loss here.  The PC-Xinu routes don't
//  make any sense to me, so I'm going with what I know but leaving
//  the way PC-Xinu did it as a trail of crumbs for those who follow.
//  Besides I may change my mind later; I can't help but think the
//  PC-Xinu approach might route output IP frames back through the
//  loopback adapter to catch frames destined for local processes.
//  But that's just speculation at this point, although something's
//  definitely up -- PC-Xinu ipsend sends a message to itself?!?!

	// PC-Xinu and MVS-Xinu define this route the same way
	trace("SETMASK add iface %d subnet route\n", ifn);
	rtadd(nif[ifn].ni_subnet, nif[ifn].ni_mask, nif[ifn].ni_ip,
		RTM_NOHOPS, ifn, RT_INF);

	/* subnet broadcast (host portion depends on BSD broadcast option) */

	// PC-Xinu defines this route as NI_LOCAL; MVS-Xinu as ifn
	trace("SETMASK MVS-Xinu add iface %d subnet broadcast route\n", SETMASK_DEVIATE);
	if (bsdbrc) {

		//  BSD broadcast (host address == all 0x0s)
		trace("SETMASK adding Berkeley broadcast (host all 0s) route\n");
		aobrc = nif[ifn].ni_subnet | ~nif[ifn].ni_mask;
		dbug(ip2dot(bufip2, aobrc));
		trace("SETMASK add bsdbrc route; %s\n", bufip2);
		rtadd(aobrc, ip_maskall, nif[ifn].ni_ip, RTM_NOHOPS,
			SETMASK_DEVIATE, RT_INF);
	} else {

		/* non-BSD (normal) broadcast (host address all 1's) */
		dbug(ip2dot(bufip2, nif[ifn].ni_brc));
		trace("SETMASK add non-BSD subnet broadcast (host all 1s) %s\n", bufip2);
		rtadd(nif[ifn].ni_brc, ip_maskall, nif[ifn].ni_ip, RTM_NOHOPS,
			SETMASK_DEVIATE, RT_INF);
	}

	/* subnet broadcast (host portion all 0's) */

	// PC-Xinu defines this route as NI_LOCAL; MVS-Xinu as ifn
	dbug(ip2dot(bufip3, nif[ifn].ni_subnet));
	trace("SETMASK MVS-Xinu add iface %d subnet broadcast route (host all 0s) %s\n", 
		SETMASK_DEVIATE, bufip3);
	rtadd(nif[ifn].ni_subnet, ip_maskall, nif[ifn].ni_ip, RTM_NOHOPS,
		SETMASK_DEVIATE, RT_INF);

	trace("\nSETMASK routes at exit <<%s>>\n", bufcall);
	dbug(rtdump(ALL_ROUTES));

	trace("SETMASK exit; OK iface %d <<%s>>\n", ifn, bufcall);
	return OK;
}



