/* resolve.c - resolve */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <string.h>	// standard C header

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  resolve  -  do shorthand DNS name resolution
 *------------------------------------------------------------------------
 */

IPaddr	resolve(char *nam) {

	IPaddr	ip;
	char	myname[64];
	char	name_buf[100];
	char	*name_suffix;


	/* if it ends in a 'dot', remove it and try exactly once */

	if (nam[strlen(nam)-1] == '.') {
		strcpy(name_buf, nam);
		name_buf[strlen(name_buf)-1] = NULLCH;
		return gname(name_buf);
	}
	if (getname(myname) == SYSERR) {
		trace("RESOLVE syserr; getname\n");
		return (IPaddr)SYSERR;
	}
	name_suffix = myname;

	/* tack on successively smaller suffixes of MY name */

	while (name_suffix = index(++name_suffix, '.')) {
		sprintf(name_buf, "%s%s", nam, name_suffix);
		if ((ip = gname(name_buf)) != (unsigned) SYSERR)
			return ip;
	}
	return gname(nam);		/* try the original	*/
}

