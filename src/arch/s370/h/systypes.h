/*	@(#)types.h 2.4 85/02/06 SMI; from UCB 4.11 83/07/01	*/

/*
 * Basic system types and major/minor device constructing/busting macros.
 */
#ifndef _TYPES_
#define	_TYPES_

// jmm - makedef, major & minor conflict with Systems/C headers

/* major part of a device */
//	#define	major(x)	((int)(((unsigned)(x)>>8)&0377))

/* minor part of a device */
//	#define	minor(x)	((int)((x)&0377))

/* make a device number */
//	#define	makedev(x,y)	((dev_t)(((x)<<8) | (y)))

// jmm - u_char, u_short, u_int, etc. conflict

//	typedef	unsigned char	u_char;
//	typedef	unsigned short	u_short;
//	typedef	unsigned int	u_int;
//	typedef	unsigned long	u_long;
//	typedef	unsigned short	ushort;		/* System V compatibility */

#ifdef vax
typedef	struct	_physadr { int r[1]; } *physadr;
typedef	struct	label_t	{
	int	val[14];
} label_t;
#endif

#ifdef mc68000
typedef	struct	_physadr { short r[1]; } *physadr;
typedef	struct	label_t	{
	int	val[13];
} label_t;
#endif

// jmm - daddr_t, etc. conflict

typedef	struct	_quad { long val[2]; } quad;
//	typedef	long	daddr_t;
//	typedef	char *	caddr_t;
//	typedef	u_long	ino_t;
//	typedef	long	swblk_t;
//	typedef	unsigned int	size_t;
//	typedef	int	time_t;
//	typedef	short	dev_t;
//	typedef	int	off_t;

//	typedef	struct	fd_set { int fds_bits[1]; } fd_set;

#endif


