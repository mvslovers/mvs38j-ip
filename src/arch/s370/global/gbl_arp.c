/* gbl_arp.c - define ARP-related global data area */

#include <conf.h>
#include <kernel.h>
#include <network.h>

int	rarpsem;
int	rarppid;
struct	arpentry	arptable[ARP_TSIZE];


