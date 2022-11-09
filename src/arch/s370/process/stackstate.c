/* stackstate.c - Debug Aid - display MVS savearea chain info */

//  TODO	use FRAMSIZ_OFFSET to show full DSA frame for CSECT

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <string.h>		// Standard C header

#undef	DEBUG				// defined = display diagnostics
#include <debug.h>

#undef	DEBUG_ABEND			// defined = abend 2nd time thru code
#define	DEBUG_NL	""		// "\n" or ""

#define	CALLER_BASE	r12		// Systems/C default pgm basereg
#define	CALLER_RETURN	r14		// Standard MVS return addr reg
#define	CALLED_BASE	r15		// Standard MVS entry point addr reg

#define	PGMLEN_OFFSET	0x1c		// offset from DCC eyecatcher
					// to program name length

#define	FRAMSIZ_OFFSET	0x0c		// offset to DCC frame size (fullword)

#define	MAXHDRLEN	0x120		// maximum header dump length
#define	NAMBUFSIZ	FUNCSTR_BUFLEN	// pgm name buffer length
#define	EYE		"DCC"		// CSECT header eyecatcher

#ifdef	DEBUG
	static	int	stackswitch = 0;	// debug
#endif

struct	s_savearea {		// Standard MVS savearea
	int	sa_resv;
	void	*sa_back;
	void	*sa_fwd;
	int	r14;
	int	r15;
	int	r0;
	int	r1;
	int	r2;
	int	r3;
	int	r4;
	int	r5;
	int	r6;
	int	r7;
	int	r8;
	int	r9;
	int	r10;
	int	r11;
	int	r12;
};
typedef	struct s_savearea	sa;

extern	int main(int, char *[]);	// main() prototype

/*------------------------------------------------------------------------
 * findCSECTname  Given an address, return related CSECT name in buffer
 *		  If we are unable to locate the eyecatcher, the buffer
 *		  will contain the address supplied in pcode
 *		  The address of the eyecatcher is returned, or NULL
 *		  if we can't find the eyecatcher
 *------------------------------------------------------------------------
 */
void *findCSECTname(void *pcode, char *pbuf) {

	int	i, hit;
	unsigned int	paddr;
	int	pgmnamelen;
	char	*ppgm;
	char	*pbase;
	char	*peye;			// @ eyecatcher
	char 	*plimit;
	char	*pchar;
	void	*pprev;

//  Find the CSECT's eyecatcher, which precedes the executable code

	paddr = (unsigned int)pcode;		// @ CSECT's executable code
	paddr = paddr << 8;
	paddr = paddr >> 8;
	pbase = peye = (char *)paddr;		// use cleaned address
	plimit = pbase - MAXHDRLEN;		// @ lowest addr we'll look for eyecatcher
	for (hit = 0;  peye > plimit; peye--) {
		if ( (memcmp(peye, EYE, sizeof(EYE) - 1)) == 0) {
			hit = 1;
			break;
		}
	}
	if (hit == 0) {
		trace("STACKSTATE unable to locate eyecatcher near 0x%x\n", pcode);
		trace("STACKSTATE DEBUG paddr 0x%x, peye 0x%x, plimit 0x%x\n", paddr, peye, plimit);
		snap("<PGM HDR> UNKNOWN", plimit, MAXHDRLEN);
		sprintf((void *)pbuf, "UNKNOWN at 0x%x", pcode);
		return NULL;			// can't find eyecatcher
	}

	snap("<PGM HDR> recognized", peye, pbase - peye);

	ppgm = peye + PGMLEN_OFFSET;
	pgmnamelen = * (int *)ppgm;
	ppgm += sizeof(int);		// pgmname follows fullword length

	trace("SASTATE HIT peye 0x%x, pgmnamelen %d\n", peye, pgmnamelen);

	if (pgmnamelen > NAMBUFSIZ) 
		pgmnamelen = NAMBUFSIZ;
	memset(pbuf, 0x00, NAMBUFSIZ + 1);
	memcpy(pbuf, ppgm, pgmnamelen);

	trace("<CSECT> 0x%x program %s\n", pcode, pbuf);

	return (void *)peye;
}

/*------------------------------------------------------------------------
 * sastate  --  display info associated with one MVS savearea
 *		returns pointer to previous savearea or NULL
 *------------------------------------------------------------------------
 */
