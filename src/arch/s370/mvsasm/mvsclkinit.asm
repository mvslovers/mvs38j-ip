
         GBLB  &WTO,&WTOSE,&CALLC

&WTO     SETB  0                  1 = issue status WTOs
&WTOSE   SETB  0                  1 = STIMER established WTO
&CALLC   SETB  0                  1 = MVSTMPOP call clkint.CLKRESUM

         MACRO
&LABEL   GETVCON &REG,&VCON
&LABEL   L     &REG,=V(&VCON)          @ VCON
         LTR   &REG,&REG               unresolved VCON?
         BNZ   *+6                     no, continue
         BALR  R14,R3                  yes, abend MVS-Xinu
         MEND

         MACRO
&LABEL   CLOCKWTO &TEXT
         GBLB  &WTO
         AIF   (not &WTO).DONE
&LABEL   WTO   &TEXT
.DONE    MEND

MVSCLKI  ALIAS C'mvsclkinit'

slnempty ALIAS C'slnempty'	int slnempty
         EXTRN slnempty
sltop    ALIAS C'sltop'		int *sltop
         EXTRN sltop
hhsec    ALIAS C'hhsec'		int hhsec
         EXTRN hhsec
nullecb  ALIAS C'nullecb'	WORD nullecb
         EXTRN nullecb
mvsstimer ALIAS C'mvsstimer'	int mvsstimer
         EXTRN mvsstimer

         AIF   (&CALLC).EXTCALL
preempt  ALIAS C'preempt'	int preempt
         EXTRN preempt
         AGO   .EXTDONE
.*
.EXTCALL ANOP
clkint   ALIAS C'clkint'	clkint()
         EXTRN clkint
pclkint  ALIAS C'pclkint'	clkint() regs R0:R15
         EXTRN pclkint
.*
.EXTDONE ANOP

***********************************************************************
**
**  Function: establish MVS STIMER for MVS-Xinu clock management
**
**  Entry: R1 -> parmlist:
**               0(4,R1) = fullword containing
**                         number of hundredths of seconds
**                         after which timer pop is to be
**                         scheduled by MVS
**
**         R13 @ standard MVS savearea
**         R14 return address
**         R15 entry address
**
**  Notes:
*        MVSTMPOP calls us with R1 = 0 to reestablish the STIMER
***********************************************************************
MVSCLKI  CSECT ,
         USING *,R15
         B     CLOCK
         DC    AL1(CLKIDL)
CLKID    DC    C'MVSCLKI &SYSDATE &SYSTIME'      dump eyecatcher
CLKIDL   EQU   *-CLKID
CLOCK    STM   R14,R12,12(R13)    save regs
         DROP  R15
         LR    R12,R15
         USING MVSCLKI,R12
         LR    R5,R13
         LA    R13,CLKISA
         LR    R9,R1
         L     R3,=V(ABEND902)
         GETVCON R4,hhsec              @ int hhsec
         L     R2,0(,R4)               assume global hhsec desired
         LTR   R9,R9                   caller provided parmlist?
         BZ    *+8                     no, br
         L     R2,0(,R9)               caller-provided hhsec
         LTR   R2,R2                   hhsec value zero?
         BNZ   *+8
         LA    R2,100                  default sec/100s
         ST    R2,0(,R4)               echo hhsec back to C code
**
**  Establish STIMER exit
**
**  REAL       The interval is decreased continuously, even when
**             our task is not executing
**  (R6)       Address of the routine to receive control
**             asynchronuously when interval expires
**  BINTVL     Fullword containing interval; 32 bit binary number
**             where low-order bit has value of 0.01 seconds
**             We don't require more time resolution than that
**
         GETVCON R6,MVSTMPOP           @ code MVSTMPOP
         STIMER REAL,(R6),BINTVL=(R4)  request timer pop exit
**
         AIF   (not &WTOSE).NOSE
         CLOCKWTO '****** MVSCLKINIT ****** STIMER established'
.NOSE    ANOP
**
         GETVCON R9,mvsstimer          @ int mvsstimer
         LA    R0,1
         ST    R0,0(,R9)               STIMER active
**
**  Return to caller
**
CLKIEXIT LR    R13,R5
         LM    R14,R12,12(R13)         restore caller regs
         BR    R14                     return to caller
