MVSEVENT CSECT ,
MVSEVENT ALIAS C'mvsevent'
** --------------------------------------------------------------------
**
**  Function: SYSEVENT macro interface
**
**  Entry: R1 -> parmlist:
**               0(4,r1) = type of SYSEVENT to issue
**               1  = DONTSWAP
**               else OKSWAP
**         R13 @ standard MVS savearea
**         R14 @ return address
**         R15 @ entry address
**
**  Exit:  R15 whatever SYSEVENT returns 
**         The doc I have doesn't indicate a return code
**
**  Notes:
**
**  1)  Here's the specific reason MVS-Xinu runs DONTSWAP
**
**      During testing, we were issuing a _lot_ of debugging
**      messages, which we echo to the MVS console
**      One of the reasons we do that is because MVS WTO doesn't lose
**      the last few messages we issue (very important for debugging),
**      while the C library isn't quite so robust; that's life
**      One of the areas which MVS versions following MVS38j improved
**      upon was gracefully recovering after exhausting WTO buffers
**      Unfortunately, MVS38j isn't quite so graceful, and our 
**      address space is in pretty sad shape after it happens
**      It looks for all the world like we just stop running, but
**      that's something I've not looked into with any depth
**      Let me summarize: badness ensues
**
**      In an effort to keep from running MVS out of WTO buffers,
**      we implemented MVSDOZE which is called from doprnt, which gets
**      called from kprintf (the standard Xinu print routine)
**      It should be readily apparent from examining MVS_WTO_GOV
**      whether MVSDOZE gets called (it's currently an MVS-Xinu
**      Configuration option); GOV means governor, as in the
**      reason some rental trucks won't go more than 55 mph no matter
**      how hard you press on the gas pedal
**
**      MVSDOZE issues STIMER WAIT to put MVS-Xinu to sleep every 'n'
**      output messages, so the MVS console has a chance to catch up
**
**      Here's the problem DONTSWAP seems to solve: MVSDOZE seemed
**      to hang around output message 200 for quite awhile
**      I once waited to see how long and it turned out to be on 
**      the order of about 25 minutes wallclock!  Too long
**
**      In the interim, MVS MIH (Missing Interrupt Handler) woke up
**      and started complaining about the unsatisfied read on the
**      CTCI device; perhaps it's related, perhaps not (probably not)
**
**      Casual observation of the RB chain during this hang reveals
**      our issuance of STIMER (SVC x'2F'), followed by what looks
**      like SVC x'71' which is PGFIX/PGFREE/PGLOAD/PGOUT
**      My swag is that SRM is trying to page us out due to what 
**      probably seems like a request for a long wait (2 seconds
**      as I write this)
**      Disclosure: I didn't actually look at the SRM code to see
**      that this was really what was happening; it's merely my
**      best guess
**
**      Seems like an MVS bug to me, but Volker's Turnkey system,
**      where I'm doing development and testing, has pretty much
**      all the MVS38j maintenance known to man on it (and perhaps
**      just a trifle more), so there's not much chance we're going 
**      to find an IBM fix and its not something I care to dig into
**
**      Therein lies our tale of DONTSWAP
**
** --------------------------------------------------------------------
         USING *,R15
         B     EVENT
         DC    AL1(EVENTIDL)
EVENTID  DC    CL8'MVSEVENT'
         DC    CL8'&SYSDATE'
         DC    CL7'&SYSTIME'
EVENTIDL EQU   *-EVENTID
EVENT    STM   R14,R12,12(R13)
         DROP  R15
         LR    R12,R15
         USING MVSEVENT,R12
         LR    R9,R13
         LA    R13,EVENTSA
**---------------------------------------------------------------------
         SPKA  0                  SYSEVENT requires key 0
         CLI   3(R1),1            DONTSWAP?
         BE    DONTSWAP
**---------------------------------------------------------------------
**  OKSWAP
**---------------------------------------------------------------------
OKSWAP   DS    0H
         SYSEVENT OKSWAP
         B     EXIT
**---------------------------------------------------------------------
**  DONTSWAP
**---------------------------------------------------------------------
DONTSWAP DS    0H
         SYSEVENT DONTSWAP
**---------------------------------------------------------------------
**  Return to caller
**---------------------------------------------------------------------
EXIT     DS    0H
         SPKA  8*16               back to key 8
         LR    R13,R9
         L     R14,12(R13)
         LM    R0,R12,20(R13)
         BR    R14
**
EVENTSA  DC    18F'-1'
         LTORG ,
         PRINT NOGEN
         U#EQU ,
         CVT   DSECT=YES,LIST=NO
         END   ,

