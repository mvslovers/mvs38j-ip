/* config.h - MVS-Xinu startup configuration */

//  See xconfig.c, xfig*.c

#ifndef H_CONFIG_H
#define H_CONFIG_H

//------------------------------------------------------------------------------config.h

#define	MAX_DNS		2	// maximum # DNS servers 
#define	MAX_IFACE	4	// maximum # network interfaces

struct	ctchalf	{
	char	ddname[8];	// device JCL DDNAME; right blank padded
	char	null;		// acts as null termination for ddname
	char	spare;
	short	cuu;		// device unit address
	char	more[4];
};
typedef struct ctchalf		ctchalf;

struct	figctc	{
	ctchalf	read;		// CTC: read half of ctc
	ctchalf	write;		// CTC: write half of ctc
};
typedef struct figctc		figctc;

#define	NIFT_LOOP	0	// loopback
#define	NIFT_CTCI	1	// CTC: CTCI
#define	NIFT_LCS	2	// CTC: LCS (reserved)

struct	fignif	{
	char	type;		// nif type; NIFT_
	Bool	enabled;	// TRUE = interface enabled
	Bool	dfgw;		// TRUE = gw == default route
	char	spare;
	Eaddr	hwa;		// ethernet hardware addr (unused)
	short	spare2;
	IPaddr	ip;		// network interface IP address
	IPaddr	mask;		// subnet mask (net=1, host=0)
	IPaddr	gwip;		// gateway IP address 
	char	more[8];
	figctc	ctc;		// CTC configuration
};
typedef struct fignif		fignif;

struct	xinufig	{
	Bool	megate;			// TRUE = this host offers gateway support
	char	spare[3];
	IPaddr	servtime;		// time server IP address
	IPaddr	servrfs;		// remote file server IP address
	IPaddr	servdns[MAX_DNS];	// DNS server IP addresses (NULL = none)
	IPaddr	servlog;		// syslog server IP address
	IPaddr	reserved[2];
	fignif	nif[MAX_IFACE];		// +0x20 network interfaces
};
typedef struct xinufig		xinufig;

#ifdef DEFINE_EXTERN
extern	xinufig	Config;		// see also xconfig.c, gbl_config.c
#endif

int	xconfig();			// MVS-Xinu startup configuration
void	xfig_iface(char **);		// Handle iface config statement
void	xfig_prt();			// Print Config
void	xfigger();			// Handle MVS-Xinu configuration file

//------------------------------------------------------------------------------config.h
#endif /* H_CONFIG_H */



