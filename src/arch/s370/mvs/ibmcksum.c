/* ibmcksum.c - ibmcksum */

#include <conf.h>
#include <kernel.h>
#include <ip.h>

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------------
 *  ibmcksum	IBM z/POPs checksum calculation
 *------------------------------------------------------------------------------
 */

//  Note: I never got this to work correctly, and since both the Stevens and
//	  Comer code both work I abandoned it.  Maybe I'll fix it someday,
//	  as with a slight Hercules modification it "should" be faster.
//	  I never looked into whether the problem was with my Hercules
//	  mod, or with the ibmcksum code itself.

//	The Hercules mod is to enable S/370 execution to use the CKSM
//	instruction (normally only availabe in S/390 and z/Arch).

//	Briefly, here's what I did to Hercules:
//
//	Add following line to feat370.h
//		#define FEATURE_CHECKSUM_INSTRUCTION
//
//	Replace the following line in opcode.c
//		 /*B241*/ GENx___x390x900 (checksum,RRE,"CKSM"),
//	with
//		 /*B241*/ GENx370x390x900 (checksum,RRE,"CKSM"),

short ibmcksum(void *buf, int len) {

	__register(1)	int	r1 = 0;
	__register(2)	void	*r2 = buf;
	__register(3)	int	r3 = len;
	__register(4)	int	r4 = 0;

	__asm {
         DC    X'B2410012'        CKSM  R1,R2   R2 even/odd pair
         BC    1,*-4              cpu-determined amount done; finish
         LR    R4,R1              save 32 bit cksm result
*
*  Convert 32 bit checksum to 16 bit checksum as per z/POPs example
*
         LR    R2,R1
         SRDL  R2,16
         ALR   R2,R2+1
         ALR   R2,R1
         SRL   R2,16              16 bit checksum in R2
R1       EQU   1
R2       EQU   2
R3       EQU   3
R4       EQU   4
	}
	trace("IBM_CKSUM r1 0x%x R2 0x%x r3 0x%x r4 0x%x\n", r1, r2, r3, r4);
	return (short) r2;
}
