/* gbl_mvs.c - define mvsgbl global data area */

#include <conf.h>
#include <kernel.h>

int	mvsonlywto = 0;			// 1 = doprnt() WTO, no fprintf
int	mvsmemlen = 0;			// mvsmem.c
void	*mvspmem = 0;			// mvsmem.c

Bool	shutxinu = FALSE;		// TRUE when shutting down MVS-Xinu

int	userpid = 0;			// sysinit, netstart

// static int esp;

// start contains (approximate) address of beginning of MVS-Xinu
// load module, set by s370/sys/initialize.c
int	start;

int	reboot = 0;			// non-zero after first boot
char	vers[100];			// Xinu version printed at startup
int	revision = REVISION;		// the revision level

/* These variables were defined in usrmain. */

int	sem;
int	pid1, pid2;
int	ptid;

#ifdef	NDEVS
struct	intmap	intmap[NDEVS];	/* interrupt dispatch table		*/
#endif

#ifdef	Ntty
struct  tty     tty[Ntty];	/* SLU buffers and mode control		*/
#endif








