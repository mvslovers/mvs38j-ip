/* bufpool.h */

#ifndef	H_BUFPOOL_H
#define	H_BUFPOOL_H

#include <mark.h>

#ifdef	PANIC_BUFPOOL			// panic if SYSERR return in bufpool code
	#define	BUFPOOL_SYSERR(x)	panic("BUFPOOL PANIC syserr")
#else
	#define	BUFPOOL_SYSERR(x)	return x
#endif

#ifndef	NBPOOLS
#define	NBPOOLS	5			/* Maximum number of pools	*/
#endif
#ifndef	BPMAXB
#define	BPMAXB	2048			/* Maximum buffer length	*/
#endif
#define	BPMINB	2			/* Minimum buffer length	*/
#ifndef	BPMAXN
#define	BPMAXN	100			/* Maximum buffers in any pool	*/
#endif
struct	bpool	{			/* Description of a single pool	*/
	void	*bpbase;		// base addr of this pool (getmem)
	int	bpsize;			/* size of buffers in this pool	*/
	int	bpmaxused;		/* max ever in use		*/
	int	bptotal;		/* # buffers this pool		*/
	char	*bpnext;		/* pointer to next free buffer	*/
	int	bpsem;			/* semaphore that counts buffers*/
	};				/*  currently in THIS pool	*/

#ifdef DEFINE_EXTERN
extern	struct	bpool bptab[];		/* Buffer pool table		*/
extern	int	nbpools;		/* current number of pools	*/
#ifdef	MEMMARK
extern	MARKER	bpmark;
#endif
#endif /* DEFINE_EXTERN */

int	freebuf(void *);	// free a buffer that was allocated from a pool by getbuf
void	*getbuf(unsigned);	// get a buffer from a preestablished buffer pool
int	mkpool(int, int);	// allocate memory for a buffer pool and link together
int	*nbgetbuf(int);		// a non-blocking version of getbuf
	poolinit();		// initialize the buffer pool routines

#endif /* H_BUFPOOL_H */





