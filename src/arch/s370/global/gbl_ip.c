/* gbl_ip.c - define IP-related global data area */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <ip.h>

/* special IP addresses */

IPaddr	ip_anyaddr = 0;
#if	BYTE_ORDER == LITTLE_ENDIAN
IPaddr	ip_loopback = 0x0100007F;	/* 127.0.0.1 */
#else	/* BYTE_ORDER */
IPaddr	ip_loopback = 0x7F000001;	/* 127.0.0.1 */
#endif	/* BYTE_ORDER */

// Removed from ipproc

#ifdef DEFINE_EXTERN
int	ippid;
int	gateway;
int	bsdbrc = BSDBRC;	// defined in Configuration
				// 0 = broadcast 255.255.255.255
				// 1 = broadcast 0.0.0.0

IPaddr	ip_maskall = 0xffffffff;	// removed from setmask

#endif 





