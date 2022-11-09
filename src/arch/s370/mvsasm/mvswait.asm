MVSWAIT  CSECT ,
MVSWAIT  ALIAS C'mvswait'
**
**  Function: wait on a single MVS ECB
**
**  Entry: R1 -> parmlist:
**               0(4,r1) = pointer to ECB
**         R13 @ standard MVS savearea
**         R14 @ return address
**         R15 @ entry address
**
         USING *,R15
         B     WAIT
         DC    AL1(WAITIDL)
WAITID   DC    C'MVSWAIT &SYSDATE &SYSTIME'      dump eyecatcher
WAITIDL  EQU   *-WAITID
WAIT     STM   R14,R12,12(R13)    save caller's regs
         DROP  R15
         LR    R12,R15
         USING MVSWAIT,R12
         LR    R5,R13
         LA    R13,WAITSA
**
         L     R3,0(,R1)          @ ECB
         LTR   R3,R3
         BNZ   NOABEND
         BALR  R14,0              debug aid
         ABEND 903
NOABEND  DS    0H
**
**  Issue MVS WAIT
**
         XC    0(4,R3),0(R3)      clear ECB before wait
         WAIT  1,ECB=(R3)         WAIT for MVS POST of ECB
**
**  Return to caller
**
         LR    R13,R5
         LM    R14,R12,12(R13)    restore caller's regs
         SLR   R15,R15            zero return code
         BR    R14                return to caller
**
WAITSA   DC    18F'-1'
         LTORG ,
         PRINT NOGEN
         U#EQU ,                  GPR & FPR reg equates
         END   ,


