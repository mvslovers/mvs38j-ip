/* name2ip.c - name2ip */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <mem.h>
#include <io.h>

/*------------------------------------------------------------------------
 *  name2ip  -  return the IP address for a given DNS name
 *------------------------------------------------------------------------
 */

IPaddr	name2ip(char *nam) {

	Bool	isnum;
	char	*p;


	isnum = TRUE;
	for (p=nam; *p; ++p) {
		isnum &= ((*p >= '0' && *p <= '9') || *p == '.');
		if (!isnum)
			break;
	}
	if (isnum)
		return dot2ip(nam);
	return resolve(nam);
}

