/* nifdump.c - nifdump */

//  TODO	finish writing this; it doesn't show the whole struct

#include <conf.h>
#include <kernel.h>
#include <network.h>

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 * nifdump - pretty-print a network interface (struct netif)
 *------------------------------------------------------------------------
 */

int nifdump(char *msg, struct netif *pnif, int dump) {

	short		hwtype, prtype, op;
	int		i;
	char		*ptemp;
	void		*pjunk;
	char		bufip[IP_MAXDOT];
	char		bufnet[IP_MAXDOT];
	char		bufsubnet[IP_MAXDOT];
	char		bufmask[IP_MAXDOT];
	char		bufbrc[IP_MAXDOT];
	char		bufnbrc[IP_MAXDOT];
	char		bufhwa[NI_MAXHWA * 3];	// xx: + null terminator
	char		bufhwb[NI_MAXHWA * 3];
	char		unknown[] = "<unknown>";
	dbug(char	bufcall[FUNCSTR_BUFLEN]);


	dbug(funcstr(bufcall, MY_CALLER));
	trace("\nNIFDUMP network interface 0x%x %s <<%s>>\n", pnif, msg, bufcall);
	switch (pnif->ni_state) {
		case NIS_UP:		ptemp = "UP"; break;
		case NIS_DOWN:		ptemp = "DOWN"; break;
		case NIS_TESTING:	ptemp = "TESTING"; break;
		default:		ptemp = "INVALID STATE";
	}
	if (strlen(pnif->ni_name) == 0)
		pjunk = (void *)&unknown;
	else
		pjunk = &pnif->ni_name;
	kprintf("NIF: Domain %s State %s\n", pjunk, ptemp);

	ip2dot(bufip, pnif->ni_ip);
	ip2dot(bufnet, pnif->ni_net);
	ip2dot(bufsubnet, pnif->ni_subnet);
	ip2dot(bufmask, pnif->ni_mask);
	ip2dot(bufbrc, pnif->ni_brc);
	ip2dot(bufnbrc, pnif->ni_nbrc);
	kprintf("NIF: IP %s Net %s Subnet %s Mask %s\n", bufip, bufnet, bufsubnet, bufmask);
	kprintf("NIF: Broadcast %s Net broadcast %s\n", bufbrc, bufnbrc);

	switch (pnif->ni_hwtype) {
		case AR_HARDWARE:
			ptemp = "ethernet";
			break;
		default:
			ptemp = "unknown hardware";
	}
	kprintf("NIF: MTU %d Hardware %s %d\n", pnif->ni_mtu, ptemp, pnif->ni_hwtype);

	hwa2string(bufhwa, &pnif->ni_hwa);
	hwa2string(bufhwb, &pnif->ni_hwb);
	kprintf("NIF: Hardware address %s\n", bufhwa);
	kprintf("NIF: Broadcast address %s\n", bufhwb);

	kprintf("NIF: VALID: IP %s Domain %s Subnet %s\n", 
		(pnif->ni_ivalid) ? "yes" : "no",
		(pnif->ni_nvalid) ? "yes" : "no",
		(pnif->ni_svalid) ? "yes" : "no");

	kprintf("NIF: Xinu device descriptor %d\n", pnif->ni_dev);
	kprintf("NIF: Queue indices: IP in %d Device out %d\n", pnif->ni_ipinq, pnif->ni_outq);
	kprintf("NIF: Device description: %s\n", pnif->ni_descr);

	if (dump)
		snap("NIFDUMP nif", (void *)pnif, sizeof(struct netif));

	trace("\n");
	return OK;
}

