PROCSW   CSECT ,
procsw   ALIAS C'procsw'
**  Entry: R1 -> parmlist:
**               0(4,r1) = pointer to pentry.gpr[0] old proc
**               4(4,r1) = pointer to pentry.gpr[0] new proc
**         R13 @ standard MVS savearea
**         R14 @ return address in resched()
**         R15 @ procsw() entry address
**
         USING *,R15
         STM   R14,R12,12(R13)    save resched() regs
         L     R2,0(,R1)          @ old proc's GPR slots (0:15)
**
**  Save old process regs
**
         STM   R0,15,0(R2)        save old proc's regs in old pentry
         L     R0,28(,R13)        retrieve old R2 value
         ST    R0,8(,R2)          save R2 in pentry GPR slot
         LA    R7,(16*4)(,R2)     @ old proc's FPRs
         STD   0,0(,R7)           save old proc float regs
         STD   2,8(,R7)           save old proc float regs
         STD   4,16(,R7)          save old proc float regs
         STD   6,24(,R7)          save old proc float regs
**
**  Restore new process regs
**
         L     R8,4(,R1)          @ new proc's GPRs
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

