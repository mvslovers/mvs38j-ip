/* udpinit.c - udpinit */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>
#include <ports.h>
#include <sem.h>

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  udpinit  -  initialize UDP protocol services
 *------------------------------------------------------------------------
 */
int udpinit(void) {

	int	i;

	/* initialize UDP ports */

	for (i=0 ; i<UPPS ; i++)
		upqs[i].up_valid = FALSE;

	udpmutex = screate(1);
	trace("UDPINIT complete\n");
	return OK;
}
 


