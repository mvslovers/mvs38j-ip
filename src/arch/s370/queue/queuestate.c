/* queuestate.c - Debug Aid - display queue info */

#include <conf.h>
#include <kernel.h>
#include <q.h>
#include <clock.h>

/*------------------------------------------------------------------------
 * queuestate  --  display queue management information
 *------------------------------------------------------------------------
 */
void queuestate() {

	kprintf("QUEUESTATE EMPTY %d MININT %d MAXINT %d\n", 
		EMPTY, MININT, MAXINT);
	return;
}





