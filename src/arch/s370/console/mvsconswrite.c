/* mvsconswrite.c - mvsconswrite */

#include <conf.h>
#include <kernel.h>

/*------------------------------------------------------------------------
 * mvsconswrite - write a buffer to the MVS master console
 *------------------------------------------------------------------------
 */

int mvsconswrite(struct devsw *pdev, unsigned char *buf, unsigned int len) {

	mvswto(buf, len);
	return OK;
}

