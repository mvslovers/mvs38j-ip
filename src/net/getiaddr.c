/* getiaddr.c - getiaddr */

#include <conf.h>
#include <kernel.h>
#include <network.h>

#define	DEBUG
#include <debug.h>

int rarpsend(int);

/*------------------------------------------------------------------------
 *  getiaddr  -  obtain this interface's complete address (IP address)
 *------------------------------------------------------------------------
 */

IPaddr getiaddr(int inum) {

	struct netif	*pif;

	trace("GETIADDR enter; interface %d\n", inum);
	if (inum == NI_LOCAL || inum < 0 || inum >= Net.nif) {
		trace("GETIADDR syserr\n");
		return (IPaddr)SYSERR;
	}
	pif = &nif[inum];
	wait (rarpsem);
	if (!pif->ni_ivalid && pif->ni_state == NIS_UP) {
		trace("GETIADDR call rarpsend to find interface %d IP addr\n", inum);
		(void) rarpsend(inum);
	}
	signal(rarpsem);

	if (!pif->ni_ivalid) {
		trace("GETIADDR syserr; ni_ivalid\n");
		return (IPaddr)SYSERR;
	}
	trace("GETIADDR exit; interface %d IPaddr 0x%x\n", inum, pif->ni_ip);
	return pif->ni_ip;
}






