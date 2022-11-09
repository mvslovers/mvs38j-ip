/* net.h */

#ifndef H_NET_H
#define H_NET_H

#include <ip.h>		// IPaddr typedef
#include <ether.h>	// Eaddr typedef
#include <netif.h>	// struct netif

/* High-level network definitions and constants */

#define	NETBUFS		64		/* number of network buffers	*/
#define	MAXNETBUF	EP_MAXLEN	/* max network buffer length	*/
#define	LRGBUFS		16		/* number of large net buffers	*/

#ifndef	MAXLRGBUF
	#define	MAXLRGBUF	2048	/* size of large net bufs	*/
					// BPMAXB is too large (20K)
#endif

#define	NETFQ		3		/* size of input-to-output queue*/

/* Network input and output processes: procedure name and parameters */

#define	NETSTK		1000		/* stack size for network setup	*/
#define	STSTK		4096		/* stack size for network timer	*/
#define	NETPRI		100		/* network startup priority	*/
#define	NETIPRI		110		/* min network input priority	*/
#define	STPRI		100		/* network timer priority	*/
#define	NETNAM		"netstart"	/* name of network setup process*/
#define	NETPROCNAM	"ctc-in netproc"/* name of network input process*/
#define	STNAM		"slowtimer"	/* name of network timer   "	*/
#define	NETARGC		1		/* count of args to net startup	*/
#define	STARGC		0		/* count of args to net timer	*/
#define	NETPROCARGC	0		/* netproc arg count		*/

struct	netinfo	{			/* info and parms. for network	*/
	int	netpool;		/* network packet buffer pool	*/
	int	lrgpool;		/* large packet buffer pool	*/
	int	nif;			/* # of configured interfaces	*/
	int	sema;			/* semaphore for global net	*/
};

#ifdef DEFINE_EXTERN
extern	struct	netinfo	Net;		/* All network parameters	*/
#endif

IPaddr	getiaddr(int);
int	getname(char *), getiname(int, char *);
int	initgate(void);
int	inithost(void);
char	*ip2name(IPaddr, char *);
int	local_out(struct ep *);
IPaddr	name2ip(char *);
Bool	netmatch(IPaddr, IPaddr, IPaddr, Bool);
IPaddr	netmask(IPaddr);
IPaddr	netnum(IPaddr);
PROCESS	netproc();
PROCESS	netstart();
int	netup_nif();
int	netup_rt();
int	netwrite(struct netif *, struct ep *, unsigned);
IPaddr	resolve(char *);
int	setmask(unsigned, IPaddr);
int	slowtimer();
#endif /* H_NET_H */



