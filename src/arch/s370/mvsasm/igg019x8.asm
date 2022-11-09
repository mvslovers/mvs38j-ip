*
         MACRO
&LABEL   BUGDIE ,
&LABEL   EX    0,*                ABENDS0C3 DEBUG
         MEND
*
         MACRO
&LABEL   DIEREG &REG
&LABEL   LTR   &REG,&REG
         BNZ   *+8
         EX    0,*                ABENDS0C3 DEBUG
         MEND
*
         TITLE ' '
*----------------------------------------------------------------------
*
*  MVS38j TCP/IP 3088 (CTC) Channel End Appendage (CHE APP)
*
*  Attributes:  RENT, LPALIB/SVCLIB resident
*  JCL:         src/arch/s370/jcl/linkche.jcl
*  IBM doc:     OS/VS2 SPL: Data Management
*
*  Entry:
*    Supervisor state
*    R1  @ RQE
*    R2  @ IOB
*    R3  @ DEB
*    R4  @ DCB
*    R7  @ UCB
*    R13 address of 16-word work area for our use
*    R14 return address
*        +0   Normal
*             Channel program posted complete, RQE made available
*        +4   Skip
*             Channel program not posted complete, but
*             RQE made available
*        +8   Re-EXCP
*             Channel program not posted complete, RQE placed
*             back on request queue for retry (IOB will
*             require changes before retry)
*       +12   Bypass
*             Channel program not posted complete, RQE
*             not made available
*    R15 appendage entry point
*  Restrictions:
*    o  Load module name IGG019xx; xx in range WA to Z9
*    o  R9, if used, must be set to binary zero before returning
*       control to the system
*    o  No SVCs or instructions that change the state of the
*       system may be issued (ex: WTO, LPSW)
*    o  No loops that test for I/O completion
*    o  No alteration of supervisor or IOS storage
*  Available work regs:  R10, R11, R12, R13
*       These regs do not need to be saved and restored prior
*       to returning to the system
*  Notes:
*       MVS38j actually seems to cope pretty well with an abend
*       in a Channel End Appendage (ABENDSA00).  I was half-
*       expecting MVS to fall over, but not so.  In the Hercules
*       environment, it's much simpler to just cause a S0C3 and
*       look at the Hercules console for the failing instruction
*       (with regs and PSW) than to dig through a dump, so that's
*       what we use for our semi-permanent "debug" code.
*
*       During development, IGG019X8 is linked page aligned to make
*       the ABENDS0C3 offset calculation easy; it's not necessary
*       once the code is "production"
*----------------------------------------------------------------------
IGG019X8 CSECT ,
R0       EQU   0       spare
R1       EQU   1       @ RQE
R2       EQU   2       @ IOB
R3       EQU   3       @ DEB
R4       EQU   4       @ DCB
R5       EQU   5
R6       EQU   6
R7       EQU   7       @ UCB
R8       EQU   8
R9       EQU   9       @ CTDEV (ctdev.h); zeroed before return
R10      EQU   10      work
R11      EQU   11      work
R12      EQU   12      base
R13      EQU   13      @ 16-word workarea or CTCHE
R14      EQU   14      return address
R15      EQU   15
*
W1       EQU   10      generic work reg
W2       EQU   11      generic work reg
*
         USING IGG019X8,R15
         B     BEGIN
         DC    AL1(IDL)
ID       DC    CL8'IGG019X8'
         DC    CL8'&SYSDATE'
         DC    CL7'&SYSTIME'
IDL      EQU   *-ID
BEGIN    DS    0H
*----------------------------------------------------------------------
*  Senseless skepticism that IOS passed us decent regs
*----------------------------------------------------------------------
         DIEREG R1
         DIEREG R2
         DIEREG R3
         DIEREG R4
         DIEREG R7
         DIEREG R13
         DIEREG R14
*        DIEREG R15                    _has_ to be right or we're dead
*----------------------------------------------------------------------
*  Save caller's regs, setup base & parm reg DSECTs
*----------------------------------------------------------------------
         DROP  R15
         USING CED,R13                 CHE APP workarea; DSCHEAPP
         STM   R14,R13,CEDR14          save all caller regs
         LR    R12,R15
         USING IGG019X8,R12
         USING UCBCMSEG,R7             UCB common segment
         USING DEBBASIC,R3             DEB basic section
         USING PSA,0                   MVS low storage
*----------------------------------------------------------------------
*  Locate SGD
*  Exit W1 available, W2 -> SGD
*----------------------------------------------------------------------
*
*  Run the subsys chain each time, so we get most recent SGD pointer
*  (in case the TCP subsystem has restarted since we last found it)
*
         L     W1,CVTPTR               @ CVT
         L     W1,CVTJESCT-CVT(,W1)    @ JESCT
         L     W1,JESSSCT-JESCT(,W1)   @ 1st SSCVT (JES2)
         USING SSCT,W1
