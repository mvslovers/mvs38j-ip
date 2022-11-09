/* comer_cksum.c - comer_cksum */

#include <conf.h>
#include <kernel.h>
#include <ip.h>

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------------
 *  comer_cksum		Douglas Comer's checksum calculation
 *------------------------------------------------------------------------------
 */

//  typed from Internetworking with TCP/IP Vol 2, 1st edition (Comer)
//  p. 70, mildy modified

short comer_cksum(unsigned short *buf, int nwords) {

	unsigned long	sum;
	short		result;


	for (sum=0; nwords>0; nwords--) {
		sum += *buf++;
//		trace("COMER_CKSUM buf 0x%x sum 0x%x nwords %d\n", buf, sum, nwords);
	}

	sum = (sum >> 16) + (sum & 0xffff);	/* add in carry 	*/ 
	sum += (sum >> 16);			/* maybe one more	*/
	result = (short) ~sum;
	result = result & 0xffff;		// turn off bits 0-15

//	trace("COMER_CKSUM result 0x%x\n", result);
	return result;
}

