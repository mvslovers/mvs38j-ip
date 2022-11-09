/* route.h - RTFREE */

/* Routing Table Entries: */
struct route {
	IPaddr	rt_net;		/* network address for this route	*/
	IPaddr	rt_mask;	/* mask for this route			*/
	IPaddr	rt_gw;		/* next IP hop				*/
	u_short	rt_metric;	/* distance metric			*/
	u_short	rt_ifnum;	/* interface number			*/
	short	rt_key;		/* sort key				*/
	short	rt_ttl;		/* time to live	(seconds)		*/
	struct	route *rt_next;	/* next entry for this hash value	*/
/* stats */
	int	rt_refcnt;	/* current reference count		*/
	int	rt_usecnt;	/* total use count so far		*/
	char	rt_func[20];	// (possibly truncated) name of last 
				// function which modified/created entry
	char	rt_null;	// null termination char for rt_func
};

/* Routing Table Global Data: */
struct rtinfo {
	struct	route	*ri_default;
	int		ri_bpool;
	Bool		ri_valid;
	int		ri_mutex;
};

#define	RT_DEFAULT	ip_anyaddr	/* the default net		*/
#define	RT_LOOPBACK	ip_loopback	/* the loopback net		*/

#define	RT_TSIZE	512	/* these are pointers; it's cheap	*/
#define	RT_INF		999	/* no timeout for this route		*/

#define	RTM_INF		16	/* an infinite metric			*/
#define	RTM_NOHOPS	0	// metric for 'no hops'

/* rtget()'s second argument... */

#define	RTF_REMOTE	0	/* traffic is from a remote host	*/
#define	RTF_LOCAL	1	/* traffic is locally generated		*/

#ifndef	RT_BPSIZE		// allow user choice in Configuration
#define	RT_BPSIZE	100	/* max number of routes			*/
#endif

/* RTFREE - remove a route reference (assumes ri_mutex HELD)		*/

#define	RTFREE(prt)					\
	if (--prt->rt_refcnt <= 0) {			\
		freebuf(prt);				\
	}

extern	struct	rtinfo	Route;
extern	struct	route	*rttable[];

#define	ALL_ROUTES	(struct route *)NULL	// rtdump parm

void		ipdbc(unsigned, struct ep *, struct route *);
int	 	rtadd(IPaddr, IPaddr, IPaddr, unsigned, unsigned, unsigned);
int		rtdel(IPaddr, IPaddr);
void		rtdump(struct route *);
int		rtfree(struct route *);
struct route	*rtget(IPaddr, Bool);
int		rthash(IPaddr);
void		rtinit(void);
struct route	*rtnew(IPaddr, IPaddr, IPaddr, unsigned,unsigned,unsigned);
void		ipredirect(struct ep *, unsigned, struct route *);







