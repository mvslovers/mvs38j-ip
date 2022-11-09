//XINUCHE  JOB CLASS=A,MSGCLASS=A,REGION=2048K
//*--------------------------------------------------------------------
//*  Assemble and linkedit MVS-Xinu Channel End Appendage (CHE)
//*--------------------------------------------------------------------
//ASM      EXEC PGM=IFOX00,REGION=2048K,
//         PARM='LIST,NOLOAD,DECK,NOXREF,NORLD,LINECOUNT(999)'
//SYSPRINT  DD SYSOUT=*
//SYSLIB    DD DSN=SYS1.AMACLIB,DISP=SHR,
//             DCB=BLKSIZE=32720
//          DD DSN=SYS1.AMODGEN,DISP=SHR
//          DD DSN=SYS1.AGENLIB,DISP=SHR
//SYSPUNCH  DD UNIT=SYSDA,SPACE=(TRK,(5,5)),DISP=(,PASS)
//SYSUT1    DD UNIT=SYSDA,SPACE=(CYL,(35,10))
//SYSUT2    DD UNIT=SYSDA,SPACE=(CYL,(35,10))
//SYSUT3    DD UNIT=SYSDA,SPACE=(CYL,(50,10))
//SYSIN     DD *
         PRINT OFF
::a arch/s370/mac/dsxgd.mac
::a arch/s370/mac/dscheapp.mac
::a arch/s370/mac/dsctdev.mac
::a arch/s370/mac/dsctche.mac
::a arch/s370/mac/dsssgd.mac
::a arch/s370/mac/stcp#opt.mac
::a arch/s370/mac/im#sect.mac
         PRINT ON
::a arch/s370/mvsasm/igg019x8.asm
//*--------------------------------------------------------------------
//*  Linkedit CHE
//*--------------------------------------------------------------------
//LKED     EXEC PGM=IEWL,COND=(5,LT),
//             REGION=2048K,
//             PARM='LET,MAP,XREF,LIST,RENT,REUS,REFR'
//SYSPRINT  DD SYSOUT=*
//SYSLMOD   DD DSN=SYS1.SVCLIB,DISP=SHR
//SYSLIN    DD DSN=*.ASM.SYSPUNCH,DISP=(OLD,DELETE,DELETE)
//          DD *
 PAGE IGG019X8               I AM SOOOO LAZY
 NAME IGG019X8(R)
//