void *sastate(sa *psa) {

	char		namecaller[NAMBUFSIZ + 1];	// caller pgm name
	char		namecalled[NAMBUFSIZ + 1];	// called pgm name
	int		lp, ret;			// load point, return addrs
	int		ep;				// entry point, called pgm
	int		retoffset;			// offset from caller lp
	unsigned int	paddr;

	snap("<SAVEAREA> at entry to SASTATE", (void *)psa, sizeof(sa));

					// caller CSECT "load point"
	lp = (int)findCSECTname(	// (eyecatcher) 
		(void *)psa->CALLER_BASE, namecaller);

	paddr = (unsigned int)psa->CALLED_BASE;
	paddr = paddr << 8;
	ep = paddr >> 8;		// save called pgm entry point addr
	
	findCSECTname((void *)psa->CALLED_BASE, namecalled);

	paddr = (int)psa->CALLER_RETURN;
	paddr = paddr << 8;
	paddr = paddr >> 8;
	ret = (int) paddr;		// return addr to caller
	retoffset = ret - lp;		// return offset from eyecatcher

	if (lp == NULL)
		kprintf(DEBUG_NL "<SAVEAREA 0x%x> regs at entry to %s; return addr %s\n", 
			psa, namecalled, namecaller);
	else
		kprintf(DEBUG_NL "<SAVEAREA 0x%x> regs at entry to %s; return addr %s+0x%x\n", 
			psa, namecalled, namecaller, retoffset);

	kprintf("           RESERVED 0x%x, HSA 0x%x, LSA 0x%x\n",
		psa->sa_resv, psa->sa_back, psa->sa_fwd);

	kprintf("           R0-R7 %8.8x %8.8x %8.8x %8.8x %8.8x %8.8x %8.8x %8.8x\n",
		psa->r0, psa->r1, psa->r2, psa->r3, 
		psa->r4, psa->r5, psa->r6, psa->r7);

	kprintf("           R8-RF %8.8x %8.8x %8.8x %8.8x %8.8x %8.8x %8.8x %8.8x\n",
		psa->r8,  psa->r9, psa->r10, psa->r11, 
		psa->r12, psa,    psa->r14, psa->r15);

	paddr = (int)psa->sa_back;
	paddr = paddr << 8;	// shift off hi byte
	paddr = paddr >> 8;	// to form 24-bit MVS38j address

	trace("STACKSTATE current SA 0x%x, next SA 0x%x\n", psa, paddr);

	if ((void *)ep == main) 
		return NULL;		// terminate SA chase at main()
	else
		return (void *)paddr;	// return HSA ptr, more chasing
}

/*------------------------------------------------------------------------
 * stackstate  --  display MVS savearea chain (SA back chain)
 *		   If psa = NULL, the caller's savearea is used
 *------------------------------------------------------------------------
 */
void stackstate(void *psa, char *msg) {

	int		limit = 15;	// max saveareas to chase
	unsigned int	paddr;
	__register(13)	sa *r13;


	if (msg != NULL)
		kprintf(DEBUG_NL "STACKSTATE entry; pid %d %s ****** %s\n", 
			currpid, proctab[currpid].pname, msg);

	paddr = (unsigned int) psa;
	paddr = paddr << 8;			// zero hi byte to form
	paddr = paddr >> 8;			// clean 24-bit address

	if (paddr == NULL) {
		paddr = (unsigned int)r13->sa_back;	// my caller's savearea ptr
		trace("STACKSTATE enter; psa NULL; substituting 0x%x\n", paddr);
	} else
		trace("STACKSTATE enter; psa 0x%x\n", paddr);

	while ((paddr) && (limit)) {
		paddr = (unsigned int)sastate((void *)paddr);
		limit--;
	}

#ifdef	DEBUG_ABEND
	if (stackswitch)
		mvsabend("STACKSTATE debug", 100);
	else
		stackswitch++;			// die next time (debug)
#endif

	return;
}


/*------------------------------------------------------------------------
 * callinfo  --  display our caller and caller's caller function names
 *------------------------------------------------------------------------
 */
void callinfo(char *msg) {

	char		namecaller[NAMBUFSIZ + 1];	// caller pgm name
	char		namecalled[NAMBUFSIZ + 1];	// called pgm name
	__register(13)	sa *r13;
	sa		*psa;


	psa = (void *)r13->sa_back;			// my caller's savearea ptr
	psa = (void *)psa->sa_back;			// my caller's savearea ptr
	findCSECTname((void *)psa->CALLED_BASE, namecalled);

	psa = (void *)psa->sa_back;			// my caller's caller's savearea ptr
	findCSECTname((void *)psa->CALLED_BASE, namecaller);

	kprintf(DEBUG_NL "CALLINFO %s %s called by %s\n", 
		msg, namecalled, namecaller);
}

