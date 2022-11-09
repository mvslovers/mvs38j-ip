PROCSAVE CSECT ,
procsave ALIAS C'procsave'
**  Entry: R1 -> parmlist
**               0(4,r1) = pointer to pentry.gpr[0]
**         R13 @ standard MVS savearea
**         R14 @ return address in resched()
**         R15 @ procsave() entry address
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
**  Return to caller
**
         LM    R14,R12,12(R13)    restore resched() regs
         BR    R14                always returns to resched()
         U#EQU ,                  GPR & FPR reg equates
         END   ,

