// memstate.c - display memlist chain

#include <conf.h>
#include <kernel.h>
#include <mem.h>

void memstate(char *msg) {

	mblock *p;

	p = &memlist;
	kprintf("MEMSTATE memlist 0x%x %s\n", p, (msg) ? msg : "******");
	for ( ; p != NULL; p = (void *)p->mnext) {
		kprintf("<MBLOCK 0x%x> mlen %d\n", p, p->mlen);
	}
	return;
}


