/* rtinit.c - rtinit */

#include <conf.h>
#include <kernel.h>
#include <sleep.h>
#include <network.h>

#define	DEBUG
#include <debug.h>

struct	rtinfo	Route;
struct	route	*rttable[RT_TSIZE];

/*------------------------------------------------------------------------
 *  rtinit  -  initialize the routing table
 *------------------------------------------------------------------------
 */
void rtinit(void) {

	int i;

	trace("\nRTINIT enter\n");
	for (i=0; i<RT_TSIZE; ++i)
		rttable[i] = 0;
	Route.ri_bpool = mkpool(sizeof(struct route), RT_BPSIZE);
	Route.ri_valid = TRUE;
	Route.ri_mutex = screate(1);
	Route.ri_default = NULL;
	trace("RTINIT exit\n");
}


