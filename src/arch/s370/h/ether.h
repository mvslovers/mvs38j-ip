/* ether.h */

#ifndef H_ETHER_H
#define H_ETHER_H
#include <xinutype.h>		// Eaddr, IPaddr

/* Ethernet definitions and constants */

#define	EP_MAXMULTI	  10	/* multicast address table size		*/

#define	EP_MINLEN	  60	/* minimum packet length		*/
#define	EP_DLEN		1500	/* length of data field ep		*/
#define	EP_CRC		   4	/* ether CRC (trailer)			*/
#define	EP_RETRY	3	/* number of times to retry xmit errors	*/
#define	EP_BRC	"\377\377\377\377\377\377"/* Ethernet broadcast address	*/
#define EP_RTO 300		/* time out in seconds for reads	*/

#define EP_NUMRCV 16		/* number LANCE recv buffers (power 2)	*/
#define EP_NUMRCVL2 4		/* log2 of the number of buffers	*/

#define	EPT_LOOP	0x0060		/* type: Loopback		*/
#define	EPT_ECHO	0x0200		/* type: Echo			*/
#define	EPT_PUP		0x0400		/* type: Xerox PUP		*/
#define	EPT_IP		0x0800		/* type: Internet Protocol	*/
#define	EPT_ARP		0x0806		/* type: ARP			*/
#define	EPT_RARP	0x8035		/* type: Reverse ARP		*/

struct	eh {			/* ethernet header			*/
	Eaddr	eh_dst;		/* destination host address		*/
	Eaddr	eh_src;		/* source host address			*/
	unsigned short	eh_type;/* Ethernet packet type (see below)	*/
};
#define	EP_EH_LEN	sizeof(struct eh)

struct	ep	{		// complete structure of Ethernet packet
	IPaddr	ep_nexthop;	// +0x00 IP addr of next hop
	short	ep_ifn;		// +0x04 originating interface number
	short	ep_len;		// +0x06 length of the packet
	short	ep_order;	// +0x08 byte order mask (for debugging)
	struct	eh ep_eh;	// +0x0A the ethernet header
				// +0x10 eth hdr: source MAC
				// +0x16 eth hdr: protocol
	char	ep_data[EP_DLEN];// +0x18 data in the packet (IP frame)
};
typedef	struct ep	ep;
//  Size of header portion of struct ep
extern	struct ep	dummy_header_ep;
#define	EP_HLEN		(sizeof(struct ep) - sizeof(dummy_header_ep.ep_data))
#define	EP_MAXLEN	(EP_HLEN + EP_DLEN)

/* Packet Byte Order Constants (set means host order) */

#define	EPO_NET		1	/* network layer		*/
#define	EPO_IP		2	/* level 1 protocols...		*/
#define	EPO_ARP		2
#define	EPO_RARP	2
#define	EPO_ICMP	4	/* level 2 protocols		*/
#define	EPO_IGMP	4
#define	EPO_TCP		4
#define	EPO_UDP		4
#define	EPO_OSPF	4
#define	EPO_DNS		5	/* application protocols	*/

/* these allow us to pretend it's all one big happy structure */

#define	ep_dst	ep_eh.eh_dst
#define	ep_src	ep_eh.eh_src
#define	ep_type	ep_eh.eh_type

#if	Noth > 0	/* this is for Othernet simulation */
struct	otblk {
	Bool	ot_valid;	/* these entries are valid?		*/
	Eaddr	ot_paddr;	/* the Othernet physical address	*/
	Eaddr	ot_baddr;	/* the Othernet broadcast address	*/
	int	ot_rpid;	/* id of process reading from othernet	*/
	int	ot_rsem;	/* mutex for reading from the othernet	*/
	int	ot_pdev;	/* Physical device devtab index		*/
	int	ot_intf;	/* the associate interface		*/
	char	*ot_descr;	/* text description of the device	*/
};
#endif	/* Noth */


#define	ETOUTQSZ	30

/* Ethernet control block descriptions */

struct	etblk	{
	int	etxpending;	/* number of packets pending output	*/
	int	etrpending;	/* 1 => a receive is pending already	*/
	int	etwtry;		/* num. of times to retry xmit errors	*/
	char	*etwbuf;	/* pointer to current transmit buffer	*/
	short	etnextbuf;	/* for checking buffers round robin	*/
#if	Noth > 0
	struct	otblk	*etoth[Noth];	/* eaddr to oaddr translations	*/
#endif	/* Noth */
};


/* ethernet function codes */

#define	EPC_PROMON	1		/* turn on promiscuous mode	*/
#define	EPC_PROMOFF	2		/* turn off promiscuous mode	*/
#define	EPC_MADD	3		/* add multicast address	*/
#define	EPC_MDEL	4		/* delete multicast address	*/

struct utdev {
	unsigned int	 ud_iomem;
	unsigned int	 ud_iosize;
	struct devsw	*ud_pdev;
	Eaddr		 ud_paddr;
	Eaddr		 ud_bcast;
	char		*ud_descr;
	int		 ud_outq;
	int		 ud_ifnum;
	int		 ud_nextbuf;
	int		 ud_xpending;
	int		 ud_xmaddr;	/* transmit base buf #		*/
	int		 ud_wretry;
	int		 ud_rmin;	/* receive ring buffer base addr*/
	int		 ud_rmax;	/* receive ring buffer max addr	*/
};

struct etdev {
	unsigned int	 ed_iomem;
	unsigned int	 ed_iosize;
	struct devsw	*ed_pdev;
	Eaddr		 ed_paddr;
	Eaddr		 ed_bcast;
	char		*ed_descr;
	int		 ed_outq;
	int		 ed_ifnum;
	int		 ed_wretry;
	struct scb	*ed_scb;
	struct cbl	*ed_cbl;
	struct rfd	*ed_rfd;	/* first in RFD ring		*/
	struct rfd	*ed_rfdend;	/* last in RFDring		*/
	struct rbd	*ed_rbd;	/* first in RBD ring		*/
	struct rbd	*ed_rbdend;	/* last in RBD ring		*/
	struct tbd	*ed_tbd;	/* transmit buffer descriptor	*/
	unsigned char	*ed_xmbuf;	/* transmit buffer base		*/
	unsigned char	 ed_irq;	/* SIRQ reg value		*/
	int		 ed_mcc;	/* # valid mca's		*/
	Eaddr		 ed_mca[EP_MAXMULTI]; /* multicast addr. table	*/
/* XXX */
	int		ed_xpending;
};

#ifdef	DEFINE_EXTERN
extern	struct utdev	ue[];
extern	struct etdev	ee[];
extern	struct	etblk	eth[];
#endif

void	efaceinit(unsigned);
void	ofaceinit(unsigned);
int	ethmcast(int, int, Eaddr, IPaddr);
int	initutdev(struct devsw *);

#endif /* H_ETHER_H */








