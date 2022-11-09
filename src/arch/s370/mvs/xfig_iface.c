/* xfig_iface.c - xfig_iface */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <config.h>	// Config
#include <string.h>	// Standard C header

#define	DEBUG
#include <debug.h>

static const char	*keys[] = {
	"gw",
	"ddname",
	"cuu",
	"CTCI",
	"LCS",
	"LOOP",
	NULL
};
#define	NUMTOKENS	7	

static	int	iface;		// interface number we're working on
				// file scoped to preserve across calls

static	ctchalf	*phalf;		// ptr to current CTC half

/*------------------------------------------------------------------------
 * xfig_iface	Handles MVS-Xinu iface configuration statement
 *------------------------------------------------------------------------
 */

void	xfig_iface(char **pptoken) { 

	char	*ptoken;

	for ( ; *pptoken; pptoken++) {

		ptoken = *pptoken;

		if (strcasecmp(ptoken, "iface") == 0) {
			phalf = (ctchalf *)NULL;		// restore default: none
			pptoken++; ptoken = *pptoken;
			iface = atoi(ptoken);			// interface #
			continue;
		}
		if (strcasecmp(ptoken, "enable") == 0) {
			Config.nif[iface].enabled = TRUE;
			continue;
		}
		if (strcasecmp(ptoken, "enabled") == 0) {	// alias
			Config.nif[iface].enabled = TRUE;
			continue;
		}
		if (strcasecmp(ptoken, "disable") == 0) {
			Config.nif[iface].enabled = FALSE;
			continue;
		}
		if (strcasecmp(ptoken, "disabled") == 0) {	// alias
			Config.nif[iface].enabled = FALSE;
			continue;
		}
		if (strcasecmp(ptoken, "type") == 0) {
			pptoken++; ptoken = *pptoken;
			if (strcmp(ptoken, "LOOP") == 0) {
				Config.nif[iface].type = NIFT_LOOP;
				continue;
			}
			if (strcmp(ptoken, "CTCI") == 0) {
				Config.nif[iface].type = NIFT_CTCI;
				continue;
			}
			if (strcmp(ptoken, "LCS") == 0) {
				Config.nif[iface].type = NIFT_LCS;
				continue;
			}
			kprintf("unknown network interface type; interface %d disabled\n", iface);
			Config.nif[iface].enabled = FALSE;
			continue;
		}
		if (strcasecmp(ptoken, "ip") == 0) {
			pptoken++; ptoken = *pptoken;
			Config.nif[iface].ip = dot2ip(ptoken);
			continue;
		}
		if (strcasecmp(ptoken, "mask") == 0) {
			pptoken++; ptoken = *pptoken;
			Config.nif[iface].mask = dot2ip(ptoken);
			continue;
		}
		if (strcasecmp(ptoken, "defaultgw") == 0) {
			pptoken++; ptoken = *pptoken;
			Config.nif[iface].gwip = dot2ip(ptoken);
			Config.nif[iface].dfgw = TRUE;
			continue;
		}
		if (strcasecmp(ptoken, "read") == 0) {
			phalf = &Config.nif[iface].ctc.read;
			continue;
		}
		if (strcasecmp(ptoken, "write") == 0) {
			phalf = &Config.nif[iface].ctc.write;
			continue;
		}
		if (strcasecmp(ptoken, "ddname") == 0) {
			if (phalf == (ctchalf *)NULL) {
				kprintf("specify read or write before ddname; ddname ignored\n");
			} else {
				pptoken++; ptoken = *pptoken;
				memset(phalf->ddname, ' ', sizeof(phalf->ddname));
				strncpy(phalf->ddname, ptoken, strlen(ptoken));
			}
			continue;
		}
		if (strcasecmp(ptoken, "cuu") == 0) {
			if (phalf == (ctchalf *)NULL) {
				kprintf("specify read or write before ddname; ddname ignored\n");
			} else {
				pptoken++; ptoken = *pptoken;
				phalf->cuu = (short) (strtol(ptoken, NULL, 16) & 0xfff);	// S/370 cuu
			}
			continue;
		}
		kprintf("XFIG_IFACE unrecognized token %s; ignored\n", ptoken);
	}
	return;
}

