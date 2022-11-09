/* gbl_clock.c - define clock-related global data area */

#include <conf.h>
#include <kernel.h>
#include <sleep.h>

unsigned int     cpudelay;

#ifdef	RTCLOCK

int	clkruns = TRUE;		/* set TRUE iff clock exists		*/
    
/* real-time clock variables and sleeping process queue pointers	*/
    
int	count6;			/* counts in 60ths of a second 6-0	*/
int	count10;		/* counts in 10ths of second 10-0	*/
unsigned	long	ctr100;	/* counts in 100ths of second 0-INF	*/
int	clmutex;		/* mutual exclusion for time-of-day	*/
long	clktime;		/* current time in seconds since 1/1/70	*/
int     defclk;			/* non-zero, then deferring clock count */
int     clkdiff;		/* deferred clock ticks			*/
int     slnempty = FALSE;	/* FALSE if the sleep queue is empty	*/
int     *sltop = 0;		/* address of key part of top entry in	*/
				/* the sleep queue if slnonempty==TRUE	*/
int     clockq;			/* head of queue of sleeping processes  */
int	preempt;		/* preemption counter.	Current process */
				/* is preempted when it reaches zero;	*/
				/* set in resched; counts in ticks	*/
int	hhsec = 10;		// MVS STIMER interval, hundredths sec
int	mvsstimer = 0;		// 1 = STIMER active
void	*pclkint;		// address of clkint() for MVSTMPOP

// extern long	cnt100;		/* counts in 100ths of second 10-0	*/

#else	/* ------------------------------------------------------------	*/

int	clkruns = FALSE;	/* no clock configured; be sure sleep	*/
				/* doesn't wait forever			*/
#endif






