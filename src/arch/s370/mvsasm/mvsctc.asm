MVSCTC   ALIAS C'mvsctc'
***********************************************************************
**
**  Function: MVS CTC I/O support
**  May not modify CTDEV, but may modify CTIOBLK; see <ctc.h>
**
**  Entry: R1 -> parmlist:
**               0(4,R1) = flag
**               4(4,R1) = address of ctioblk
**
**         R13 @ standard MVS savearea
**         R14 return address
**         R15 entry address
**
**  Notes:
**  Flag specifies the requested operation
**  Flag +00   unused
**  Flag +01   unused
**  Flag +02   desired CTC; 0=read, 1=write
**  Flag +03   operation (open, close, read, write, copy)
**             must be in sync with ctc.h
**
**  Register usage
**  R2   CTIOBLK addr
**  R3   parm register
**  R4   buffer addr
**  R5   buffer length
**  R6   ECB addr
**  R7   IOB addr
**  R8   DCB addr
**  R9   caller's SA addr
**  R12  program base
**  R13  our SA addr
***********************************************************************
MVSCTC   CSECT ,
         USING *,R15
         B     CTCCODE
         DC    AL1(CTCIDL)
CTCID    DC    C'MVSCTC &SYSDATE &SYSTIME'      dump eyecatcher
CTCIDL   EQU   *-CTCID
CTCCODE  STM   R14,R12,12(R13)         save regs
         DROP  R15
         LR    R3,R1                   safer parm reg
         LR    R12,R15
         USING MVSCTC,R12
         LR    R9,R13
         LA    R13,CTCSA
*
OPOPEN   EQU   1
OPCLOSE  EQU   2
OPREAD   EQU   3
OPWRITE  EQU   4
OPCOPY   EQU   5
*
         L     R2,4(,R1)               @ ctioblk; DSCTIO
         USING CTIOBLK,R2
         LA    R8,CIODCB               @ DCB (read or write)
         USING IHADCB,R8
*
         CLI   2(R3),0                 read operation?
         BNE   OPW
*
         CLI   3(R3),OPREAD
         BE    READ
         CLI   3(R3),OPOPEN
         BE    OPENR
         CLI   3(R3),OPCLOSE
         BE    CLOSE
         CLI   3(R3),OPCOPY
         BE    COPYR
*
OPW      DS    0H
         CLI   3(R3),OPWRITE
         BE    WRITE
         CLI   3(R3),OPOPEN
         BE    OPENW
         CLI   3(R3),OPCLOSE
         BE    CLOSE
         CLI   3(R3),OPCOPY
         BE    COPYW
*
         LA    R0,1                    reason code = invalid flag
         B     ABEND992
*----------------------------------------------------------------------
*  Copy read DCB, read CCW; returns DCB pointer
*----------------------------------------------------------------------
COPYR    DS    0H
         LA    R8,DCBREAD              DCB for read operations
         LA    R15,CIODCB              DCB target area; return value
         MVC   0(L$DCB,R15),0(R8)      copy DCB
         MVC   CIOCCW,CCWREAD          copy CCW
         B     EXIT
*----------------------------------------------------------------------
*  Copy write DCB, write CCW; returns DCB pointer
*----------------------------------------------------------------------
COPYW    DS    0H
         LA    R8,DCBWRITE             DCB for write operations
         LA    R15,CIODCB              DCB target area; return value
         MVC   0(L$DCB,R15),0(R8)      copy DCB
         MVC   CIOCCW,CCWWRITE         copy CCW
         B     EXIT
*----------------------------------------------------------------------
*  Open read DCB; returns DCB pointer
*----------------------------------------------------------------------
OPENR    DS    0H
         OPEN  ((R8),INPUT)
         LA    R0,2                    reason code = open fail
         TM    DCBOFLGS,DCBOFOPN       opened OK?
         BNO   ABEND992
         LR    R15,R8                  return DCB addr to caller
         B     EXIT
*----------------------------------------------------------------------
*  Open write DCB; returns DCB pointer
*----------------------------------------------------------------------
OPENW    DS    0H
         OPEN  ((R8),OUTPUT)
         LA    R0,2                    reason code = open fail
         TM    DCBOFLGS,DCBOFOPN       opened OK?
         BNO   ABEND992
         LR    R15,R8                  return DCB addr to caller
         B     EXIT
