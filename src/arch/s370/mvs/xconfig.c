/* xconfig.c - xconfig */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <config.h>	// the object of our desire
#include <string.h>	// Standard C header

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 * xconfig	Initialize struct xinuconfig
 *		called from sysinit prior to starting network
 *------------------------------------------------------------------------
 */

int xconfig() {

	int	i;
	char	*ptxt;
	fignif	*pnif;
	char	bufip1[IP_MAXDOT];
	char	bufip2[IP_MAXDOT];
	char	bufip3[IP_MAXDOT];

//  Set Xinu Configuration file manifest constant defaults

	memset(&Config, 0x00, sizeof(Config));

	Config.megate		= NET_FEATURE_GATEWAY;	// TRUE = host offers gateway services

//  FIXME need server configuration statement to specify server IP addresses

	Config.servtime		= dot2ip(TSERVER);	// time server
	Config.servrfs		= dot2ip(RSERVER);	// RFS server
	Config.servdns[0]	= dot2ip(NSERVER);	// DNS server
	Config.servlog		= dot2ip(LOGHOST);	// syslog server


//  Set MVS-Xinu network interface defaults
//  The CTC device addresses are customized to conform to Volker Bandke's Turnkey MVS38j
//  The nif[0] IP address is the customary loopback (non-routable) address;
//  the loopback ddnames are dummies as a debug aid
//  The nif[1] IP address follows Roger Bowler's herctcp.html recommendation and resides
//  in the standard internet non-routable subnet
//  The nif[2] IP address is similarly non-routable

	Config.nif[0].enabled			= TRUE;
	Config.nif[0].ip 			= dot2ip("127.0.0.1");
	Config.nif[0].mask			= dot2ip("255.0.0.0");
	memcpy(Config.nif[0].ctc.read.ddname, 	"LOOP    ", 8);
	memcpy(Config.nif[0].ctc.write.ddname, 	"LOOP    ", 8);

	Config.nif[1].enabled			= TRUE;
	Config.nif[1].ip 			= dot2ip("192.168.200.1");
	Config.nif[1].mask			= dot2ip("255.255.255.0");
	Config.nif[1].gwip 			= dot2ip("192.168.0.1");
	Config.nif[1].dfgw			= TRUE;
	Config.nif[1].ctc.read.cuu		= 0x500;
	Config.nif[1].ctc.write.cuu		= 0x501;
	memcpy(Config.nif[1].ctc.read.ddname, 	"CTC500  ", 8);
	memcpy(Config.nif[1].ctc.write.ddname,	"CTC501  ", 8);

	Config.nif[2].enabled			= FALSE;
	Config.nif[2].ip 			= dot2ip("10.11.12.1");
	Config.nif[2].mask			= dot2ip("255.0.0.0");
	Config.nif[2].ctc.read.cuu		= 0x504;
	Config.nif[2].ctc.write.cuu		= 0x505;
	memcpy(Config.nif[2].ctc.read.ddname,	"CTC502  ", 8);
	memcpy(Config.nif[2].ctc.write.ddname,	"CTC5053 ", 8);

	Config.nif[3].enabled			= FALSE;
	Config.nif[3].ip 			= dot2ip("192.168.254.30");
	Config.nif[3].mask			= dot2ip("255.255.255.0");
	Config.nif[3].ctc.read.cuu		= 0x510;
	Config.nif[3].ctc.write.cuu		= 0x511;
	memcpy(Config.nif[3].ctc.read.ddname,	"CTC504  ", 8);
	memcpy(Config.nif[3].ctc.write.ddname,	"CTC505  ", 8);

	trace("XCONFIG established configuration defaults\n");

//  Read & parse MVS file specifying configuration overrides, set new values

	xfigger();				// handle configuration file

//  TODO Verify valid configuration

//  Report final values following xfigger

	trace("XCONFIG ");
	kprintf("MVS-Xinu configuration summary:\n");
	xfig_prt();

	return OK;
}



