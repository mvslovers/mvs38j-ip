/* mark.h - unmarked */

#ifndef H_MARK_H
#define	H_MARK_H

#ifndef	MAXMARK
#define	MAXMARK	20		/* maximum number of marked locations	*/
#endif

#ifdef	MEMMARK
#ifdef DEFINE_EXTERN
extern	int	*(marks[]);
extern	int	nmarks;
extern	int	mkmutex;
typedef	int	MARKER[1];	/* by declaring it to be an array, the	*/
				/* name provides an address so forgotten*/
				/* &'s don't become a problem		*/
#endif /* DEFINE_EXTERN */

#define	unmarked(L)		(L[0]<0 || L[0]>=nmarks || marks[L[0]]!=L)
#define marked(L)               !unmarked(L)

SYSCALL mark(int *);		// mark a location if it hasn't been marked
_mkinit();			// called once at system startup

#endif /* MEMMARK */
#endif /* H_MARK_H */

