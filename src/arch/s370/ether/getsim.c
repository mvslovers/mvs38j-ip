/* getsim.c - Othernet initialization */

#include <conf.h>
#include <kernel.h>
#include <network.h>

#define	DEBUG
#include <debug.h>

#if Noth > 0
	static	int	getsim();
	static	int	itod[] = { NI_LOCAL, ETHER, OTHER1, OTHER2 };
	static	IPaddr	iftoip[] = { {0, 0, 0, 0}, {0, 0, 0, 0},
			{130, 10, 59, 0}, {209, 6, 36, 0} };
#else
	//  The loopback netif does not have an associated Xinu device
	static	int	itod[] = { -1, ETHER };
#endif


//  ----------------------------------------------------------------------------
//  Othernet initialization (if Othernet devices configured)
//  ----------------------------------------------------------------------------

#if Noth > 0
int getsim() {

	char *prompt = "Othernet number? ";
	char *badval = "Choose '1' or '2'.\n";
	char answer[2];

	trace("GETSIM (othernet) entered!!!!!!!!!!!!!!!\n");

start:
	write(CONSOLE, prompt, strlen(prompt));
	read(CONSOLE, answer, sizeof(answer));
	if (answer[0] == '1' && answer[1] == '\n')
		return OTHER1;
	if (answer[0] == '2' && answer[1] == '\n')
		return OTHER2;
	write(CONSOLE, badval, strlen(badval));
	goto start;
}
#endif /* Noth > 0 */

