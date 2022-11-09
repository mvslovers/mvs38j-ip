/* io.h -  fgetc, fputc, getchar, isbaddev, putchar */

#ifndef H_IO_H
#define H_IO_H

#ifdef	DEFINE_EXTERN
	extern	int	INTVECI();
	extern	int	INTVECO();
	extern	struct vector disptab[];
	#ifdef	NDEVS
		extern	struct	intmap intmap[NDEVS];
	#endif
#endif

#define	INTVECI	inint      	/* input interrupt dispatch routine	*/
#define	INTVECO	outint		/* output interrupt dispatch routine	*/

#define BASE    060		/* base address of the autovector table */
				/* L.L.					*/
#define OFFSET	04		/* offset of each autovector, L.L.	*/

#ifdef	NDEVS
	#define	isbaddev(f)	( (f)<0 || (f)>=NDEVS )
#endif

#define	BADDEV		-1

#define level(vec)      ((vec-BASE)/OFFSET)     /* determine autovector level */
                                                /* L.L.                       */

/* fd_to_dd()  converts file descriptor to device descriptor
 * -- to be used in converting old XINU code to new I/O formats
 */
#define fd_to_dd(f)	(proctab[getpid()].fildes[f])

#define findfd()	(dd_to_fd(FDFREE))

/* In-line I/O procedures */

//  Conflicts with Standard C headers: getchar, putchar, fgetc, fputc

//	#define	getchar()	getc(CONSOLE)
//	#define	putchar(ch)	putc(CONSOLE,(ch))
//	#define	fgetc(unit)	getc((unit))
//	#define	fputc(unit,ch)	putc((unit),(ch))

/* Unix open flags */
#define	O_RDONLY	000		/* open for reading */
#define	O_WRONLY	001		/* open for writing */
#define	O_RDWR		002		/* open for read & write */
#define	O_NDELAY	00004		/* non-blocking open */
#define	O_APPEND	00010		/* append on each write */
#define	O_CREAT		01000		/* open with file create */
#define	O_TRUNC		02000		/* open with truncation */
#define	O_EXCL		04000		/* error on create if file exists */

/* conversion cheats -- to aid in upgrading old XINU code
 *			to the new unix like I/O calls      
 */
#define o_r		O_RDONLY
#define o_w		O_WRONLY
#define	o_rw		O_RDWR
#define o_o		0
#define o_n		O_CREAT | O_EXCL
#define o_dc		O_CREAT		/* don't care, old or new */

#define o_rn		o_r | o_n
#define o_ro		o_r | o_o
#define	o_rdc		o_r | o_dc
#define o_wn		o_w | o_n
#define o_wo		o_w | o_o
#define o_wdc		o_w | o_dc
#define o_rwn		o_rw | o_n
#define o_rwo		o_rw | o_o
#define o_rwdc		o_rw | o_dc

//  Map old Xinu device primitives to renamed primitives
//  Fortunately, all the Xinu code that calls the Xinu primitives
//  seems to include this header, so this should work out fairly well

#undef	close
#define	close(x)		xdevclose(x)
#undef	control
#define	control(...)		xdevcontrol(__VA_ARGS__)
#undef	getc
#define	getc(x)			xdevgetc(x)
#undef	init
#define	init(x)			xdevinit(x)
#undef	open
#define	open(x,y,z)		xdevopen(x,y,z)
#undef	putc
#define	putc(x,y)		xdevputc(x,y)
#undef	read
#define	read(x,y,z)		xdevread(x,y,z)
#undef	seek
#define	seek(x,y)		xdevseek(x,y)
#undef	write
#define	write(x,y,z)		xdevwrite(x,y,z)

struct	intmap	{		/* device-to-interrupt routine mapping	*/
	int	(*iin)();	/* address of input interrupt routine	*/
	int	icode;		/* argument passed to input routine	*/
	int	(*iout)();	/* address of output interrupt routine	*/
	int	ocode;		/* argument passed to output routine	*/
};

struct	vector	{
	int     (*vproc)();     /* address of interrupt procedure       */
};

// Renamed Xinu device primitives

int xdevclose(int);
int xdevcontrol(int, int, ...);
int xdevgetc(int);
int xdevinit(int);
int xdevopen(int, const void *, const void *);
int xdevputc(int, char);
int xdevread(int, void *, unsigned int);
int xdevseek(int, long);
int xdevwrite(int, const void *, unsigned int);

//	int printf(char *, ...);
//	sprintf(char *, const char *, ...);

#endif /* H_IO_H */




