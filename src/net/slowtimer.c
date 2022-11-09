/* slowtimer.c - slowtimer */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sleep.h>
#include <date.h>
#include <network.h>

#ifdef NETSTART_ARP
#include <arp.h>
#endif

#ifdef NETSTART_IP
#include <ip.h>
#endif

#ifdef NETSTART_RIP
#include <rip.h>
#endif

#ifdef OSPF
#include <ospf.h>
#endif /*OSPF*/

#define	STGRAN	1		/* Timer granularity (delay) in seconds	*/

/*------------------------------------------------------------------------
 * slowtimer - handle long-term periodic maintenance of network tables
 *------------------------------------------------------------------------
 */
PROCESS slowtimer(void) {
	unsigned long	lasttime, now;	/* prev and current times (secs)*/
	int		delay;		/* actual delay in seconds	*/

	signal(Net.sema);

	gettime(&lasttime);
	while (1) {
		sleep(STGRAN);
		gettime(&now);
		delay = now - lasttime;
		if (delay <= 0 || delay > 4*STGRAN)
			delay = STGRAN;	/* likely clock reset */
		lasttime = now;
#ifdef NETSTART_ARP
		arptimer(delay);
#endif
#ifdef NETSTART_IP
		ipftimer(delay);
#endif
#ifdef NETSTART_RIP
		rttimer(delay);
#endif
#ifdef OSPF
		ospftimer(delay);
#endif /* OSPF */
	}
}

