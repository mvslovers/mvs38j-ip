/* rthash.c - rthash */

#include <conf.h>
#include <kernel.h>
#include <network.h>

#undef	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  rthash  -  compute the hash for "net"
 *------------------------------------------------------------------------
 */

int	rthash(IPaddr net) {

	int		bc = IP_ALEN;	/* # bytes to count	*/
	unsigned int	hv = 0;		/* hash value		*/
	int		result;
	dbug(char	bufip[IP_MAXDOT]);
	dbug(char	bufcall[FUNCSTR_BUFLEN]);


	dbug(funcstr(bufcall, MY_CALLER));
	dbug(ip2dot(bufip, net));

	if      (IP_CLASSA(net)) bc = 1;
	else if (IP_CLASSB(net)) bc = 2;
	else if (IP_CLASSC(net)) bc = 3;
	else if (IP_CLASSD(net)) {
			result = ((net >> 24) & 0xf0) % RT_TSIZE;
			trace("RTHASH IP %s hash %d\n", bufip, result);
			return result;
		}

	while (bc--)
		hv += ((char *)&net)[bc] & 0xff;

	result = hv % RT_TSIZE;

	trace("RTHASH IP %s hash %d\n", bufip, result);
	return result;
}


