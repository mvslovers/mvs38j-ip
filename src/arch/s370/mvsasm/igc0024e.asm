*
*  magic svc T3
*
IGC0024E CSECT ,       REGISTER ENTRY CONTENTS
R0       EQU   0       as per time of SVC; returned to SVC issuer
R1       EQU   1       as per time of SVC; returned to SVC issuer
R2       EQU   2       unpredictable
R3       EQU   3       @ CVT
R4       EQU   4       @ TCB
R5       EQU   5       @ SVRB (SVC types 2, 3, 4)
R6       EQU   6       @ entry point
R7       EQU   7       @ ASCB
R8       EQU   8       unpredictable
R9       EQU   9       unpredictable
R10      EQU   10      unpredictable
R11      EQU   11      unpredictable
R12      EQU   12      unpredictable
R13      EQU   13      as per time of SVC
R14      EQU   14      return address
R15      EQU   15      as per time of SVC; returned to SVC issuer
         USING TCB,R4
         USING IGC0024E,R6
         B     BEGIN
         DC    AL1(IDL)
ID       DC    CL8'IGC0024E'                SVC 245
         DC    CL8'&SYSDATE'
         DC    CL7'&SYSTIME'
IDL      EQU   *-ID
AUTHFLD  DC    CL8'AUTHSVC'
*
BEGIN    L     R8,TCBJSCB                   @ JSCB
         USING IEZJSCB,R8
*
         CL    R0,AUTHFLD                   R0=C'AUTH' -> APF AUTH ON
         BNE   AUTHOFF
*
AUTHON   OI    JSCBOPTS,JSCBAUTH            JSCBAUTH = APF AUTH'D
         SLR   R15,R15
         BR    R14
*
AUTHOFF  NI    JSCBOPTS,255-JSCBAUTH        JSCBAUTH = NOT APF AUTH'D
         LA    R15,4
         BR    R14
*
         PRINT NOGEN
         IKJTCB
         IEZJSCB
         END   IGC0024E
