/* q.h - firstid, firstkey, isempty, lastkey, nonempty */

#ifndef H_Q_H
#define H_Q_H

/* q structure declarations, constants, and inline procedures		*/

#ifndef	NQENT
#define	NQENT		NPROC + NSEM + NSEM + 4	/* for ready & sleep	*/
#endif

struct	qent	{		/* one for each process plus two for	*/
				/* each list				*/
	int	qkey;		/* key on which the queue is ordered	*/
	int	qnext;		/* pointer to next process or tail	*/
	int	qprev;		/* pointer to previous process or head	*/
	};

#ifdef DEFINE_EXTERN
extern	struct	qent q[];
extern	int	nextqueue;
#endif

/* inline list manipulation procedures */

#define	isempty(list)	(q[(list)].qnext >= NPROC)
#define	nonempty(list)	(q[(list)].qnext < NPROC)
#define	firstkey(list)	(q[q[(list)].qnext].qkey)
#define lastkey(tail)	(q[q[(tail)].qprev].qkey)
#define firstid(list)	(q[(list)].qnext)

/* gpq constants */

#define	QF_WAIT		0	/* use semaphores to mutex		*/
#define	QF_NOWAIT	1	/* use disable/restore to mutex		*/

void	*deq(int), *headq(int);
int	enq(int, void *, int);
int	freeq(int);
int	getlast(int);
int	insert(int, int, int);
int	lenq(int);
int	newqueue();
void	queueinit();
void	*seeq(int);
int	newq(unsigned, unsigned);

#endif /* H_Q_H */


