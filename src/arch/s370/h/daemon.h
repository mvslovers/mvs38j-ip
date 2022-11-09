/* daemon.h */

#ifndef	H_DAEMON_H
#define	H_DAEMON_H			// only generate once

/* FINGERD process info */

#define	FINGERD		fingerd		/* FINGERD process		*/
extern	PROCESS		FINGERD();
#define	FNGDSTK		2048		/* stack size for FINGERD	*/
#define	FNGDPRI		20		/* FINGERD priority		*/
#define	FNGDNAM		"fingerd"	/* name of FINGERD process	*/
#define	FNGDARGC	0		/* count of args to FINGERD	*/

/* ECHOD process info */

#define	ECHOD		echod		/* ECHOD daemon			*/
#define	ECHOP		echop		/* ECHO process			*/
extern	PROCESS		ECHOD(), ECHOP();
#define	ECHOSTK		1024		/* stack size for ECHOD		*/
#define	ECHOPRI		20		/* ECHOD priority		*/
#define	ECHODNAM	"echod"		/* name of ECHOD process	*/
#define	ECHOPNAM	"echop"		/* name of ECHOP process	*/

PROCESS		echod(void);
PROCESS		egp();
PROCESS		fingerd(void);
int		lfing(char *, int);
int		rwho();
PROCESS		rwhod(void);
PROCESS		rwhoind(void);
int		snmpd();
PROCESS		udpecho();

#endif
