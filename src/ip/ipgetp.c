/* ipgetp.c - ipgetp */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <proc.h>

#define	DEBUG
#include <debug.h>

static	int	ifnext = NI_LOCAL;

/*------------------------------------------------------------------------
 * ipgetp  --  choose next IP input queue and extract a packet
 *------------------------------------------------------------------------
 */

struct ep *ipgetp(int *pifnum) {

	struct	ep	*pep;
	int		i;


	trace("IPGETP entered\n");
	recvclr();	/* make sure no old messages are waiting */
	while (TRUE) {
		for (i=0; i < Net.nif; ++i, ++ifnext) {
			if (ifnext >= Net.nif)
				ifnext = 0;
			if (nif[ifnext].ni_state == NIS_DOWN) {
				trace("IPGETP interface %d down; continue\n", ifnext);
				continue;
			}
			if (pep = NIGET(ifnext)) {
				*pifnum = ifnext;
				trace("\nIPGETP return pep 0x%x from ifnum %d\n", pep, ifnext);
				return pep;
			}
		}
		ifnext = receive();
		trace("IPGETP received message (interface index) 0x%x\n", ifnext);
	}
	/* can't reach here */
}




