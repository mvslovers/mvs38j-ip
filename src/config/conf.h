/* conf.h (GENERATED FILE; DO NOT EDIT) */

#define	NULLPTR	(char *)0

/* Device table declarations */
struct	devsw	{			/* device table entry */
	int	dvnum;
	char	*dvname;
	int	(*dvinit)(struct devsw *);
	int	(*dvopen)(struct devsw *, void *, void *);
	int	(*dvclose)(struct devsw *);
	int	(*dvread)(struct devsw *, char *, unsigned);
	int	(*dvwrite)(struct devsw *, unsigned char *, unsigned);
	int	(*dvseek)(struct devsw *, long);
	int	(*dvgetc)(struct devsw *);
	int	(*dvputc)(struct devsw *, unsigned char);
	int	(*dvcntl)(struct devsw *, int, void *, void *);
	void	*dvcsr;
	int	dvivec;
	int	dvovec;
	void	(*dviint)(struct devsw *, unsigned char);
	void	(*dvoint)(struct devsw *);
	void	*dvioblk;
	int	dvminor;
	};

extern	struct	devsw devtab[];		/* one entry per device */


/* Device name definitions */

#define	CONSOLE     0			/* type oprcons  */
#define	ETHER       1			/* type eth      */
#define	ETHER1      2			/* type eth      */
#define	ETHER2      3			/* type eth      */
#define	UDP         4			/* type dgm      */
#define	DGRAM0      5			/* type dg       */
#define	DGRAM1      6			/* type dg       */
#define	DGRAM2      7			/* type dg       */
#define	DGRAM3      8			/* type dg       */
#define	DGRAM4      9			/* type dg       */
#define	DGRAM5      10			/* type dg       */
#define	DGRAM6      11			/* type dg       */
#define	DGRAM7      12			/* type dg       */
#define	DGRAM8      13			/* type dg       */
#define	DGRAM9      14			/* type dg       */
#define	DGRAMA      15			/* type dg       */
#define	DGRAMB      16			/* type dg       */
#define	DGRAMC      17			/* type dg       */
#define	DGRAMD      18			/* type dg       */
#define	DGRAME      19			/* type dg       */
#define	DGRAMF      20			/* type dg       */
#define	TCP         21			/* type tcpm     */
#define	TCP0        22			/* type tcp      */
#define	TCP1        23			/* type tcp      */
#define	TCP2        24			/* type tcp      */
#define	TCP3        25			/* type tcp      */
#define	TCP4        26			/* type tcp      */
#define	TCP5        27			/* type tcp      */
#define	TCP6        28			/* type tcp      */
#define	TCP7        29			/* type tcp      */
#define	TCP8        30			/* type tcp      */
#define	TCP9        31			/* type tcp      */
#define	TCPA        32			/* type tcp      */
#define	TCPB        33			/* type tcp      */
#define	TCPC        34			/* type tcp      */
#define	TCPD        35			/* type tcp      */
#define	TCPE        36			/* type tcp      */
#define	TCPF        37			/* type tcp      */

#define	Noprcons	1	/* number of devices in class oprcons	*/
#define	Neth	3	/* number of devices in class eth	*/
#define	Ndgm	1	/* number of devices in class dgm	*/
#define	Ndg	16	/* number of devices in class dg	*/
#define	Ntcpm	1	/* number of devices in class tcpm	*/
#define	Ntcp	16	/* number of devices in class tcp	*/

#define	NDEVS	38	/* Total number of devices */

/* Declarations of I/O routines referenced */

