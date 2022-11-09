/* gbl_net.c - define net global data area */

#include <conf.h>
#include <kernel.h>
#include <network.h>

#ifdef DEFINE_EXTERN
struct	netif		nif[NIF];
struct	netinfo		Net;
#endif


