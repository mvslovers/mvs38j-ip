/* clkinit.c - clkinit, updateleds, dog_timeout */

#include <conf.h>
#include <kernel.h>
#include <sleep.h>
#include <clock.h>
#include <proc.h>	// clkinit() REQUIRES process mgt be active <---

#ifdef	RTCLOCK			// whole source conditional on this

/*
 *------------------------------------------------------------------------
 * clkinit - initialize the clock and sleep queue (called at startup)
 *------------------------------------------------------------------------
 */
int clkinit(void) {

	clockq = newqueue();		// alloc q element for clock mgt
	preempt = QUANTUM;		// initial time quantum
	compute_delay();		// i386 cpudelay setup

#if 0					// OBSOLETE code
	int clkpid = create(clkint,INITSTK,INITPRIO,"TimerPop",INITARGS);
	resume(clkpid);			// start clkint, let it hang
	kprintf("CLKINIT RESUMED ******\n");
#endif

	mvsclkinit(QUANTUM);		// request periodic timer pops
	return OK;
}

void compute_delay(void) {
#ifndef MVS38J
	extern unsigned int	cpudelay;
	unsigned int		tcount;
	unsigned int		dlow, dhigh;

	cpudelay = dlow = 1;
	tcount = 0;

	/* find high bound for cpudelay */

	while (tcount < 1190) {
		dlow = cpudelay;
		dhigh = cpudelay = 2 * cpudelay;

		delay(100);	/* 1 ms arch/i386/asm/startup.s */
	
	}

	while (dlow < dhigh) {

		cpudelay = (dlow + dhigh) / 2;

		delay(100);	/* 1 ms */

		if (tcount == CLKHZ)
			break;		/* exact match */
		if (tcount < CLKHZ)	/* too small */
			cpudelay = dlow = cpudelay + 1;
		else	/* too big */
			dhigh = cpudelay - 1;

	}
#endif /* MVS38J */
}

#ifndef	MVS38J
/*
 * dog_timeout -- called when the watchdog timer determines that
 * interrupts have been disabled for too long
 */
void dog_timeout(void)
{
    STATWORD ps;

    disable(ps);
    kprintf("\n\nWatchdog timeout!! -- interrupts disabled too long.\n");
    ret_mon();
    restore(ps);
    return;
}

int clktest(void)
{
	kprintf("from clkint, ctr100 %d\n", ctr100);
}
#endif /* MVS38J */
#endif /* RTCLOCK */












