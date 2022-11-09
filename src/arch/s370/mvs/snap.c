/* snap.c - snap */

#include <conf.h>
#include <kernel.h>
#include <network.h>

#define	OPL	16		/* octets printed per line	*/

/*------------------------------------------------------------------------
 * snap - pretty-print memory in hexadecimal with title
 *------------------------------------------------------------------------
 */
void snap(char *title, unsigned char *buf, int dlen)
{
	char	c[OPL+1];				// char translation buffer
	int	i, ct, saylast = 0;
	char	twoblanks[] = "  ";
	char	threeblanks[] = "   ";
	char	bufcall[FUNCSTR_BUFLEN] = "unknown";


//  Set MVS_SNAP_FUNCSTR only when funcstr isn't calling snap, otherwise
//  we'll just loop back and forth between the two calling each other

#ifdef	MVS_SNAP_FUNCSTR				// OK to call funcstr?
	funcstr(bufcall, MY_CALLER);			// who called me?
#endif

	if (title != NULL) kprintf("\n%s <<%s>>\n", title, bufcall);
	if (dlen < 0) dlen = 0;
	kprintf("<SNAP> at 0x%X, length %d 0x%x\n", buf, dlen, dlen);
	memset(c, ' ', OPL);
	c[OPL] = '\0';
	kprintf("%8.8x +00 +00:\t", buf);
	for (i=0; i<dlen; ++i) {
		ct = buf[i] & 0xff;
		c[i % OPL] = isprint(ct) ? ct : '.';
		kprintf( ((i % 4) == 3) ? "%02x " : "%02x", ct);
		if ( (i) && ((i % OPL) == (OPL - 1)) ) {
			kprintf(" <%s>\n", c);
			if ((i+1) < dlen)
				kprintf("%8.8x +%x +%d:\t", &buf[i+1], i+1, i+1);
		}
	}
	for (; i % OPL; ++i) {
		kprintf( "%s", ((i % 4) == 3) ? threeblanks : twoblanks);
		c[i%OPL] = ' ';
		saylast = 1;
	}
	if (saylast)
		kprintf(" <%s>\n", c);
	kprintf("\n");
}

