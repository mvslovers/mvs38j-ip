/* ports.h - isbadport */

#ifndef H_PORTS_H
#define H_PORTS_H
#ifndef	NPORTS
#define	NPORTS		20		/* default max number of ports	*/
#endif	/* NPORTS */

#define	MAXMSGS		50		/* maximum messages on all ports*/
#define	PTFREE		'\01'		/* port is Free			*/
#define	PTLIMBO		'\02'		/* port is being deleted/reset	*/
#define	PTALLOC		'\03'		/* port is allocated		*/
#define	PTEMPTY		-1		/* initial semaphore entries	*/

#define	PTNODISP	0		/* ptclear() null disposal func */

struct	ptnode	{			/* node on list of message ptrs	*/
	int	ptmsg;			/* a one-word message		*/
	struct	ptnode	*ptnext;	/* address of next node on list	*/
};

struct	pt	{			/* entry in the port table	*/
	char	ptstate;		/* port state (FREE/LIMBO/ALLOC)*/
	int	ptssem;			/* sender semaphore		*/
	int	ptrsem;			/* receiver semaphore		*/
	int	ptmaxcnt;		/* max messages to be queued	*/
	int	ptseq;			/* sequence changed at creation	*/
	struct	ptnode	*pthead;	/* list of message pointers	*/
	struct	ptnode	*pttail;	/* tail of message list		*/
};

#ifdef DEFINE_EXTERN
extern	struct	ptnode	*ptfree;	/* list of free nodes		*/
extern	struct	pt	ports[];	/* port table			*/
extern	int	ptnextp;		/* next port to examine when	*/
					/*   looking for a free one	*/
#ifdef	MEMMARK
extern	MARKER	ptmark;
#endif
#endif /* DEFINE_EXTERN */

#define	isbadport(portid)	( (portid)<0 || (portid)>=NPORTS )

SYSCALL pcount(int);
SYSCALL pcreate(unsigned int);
SYSCALL	pdelete(int, void (*)());
SYSCALL pinit(int);
SYSCALL preceive(int);
SYSCALL preset(int, void (*)());
SYSCALL psend(int, WORD);
_ptclear(struct pt *, int, void (*)());

#endif /* H_PORTS_H */