**
CLKISA   DC    18F'-1'
         LTORG ,
         DROP  ,

         EJECT ,
***********************************************************************
**
**  Function: handle STIMER timer pop
**
**  Entry: R0-R1   unknown, control program information
**         R2-R12  unpredictable
**         R13     @ standard MVS savearea
**         R14     @ return address
**         R15     @ entry address
**
***********************************************************************
MVSTMPOP CSECT ,
         USING *,R15
         B     POP
         DC    AL1(POPIDL)
POPID    DC    C'MVSTMPOP &SYSDATE &SYSTIME'     dump eyecatcher
POPIDL   EQU   *-POPID
POP      STM   R14,R12,12(R13)    save regs
         DROP  R15
         LR    R12,R15
         USING MVSTMPOP,R12
         LR    R5,R13             save caller's SA
         LA    R13,POPSA          no back linkage; keep dump terse
         L     R3,=V(ABEND902)
         SPKA  8*16               go to USER protect key for safety
         CLOCKWTO '****** MVSTMPOP ****** TICK'
*----------------------------------------------------------------------
*  Decrement int *sltop
*  sltop points at qkey of next-to-awaken proc's sleep queue entry
*  PC-Xinu clkint calls wakeup & resched because PC-Xinu preemptively
*  reschedules work; MVS-Xinu doesn't
*  MVS-Xinu requires resched to call wakeup, since it's not done here
*----------------------------------------------------------------------
         GETVCON R9,slnempty           V(slnempty)
         L     R8,0(,R9)               slnempty
         LTR   R8,R8                   slnempty == 0?
         BZ    NOSLTOP                 yes, no sleep queue entries
         LA    R0,1
         GETVCON R9,sltop              V(sltop)
         L     R9,0(,R9)               sltop = @ qkey
         L     R8,0(,R9)               qkey
         SR    R8,R0                   qkey--
         ST    R8,0(,R9)
NOSLTOP  DS    0H
.*
         AIF   (&CALLC).CALLC
*----------------------------------------------------------------------
*  &CALLC is set 0, we don't call the clkint.CLKRESUM code
*  The old code at CALLINT is preserved in case I change my mind
*  Currently, CLKRESUM doesn't work very well
*----------------------------------------------------------------------
         GETVCON R9,preempt
         LA    R0,1
         L     R8,0(,R9)
         SR    R8,R0                   preempt--
         ST    R8,0(,R9)
         AGO   .NEWPOP
.*
.CALLC   ANOP
*----------------------------------------------------------------------
*  If &CALLC is set 1, call the clkint.CLKRESUM code
*----------------------------------------------------------------------
         CLOCKWTO '****** MVSTMPOP ****** enter, CALLING clkint()'
**
**  Call clkint() to field the timer pop
**
CALLINT  GETVCON R9,CLKRESUM
         LA    R0,16              prefix length
         SR    R9,R0              back up to prefix
         CLC   =C'CLKRESUM',0(R9) correct eyecatcher?
         BNE   ABEND902
         L     R15,8(,R9)         @ clkint.CLKRESUM code
         LTR   R15,R15            uninitialized ptr?
         BZ    ABEND902
         L     R8,12(,R9)         @ clkint.CLKSAVE regs
         LTR   R8,R8              uninitialized ptr?
         BZ    ABEND902
*
         STM   R0,R15,POPREGS     save our regs before call
         LM    R0,R13,0(R8)       restore clkint's regs
         BALR  R14,R15            call clkint.CLKRESUM
         BALR  R4,0               reestablish base reg
         USING *,R4
         LM    R0,R14,POPREGS     restore our R0:R14
         DROP  R4
         CLOCKWTO 'MVSTMPOP ****** clkint() RETURNED ******'
.NEWPOP  ANOP
*----------------------------------------------------------------------
*  If nulluser is waiting, POST it's ECB (nullecb)
*----------------------------------------------------------------------
         GETVCON R7,nullecb       V(nullecb)
         TM    0(R7),ECBWAIT      WAITing to be POSTed?
         BNO   NOPOST1            no, no POST desired
         CLOCKWTO 'MVSTMPOP ****** nullecb WAIT'
         TM    0(R7),ECBPOST      WAITING but already POSTED?
         BO    NOPOST2            yes, skip redundant POST
         CLOCKWTO 'MVSTMPOP ****** nullecb NOT POST ******'
         LA    R9,105             0 =< POST code < 2**24
         POST  (R7),(R9)          wake up nulluser()
         CLOCKWTO 'MVSTMPOP ****** POST ECB ****** POSTED'
         B     POSTFIN
