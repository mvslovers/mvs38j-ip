/* mvs38j.h - MVS38j header */
#ifndef H_MVS38J_H
//------------------------------------------------------------------------------mvs38j.h
#define H_MVS38J_H			// only generate header once

#include <xgd.h>			// MVS38j Xinu Global Data

typedef	unsigned short	UHWORD;
typedef	unsigned short	USHORT;
typedef	short		HWORD;
typedef	unsigned char	BYTE;

#define	MVSPAGESIZE	4096		// size of MVS page frame
#define	MVSMAXIOSIZE	0xffff		// max MVS38j I/O size

					// arch/s370/s370c/intr.c:
#define	SUPERVISOR	0		// s370state() supervisor state
#define	PROBLEM		1		// s370state() problem state

#ifdef DEFINE_EXTERN
extern	int	mvsonlywto;		// 1 = doprnt() WTO only, no fprintf
extern	int	mvsmemlen;		// mvsmem.c memory length allocated
extern	void	*mvspmem;		// mvsmem.c @ memory allocation
extern	Bool	shutxinu;		// TRUE @ MVS-Xinu shutdown
extern	int	userpid;		// defmvs, sysinit, netstart
extern	static int esp;
extern	int	start;
extern	int	reboot;			// non-zero after first boot
extern	char	vers[100];		// Xinu version printed at startup
extern	int	revision;		// the revision level
/* These variables were defined in usrmain. */
extern	int	sem;
extern	int	pid1, pid2;
extern	int	ptid;
#ifdef	NDEVS
extern	struct	intmap	intmap[NDEVS];	/* interrupt dispatch table		*/
#endif
#ifdef	Ntty
extern	struct  tty     tty[Ntty];	/* SLU buffers and mode control		*/
#endif
#endif /* DEFINE_EXTERN */

#define	disable(w)	mvsdisable(w, __FILE__)
#define	enable()	mvsenable(__FILE__)
#define	restore(w)	mvsrestore(w, __FILE__)
#define	s370state(r)	mvsstate(r, __FILE__)
#define	mvskey0()	__asm {  SPKA 0*16               SYSTEM protection key }
#define	mvskey8()	__asm {  SPKA 8*16               USER protection key   }
#define	asmnop()	__asm {  NOPR 0                  null routine          }

#define	DONTSWAP	1		// mvsevent parm
#define	OKSWAP		2		// mvsevent parm

void	*findsgd();			// locate MVS38j TCP/IP SGD
void	*findxgd();			// locate MVS-Xinu XGD
void	iohook();			// CHE APP intersection with resched
void	mvsabend(char *, int);		// caller msg, abend code
int	mvsauth(int);			// 1=APF auth on, 0=off
void	mvsdisable(STATWORD, char *);	// disable S/370 interrupts
void	*mvsdoze(int);			// WTO buffer kludge support
void	mvsenable(char *);		// enable S/370 interrupts
void	*mvsenv(int, pentry *);		// Systems/C environment manager
void	mvsevent(int);			// SYSEVENT interface
void	mvsrestore(STATWORD, char *);	// restore saved interrupt state
void	mvsstate(int, char *);		// sets S/370 supervisor or problem state
void	mvswait(void *);		// ECB addr
void	*mvswaitlist(int, int, void *, ...);	// wait for multiple events
void	mvswto(unsigned char *, int);	// issue WTO from string
void	stackstate(void *, char *);	// savearea ptr or NULL, message w/o \n
int	testauth();			// TESTAUTH macro
#ifdef	MVS_LOCKMGR
	void	mvslock(int, char *);		// seize lock[n]
	void	mvsunlock(int, char *);		// release lock[n]
#else
	#define	mvslock(x,y)	asmnop()	// don't generate mvslock call code	
	#define	mvsunlock(x,y)	asmnop()	// don't generate mvsunlock call code
#endif

//  Tell Systems/C which functions are to run under their own environments
//  that were created by mvsenv(ENVCREATE,ppentry)
//  It is important that this list be accurate, or Systems/C will incorrectly
//  initialize the PROCESS function (expect an early abend in the function
//  if you get it wrong, typically in the function's entry prologue)

#pragma	prolkey(clkint,		"DCALL=SUPPLIED,ISTK=16384")
#pragma	prolkey(ctci_ioread,	"DCALL=SUPPLIED,ISTK=16384")
#pragma	prolkey(ctci_iowrite,	"DCALL=SUPPLIED,ISTK=16384")
#pragma	prolkey(echod,		"DCALL=SUPPLIED,ISTK=16384")
#pragma	prolkey(egp,		"DCALL=SUPPLIED,ISTK=16384")
#pragma	prolkey(fingerd,	"DCALL=SUPPLIED,ISTK=16384")
#pragma	prolkey(igmp_update,	"DCALL=SUPPLIED,ISTK=16384")
#pragma	prolkey(ipproc,		"DCALL=SUPPLIED,ISTK=16384")
#pragma	prolkey(netstart,	"DCALL=SUPPLIED,ISTK=16384")
#pragma	prolkey(netproc,	"DCALL=SUPPLIED,ISTK=16384")
#pragma	prolkey(nulldisp,	"DCALL=SUPPLIED,ISTK=16384")
#pragma	prolkey(ospf,		"DCALL=SUPPLIED,ISTK=16384")
#pragma	prolkey(rip,		"DCALL=SUPPLIED,ISTK=16384")
#pragma	prolkey(ripout,		"DCALL=SUPPLIED,ISTK=16384")
#pragma	prolkey(sample,		"DCALL=SUPPLIED,ISTK=16384")
#pragma	prolkey(slowtimer,	"DCALL=SUPPLIED,ISTK=16384")
#pragma	prolkey(snmpd,		"DCALL=SUPPLIED,ISTK=16384")
#pragma	prolkey(tcpinp,		"DCALL=SUPPLIED,ISTK=16384")
#pragma	prolkey(tcpout,		"DCALL=SUPPLIED,ISTK=16384")
#pragma	prolkey(tcptimer,	"DCALL=SUPPLIED,ISTK=16384")
#pragma	prolkey(udpecho,	"DCALL=SUPPLIED,ISTK=16384")

//  Definitions for process management create.c 

#pragma	prolkey(envcreate,	"DCALL=ALLOCATE,ISTK=16384")	// exit R1 -> env
#pragma	prolkey(envbegin,	"DCALL=SUPPLIED,ISTK=16384")	// entry R0 -> env
#pragma	prolkey(envdestroy,	"DCALL=DESTROY,ISTK=16384")	// entry R0 -> env

//  Function main is also created as a process, this will have to be resolved later

//------------------------------------------------------------------------------mvs38j.h
#endif





