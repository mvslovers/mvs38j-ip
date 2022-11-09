MVSDOZE  CSECT ,
MVSDOZE  ALIAS C'mvsdoze'
MVSCLKI  ALIAS C'mvsclkinit'
         EXTRN MVSCLKI
** --------------------------------------------------------------------
**
**  Function: delay MVS-Xinu for caller-specified # of
**            hundredths of a second
**
**  Kludge to avoid running MVS out of WTO buffers
**  Called from kprintf via doprnt
**  Interacts with rest of our STIMER code in mvsclkinit
**
**  Entry: R1 -> parmlist:
**               0(4,r1) = # hundredths of a second to delay
**         R13 @ standard MVS savearea
**         R14 @ return address
**         R15 @ entry address
**
**  Exit:  R15 -> MICS and subsequent data (debug aid)
**
** --------------------------------------------------------------------
         USING *,R15
         B     DOZE
         DC    AL1(DOZEIDL)
DOZEID   DC    C'MVSDOZE &SYSDATE &SYSTIME'
DOZEIDL  EQU   *-DOZEID
DOZE     STM   R14,R12,12(R13)
         DROP  R15
         LR    R12,R15
         USING MVSDOZE,R12
         LR    R9,R13
         LA    R13,DOZESA
**---------------------------------------------------------------------
**  Verify caller's request conforms to our maximum
**---------------------------------------------------------------------
         L     R3,0(,R1)
         C     R3,MAXHHSEC        too long?
         BNH   *+8
         L     R3,MAXHHSEC        yes, use our max
         ST    R3,HHSECREQ        # hundredths sec caller wants
**---------------------------------------------------------------------
**  Test interval timer to see if someone else has timer running
**  (MVS-Xinu usually does); also cancels other STIMER if there is one
**---------------------------------------------------------------------
         TTIMER CANCEL,MIC,MICS   sets MICS non-zero if other STIMER
**---------------------------------------------------------------------
**  Issue MVS STIMER to put us asleep for caller-supplied duration
**---------------------------------------------------------------------
         STIMER WAIT,BINTVL=HHSECREQ
         SLR   R15,R15            zero rc
**
         B     NEWSTIMR           always do a timer pop
**
         SLR   R15,R15            assume no previous STIMER
         OC    MICS,MICS          previous STIMER we cancelled?
         BZ    EXIT               no, done
**---------------------------------------------------------------------
**  Restore STIMER that we previously cancelled
**  The TTIMER set MICS to the number of microseconds (millionth of
**  a second) remaining, with bit 51 being the low order bit
**  representing a microsecond
**  Apply a conversion factor to get MICS in hundredths of a second
**  as MVSCLKI requires, and call it to reset its STIMER
**---------------------------------------------------------------------
NEWSTIMR DS    0H
         LM    R4,R5,MICS         pick up doubleword = 64 bits
         SRDL  R4,(63-51)         shift bit 51 to bit 63 (12 bits)
         L     R7,FACTOR          conversion factor
         DR    R4,R7              convert microsecs to 100th secs
         LTR   R5,R5              at least one?
         BNZ   *+8                yes, br
         LA    R5,1               need at least something for
*                                 the STIMER to be reestablished
         ST    R5,HHSEC           hundredths seconds before timer pop
         LA    R1,HHSEC           # of hundredths of seconds to wait
         L     R15,=V(MVSCLKI)    mvsclkinit will set STIMER
         BALR  R14,R15            set STIMER was before CANCEL
         LA    R15,MICS           debug aid; return value
**---------------------------------------------------------------------
**  Return to caller
**---------------------------------------------------------------------
EXIT     LR    R13,R9
         L     R14,12(R13)
         LM    R0,R12,20(R13)
         BR    R14
**
MICS     DC    D'0'               microseconds remaining (at bit 51)
FACTOR   DC    A(1000000/100)     # millionth secs in hundredth sec
HHSECREQ DC    A(0)               # hh secs caller wants
HHSEC    DC    A(0)               delay time we request of MVSCLKI
MAXHHSEC DC    A(5*100)           max delay time
DOZESA   DC    18F'-1'
         LTORG ,
         PRINT NOGEN
         U#EQU ,
         END   ,




