/* proc.h - isbadpid */

#ifndef H_PROC_H
//------------------------------------------------------------------------------proc.h
#define H_PROC_H		// only generate once

#include <setjmp.h>		// Standard C header

// WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
// The currpid field may only be updated by resched(); see comments there
// WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING

/* process table declarations and defined constants			*/

#ifndef	NPROC				/* set the number of processes	*/
#define	NPROC		30		/*  allowed if not already done	*/
#endif

#ifndef	_NFILE
#define _NFILE		20		/* # of files allowed */
#endif

#define	FDFREE		-1		/* free file descriptor */

/* process state constants */

#define	PRCURR		1		/* process is currently running	*/
#define	PRFREE		2		/* process slot is free		*/
#define	PRREADY		3		/* process is on ready queue	*/
#define	PRRECV		4		/* process waiting for message	*/
#define	PRSLEEP		5		/* process is sleeping		*/
#define	PRSUSP		6		/* process is suspended		*/
#define	PRWAIT		7		/* process is on semaphore queue*/
#define	PRTRECV		8		/* process is timing a receive	*/
#define	PRDEAD		9		// debug, prevents pentry reuse
					// and procstate() reports it
					// set in killpid()

/* miscellaneous process definitions */

#define	PNMLEN		16		/* length of process "name"	*/

#define	NULLPROC	0		/* id of the null process; it	*/
					/*  is always eligible to run	*/

#define	BADPID		-1		/* used when invalid pid needed	*/

#define MAXPRIORITY	500		// NullDisp priority

#define	isbadpid(x)	(x<=0 || x>=NPROC)
#define proclock()	mvslock(KPROC, __FILE__)
#define procunlock()	mvsunlock(KPROC, __FILE__)

#ifdef	MVS38J
	#define	getpid() currpid	// no need to call function in MVS-Xinu
#else
	SYSCALL	getpid(void);		// replaced by getpid macro
#endif

/* process table entry */

struct	pentry	{
	char	pname[PNMLEN];		// process name

	// +0x10

	char	pstate;			// process state: PRCURR, etc
	char	pflag;			// misc status flags (pefenum)
	char	phasmsg;		// nonzero iff pmsg is valid
	Bool	ptcpumode;		// proc is in TCP urgent mode
	int	pprio;			// process priority
	int	psem;			// semaphore if process waiting	*/
	int	ppid;			// process id

	// +0x20

	WORD	pmsg;			// message sent to this process
	WORD	pnxtkin;		// next-of-kin notified of death
	WORD	paddr;			// initial code address
	int	pargs;			// initial number of arguments

	// +0x30	+48

	WORD	lock;			// Compare and Swap lock word
	void	*penv;			// Systems/C environment ptr
	int	nlocks;			// # locks held by this process
	int	reserved;		// unused, alignment

	// +0x40	+64

	WORD	gpr[16];		// S/370 R0:R15 (procsw)

	// +0x80	+128		procsw() REQUIRES fpr follow gpr

	WORD	fpr[8];			// S/370 FP0, FP2, FP4, FP6

	// +0xA0	+160

	WORD	topsa[18];		// process' first savearea
	WORD	topspare[2];		// alignment, currently unused

	// +0xF0	+240

	WORD	reschwk[4];		// four words for resched() 

#if 0					// Future:
	void	*pascb;			// MVS ASCB ptr or NULL
	void	*ptcb;			// MVS TCB ptr or NULL
	WORD	ecb;			// MVS ECB
	WORD	cr[16];			// S/370 Control Registers
#endif
// Obsolete:
	STATWORD pirmask;		/* saved interrupt mask		*/
	unsigned long	pesp;		/* saved stack pointer		*/
	int	pbase;			/* base of run time stack	*/
	int	pstklen;		/* stack length			*/
	WORD	plimit;			/* lowest extent of stack	*/
	short	pdevs[2];		/* devices to close upon exit	*/
	int	fildes[_NFILE];		/* file - device translation	*/
};
typedef struct pentry pent;
typedef struct pentry pentry;
enum	pefenum {			// pentry flag: pflag;
	PEFCREATE	= 0x80,		// environment created (mvsenv.c)
	PEFINIT		= 0x40,		// environment begun (mvsenv.c)
	PEFFINI		= 0x20		// process completed (resched.c)
};

#ifdef DEFINE_EXTERN
extern	struct	pentry proctab[];
extern	int	numproc;		/* currently active processes	*/
extern	int	nextproc;		/* search point for free slot	*/
extern	int	currpid;		/* currently executing process	*/
extern	int	lastpid;		/* currently executing process	*/
extern	int	nulldisppid;		// nulldisp process' pid
extern	jmp_buf	nulljump;		// nulldisp setjmp/longjmp env
extern	char	*procstatdesc[];	// PRstatus text descriptions
#endif

#define	ENVCREATE	1		// create Systems/C environment
#define ENVDESTROY	2		// destroy Systems/C environment
#define	ENVBEGIN	3		// start using Systems/C environment

#define	FUNCSTR_BUFLEN	40		// length of funcstr output buffer
#define	MY_CALLER	((void *)2)	// funcstr arg for 'who called me?'

void	callinfo(char *);		// message
SYSCALL	create(PROCESS (*)(), int, int, char *, int, ...);
char	*funchist(char *, int, void *);	// function call history
char	*funcstr(char *, void *);	// function name as string
SYSCALL	killpid(int);
SYSCALL	newpid(void);
PROCESS	nulldisp();			// resched assist process
void	nullkill(pentry *, int);	// nulluser process mgt interface
void	procinit();
void	procload(void *);
void	procstate(int, char *);		// pid | NPROC, msg
void	procsave(void *);
void	procsw(void *, void *);
int	ready(int, int);
int	resched();
SYSCALL resume(int);
PROCESS	sample();
SYSCALL	suspend(int);

//------------------------------------------------------------------------------proc.h
#endif /* H_PROC_H */



