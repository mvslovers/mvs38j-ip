/* findsgd.c - findsgd */

#include <conf.h>
#include <kernel.h>
#include <xgd.h>	// MVS-Xinu Global Data 

/*------------------------------------------------------------------------
 *  findsgd  --  locate MVS38j TCP/IP SGD (Subsystem Global Data)
 *------------------------------------------------------------------------
 */
void *
findsgd()
{
	int	rc = NULL;
	__register(8) 	void * r8 = 0;			// workreg
	__register(9) 	void * r9 = 0;			// SGD address
	char		buf[80];
	__asm {
FINDSGD   TITLE 'Find MVS38j TCP/IP global data area'
* Copyright 2003 James M. Morrison
*----------------------------------------------------------------------
* Function:    Find MVS38j TCP/IP global data area - SGD
*
* Entry:
*              R0    ignored
*              R1    ignored
*
* Exit:
*              R8   destroyed
*              R9 = NULL or address of SGD
*
* Notes:
*              Intended to be called from Systems/C code
*----------------------------------------------------------------------
         TITLE ' '
*FINDSGD  CSECT ,
R8       EQU   8
R9       EQU   9
*----------------------------------------------------------------------
*  Locate MVS38j TCP/IP SGD
*----------------------------------------------------------------------
         USING PSA,0
         L     R8,CVTPTR               @ CVT
         L     R8,CVTJESCT-CVT(,R8)    @ JESCT
         L     R8,JESSSCT-JESCT(,R8)   @ 1st SSCVT (JES2)
         USING SSCT,R8
LOOP     CLC   SSCTSNAM,SSNAME         same subsys name?
         BE    FOUND                   found subsys
         ICM   R8,15,SSCTSCTA          @ next subsys
         BNZ   LOOP
         SLR   R9,R9                   SS not found, return NULL
         B     FINISH
SSNAME   DC    CL4'TCP'                MVS38j TCP/IP subsystem name
*
FOUND    DS    0H
         L     R9,SSCTSUSE-SSCT(,R8)   @ SGD or NULL
FINISH   DS    0H                      return to C code
         }
//	kprintf("FINDSGD TCP SSCVT 0x%x; SGD 0x%x\n", r8, r9);
	return r9;		// @ SGD or NULL

	__asm {
         PRINT NOGEN
         IHAPSA ,
         CVT   DSECT=YES,LIST=NO
         IEFJESCT ,
         IEFJSCVT ,
         IEFJSSVT ,
         DSSSGD ,                      MVS38j TCP/IP SGD
	}
}


