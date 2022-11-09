/* xinutype.h - IPaddr, Eaddr */

#ifndef	H_XINUTYPE_H
#define	H_XINUTYPE_H


// from ip.h:

#define	IP_ALEN	4		// IPv4 address length in bytes (octets)
typedef	unsigned long IPaddr;	//  internet address

// from ether.h:

#define	EP_ALEN	6		// # octets in physical ethernet address
typedef	unsigned char	Eaddr[EP_ALEN]; // physical Ethernet address

#endif 


