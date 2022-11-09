MVSWAITN CSECT ,
MVSWAITN ALIAS C'mvswaitlist'
**---------------------------------------------------------------------
**
**  Function: wait on multiple MVS ECBs
**
**  Entry: R1 -> parmlist:
**               0(4,r1) = number of ECBs which need to be
**                         POSTed before WAIT completes
**               4(4,r1) = number of ECBs supplied in 
**                         remainder of parmlist
**         (4*n)+8(4,r1) = address of ECB
**         R13 @ standard MVS savearea
**         R14 @ return address
**         R15 @ entry address
**
**  Notes:
**       The ECBs must be cleared by the caller prior to calling us
**       The caller's parameter list is modified to make it a VL
**       parmlist for the WAIT SVC
**---------------------------------------------------------------------
         USING *,R15
         B     WAIT
         DC    AL1(WAITIDL)
WAITID   DC    C'MVSWAITN &SYSDATE &SYSTIME'      dump eyecatcher
WAITIDL  EQU   *-WAITID
WAIT     STM   R14,R12,12(R13)    save caller's regs
         DROP  R15
         LR    R12,R15
         USING MVSWAITN,R12
         LR    R8,R1              parmlist ptr to safer reg
         LR    R9,R13
         LA    R13,WAITSA
*
         L     R3,0(,R8)          # ECBs needed to awaken
         LTR   R3,R3
         BNZ   NOABEND
*
ABEND    ABEND 903
NOABEND  DS    0H
         L     R4,4(,R8)          # ECB addresses supplied
         LTR   R4,R4
         BZ    ABEND
         BCTR  R4,0               relative zero
         SLL   R4,2               byte index to last ECB addr
         LA    R14,8(R4,R8)       @ hi byte of last ECB addr
         OI    0(R14),X'80'       assure caller's parmlist VL
**
**  Issue MVS WAIT
**
         LA    R5,8(,R8)          @ 1st ECB addr
         WAIT  (R3),ECBLIST=(R5)  WAIT for MVS POST(s) of ECB(s)
**
**  Return to caller
**
         LR    R13,R9
         L     R14,12(,R13)       restore return address
         LR    R15,R8             debug: return parmlist addr
         LM    R0,R12,20(R13)     restore caller's regs
         BR    R14                return to caller
*
WAITSA   DC    18F'-1'
         LTORG ,
         PRINT NOGEN
         U#EQU ,                  GPR & FPR reg equates
         END   ,
