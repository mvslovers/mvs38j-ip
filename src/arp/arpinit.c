/* arpinit.c - arpinit */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  arpinit  -  initialize data structures for ARP processing
 *------------------------------------------------------------------------
 */
void arpinit() {
	int	i;

	rarpsem = screate(1);
	rarppid = BADPID;

	for (i=0; i<ARP_TSIZE; ++i)
		arptable[i].ae_state = AS_FREE;
	trace("ARPINIT complete\n");
}


