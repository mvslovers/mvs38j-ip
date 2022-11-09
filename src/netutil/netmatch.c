/* netmatch.c - netmatch */

#include <conf.h>
#include <kernel.h>
#include <network.h>

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  netmatch  -  Is "dst" on "net"?
 *------------------------------------------------------------------------
 */

//	dst	destination IP
//	net	network IP
//	mask	network mask
//	islocal	TRUE = iface from which frame originated == NI_LOCAL

Bool	netmatch(IPaddr dst, IPaddr net, IPaddr mask, Bool islocal) {

	Bool		ans;
	dbug(char	bufdst[IP_MAXDOT]);
	dbug(char	bufnet[IP_MAXDOT]);
	dbug(char	bufcall[FUNCSTR_BUFLEN]);


	dbug(ip2dot(bufdst, dst));
	dbug(ip2dot(bufnet, net));
	dbug(funcstr(bufcall, MY_CALLER));

	if ((dst & mask) != (net & mask)) {
		trace("NETMATCH exit FALSE <<%s>>; dest %s NOT ON network %s mask %08x\n",
			bufcall, bufdst, bufnet, mask);
		return FALSE;
	}
	/*
	 * local srcs should only match unicast addresses (host routes)
	 */
	if (islocal) {
		trace("NETMATCH -- local\n");
		if (isbrc(dst) || IP_CLASSD(dst)) {
			ans = mask != ip_maskall;
			trace("NETMATCH exit %s <<%s>>; Q: dest %s on network %s mask %08x\n",
				ans ? "TRUE" : "FALSE", bufcall, bufdst, bufnet, mask);
			return ans;
		}
	}

	trace("NETMATCH exit TRUE <<%s>>; dest %s ON network %s mask %08x\n",
		bufcall, bufdst, bufnet, mask);
	return TRUE;
}