SSLOOP   DS    0H
         CLC   SSCTSNAM,SSNAME         same subsys name?
         BE    SSFND                   found subsys
         ICM   W1,15,SSCTSCTA          @ next subsys
         BNZ   SSLOOP
*
         BUGDIE ,                      subsystem not found
*
SSFND    DS    0H                      W1 -> TCP's SSCT
         L     W2,SSCTSUSE-SSCT(,W1)   @ SGD or null
         ST    W2,CED@SGD              debug: save SGD addr
*
         DIEREG W2                     SGD not present (SS not up)
         USING SGD,W2
         CLC   =C'SGD',SGDID           eyecatcher OK?
         BE    HUNTXGD                 yes, go find XGD
         BUGDIE ,                      SGD eyecatcher wrong
*----------------------------------------------------------------------
*  Locate XGD
*  Entry W1 available, W2 -> SGD
*  Exit W1 -> XGD, W2 available
*----------------------------------------------------------------------
HUNTXGD  DS    0H
         L     W1,SGD@XGD              zero or @ XGD
         DROP  W2
         ST    W1,CED@XGD              debug: save XGD addr
         DIEREG W1                     XGD missing
         USING XGD,W1
         CLC   =C'XGD',XGDEYE          verify xgd eyecatcher
         BE    SAVR13
         BUGDIE ,                      xgd eyecatcher invalid
SAVR13   ST    R13,XGDCHEAP            debug: R13 -> xgd.cheappr13
*----------------------------------------------------------------------
*  Run the xgd.ctclink chain to find the ctdev associated with the
*  channel end for the ctdev that we are being called to support
*  Entry W1 -> XGD, W2 available
*  Exit R9 -> CTDEV, W1 available, W2 available
*----------------------------------------------------------------------
*  Be careful with R9; since we use it here, it must be zeroed 
*  before returning to the caller.  IBM requires it thus.
*  When we break here, it's usually due to a bug in ctci_init
*  Debug hints: 
*      R7 -> UCB, UCB+4(2) = CUU for which we're searching
*      W1 = R10, W2 = R11
*
         L     R9,XGDCHELK             @ 1st ctdev
         DROP  W1
         USING CTDEV,R9                ctdev <ctc.h>
         LTR   R9,R9
         BNZ   DEVCK
         BUGDIE ,                      xgd.ctclink zero
*
DEVLOOP  DS    0H
         DIEREG R9                     UCB not represented by ctdev
*
DEVCK    DS    0H
         CLC   UCBCHAN,CTDCUU          found ctdev for CTC?
         BE    DEVFND                  yes, br
*
         L     R9,CTD@NEXT             @ next ctdev or null
         B     DEVLOOP
*
DEVFND   DS    0H
*----------------------------------------------------------------------
*  Having found the CTDEV, switch from our dinky little savearea
*  that IOS provided us to CTCHE, an area provided us by the
*  ctci_ioread or ctci_iowrite Xinu processes
*  The main reasons for CTCHE are that it's easy to find in a SYSUDUMP
*  and is large enough to save extra debug info
*  Entry R9 -> CTDEV, W1 available, W2 available, R13 -> IOS' savearea
*  Exit  R9 -> CTDEV, W1 available, W2 available, R13 -> CTCHE
*----------------------------------------------------------------------
         LR    W1,R13                 debug aid
         L     W2,CTD@WORK            ctdev.chework -> ctche
         DIEREG W2                    ouch, no ctche
         LR    R13,W2
         USING CTCHE,R13
*----------------------------------------------------------------------
*  Having found CTDEV and CTCHE, update them
*  Entry R9 -> CTDEV, W1 available, W2 available, R13 -> CTCHE
*  Exit  R9 -> CTDEV, W1 available, W2 available, R13 -> CTCHE
*----------------------------------------------------------------------
         STM   R0,R15,CHEREGS          these regs restored before exit
         OI    CTDCHEF1,CTDC1FDC       debug: found CTDEV
         ST    R9,CHE@CTD              @ related CTDEV
         ST    R7,CHE@UCB              @ related UCB
         ST    R4,CHE@DCB              @ related DCB
         ST    R3,CHE@DEB              @ related DEB
         ST    R2,CHE@IOB              @ related IOB
         ST    R1,CHE@RQE              @ related RQE
