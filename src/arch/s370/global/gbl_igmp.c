/* gbl_igmp.c - define IGMP global data area */

// Defines whatever igmp/iginit.c defines - jmm

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sleep.h>
#include <network.h>
#include <igmp.h>

// Added following:  - jmm (was extern)
int	hgseed;


struct	hginfo	HostGroup;
struct	hg	hgtable[HG_TSIZE];

