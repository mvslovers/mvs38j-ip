//  ipshowhdr.c - ipshowhdr

#include <conf.h>
#include <kernel.h>
#include <network.h>

#define	DEBUG
#include <debug.h>

#undef	DEBUG_DUMP_IP_HEADER	// defined = snap IP header

struct	IPprot2txt	{
	short	prot;		// IP protocol number
	char	text[5];	// IP protocol descriptive text
};
typedef	struct	IPprot2txt	IPprot2txt;

IPprot2txt	iptxt[] = {
	1,	"ICMP",
	2,	"IGMP",
	6,	"TCP",
	8,	"EGP",
	17,	"UDP",
	89,	"OSPF"
};
#define	NUMPROTS	6	// # of protocols in iptxt

/*------------------------------------------------------------------------
 *  ipshowhdr	Display IP header
 *		Returns total packet length
 *------------------------------------------------------------------------
 */

//  phdr	address of IP frame 
//  pcheat	NULL or address of struct ipcheat to be filled in

int ipshowhdr(void *phdr, ipcheat *pcheat) {

	struct ip	*pip;			// IP header
	IPprot2txt	*ptxt;
	int		v, hlen, fragoff, i;
	char		flagtxt[80] = "";
	char		sourcedot[16];		// xxx.xxx.xxx.xxx
	char		destdot[16];
	IPprot2txt	dummytxt[] = {0, "????"};

	pip = phdr;
	v = pip->ip_verlen >> 4;		// top 4 bits = IP version
	hlen = IP_HLEN(pip);			// header length in bytes

	if ( (pip->ip_fragoff & 0x4000) == 0x4000)
		strcpy(flagtxt, "dont-fragment ");
	if ( (pip->ip_fragoff & 0x2000) == 0x2000)
		strcat(flagtxt, "more-fragments");
	if (flagtxt[0] == 0x00)
		strcpy(flagtxt, "none");

	fragoff = pip->ip_fragoff & 0x1fff;	// bottom 13 bits

	ptxt = NULL;
	for (i = 0; i < NUMPROTS; i++) {	// find protocol descriptive text
		if (iptxt[i].prot == pip->ip_proto) {
			ptxt = (void *) &iptxt[i];
			break;
		}
	}
	if (ptxt == NULL)
		ptxt = dummytxt;
	
	ip2dot(sourcedot, pip->ip_src);
	ip2dot(destdot,   pip->ip_dst);

	//  Ignore options for now

#ifdef	DEBUG_DUMP_IP_HEADER
	snap("IPSHOWhdr IP header", phdr, hlen);
#endif

	trace("IPSHOWhdr buffer 0x%x\n"
	      "          IPv%d IP-hdrlen %d TOS %d PACKETLEN %d\n"
	      "          ID %d FLAGS %s FRAGOFFSET %d \n"
	      "          TTL %d PROTOCOL %d (%s) CKSUM 0x%x\n"
	      "          SOURCE %s DEST %s\n",
		phdr, v, hlen, pip->ip_tos, pip->ip_len,
		pip->ip_id, flagtxt, fragoff,
		pip->ip_ttl, pip->ip_proto, ptxt->text, pip->ip_cksum,
		sourcedot, destdot);

	//  Oops, I wrote this routine before I realized there was
	//  an ipdump; call it too since it calls protocol-dependent
	//  code to format lower-level data

	ipdump(pip, EPO_IP);

	//  Fill in return info

	if (pcheat != (struct ipcheat *)NULL) {
		pcheat->buf = phdr;
		pcheat->hlen = hlen;
		pcheat->paclen = pip->ip_len;
		pcheat->data = (void *) ( (char *)pip + hlen);
		pcheat->datalen = pip->ip_len - hlen;
	}

	return pip->ip_len;
		
}




