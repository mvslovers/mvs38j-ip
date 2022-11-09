/* cksum.c - cksum */

#include <conf.h>
#include <kernel.h>
#include <ip.h>

#define	DEBUG
#include <debug.h>

#undef	DEBUG_DUMP_BUFFER

/*------------------------------------------------------------------------------
 *  cksum - return 16-bit ones complement of 16-bit ones complement sum
 *------------------------------------------------------------------------------
 */

USHORT cksum(USHORT *buf, unsigned len) {

	short		result;


	trace("CKSUM enter; buf 0x%x len %d\n", buf, len);
	callinfo("CKSUM");

#ifdef	DEBUG_DUMP_BUFFER
	snap("CKSUM buffer", (void *)buf, len);
#endif

	result = stevens_cksum((void *)buf, len);

	trace("CKSUM exit; calculated cksum 0x%x\n", result);
	return result;

}



