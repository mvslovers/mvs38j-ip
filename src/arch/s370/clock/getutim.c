/* getutim.c - getutim */

#include <conf.h>
#include <kernel.h>
#include <sleep.h>
#include <date.h>
#include <network.h>
#include <io.h>

#define	DEBUG
#include <debug.h>

#ifdef	GETUTIM_FAKE
//  ----------------------------------------------------------------------------
//  mvstod1970	Return OK or SYSERR, when OK tod contains TOD relative to
//		Jan 1, 1970
//		The MVS TOD clock is a 64 bit value (unsigned long long)
//		normally relative to January 1, 1900.
//		Bits 0 - 31 (high order word) are approximately the
//		the number of seconds since Jan 1, 1900
//		The MVS TOD clock is assumed to be set to UTC
//  ----------------------------------------------------------------------------

int mvstod1970(unsigned long *tod) {

	__register(3) unsigned long *r3;	// 1st word of MVS TOD clock
	unsigned long 	now;

	// difference in seconds between Jan 1, 1970 and Jan 1, 1900
	unsigned long	adjust = (70*365 + 17) * 86400;

	__asm {
         LA    3,TOD              successful return = @ MVS TOD clock
         STCK  0(3)               retrieve MVS TOD clock (64 bits)
         BC    8,*+6              cc=0, leave R3 set
         SR    3,3                else, R3=0 (TOD clock error)
	}

	if (r3 == 0)
		return SYSERR;

	now = *r3;		// give compiler clue TOD changed
	now -= adjust;		// adjust to 1970
	*tod = now;		// return 1970-relative TOD
	return OK;

	__asm {
TOD      DC    D'0'
	}
}
#endif	// GETUTIM_FAKE

/*------------------------------------------------------------------------
 * getutim  --  obtain time in seconds past Jan 1, 1970, UTC (GMT)
 *
 *		GETUTIM_FAKE:
 *		Early in the MVS-Xinu port, we don't want to talk to a 
 *		time server, so we just use the	MVS TOD clock after 
 *		adjusting from 1900 to 1970
 *------------------------------------------------------------------------
 */
SYSCALL getutim(unsigned long *timvar) {

	int	dev;
	int	len;
	int	ret;
	u_long	now;
	u_long	utnow;
	char	*msg = "No time server response\n";

	trace("\nGETUTIM enter; timvar 0x%x\n", timvar);

#ifdef	GETUTIM_FAKE

	trace("GETUTIM fake; no time server queried\n");
	ret = mvstod1970(&now);

#else

	wait(clmutex);
	ret = OK;
	if (clktime < SECPERHR) {	/* assume small numbers invalid	*/
		if ((dev=open(UDP, TSERVER, ANYLPORT)) == SYSERR ||
		    control(dev,DG_SETMODE,DG_TMODE|DG_DMODE) == SYSERR) {
			panic("GETUTIM no time server response");
			ret = SYSERR;
		}
		write(dev, msg, 2);	/* send junk packet to prompt */
		if (read(dev,&utnow,4) != 4) {
			trace("GETUTIM syserr read\n");
			ret = SYSERR;
		} else
			clktime = net2xt( net2hl(utnow) );
		close(dev);
	}
	*timvar = clktime;
	signal(clmutex);

#endif

	trace("GETUTIM exit; *nix epoch seconds %d\n", ret);
	return(ret);
}




