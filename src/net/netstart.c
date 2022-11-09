/* netstart.c - netstart */

// TODO Review RIP

#include <conf.h>
#include <kernel.h>
#include <sleep.h>
#include <network.h>
#include <snmp.h>
#include <tcptimer.h>
#include <proc.h>
#include <date.h>
#include <q.h>
#include <config.h>	// MVS-Xinu configuration

#ifdef OSPF
#include <ospf.h>
#endif /* OSPF */

#ifdef	NETSTART_ARP
#include <arp.h>
#endif

#ifdef	NETSTART_UDP
#include <udp.h>
#endif

#ifdef SNMP
	#define	SNMP_CODE(x)	__VA_ARGS__
#else
	#define	SNMP_CODE(x)
#endif

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 *  netstart	initialize net
 *		Defines specified in Configuration (and hence conf.h)
 *		are used to control which protocols, daemons, etc
 *		are started; useful during porting efforts as code is
 *		being made functional
 *------------------------------------------------------------------------
 */

PROCESS netstart() {		// MVS-Xinu: userpid global, not parm

	unsigned long	now;
	int		i;
	struct netif	*pif;


	trace("\nNETSTART starting network\n");

	if (clkruns == FALSE)
		panic("netstart: no clock");

	// Allocate buffers & create semaphore
	// Code removed from inithost, initgate

	trace("NETSTART netpool alloc\n");
	Net.netpool = mkpool(MAXNETBUF, NETBUFS);

	trace("NETSTART lrgpool alloc\n");
	Net.lrgpool = mkpool(MAXLRGBUF, LRGBUFS);

	Net.sema = screate(1);
	Net.nif = NIF;

#ifdef	NETSTART_UDP
	udpinit();			// init UDP
#else
	trace("NETSTART udp disabled\n");
#endif


#ifdef NETSTART_ARP
	arpinit();			// init ARP
#else
	trace("NETSTART arp disabled\n");
#endif


#ifdef NETSTART_IP
	ipfinit();			// init IP fragment queue
#else
	trace("NETSTART ip disabled\n");
#endif


#ifndef	SNMP
	trace("NETSTART snmp disabled\n");
#endif
	SNMP_CODE(IfNumber = Net.nif - 1);


/* FIXME need command to set/clear: gateway, IpForwarding */

	trace("NETSTART mode: %s\n", Config.megate ? "gateway" : "host");
	if (Config.megate) {
		gateway = 1;			// we're a gateway
		SNMP_CODE(IpForwarding = 1);	// tell SNMP: gateway
	} else {
		gateway = 0;			// we're just a host
		SNMP_CODE(IpForwarding = 2);	// tell SNMP: not a gateway
	}

//  Initialize routing

	trace("NETSTART initialize routing\n");
	rtinit();

//  Initialize network interfaces
//  MVS-Xinu obsoleted initgate & inithost; they looked almost exactly
//  the same, and I preferred the buffers and semaphore be initialized
//  here in netstart in case they were needed earlier in net starup.
//  After removing all that stuff, there was nothing left in either
//  inithost or initgate besides the netif init which is now in
//  netup_nif.  The little bit of code in netstart that initialized
//  the interfaces was moved to efaceinit

	trace("NETSTART initialize network interfaces\n");
	netup_nif();

//  Finish establishing initial routing configuration 
//  This does the loopback & default route
//  Netup_nif calls network interface code which is responsible 
//  for establishing their own routes
//  MVS-Xinu: NI_PRIMARY efaceinit

	trace("NETSTART establish initial routes\n");
	netup_rt();

	/*
	 * wait()'s synchronize to insure initialization is done
	 * before proceeding.
	 */

#ifdef NETSTART_IP
	trace("NETSTART starting IPPROC\n");
	resume(create(ipproc, IPSTK, IPPRI, IPNAM, IPARGC));
	wait(Net.sema);
	trace("NETSTART IPPROC started\n");
#endif

#ifdef NETSTART_SLOWTIMER
	trace("NETSTART starting SLOWTIMER\n");
	resume(create(slowtimer, STSTK, STPRI, STNAM, STARGC));
	wait(Net.sema);
	trace("NETSTART SLOWTIMER started\n");
#endif

#ifdef NETSTART_TCP
	trace("NETSTART starting TCP processes\n");
	resume(create(tcptimer, TMSTK, TMPRI, TMNAM, TMARGC));
	wait(Net.sema);
	resume(create(tcpinp, TCPISTK, TCPIPRI, TCPINAM, TCPIARGC));
	wait(Net.sema);
	resume(create(tcpout, TCPOSTK, TCPOPRI, TCPONAM, TCPOARGC));
	wait(Net.sema);
	trace("NETSTART TCP processes started\n");
#endif

	netup_gath();		// gather routes
	getutim(&now);		// query time server

#ifdef	MULTICAST
	hginit();
#else
	trace("NETSTART multicast disabled\n");
#endif


#ifdef	NETSTART_RIP
	resume(create(rip, RIPISTK, RIPPRI, RIPNAM, RIPARGC));
#else
	trace("NETSTART rip disabled\n");
#endif


#ifdef	NETSTART_OSPF
	resume(create(ospf, OSPFSTK, OSPFPRI, OSPFNAM, 0));
#else
	trace("NETSTART ospf disabled\n");
#endif


#ifdef	NETSTART_SNMP
	resume(create(snmpd, SNMPSTK, SNMPPRI, SNMPDNAM, 0));
#else
	trace("NETSTART snmp disabled\n");
#endif

#ifdef NETSTART_RWHO
	rwho();
#else
	trace("NETSTART rwho disabled\n");
#endif


#ifdef NETSTART_FINGERD
	resume(create(FINGERD, FNGDSTK, FNGDPRI, FNGDNAM, FNGDARGC));
#else
	trace("NETSTART fingerd disabled\n");
#endif


#ifdef NETSTART_ECHOD
	resume(create(ECHOD, ECHOSTK, ECHOPRI, ECHODNAM, 0));
#else
	trace("NETSTART echod disabled\n");
#endif


#ifdef NETSTART_UDPECHO
	resume(create(udpecho, 1000, 50, "udpecho", 0));
#else
	trace("NETSTART udpecho disabled\n");
#endif

	if (userpid) {
		resume(userpid);
		trace("NETSTART resumed userpid %d\n", userpid);
		dbug(procstate(NPROC, "NETSTART complete"));
	} else {
		trace("NETSTART no user process to resume\n");
	}

//  Start the network input procedure, netproc 

	resume(create(netproc, NETSTK, NETPRI, NETPROCNAM, NETPROCARGC));

	trace("NETSTART routes at startup completion\n");
	rtdump(ALL_ROUTES);

	trace("NETSTART network startup complete\n");
}





