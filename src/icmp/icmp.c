/* icmp.c - icmp */

#include <conf.h>
#include <kernel.h>
#include <network.h>

#define	DEBUG
#include <debug.h>

/*
 * ICT_REDIRECT	- pa2 == gateway address
 * ICT_PARAMP	- pa2 == (packet) pointer to parameter error
 * ICT_MASKRP	- pa2 == mask address
 * ICT_ECHORQ	- pa1 == seq, pa2 == data size
 */

struct ep *icsetbuf(int, void *, Bool *, Bool *);
Bool icerrok(struct ep *);
void icsetsrc(struct ip *);
int icsetdata(int, struct ip *, void *);


/*------------------------------------------------------------------------
 *  icmp -  send an ICMP message
 *------------------------------------------------------------------------
 */

int icmp(u_short type, u_short code, IPaddr dst, void *pa1, void *pa2) {

	struct	ep	*pep;
	struct	ip	*pip;
	struct	icmp	*pic;
	Bool		isresp, iserr;
	int		datalen;
	char		bufdst[IP_MAXDOT];


	ip2dot(bufdst, dst);
	trace("\nICMP enter; type %d code %d dest %s pa1 0x%x pa2 0x%x\n", 
		type, code, bufdst, pa1, pa2);

	IcmpOutMsgs++;

	pep = icsetbuf(type, pa1, &isresp, &iserr);
	if (pep == 0) {
		IcmpOutErrors++;
		trace("ICMP syserr; icsetbuf\n");
		return SYSERR;
	}
	pip = (struct ip *)pep->ep_data;
	pic = (struct icmp *) pip->ip_data;

	datalen = IC_HLEN;

	/* we fill in the source here, so routing won't break it */

	if (isresp) {
		if (iserr) {
			if (!icerrok(pep)) {
				freebuf(pep);
				return OK;
			}
			memcpy(pic->ic_data, pip, IP_HLEN(pip)+8);
			datalen += IP_HLEN(pip)+8;
		}
		icsetsrc(pip);
	} else
		pip->ip_src = ip_anyaddr;
	pip->ip_dst = dst;

	pic->ic_type = (char) type;
	pic->ic_code = (char) code;
	if (!isresp) {
		if (type == ICT_ECHORQ)
			pic->ic_seq = (int) pa1;
		else
			pic->ic_seq = 0;
		pic->ic_id = getpid();
	}
	datalen += icsetdata(type, pip, pa2);

	pic->ic_cksum = 0;
	pic->ic_cksum = cksum((WORD *)pic, datalen);

	dbug(ipdump(pip, 0));

	ipsend(dst, pep, datalen, IPT_ICMP, IPP_INCTL, IP_TTL);

	trace("ICMP exit; OK\n");
	return OK;
}


