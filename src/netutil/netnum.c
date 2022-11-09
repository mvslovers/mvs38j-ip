/* netnum.c - netnum */

#include <conf.h>
#include <kernel.h>
#include <network.h>

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  netnum  -  compute the network portion of a given IP address
 *------------------------------------------------------------------------
 */

IPaddr	netnum(IPaddr ipa) {

	IPaddr	result;
	IPaddr	mask;
	dbug(char	bufip[IP_MAXDOT]);
	dbug(char	bufnet[IP_MAXDOT]);
	dbug(char	bufcall[FUNCSTR_BUFLEN]);


	dbug(funcstr(bufcall, MY_CALLER));

	mask = ~0;
	if (IP_CLASSA(ipa)) mask = hl2net(0xff000000);
	if (IP_CLASSB(ipa)) mask = hl2net(0xffff0000);
	if (IP_CLASSC(ipa)) mask = hl2net(0xffffff00);

	result = ipa & mask;
	trace("NETNUM IP %s resides in network %s <<%s>>\n", 
		ip2dot(bufip, ipa), 
		ip2dot(bufnet, result),
		bufcall);
	return result;
}

