/* xfig_gate.c - xfigger */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <config.h>	// Config
#include <string.h>	// Standard C header

#define	DEBUG
#include <debug.h>

/*------------------------------------------------------------------------
 * xfig_gate	Handles MVS-Xinu gateway configuration statement
 *------------------------------------------------------------------------
 */

void	xfig_gate(char **pptoken) { 

	char	*ptoken;

	for ( ; *pptoken; pptoken++) {

		ptoken = *pptoken;

		if (strcasecmp(ptoken, "enable") == 0) {
			Config.megate = TRUE;
			continue;
		}
		if (strcasecmp(ptoken, "enabled") == 0) {	// alias
			Config.megate = TRUE;
			continue;
		}
		if (strcasecmp(ptoken, "disable") == 0) {
			Config.megate = FALSE;
			continue;
		}
		if (strcasecmp(ptoken, "disabled") == 0) {	// alias
			Config.megate = FALSE;
			continue;
		}
		kprintf("XFIG_GATE unrecognized token %s; ignored\n", ptoken);
	}
	return;
}
 


