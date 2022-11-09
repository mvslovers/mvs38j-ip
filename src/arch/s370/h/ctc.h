/* ctc.h - MVS-Xinu CTC header */

#ifndef H_CTC_H
#define H_CTC_H			// only generate header once
//------------------------------------------------------------------------------ctc.h

//  Hercules supports at least two types of CTCs: CTCI and LCS
//  We currently support the CTCI flavor, further doc in ctci.h

#include <mvsio.h>		// mvsctcdcb, etc

//  ----------------------------------------------------------------------------
//  Manifest constants
//  ----------------------------------------------------------------------------

#define	CTC_RETRY	0		// # times to retry read or write
#define	CTCI_BUFLEN	(20*1024)	// CTC buffer length
#define	CTC_BUFLEN	CTCI_BUFLEN	// CTC buffer length

#if CTC_BUFLEN > MVSMAXIOSIZE		// exceeds max MVS38j I/O length?
	#undef	CTC_BUFLEN
	#define	CTC_BUFLEN MVSMAXIOSIZE
	#warn	<ctc.h> CTC_BUFLEN redefined to MVSMAXIOSIZE
#endif

#define	CTC_READBUFS	3		// # CTC read buffers
#define	CTC_WRITEBUFS	3		// # CTC write buffers

//  Orders passed to MVSCTC from ctci_ioread, ctci_iowrite

#define	CTC_OPOPENR	0x0001		// open read
#define	CTC_OPCLOSER	0x0002		// close read
#define	CTC_OPREAD	0x0003		// read
#define	CTC_BOGUS	0x0004		// conflicts with write
#define	CTC_OPCOPYR	0x0005		// copy read DCB

#define	CTC_OPOPENW	0x0101		// open write
#define	CTC_OPCLOSEW	0x0102		// close write
#define	CTC_OPBOGUS2	0x0103		// conflicts with read
#define	CTC_OPWRITE	0x0104		// write
#define	CTC_OPCOPYW	0x0105		// copy write DCB

//  CTCI I/O process creation parms

#define	CTC_PROC_READ	"ctcioread"	// ctc read process name
#define	CTC_PROC_WRITE	"ctciowrite"	// ctc write process name
#define	CTC_STK		1000		// stack size (bogus)
#define	CTC_PRI		300		// startup priority

//  ----------------------------------------------------------------------------
//  MVS-Xinu CTC structures
//  ----------------------------------------------------------------------------

//  CTCHE provides IGG019X8, the CTCI channel end appendage
//  MUST BE KEPT IN SYNC WITH DSCTCHE MACRO
//  with a workarea; allocated by ctci_ioread, ctci_iowrite;
//  anchored ctdev.chework

struct	ctche	{
	char		cheid[16];	// +0x00 eyecatcher

	void		*ucb;		// +0x10 @ CTC UCB R7
	void		*dcb;		// +0x14 @ CTC DCB R4
	void		*deb;		// +0x18 @ CTC DEB R3
	void		*iob;		// +0x1C @ CTC IOB R2

	void		*rqe;		// +0x20 @ CTC RQE R1
	void		*ctdev;		// +0x24 @ ctdev
	void		*spare[2];	// +0x28

	WORD		cheregs[16];	// +0x30 R0:R15
	WORD		chex[16];	// +0x70 CHE's choice
};
typedef	struct	ctche	ctche;

//  CTCI I/O control blocks for communication between ctci_io*, MVSCTC, IGG019X8
//  Allocated by ctci_ioread, ctci_iowrite when they initialize
//  Anchored in	 ctdev.ctioblk
//  Fields buf, buflen, and pecb must stay in sequence for MVSCTC
//  ctci_ioread sets iolen to length of data read on good I/O,
//                            -1 otherwise

struct	ctioblk	{
	char		eye[16];	// +0x00 eyecatcher

	void		*pctdev;	// +0x10 @ related ctdev
	int		poolid;		// +0x14 buffer pool id
	int		iolen;		// +0x18 resulting I/O length
	void		*spare[1];	// +0x1c alignment, spare

	void		*buf;		// +0x20 @ current CTCI buffer
	int		buflen;		// +0x24 buffer length
	void		*pecb;		// +0x28 @ MVS ECB
	void		*spare2[1];	// +0x2c

	mvsiob		iob;		// +0x30 MVS IOB control block
	mvsctcdcb	dcb;		// +0x70 MVS DCB control block
	ccw0		ccw;		// +0xd0 S/370 channel program
	WORD		more[6];	// +0xd8
};
typedef	struct ctioblk	ctioblk;

//  CTC device info for half of a CTC pair (one MVS CTC)
//  MUST BE KEPT IN SYNC WITH DSCTDEV MACRO
//  Resides in ctblk
//  Anchored: xgd.ctclink
//  ctdev.next set by ctci_init, referenced by IGG019X8

struct	ctdev	{
	char		eye[8];		// +0x00 "CTDEV" (ctci_init)
	char		flag1;		// +0x08 status flag; ctdev_enum
	char		cheflag;	// +0x09 CHE debug flag (see DSCTDEV macro)
	Bool		disabled;	// +0x0a TRUE = device configured disabled
	char		resv[1];	// +0x0b 0x00s
	void		*next;		// +0x0c @ next ctdev

	void		*chework;	// +0x10 @ ctche; DSCTCHE macro
	WORD		ecb;		// +0x14 CTC device ECB
	void		*ctioblk;	// +0x18 @ related ctioblk
	void		*ioword;	// +0x1c @ ctreadstat/ctwritestat (ctci_io*)

	void		*config;	// +0x20 @ Config.nif[n].ctchalf (read or write)
	void		*spare[1];	// +0x24
	int		userpid;	// +0x28 I/O requestor's pid
	int		iopid;		// +0x2c CTCI I/O pid (ctci_ioread, ctci_iowrite)

	int		sem;		// +0x30 CTC device half (read/write) semaphore #
	short		cuu;		// +0x34 MVS device address (cuu)
	short		spare2;		// +0x36 unused
	void		*spare3[2];	// +0x38
};
typedef	struct ctdev	ctdev;
enum	ctdev_enum {			// ctdev.flag1
	CTIOPEND	= 0x80,		// CTC I/O started (MVS-Xinu)
	CTIOCOMPLETE	= 0x40		// CTC I/O complete (CHE APP)
};

//  CTC device control block, anchored devsw.dvioblk

struct	ctblk	{
	Bool		ctcinit;	// TRUE = CTC initialized; ctci_init
	Bool		shutdown;	// TRUE = terminate ctcio* processes
	char		spare[2];
	void 		*pdevsw;	// @ devsw for this CTC device
	void		*more[2];

	ctdev		read;		// read half of CTC pair
	ctdev		write;		// write half of CTC pair
};
typedef	struct ctblk	ctblk;

//  ----------------------------------------------------------------------------
//  External data areas, function prototypes
//  ----------------------------------------------------------------------------

#ifdef DEFINE_EXTERN
	extern	struct	ctblk	ctcdev[Neth];	// CTC device control block(s)
	extern	int	ctcecb;			// CTC I/O process ECB
#endif


void	*mvsctc(int, void *);			// flag, ctioblk
int	ctcpostread(ctblk *);			// examine read I/O status
Bool	ctcpostwrite(ctblk *);			// examine write I/O status

//------------------------------------------------------------------------------ctc.h
#endif