extern	int	ionull(struct devsw *);
extern	void	ioerr(void);
extern	int	mvsconswrite(struct devsw *, unsigned char *, unsigned);
extern	void	oprconsiin(struct devsw *, unsigned char);
extern	void	oprconsoin(struct devsw *);
extern	int	ctci_init(struct devsw *);
extern	int	noopen(struct devsw *, void *, void *);
extern	int	noclose(struct devsw *);
extern	int	ctci_read(struct devsw *, char *, unsigned);
extern	int	ctci_write(struct devsw *, unsigned char *, unsigned);
extern	int	noseek(struct devsw *, long);
extern	int	nocntl(struct devsw *, int, void *, void *);
extern	int	nogetc(struct devsw *);
extern	int	noputc(struct devsw *, unsigned char);
extern	void	noiint(struct devsw *, unsigned char);
extern	void	nooint(struct devsw *);
extern	int	dgmopen(struct devsw *, void *, void *);
extern	int	noread(struct devsw *, char *, unsigned);
extern	int	nowrite(struct devsw *, unsigned char *, unsigned);
extern	int	dgmcntl(struct devsw *, int, void *, void *);
extern	int	dginit(struct devsw *);
extern	int	dgclose(struct devsw *);
extern	int	dgread(struct devsw *, char *, unsigned);
extern	int	dgwrite(struct devsw *, unsigned char *, unsigned);
extern	int	dgcntl(struct devsw *, int, void *, void *);
extern	int	tcpmopen(struct devsw *, void *, void *);
extern	int	tcpmcntl(struct devsw *, int, void *, void *);
extern	int	tcpinit(struct devsw *);
extern	int	tcpclose(struct devsw *);
extern	int	tcpread(struct devsw *, char *, unsigned);
extern	int	tcpwrite(struct devsw *, unsigned char *, unsigned);
extern	int	tcpcntl(struct devsw *, int, void *, void *);
extern	int	tcpgetc(struct devsw *);
extern	int	tcpputc(struct devsw *, unsigned char);

//  ----------------------------------------------------------------------------

//  Section Three of the Configuration file, constant definitions

//------------------------------------------------------------------------------conf.h

/*------------------------------------------------------------------------------*/
/*  Fakeout Xinu environment by using Standard C headers	 		*/
/*------------------------------------------------------------------------------*/
#include <stdio.h>

#define MVS38J				// target platform
#define MVSPAGES	((1024+512)/4)	// # 4K pages of memory to allocate

#ifndef	LITTLE_ENDIAN
	#define	LITTLE_ENDIAN	0x1234
#endif
#ifndef	BIG_ENDIAN
	#define	BIG_ENDIAN	0x4321
#endif
#define	BYTE_ORDER	BIG_ENDIAN	// S/370 memory layout

#undef	MVS_LOCKMGR			// defined = call mvslock, mvsunlock

#define	MEMMARK				// memory marking supported
#define	RTCLOCK				// real time clock supported
#define	NPROC	    	50		// number of user processes
#define	NSEM	    	200		// number of semaphores
#undef	STKCHK				// resched ignores stack overflow
					// Systems/C expands stack as required

#define	SCHED_NO_PREEMPT		// MVS-Xinu doesn't preempt processes

#define DEFINE_EXTERN			// define externs for global data
					// (reserved for possible future
					// reentrancy considerations; doesn't
					// do much of anything at the moment)

//  Processes for sysinit() to start

#define	SYSINIT_INITDEVS		// init() devices
#define	SYSINIT_NULLDISP		// nulluser+process interface
#define	SYSINIT_NET			// network
#undef	SYSINIT_USER			// user process
#undef	SYSINIT_SAMPLE			// sample processes (debug process mgt)

//  Network features

#define	NET_FEATURE_GATEWAY	FALSE	// TRUE = host offers gateway services

//  Protocols for netstart() to start

#define	NETSTART_IP			// IP protocol
#define	NETSTART_ARP			// ARP protocol
#define	NETSTART_UDP			// UDP protocol

#undef	NETSTART_TCP			// TCP protocol
#undef	NETSTART_RIP			// RIP protocol
#undef	NETSTART_SNMP			// SNMP protocol
#undef	NETSTART_OSPF			// OSPF protocol

//  Daemons for netstart() to start

#undef	NETSTART_SLOWTIMER		// slowtimer table maintenance
#undef	NETSTART_RWHO			// rwho
#undef	NETSTART_FINGERD		// TCP finger
#undef	NETSTART_ECHOD			// TCP echo
#undef	NETSTART_UDPECHO		// UDP echo

