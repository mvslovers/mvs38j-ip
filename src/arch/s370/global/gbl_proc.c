/* gbl_proc.c - define process global data area */

#include <conf.h>
#include <kernel.h>

#ifdef DEFINE_EXTERN
struct	pentry	proctab[NPROC]; /* process table			*/
int	nextproc;		/* next process slot to use in create	*/
int	numproc;		/* number of live user processes	*/
int	currpid;		/* id of currently running process	*/
int	lastpid;		/* id of last running process		*/
int	nulldisppid;		// nulldisp process' pid
unsigned long currSP;	/* REAL sp of current process */
char	*procstatdesc[] = { 	// matches proc.h defines
	"CURRENT  ", 	// 1
	"FREE     ", 	// 2
	"READY    ", 	// 3
	"RECEIVE  ", 	// 4
	"SLEEP    ", 	// 5
	"SUSPENDED", 	// 6
	"WAITING  ", 	// 7
	"TIME-RECV",	// 8
	"FINISHED " };	// 9 bogus, debug usage = PRDEAD in killpid()
#endif


