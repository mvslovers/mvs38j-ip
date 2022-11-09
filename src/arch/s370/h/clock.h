/* clock.h - time management */
#ifndef H_CLOCK_H
#define H_CLOCK_H
//------------------------------------------------------------------------------clock.h
#include <sleep.h>

#ifdef DEFINE_EXTERN
extern	unsigned int     cpudelay;
extern	int	clkruns;		/* set TRUE iff clock exists		*/

/* real-time clock variables and sleeping process queue pointers	*/

extern	int	count6;			/* counts in 60ths of a second 6-0	*/
extern	int	count10;		/* counts in 10ths of second 10-0	*/
extern	int	clmutex;		/* mutual exclusion for time-of-day	*/
extern	long	clktime;		/* current time in seconds since 1/1/70	*/
extern	int     defclk;			/* non-zero, then deferring clock count */
extern	int     clkdiff;		/* deferred clock ticks			*/
extern	int     slnempty;		/* FALSE if the sleep queue is empty	*/
extern	int     *sltop;			/* address of key part of top entry in	*/
					/* the sleep queue if slnonempty==TRUE	*/
extern	int     clockq;			/* head of queue of sleeping processes  */
extern	int	preempt;		/* preemption counter.	Current process */
					/* is preempted when it reaches zero;	*/
					/* set in resched; counts in ticks	*/
extern	int	hhsec;			// MVS STIMER interval; hundredths sec
extern	int	mvsstimer;		// 1=STIMER outstanding
extern	void	*pclkint;		// address of clkint()

#endif /* DEFINE_EXTERN */

//------------------------------------------------------------------------------clock.h
#endif /* H_CLOCK_H */



