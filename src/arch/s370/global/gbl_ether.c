/* gbl_ether.c - define ether-related global data area */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <ether.h>

				// from PC-Xinuether/ue/ue_init.c
struct utdev ue[Neth];		/* should be "Nue" */

struct ep	dummy_header_ep;	// for ether.h EP_HLEN define