*----------------------------------------------------------------------
*  Close DCB; returns DCB pointer
*----------------------------------------------------------------------
CLOSE    DS    0H
         CLOSE ((R8),)
         LR    R15,R8                  return DCB addr to caller
         B     EXIT
*----------------------------------------------------------------------
*  Start the read; returns IOB pointer
*  Start the write; returns IOB pointer
*----------------------------------------------------------------------
READ     DS    0H
WRITE    DS    0H
         LM    R4,R6,CIOPARMS          @ buf, l' buf, @ ECB
         LA    R7,CIOIOB               @ IOB
         USING IOBSTDRD,R7
         STCM  R4,7,CIOCCW+1           CCW @ buffer
         STCM  R5,3,CIOCCW+6           CCW <- length of buffer
         XC    CIOIOB,CIOIOB           clear IOB for new request
         MVI   IOBFLAG1,IOBUNREL       unrelated to another EXCP
         STCM  R6,7,IOBECBPB           IOB -> ECB
         LA    R0,CIOCCW               CCW addr
         STCM  R0,7,IOBSTRTB           IOB -> channel program
         STCM  R8,7,IOBDCBPB           IOB -> DCB
         XC    0(4,R6),0(R6)           clear ECB
         EXCP  (R7)                    start the channel program
         LR    R15,R7                  return IOB addr to caller
*----------------------------------------------------------------------
*  Return to caller with R15 set
*----------------------------------------------------------------------
EXIT     LR    R13,R9
         L     R14,12(,R13)
         LM    R0,R12,20(R13)          restore caller regs
         BR    R14                     return to caller
**
ABEND992 ABEND 992                     kill MVS-Xinu; CTC related 
**
         LTORG ,
         DS    0D
CTCSA    DC    18F'-1'
         EJECT ,
SLI      EQU   X'20'                   suppress incorrect length
*----------------------------------------------------------------------
*  Read I/O control blocks
*----------------------------------------------------------------------
         DC    0D'0',CL8'DCBREAD'      DCB
DCBREAD  DCB   DSORG=PS,MACRF=E,DDNAME=CTCREAD,IOBAD=IOBRD,CENDA=X8
L$DCB    EQU   *-DCBREAD
***
         DC    0D'0',CL8'IOBREAD'      IOB
IOBRD    DC    XL(IOBEXTEN-IOBSTDRD)'00'
         DC    XL32'00'                experimental slop
***
         DC    0D'0',CL8'CCWREAD'      CCW (channel program)
CCWREAD  DC    X'02'                   opcode = unchained READ
CCW1@BUF DC    AL3(*-*)                buffer address
         DC    AL1(SLI)                flags
         DC    AL1(0)                  unused
CCW1LBUF DC    AL2(0)                  buffer length
***
         DC    0D'0',CL8'END-READ'
         EJECT ,
*----------------------------------------------------------------------
*  Write I/O control blocks
*----------------------------------------------------------------------
         DC    0D'0',CL8'DCBWRITE'     DCB
DCBWRITE DCB   DSORG=PS,MACRF=E,DDNAME=CTCWRITE,IOBAD=IOBWT,CENDA=X8
***
         DC    0D'0',CL8'IOBWRITE'     IOB
IOBWT    DC    XL(IOBEXTEN-IOBSTDRD)'00'
         DC    XL32'00'                experimental slop
***
         DC    0D'0',CL8'CCWWRITE'     CCW (channel program)
CCWWRITE DC    X'01'                   opcode = unchained WRITE
CCW2@BUF DC    AL3(*-*)                buffer address
         DC    AL1(0)                  flags
         DC    AL1(0)                  unused
CCW2LBUF DC    AL2(0)                  buffer length
***
         DC    0D'0',CL8'ENDWRITE'
         EJECT ,
*----------------------------------------------------------------------
*  Patch area
*----------------------------------------------------------------------
         PRINT DATA
PATCH    DC    8S(*)
         DSCTIO ,                      CTIOBLK
         PRINT NOGEN
         U#EQU ,                       GPR equates
         DCBD  DSORG=XE,DEVD=TA
         IHAECB ,
         IEZIOB ,
         END   ,

