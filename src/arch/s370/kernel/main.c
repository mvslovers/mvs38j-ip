/* -------------------------------------------------------------------------- */
/* Main entry point to MVS38j TCP/IP short stack                              */
/* -------------------------------------------------------------------------- */

#undef	DEBUG

int main( int argc, char *argv[]) {

/************************************************************************/
/***				NOTE:				      ***/
/***								      ***/
/***   This is where the system begins after the C environment has    ***/
/***   been established.  Interrupts are initially ENABLED.           ***/
/***   This routine turns itself into the null process after 
/***   initialization.  
/***   Because
/***   the null process must always remain ready to run, it cannot    ***/
/***   execute code that might cause it to be suspended, wait for a   ***/
/***   semaphore, or put to sleep, or exit.  In particular, it must   ***/
/***   not do I/O unless it uses kprintf.
/***								      ***/
/************************************************************************/

#ifdef	DEBUG
	__register(13)	void *r13;

	kprintf("MAIN r13 0x%x\n", r13);
#endif

	sysinit();			// start MVS-Xinu

	nulluser();			// no-work process

	return 0;   
}

