/* mvsabend.c - MVS-specific ABEND macro support */

#include <conf.h>
#include <kernel.h>

/*------------------------------------------------------------------------
 * mvsabend - issue goodbye message, ABEND
 *------------------------------------------------------------------------
 */
void mvsabend(char *who, int abend) {
	__register(6)   void * r6 = abend;

	kprintf("MVSABEND from %s ABEND code %d\n", who, abend);
	__asm {
         ABEND (6),DUMP
	}

} /* mvsabend */

