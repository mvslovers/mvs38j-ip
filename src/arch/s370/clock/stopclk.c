/* stopclk.c - stopclk */

// split out from ssclock.c

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>

#ifdef	RTCLOCK

/*------------------------------------------------------------------------
 *  stopclk  --  put the clock in defer mode
 *------------------------------------------------------------------------
 */
stopclk()
{
	defclk++;
}

#endif

