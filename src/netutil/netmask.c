/* netmask.c - netmask */

#include <conf.h>
#include <kernel.h>
#include <network.h>

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  netmask  -  calculate the default mask for the given IP address
 *------------------------------------------------------------------------
 */

IPaddr	netmask(IPaddr net) {

	IPaddr		netpart, result;
	int		i;
	dbug(char	bufnet[IP_MAXDOT]);


	if (net == 0) {
		trace("NETMASK net 0.0.0.0 mask 0.0.0.0\n");
		return net;
	}

	dbug(ip2dot(bufnet, net));

	netpart = netnum(net);		// get network portion of IP

	/* check for net match (for subnets) */

	for (i=0; i<Net.nif; ++i) {
		if (  nif[i].ni_svalid 
		   && nif[i].ni_ivalid 
		   && nif[i].ni_net == netpart) {
			result = nif[i].ni_mask;
			trace("NETMASK IP %s default mask %08x (net match)\n",
				bufnet, result);
			return result;
		}
	}

	if (IP_CLASSA(net)) {
		result = hl2net(0xff000000);
		trace("NETMASK Class A network %s default mask %08x\n", 
			bufnet, result);
		return result;
	}

	if (IP_CLASSB(net)) {
		result = hl2net(0xffff0000);
		trace("NETMASK Class B network %s default mask %08x\n",
			bufnet, result);
		return result;
	}

	if (IP_CLASSC(net)) {
		result = hl2net(0xffffff00);
		trace("NETMASK Class C network %s default mask %08x\n",
			bufnet, result);
		return result;
	}

	result = ~0;
	trace("NETMASK Host IP %s mask %08x\n", bufnet, result);
	return result;
}




