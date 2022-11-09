/* gbl_ctc.c - define CTC global data area */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <ctc.h>

#ifdef DEFINE_EXTERN

#ifdef	Neth
struct	ctblk	ctcdev[Neth];		// CTC device blocks
#endif

int	ctcecb = 0;			// CTC I/O process ECB
#endif






