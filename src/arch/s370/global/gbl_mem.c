/* gbl_mem.c - define memory global data area */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <mark.h>
#include <ports.h>
#include <bufpool.h>

#ifdef DEFINE_EXTERN
mblock	memlist;		// list of free memory blocks
char	*maxaddr;		/* max memory address 			*/

int	*marks[MAXMARK];
int	nmarks;
int	mkmutex;

#ifdef	MEMMARK
MARKER	ptmark;
#endif
struct	ptnode	*ptfree;		/* list of free queue nodes	*/
struct	pt	ports[NPORTS];
int	ptnextp;

struct	bpool	bptab[NBPOOLS];
int	nbpools;
#ifdef	MEMMARK
MARKER	bpmark;				/* self initializing mark	*/
#endif
#endif


