/* icmpdump.c - icmpdump */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 * icmpdump - pretty-print an ICMP packet
 *------------------------------------------------------------------------
 */

int icmpdump(struct icmp *pic, int dlen, int order) {

	short	cksum, id, seq;
	int	hlen = sizeof(struct icmp);
	int	i;

	if (order & EPO_ICMP) {
		cksum = pic->ic_cksum;
		id = pic->ic_id;
		seq = pic->ic_seq;
	} else {
		cksum = net2hs(pic->ic_cksum);
		id = net2hs(pic->ic_id);
		seq = net2hs(pic->ic_seq);
	}

	switch (pic->ic_type) {
		case ICT_ECHORP:
			kprintf("ICMP: type %d ECHORP code %d cksum 0x%x *** tid 0x%x seq 0x%x\n", 
				pic->ic_type, pic->ic_code, cksum & 0xffff, id & 0xffff, seq);
			break;
		case ICT_ECHORQ:
			kprintf("ICMP: type %d ECHORQ code %d cksum 0x%x *** tid 0x%x seq 0x%x\n", 
				pic->ic_type, pic->ic_code, cksum & 0xffff, id & 0xffff, seq);
			break;
		case ICT_REDIRECT:
			kprintf("ICMP: type %d REDIRECT code %d cksum 0x%x *** tgw 0x%x\n", 
				pic->ic_type, pic->ic_code, cksum & 0xffff, pic->ic_gw);
			break;
		case ICT_MASKRQ:
			kprintf("ICMP: type %d MASKRQ code %d cksum 0x%x *** tmask 0x%x\n", 
				pic->ic_type, pic->ic_code, cksum & 0xffff, pic->ic_gw);
		case ICT_MASKRP:
			kprintf("ICMP: type %d MASKRP code %d cksum 0x%x *** tmask 0x%x\n", 
				pic->ic_type, pic->ic_code, cksum & 0xffff, pic->ic_gw);
			break;
		default:
			kprintf("ICMP: type %d <UNKNOWN> code %d cksum 0x%x\n", 
				pic->ic_type, pic->ic_code, cksum & 0xffff);
	}
	hexdump(pic->ic_data, dlen - hlen);
}






