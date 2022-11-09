//XINU    JOB  CLASS=A,MSGCLASS=A,REGION=3072K,TIME=(,45),COND=(5,LT)
//*--------------------------------------------------------------------
//COMPRESS EXEC PGM=IEBCOPY
//SYSPRINT  DD DUMMY
//LINKLIB   DD DSN=SYS2.LOCAL.LINKLIB,DISP=SHR
//SVCLIB    DD DSN=SYS1.SVCLIB,DISP=SHR
//SYSUT1    DD UNIT=SYSDA,SPACE=(CYL,(20,20))
//SYSUT2    DD UNIT=SYSDA,SPACE=(CYL,(20,20))
//SYSIN     DD *
  COPY INDD=LINKLIB,OUTDD=LINKLIB
//*--------------------------------------------------------------------
//DELETE   EXEC PGM=IEHPROGM
//SYSPRINT  DD DUMMY
//LKEDPRT   DD UNIT=3380,VOL=SER=MVS809,DISP=SHR
 SCRATCH DSNAME=JMM.XINU.LKEDPRT,VOL=3380=MVS809
 UNCATLG DSNAME=JMM.XINU.LKEDPRT 
//*--------------------------------------------------------------------
//*  APF authorize XINU if you don't wish to run it under TSO TEST
//*  If you want to run XINU under TSO TEST, you will require the 
//*  MAGIC SVC 245
//*  Update: SVC 97 barfs with the current code; never mind
//*--------------------------------------------------------------------
//LKED     EXEC PGM=IEWL,
//             REGION=1024K,
//             PARM='LET,MAP,LIST,TEST,XREF'
//SYSPRINT  DD DSN=JMM.XINU.LKEDPRT,
//             UNIT=3380,VOL=SER=MVS809,
//             DCB=(RECFM=FBA,LRECL=121,BLKSIZE=242),   2904
//             SPACE=(TRK,(15,15),RLSE),
//             DISP=(,CATLG)
//SYSLIB    DD DSN=SYS1.LINKLIB,DISP=SHR   LKED wants "something"
//SYSUT1    DD UNIT=VIO,SPACE=(1024,(50,50))
//SYSLMOD   DD DSN=SYS2.LOCAL.LINKLIB,DISP=SHR
//* SETCODE AC(1)
//SYSLIN    DD *
::e xinu.obj38
 SETCODE AC(1)
 PAGE @SC00001
 NAME XINU(R)
//*--------------------------------------------------------------------
//GO       EXEC PGM=XINU
//STEPLIB   DD DSN=SYS2.LOCAL.LINKLIB,DISP=SHR
//SYSPRINT  DD DUMMY SYSOUT=*
//STDOUT    DD DUMMY SYSOUT=*
//STDERR    DD DUMMY SYSOUT=*
//STDIN     DD DUMMY
//*
//CTC500    DD UNIT=500,DISP=OLD    IP 192.168.200.1     TUN/TAP
//CTC501    DD UNIT=501,DISP=OLD
//CTC502    DD UNIT=502,DISP=OLD    IP 10.10.10.1        TUN/TAP
//CTC503    DD UNIT=503,DISP=OLD
//CTC504    DD UNIT=504,DISP=OLD    IP 192.168.254.30    LAN
//CTC505    DD UNIT=505,DISP=OLD
//*
//CONFIG    DD *
* ------------------------------------------------------------------- *
*                 MVS-Xinu configuration for Turnkey 3
*  These definitions must match the Hercules configuration CTCIs
* ------------------------------------------------------------------- *
gateway disable
*
iface 0 type LOOP enable
iface 0 ip 127.0.0.1 mask 255.0.0.0
*
iface 1 type CTCI enable
iface 1 ip  192.168.100.1
iface 1 mask 255.255.255.0
iface 3 defaultgw 192.168.100.55
iface 1 read ddname CTC500 cuu 500
iface 1 write ddname CTC501 cuu 501
*
iface 2 type CTCI disable
iface 2 ip  192.168.200.1
iface 2 mask 255.255.255.0
iface 2 read ddname CTC502 cuu 502
iface 2 write ddname CTC503 cuu 503
*
iface 3 type CTCI disable
iface 3 ip  10.10.10.1
iface 3 mask 255.0.0.0
iface 3 read ddname CTC504 cuu 504
iface 3 write ddname CTC505 cuu 505
*
//*SYSUDUMP  DD SYSOUT=*
//SYSABEND  DD SYSOUT=*
//