NOPOST1  DS    0H
         CLOCKWTO 'MVSTMPOP ****** nullecb NOT WAIT'
         B     POSTFIN
NOPOST2  DS    0H
         CLOCKWTO 'MVSTMPOP ****** nullecb ALREADY POSTED'
POSTFIN  DS    0H
*----------------------------------------------------------------------
*  Establish another STIMER request
*----------------------------------------------------------------------
         SLR   R1,R1              use whatever's in hhsec
         GETVCON R15,MVSCLKI
         BALR  R14,R15            request new STIMER
*----------------------------------------------------------------------
*  Return to caller
*----------------------------------------------------------------------
POPEXIT  LR    R13,R5
         LM    R14,R12,12(R13)    restore caller regs
         SPKA  0*16               back to SYSTEM protect key
         BR    R14                return to caller
**
         DS    0D
         AIF   (not &CALLC).NOPR
         DC    CL8'POPREGS'
POPREGS  DC    16F'-1'            our regs across clkint()
.NOPR    ANOP
         DC    CL8'POPSA'
POPSA    DC    18F'-1'            our savearea
         LTORG ,
         DROP  ,

         EJECT ,
***********************************************************************
**
**  Function: Cancel STIMER
**
**  Entry: R1      unpredictable
**         R13     @ standard MVS savearea
**         R14     @ return address
**         R15     @ entry address
**
***********************************************************************
MVSTMOFF CSECT ,
MVSTMOFF ALIAS C'mvsclkoff'
         USING *,R15
         B     OFF
         DC    AL1(OFFIDL)
OFFID    DC    C'MVSTMOFF &SYSDATE &SYSTIME'     dump eyecatcher
OFFIDL   EQU   *-OFFID
OFF      STM   R14,R12,12(R13)         save regs
         DROP  R15
         LR    R12,R15
         USING MVSTMOFF,R12
         LR    R5,R13                  save caller's SA
         LA    R13,OFFSA               no back linkage; keep dump terse
         L     R3,=V(ABEND902)
**
**  Cancel outstanding STIMER, if there is one
**
         GETVCON R6,mvsstimer
         L     R0,0(,R6)               non-zero = STIMER active
         LTR   R0,R0                   STIMER active?
         AIF   (&WTO).SKIP1
.*  No WTO desired:
         BZ    OFFEXIT                 no, nothing to do; br
         AGO   .SKIPFIN
.*  WTO desired:
.SKIP1   ANOP  ,
         BNZ   CANCEL                  yes, br
         WTO   '****** MVSTMOFF no STIMER outstanding ******'
         B     OFFEXIT
.SKIPFIN ANOP  ,
**
CANCEL   TTIMER CANCEL                 disable timer pop exit
         SLR   R0,R0
         ST    R0,0(,R6)               mvsstimer = 0;
         AIF   (not &WTO).OFFNWTO
         WTO   '****** MVSTMOFF cancelled STIMER ******'
.OFFNWTO ANOP   ,
**
**  Return to caller
**
OFFEXIT  LR    R13,R5
         LM    R14,R12,12(R13)         restore caller regs
         BR    R14                     return to caller
OFFSA    DC    18F'-1'                 our savearea
         LTORG ,
         DROP  ,

         EJECT ,
***********************************************************************
*  Unresolved VCONs, abend MVS-Xinu
*  Entry: R3   entry point
*         R14  debug aid; return address
***********************************************************************
ABEND902 CSECT ,
         USING *,R3
         WTO   'MVS-Xinu install error; unresolved VCON(s)'
         ABEND 902                     kill MVS-Xinu; unresolved VCONs

CLKPATCH CSECT ,
         PRINT DATA
PATCH    DC    8F'-1'
         PRINT NOGEN
         U#EQU ,                       GPR equates
         IHAECB ,                      Event Control Block
         END   ,





