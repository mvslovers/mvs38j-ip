/* sleep.h */

#ifndef	H_SLEEP_H
#define H_SLEEP_H

#include <clock.h>

/* Intel 8254-2 clock chip constants */
#define	CLKHZ		1190		/* kHz 				*/
#define	DIVIDER		(CLKHZ * 10)	/* 10ms interrupt rate		*/

#ifdef DEFINE_EXTERN
extern	int	clkruns;	/* 1 iff clock exists; 0 otherwise	*/
				/* Set at system startup.		*/
extern	int	clockq;		/* q index of sleeping process list	*/
extern	int	count6;		/* used to ignore 5 of 6 interrupts	*/
extern	int	count10;	/* used to ignore 9 of 10 ticks		*/
extern	long	clktime;	/* current time in secs since 1/1/70	*/
extern	int	clmutex;	/* mutual exclusion sem. for clock	*/
extern	int	*sltop;		/* address of first key on clockq	*/
extern	int	slnempty;	/* 1 iff clockq is nonempty		*/

extern	int	defclk;		/* >0 iff clock interrupts are deferred	*/
extern	int	clkdiff;	/* number of clock clicks deferred	*/
#endif /* DEFINE_EXTERN */

#ifdef	MVS_GEN_CLOCKSTATE
	void	clockstate(int, char *);// debug aid; pid | NULL, msg
#else
	#define		clockstate(x,y)
#endif
INTPROC	clkint();			// timer pop C code (currently unused MVS-Xinu)
int	clktest(void);			// clkinit.c (unused MVS-Xinu)
void	compute_delay(void);		// clkinit.c (unused MVS-Xinu)
void	dog_timeout(void);		// clkinit.c (unused MVS-Xinu)
int	insertd(int, int, int);		// pid, head, key
void	mvsclkinit(int);		// enable STIMER, specify sec/100
void	mvsclkoff();			// cancel STIMER (stop "clock")
SYSCALL	sleep10(int);			// suspend caller int/10 seconds
SYSCALL sleep(int);			// suspend caller int seconds
SYSCALL	unsleep(int);			// ready pid before sleep interval elapsed
#ifdef	MVS38J
int	wakeup();			// mark process(es) ready after interval elapsed
#else
INTPROC	wakeup();			// mark process(es) ready after interval elapsed
#endif

#endif /* H_SLEEP_H */








