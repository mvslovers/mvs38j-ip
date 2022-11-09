/* sysinit.c - sysinit */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sleep.h>
#include <mem.h>
#include <q.h>
#include <io.h>
#include <network.h>

#undef	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  sysinit  --  initialize all Xinu data structures and devices
 *------------------------------------------------------------------------
 */
int sysinit() {
	int		i, j, rc;
	sgd		*psgd;
	xgd		*pxgd;
	__register(8) 	unsigned long r8 = sizeof(xgd);	// XGD getmain length
	__register(9) 	void * r9 = 0;			// XGD getmain address
	int		samplepid1, samplepid2;

	rc = setvbuf(stdout, NULL, _IOLBF, 8*1024);	// stdout line buffered
	if (rc) {
		kprintf("SYSINIT setvbuf stdout error\n");
		exit(1);
	}
	rc = setvbuf(stderr, NULL, _IOLBF, 8*1024);	// stderr same
	if (rc) {
		kprintf("SYSINIT setvbuf stderr error\n");
		exit(1);
	}

	sprintf(vers, "Xinu %s", VERSION);
	kprintf("MVS-Xinu initializing, version: %s\n", vers);
	if (reboot++) kprintf("Reboot %d\n", reboot);

	rc = testauth();		// check APF authorization
	if (rc != 0) {
#ifdef	MVS_TSO_TEST			// play games for TSO TEST
		kprintf("SYSINIT APF authorizing for TSO TEST\n");
		mvsauth(1);		// turn on JSCBAUTH
		rc = testauth();
		if (rc != 0) {
#endif
			kprintf("MVS-Xinu APF authorization error\n");
			exit(47);
#ifdef	MVS_TSO_TEST
		}
#endif
	}
	kprintf("MVS-Xinu APF authorized\n");

//  Most of MVS-Xinu runs in supervisor state, key 8
//  We limit the amount of code running in key 0 to that
//  which is necessary, for obvious safety reasons

	s370state(SUPERVISOR);		// supervisor state

//  Issue SYSEVENT DONTSWAP
//  MVS-Xinu termination (xdone) issues SYSEVENT OKSWAP

	mvsevent(DONTSWAP);		// issue SYSEVENT DONTSWAP

//  Initialize MVS-Xinu XGD

	psgd = findsgd();		// locate SGD
	if (psgd == NULL) {
		kprintf("MVS38j TCP/IP subsystem not initialized\n");
		exit(69);
	} else {
		kprintf("MVS38j TCP/IP subsystem initialized\n");
	}
	mvskey0();			// system key
	__asm {
         GETMAIN RU,LV=(8),SP=241       alloc XGD
         LR    9,1                      give C code XGD ptr
	}
	psgd->sgd_xgd = r9;		// store XGD pointer in SGD
	pxgd = r9;
	memset(pxgd, 0x00, sizeof(xgd));
	strcpy(pxgd->eye, "XGD");	// set eyecatcher for IGG019X8
	mvskey8();			// back to user key

#ifdef	MVS_TSO_TEST			// play games for TSO TEST
					// (doesn't work)
	s370state(PROBLEM);		// problem state
	kprintf("SYSINIT removing APF authorizing for TSO TEST\n");
	mvsauth(0);			// turn off JSCBAUTH
#endif

	kprintf("SYSINIT meminit\n");
	meminit();			//  Initialize free memory list

	kprintf("SYSINIT queueinit\n");
	queueinit();			// Initialize queue mgt

	kprintf("SYSINIT procinit\n");
	procinit();			//  Initialize process mgt
			// Note: procinit() sets numproc=1 for us

	kprintf("SYSINIT seminit\n");
	seminit();			//  Initialize semaphores
	
#ifdef	MEMMARK
	kprintf("SYSINIT _mkinit\n");
	_mkinit();			// initialize memory marking
#endif

//  We have now initialized enough of Xinu so that preemption can
//  begin (if supported); initialize clock management, which will
//  establish a timer pop exit to be periodically called by MVS

//  Clkinit() requires that process mgt already be initialized, so
//  that it may create, resume and "hang" the clkint process.

#ifdef	RTCLOCK
	kprintf("SYSINIT clkinit\n");
	clkinit();			// initialize clock
	kprintf("SYSINIT clkinit complete\n");
	clockstate(NULL, "SYSINIT post CLKINIT");
#endif
	kprintf("clock %sabled\n", clkruns == 1?"en":"dis");

#ifdef	SYSINIT_NULLDISP
	resume(create(nulldisp, INITSTK, MAXPRIORITY, "NullDisp", INITARGS));
	trace("SYSINIT started NullDisp\n");
#else
	trace("SYSINIT did not start NullDisp\n");
#endif

//  Initialize Config, MVS-Xinu system configuration

	xconfig();		// before we start network or user
				// before we start Xinu devices
				// (ctci_init requires CTC cuu from config)
				// after the basic Xinu system is up

//  Initialize Xinu devices (if configured)
//  MVS-Xinu initializes the devices after the clock is initialized;
//  PC-Xinu initializes the devices before.


#ifdef SYSINIT_INITDEVS
#ifdef NDEVS
	kprintf("\nSYSINIT init devices\n\n");
	for ( i=0 ; i<NDEVS ; i++ ) {
	    xdevinit(i);		// initialize devices
	}
	kprintf ("SYSINIT device init complete\n");
#endif /* NDEVS */
#endif /* SYSINIT_NDEVS */

	enable();			// enable interrupts
	kprintf("SYSINIT enabled\n");

// Create sample process to debug process mgt

#ifdef	SYSINIT_SAMPLE
	samplepid1 = create(sample,INITSTK,INITPRIO,"Sample1",INITARGS);
	samplepid2 = create(sample,INITSTK,INITPRIO,"Sample2",INITARGS);
	resume(samplepid1);
	resume(samplepid2);
	kprintf("SYSINIT all samples resumed\n");
#else
	kprintf("SYSINIT no samples started\n");
#endif

#ifdef SYSINIT_USER
	kprintf("SYSINIT create user process\n");
	/* create a process to execute the user's main program */
	userpid = create(main, INITSTK, INITPRIO, INITNAME, INITARGS);
	kprintf("SYSINIT created user process\n");
#else
	userpid = 0;		// no user process active
	kprintf("SYSINIT no user process active\n");
#endif

//  Start the network
//  In theory, netstart will resume userpid once the network is
//  finished initializing (so userpid can use the network)
//  Current MVS-Xinu reality is that there is no userpid process activated
//  Further, PC-Xinu passed userpid as a parm to netstart
//  MVS-Xinu never passes parms to new processes; it was too much
//  trouble for just the one instance (netstart) where PC-Xinu did it

#ifdef SYSINIT_NET
	kprintf("\nSYSINIT starting network\n");
	resume(create(netstart, NETSTK, NETPRI, NETNAM, 0));
	kprintf("SYSINIT network started\n");
#endif

	numproc--;		// allow xdone() in killpid()

	procstate(NPROC, "sysinit pre-exit");
	kprintf("SYSINIT **** complete ****\n\n");
	return(OK);

	__asm {
         LTORG ,
	}

} /* sysinit */





