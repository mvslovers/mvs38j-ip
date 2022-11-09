/* ipcksum.c - ipcksum */

#include <conf.h>
#include <kernel.h>
#include <ip.h>

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------------
 *  stevens_cksum	W. Richard Stevens' checksum calculation
 *	addr	address of buffer
 *	len	length of buffer, in bytes
 *------------------------------------------------------------------------------
 */

//  typed from Unix Network Programming Vol 1 (W. Richard Stevens) p. 672
//  mildly modified

unsigned short stevens_cksum(unsigned short *addr, int len) {

	int	nleft = len;
	int	sum = 0;
	unsigned short	*w = addr;
	unsigned short	answer = 0;


	while (nleft > 1) {
		sum += *w;
//		trace("STEVENS_CKSUM -even- word 0x%x sum 0x%x nleft %d w 0x%x\n", *w, sum, nleft, w);
		w++;
		nleft -= sizeof(short);
	}
	if (nleft == 1) {
		*(unsigned char *) (&answer) = *(unsigned char *) w;
		sum += answer;
	}
	sum = (sum >> 16) + (sum & 0xffff);

	sum += (sum >> 16);

	answer = ~sum;
//	trace("STEVENS_CKSUM answer 0x%x\n", answer);
	return answer;
}

/*------------------------------------------------------------------------------
 *  ipcksum	For purposes of computing the IP checksum, the checksum field
 *		itself is to be set to zero
 *------------------------------------------------------------------------------
 */

USHORT ipcksum(void *pbuf) {

	struct ip	*pip = (void *)pbuf;		// @ IP frame
	int		hlen;
	short		save;				// saved IP cksum field
	short		result;


	hlen = IP_HLEN(pip);				// length of IP header in bytes
	save = pip->ip_cksum;				// save IP frame's checksum field
	pip->ip_cksum = 0;				// make zero for checksum calculation

	result = stevens_cksum((void *)pip, hlen);	// calculate checksum

	pip->ip_cksum = save;				// restore checksum field

	trace("IPCKSUM buffer 0x%x hlen %d checksum 0x%x\n", pbuf, hlen, result);

	return result;					// return checksum
}