#undef	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 * funcstr	Place a function name into caller-provided buffer
 *		Returns pointer to caller's buffer
 *------------------------------------------------------------------------
 */

//	buf	address of caller's buffer; implied length FUNCSTR_BUFLEN
//		Updated as per doc in findCSECTname or contains <unknown>
//	arg	One of the following:
//		1) number less than 4096, requesting we backtrack saveareas
//		   to locate the address stored in the savearea's CALLED_BASE.
//		   0=funcstr, 1=funcstr caller, 2=funcstr caller caller, etc.
//		   MY_CALLER is commonly used to get a function's caller
//		2) address of storage in MVS-Xinu for which the function name 
//		   is to be determined

//  ----------------------------------------------------------------------------
//  Note:  see Configuration MVS_SNAP_FUNCSTR and snap.c for important
//  information about funcstr's interaction with snap
//  Careless setting of DEBUG and calling snap from here will initiate
//  a looooong loop of funcstr and snap calling each other
//  Now that funcstr is "debugged" [ha!] it should be OK to define
//  MVS_SNAP_FUNCSTR in Configuration
//  ----------------------------------------------------------------------------

char *funcstr(char *buf, void *arg) {

	__register(13)	sa *r13;
	sa		*psa;
	void		*pcode;
	int		x = (int)arg;


	trace("FUNCSTR enter; buf 0x%x arg 0x%x\n", buf, arg);
	if (x > 4096) {
		pcode = arg;				// caller supplied address
	} else {
		psa = (void *)r13->sa_back;		// my caller's savearea ptr
		while (x > 0) {
			psa = (void *)psa->sa_back;	// my caller's savearea ptr
			trace("FUNCSTR psa 0x%x\n", psa);
			if (psa == NULL) {
				strncpy(buf, "<unknown>", FUNCSTR_BUFLEN);
				return buf;		// nobody home
			}
			x--;
		}
		pcode = (void *)psa->CALLED_BASE;
	}

	trace("FUNCSTR findCSECTname pcode 0x%x buf 0x%x\n", pcode, buf);
	findCSECTname(pcode, buf);
	trace("FUNCSTR findCSECTname returned buf %s\n", buf);
	return buf;
}


/*------------------------------------------------------------------------
 * funchist	Place as much function call history as will fit in 
 *		caller-supplied buffer
 *		Use funcstr to chase the savearea chain
 *		Returns pointer to caller's buffer
 *------------------------------------------------------------------------
 */

//	buf	address of caller's buffer
//	len	length of caller's buffer (maximum subject to sizeof work)
//		Note length includes the room required for terminating '\0'
//	psa	NULL = begin at current savearea
//		else, specified savearea is used

#undef	DEBUG
#include <debug.h>

char *funchist(char *buf, int len, void *psa) {

	char	work[FUNCSTR_BUFLEN + 128];	// work buffer; funcstr assumes
						// at least FUNCSTR_BUFLEN length
	char	*pbuf = work;
	void	*pcode;				// pointer to function code
	int	max = len - 1;			// reserve room for trailing '\0'
	int	n = 2;				// start at caller's savearea
	

	dbug(memset(work, 0xff, sizeof(work)));
	work[0] = '\0';				// start strlen off right
	if (psa == NULL) {
		while (strlen(work) < max) {
			funcstr(pbuf, n++);			// deposit func name in buffer
			if (*pbuf == '<') break;		// <unknown> returned; done
			strncat(work, " ", sizeof(work));	// append space
			pbuf = work + strlen(work);		// @ slot for next funcstr
		}
	} else {
		while ( (psa) && (strlen(work) < max) ) {
			pcode = ((sa *)psa)->CALLED_BASE;	// @ function entry point
			trace("FUNCHIST pre-funcstr psa 0x%x pbuf 0x%x pcode 0x%x work %s\n", psa, pbuf, pcode, work);
			funcstr(pbuf, pcode);			// deposit func name in buffer
			if (*pbuf == '<') break;		// <unknown> returned; done
			strncat(work, " ", sizeof(work));	// append space
			pbuf = work + strlen(work);		// @ slot for next funcstr
			psa = ((sa *)psa)->sa_back;		// caller's savearea ptr
		}
	}
	snap("FUNCHIST buffer", work, sizeof(work));
	memcpy(buf, work, max);			// return data to caller
	buf[max] = '\0';			// terminate maximum string
	return buf;
}


