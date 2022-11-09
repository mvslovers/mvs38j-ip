/* getname.c - getname, getiname */

#include <conf.h>
#include <kernel.h>
#include <network.h>

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  getname  -  get name of this host and place it where specified
 *------------------------------------------------------------------------
 */
SYSCALL
getname(char *nam)
{
	return getiname(NI_PRIMARY, nam);
}

/*------------------------------------------------------------------------
 *  getiname  -  get name of this host and place it where specified
 *------------------------------------------------------------------------
 */
int getiname(int ifn, char *nam) {

	struct	netif *intf;
	IPaddr	myaddr;
	char	*p;

	trace("GETINAME enter; ifn %d buf 0x%x\n", ifn, nam);
	if (ifn < 0 || ifn >= Net.nif) {
		trace("GETINAME syserr\n");
		return SYSERR;
	}
	intf = &nif[ifn];
	*nam = NULLCH;
	if (!intf->ni_nvalid) {
		myaddr = getiaddr(ifn);
		if (ip2name(myaddr, intf->ni_name) == (char *)SYSERR) {
			trace("GETINAME syserr ip2name\n");
			return SYSERR;
		}
		intf->ni_nvalid = TRUE;
	}
	for (p=intf->ni_name ; p && *p != NULLCH; )
		*nam++ = *p++;
	*nam = NULLCH;
	trace("GETINAME return OK\n");
	return OK;
}