*
*  Note CHEEYE will contain DSCTCHE, which is a different eyecatcher
*  than ctche.cheid initially contained (CTCHE); this makes it easy 
*  for us to tell if IGG019X8 got at least this far
*
         MVC   CHEEYE+00(8),=CL8'DSCTCHE'
         MVC   CHEEYE+08(8),=CL8'DSCTCHE'
         MVC   CHECHEX+00(8),=CL8'DSCTCHE'
         MVC   CHECHEX+08(8),ID        save CSECT name
         MVC   CHECHEX+16(8),ID+8      save assemble date
         MVC   CHECHEX+24(8),ID+16     save assemble time
*----------------------------------------------------------------------
*  Post I/O completion where MVS-Xinu can find it
*  Entry R9 -> CTDEV, W1 available, W2 available, R13 -> CTCHE
*  Exit  R9 -> CTDEV, W1 available, W2 available, 
*        R12 trashed, R13 -> CTCHE
*----------------------------------------------------------------------
*
*  Don't forget: W1 = R10, W2 = R11
*
*  Setting the following bit causes iohook to awaken the related CTC
*  process, which will (if there are I/Os waiting to be started)
*  start the next CTC I/O
*
         OI    CTDFLG1,CTDF1IOC        indicate I/O complete
*
*  MVS38j branch-entry POST of ECB in CTDEV
*  Branch-entry POST is described in OS/VS2 SPL: Data Management
*  under the CHE & SIO Appendage doc
*
*  Regs at entry to branch-entry POST:
*     R10 = ECB completion code = address of related CTDEV
*     R11 = ECB address
*     R12 = TCB address
*     R14 = return address
*     R15 = branch-entry POST entry point
*
*  Note we trash R12 (our base reg) but it doesn't really matter;
*  the code doesn't need a base reg before returning to caller
*
         DROP  R12                     in case we forget
POST     DS    0H
         LA    R11,CTDECB              @ CTC ECB
         OI    CTDCHEF1,CTDC1P1        debug: begin POST
         L     R15,CVTPTR              @ CVT
         L     R15,CVT0PT01-CVT(,R15)  @ branch-entry POST
         LR    R10,R9                  POST code = @ CTDEV
         L     R12,DEBTCBAD            @ TCB from DEB
         LA    R12,0(,R12)             clean hi byte
         BALR  R14,R15                 branch enter POST
         OI    CTDCHEF1,CTDC1P2        debug: end POST
*----------------------------------------------------------------------
*  Return to system
*  Entry R12 trashed, R13 -> CTCHE
*----------------------------------------------------------------------
*
*  Since we use the +0 return, the channel program is posted
*  complete (IOB ECB), and the RQE is made available (i.e., we don't
*  leak storage).  At least that's what I hope "available" means.
*
*  We retore R0-R1 in case later versions of this code wants to
*  use them 
*  We haven't modified R2-R8
*  If we got this far we've used R9 and it must be zeroed
*  before we return to our caller
*  It is not necessary for us to restore R10-R13 before returning
*  We restore R14-R15 before returning; they were used for POST
*
         LM    R0,R1,CHER0             restore possibly modified regs
         LM    R14,R15,CHER14          restore possibly modified regs
         SR    R9,R9                   show we changed R9
         BR    R14                     normal +0 return address
*
*  While it might seem redundant to both manually POST the MVS-Xinu
*  ECB (pointed to by CTDEV), and then let IOS POST the IOB ECB,
*  there's a "reason" we do it this way.
*  The MVS-Xinu ECB is the one being waited on, and (currently)
*  indicates any CTC I/O completion.
*  While the IOB ECB isn't (currently) being waited on, it's 
*  handy to have IOS provide a descriptive POST code for the I/O,
*  and the +0 return handily provides that
*
*----------------------------------------------------------------------
*  Data areas
*----------------------------------------------------------------------
SSNAME   DC    CL4'TCP'                MVS38j TCP/IP subsystem name
         LTORG ,
*----------------------------------------------------------------------
*  Beginning of XGD reserved for CHE APP; see also xgd.h
*----------------------------------------------------------------------
         DSXGD ,                       MVS-Xinu XGD
*
         DSCHEAPP ,                    CHE APP workarea
*
         DSCTDEV ,                     <ctc.h> ctdev equivalent
*
         DSCTCHE ,                     <ctc.h> ctche equivalent
*
         DSSSGD ,                      MVS38j TCP/IP SGD
         EJECT ,
         PRINT NOGEN
         IECDRQE ,
         IEZIOB ,
         IEZDEB ,
         DCBD  DSORG=XE,DEVD=TA
         IEFUCBOB ,
         IHAPSA ,
         CVT   DSECT=YES,LIST=NO
         IEFJESCT ,
         IEFJSCVT ,
         IEFJSSVT ,
         END   IGG019X8
