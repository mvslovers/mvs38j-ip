/* mvsmem.c - MVS-specific memory support */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <string.h>	// Standard C header

/*------------------------------------------------------------------------
 * mvsallocmem - return address of allocated MVS memory or panic
 *------------------------------------------------------------------------
 */
void *mvsallocmem(int len) {
	void	*p;

	kprintf("MVSALLOCMEM entered\n");
	p = malloc(len);		// allocate MVS memory
	if (p == NULL) {
		kprintf("MVSALLOCMEM panic\n");
		panic("mvsallocmem");
	}
	memset(p, 0x00, len);
	mvsmemlen = len;
	mvspmem = p;
	kprintf("MVSallocmem allocated %d 0x%x bytes at 0x%08x\n", len, len, p);
	kprintf("MVSALLOCMEM exited\n");
	return p;

} /* mvsallocmem */

/*------------------------------------------------------------------------
 * mvsfreemem - release memory gotten from MVS
 *------------------------------------------------------------------------
 */
void mvsfreemem() {

	if (mvsmemlen) free(mvspmem);	// free malloc'd memory

} /* mvsfreemem */




