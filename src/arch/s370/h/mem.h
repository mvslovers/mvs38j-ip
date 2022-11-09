/* mem.h - freestk, roundew, truncew , roundmb, truncmb */
#ifndef H_MEM_H
#define H_MEM_H
#include <lock.h>
//------------------------------------------------------------------------------mem.h
/*----------------------------------------------------------------------
 * roundew, truncew - round or trunctate address to next even word
 *----------------------------------------------------------------------
 */
#define	roundew(x)	(WORD *)( (3 + (WORD)(x)) & ~03 )
#define	truncew(x)	(WORD *)( ((WORD)(x)) & ~03 )


/*----------------------------------------------------------------------
 * roundmb, truncmb -- round or truncate address up to size of mblock
 *----------------------------------------------------------------------
 */
#define	roundmb(x)	(WORD *)( (7 + (WORD)(x)) & ~07 )
#define	truncmb(x)	(WORD *)( ((WORD)(x)) & ~07 )


/*----------------------------------------------------------------------
 *  freestk  --  free stack memory allocated by getstk
 *----------------------------------------------------------------------
 */
#define freestk(p,len)	freemem((unsigned)(p)			\
				- (unsigned)(roundmb(len))	\
				+ (unsigned)sizeof(int),	\
				roundmb(len) )

#define memlock()	mvslock(KMEM, __FILE__)
#define memunlock()	mvsunlock(KMEM, __FILE__)

// Mblock represents free MVS memory
// The free memory begins immediately after mlen except in the
// case of the memlist block, which is the anchor.

typedef struct	_mblock	{
	struct mblock	*mnext;		// ptr to next mblock
	unsigned int	mlen;		// size of free memory
} mblock;

#ifdef DEFINE_EXTERN
extern	mblock	memlist;		/* head of free memory list	*/
extern	char	*maxaddr;		/* max memory address		*/
//	extern	WORD	_end;		/* address beyond loaded memory	*/
//	extern	WORD	*end;		/* &_end + FILLSIZE		*/
#endif /* DEFINE_EXTERN */

SYSCALL	freemem(void *, unsigned);	// free a memory block, returning it to memlist
WORD	*getmem(unsigned);		// allocate heap storage, returning lowest WORD address
void	meminit();			// init memory management
void	memstate(char *);		// msg
void	*mvsallocmem(int);		// MVS-Xinu memory allocate
void	mvsfreemem();			// MVS-Xinu memory release

//------------------------------------------------------------------------------mem.h
#include <bufpool.h>
#include <mark.h>
#include <msg.h>
#include <ports.h>
//------------------------------------------------------------------------------mem.h
#endif /* H_MEM_H */