#define	IP2NAME_FAKE			// <debug> net/ip2name option (no DNS)
#define	GETUTIM_FAKE			// <debug> arch/s370/clock/getutim 

//	#define BSDURG				/* TCP option 	 		*/

//	#define	ETDAEMON			/* Ethernet network daemon runs	*/
//	#define OSPF				/* define if using OSPF		*/
//	#define	SNMP				/* define for SNMP & MIB	*/
//	#define RIP				/* define if using RIP		*/
//	#define MULTICAST			/* define if using multicasting	*/

#ifdef	OSPF
#define	MULTICAST			/* OSPF uses multicasting	*/
#define	NAREA		1		/* max number of OSPF Areas	*/
#endif

#define	BSDBRC		0		// 1 = BSD broadcast (host all 0s)
					// 0 = normal (host all 1s)

#define	CONTACT   "mvs38j-ip@yahoogroups.com"	/* find out more about MVS-Xinu */
#define	LOCATION    	"Xinu Lab"		/* Host's physical location	*/
#define	IPADDR1		"192.168.200.1"		/* interface 1 IP address	*/
#define	DEFRTR		"192.168.200.2"		/* default router		*/
#define	TSERVER		"10.3.0.1:37"		/* Time server address		*/
#define	RSERVER		"10.3.0.1:2001"		/* Remote file server address	*/
#define	NSERVER		"10.3.0.1:53"		/* Domain Name server address	*/
#define	LOGHOST		"10.3.0.1:514"		/* syslog server address	*/

#define	BINGID		9		/* Othernet simlated net param. */

#define	SMALLMTU	400		/* for OTHER2; sim. small MTU	*/

#define	NBPOOLS		10		// max # buffer pool; mkpool
#define	BPMAXB		(20*1024)	/* max buffer size for mkpool	*/
#define BPMAXN		128		/* max # buffers in a buf pool	*/

#define	RT_BPSIZE	20		// # routes in route table <route.h>
					// remove to take <route.h> default 100

#define	TCPSBS		4096		/* TCP send buffer sizes	*/
#define	TCPRBS		8192		/* TCP receive buffer sizes	*/

#define	NPORTS		100		/* number of ports		*/

//  Clock management 
//  Time intervals are specified in hundredths of a second in int hhsec

#define	QUANTUM		10		// clock ticks until preemption
					// defclock.c, mvsclkinit.c

#define	TIMERGRAN	1		// tcptimer() timer granularity, secs/10

//  Define N<class> (number of devices in <class>) if config didn't already

#ifndef	Noth
#define	Noth		0		// zero "oth" class devices
#endif

#ifndef	Ndg
#define	Ndg		0		// zero "dg" class devices
#endif

//  Debugging defines

#define	PANIC_BUFPOOL			// defined = BUFPOOL_SYSERR will panic
#undef	MVS_DUMP_PROCSTATE		// defined = procstate() dumps pentry
#undef	MVS_GEN_CLOCKSTATE		// defined = generate clockstate()
#define	MVS_MAGIC_SVC		245	// SVC # mvsauth will use
#define	MVS_PANIC_ABCOD		809	// panic() abend code; undefined = exit(69)
#define	MVS_PROC_KEEP_FINI		// defined = preserve completed pentry
#define	MVS_PROC_KEEP_ENV		// defined = don't call mvsenv(ENVDESTROY,)
#undef	MVS_TRACE_LOCK			// defined = trace mvs[un]lock
#undef	MVS_TSO_TEST			// DOESN'T WORK, SVC97 problems
#define	MVS_XDONE_DEBUG "XDONE DEBUG"	// undefined = no xdone termination summary
					// defined = panic() checks msg
#define	MVS_WTO_GOV			// defined = kprintf calls mvsdoze
					// to limit MVS console message flooding
#define	MVS_SNAP_FUNCSTR		// defined = OK for snap to call funcstr;
					// should be undefined if you wish to 
					// call snap from funcstr to prevent
					// recursion, recursion, recursion....
//------------------------------------------------------------------------------conf.h




