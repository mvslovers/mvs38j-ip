PROCLOAD CSECT ,
procload ALIAS C'procload'
**
**  Function: restore R0:R13 for caller
**
**  Entry: R1 -> parmlist:
**               0(4,r1) = pointer to pentry.gpr[0] new proc
**         R13 @ standard MVS savearea
**         R14 @ return address in resched()
**         R15 @ procload() entry address
**
         USING *,R15
         STM   R14,R12,12(R13)    save resched() regs (debug aid)
**
**  Restore process's regs
**
         L     R8,0(,R1)          @ proc's GPR slots (0:15)
         LA    R9,(16*4)(,R8)     @ new proc's FPRs
         LD    0,0(,R9)           restore new proc float regs 
         LD    2,8(,R9)           restore new proc float regs 
         LD    4,16(,R9)          restore new proc float regs
         LD    6,24(,R9)          restore new proc float regs
         LM    R0,R13,0(R8)       restore new proc regs
         SR    R15,R15            R15 never restored; = 0
**
**  Return to caller
**
         BR    R14                always returns to resched()
         U#EQU ,                  GPR & FPR reg equates
         END   ,

