/* mvswto.c - mvswto */

#include <conf.h>
#include <kernel.h>
#include <string.h>

/*------------------------------------------------------------------------
 *  mvswto  --  issue WTO from passed string
 *------------------------------------------------------------------------
 */
void mvswto(unsigned char *str, int reqlen) {
	struct	wtobuf {
	char	wlen[2];	// length of A+B+C	A
	char	wmcs[2];	// MCS flags		B
	char	wtxt[126];	// WTO max text		C
	char	wdesc[2];	// descriptor codes
	char	wrout[2];	// routing codes
};
	struct wtobuf	wbuf;
	__register(1) 	void * r1 = &wbuf;	// SVC parmlist ptr
	int		efflen;			// effective length
	unsigned char	*p;

	memset((void *)&wbuf, 0x00, sizeof(wbuf));
	wbuf.wmcs[0] = 0x80;			// rout+desc present
	efflen = reqlen;
	if (efflen > sizeof(wbuf.wtxt)) {
		efflen = sizeof(wbuf.wtxt);	// trim length
	}
	if (efflen == 0) {
		str = " ";			// caller wants blank line
		efflen = 1;
	}
	memcpy((void *)wbuf.wtxt, str, efflen);	// copy WTO text
	efflen += 4;				// add overhead A+B
	wbuf.wlen[1] = efflen;			// always fits in 1 byte
	p = (unsigned char *)&wbuf + efflen;	// at desc field
	p[0] = 0x01; p[3] = 0x20;		// see sample expansion
	__asm {
         SVC   35                      issue WTO SVC
         }
	return;

	__asm {
XTO      WTO   'X',ROUTCDE=11,DESC=8,MF=L    sample expansion
         PRINT GEN
         IEZWPL ,                      WTO/WTOR parmlist
	}
}


