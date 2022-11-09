/* hwa2string.c - hwa2string */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  hwa2string - create colon-separated string representing hardware addr
 *------------------------------------------------------------------------
 */

char *hwa2string(char *pbuf, struct hwa *phwa) {

	char	*pch = pbuf;
	int	i;


	sprintf(pch, "%02x", phwa->ha_addr[0] & 0xff);
	pch += strlen(pch);
	for (i = 1; i < phwa->ha_len; ++i) {
		sprintf(pch, ":%02x", phwa->ha_addr[i] & 0xff);
		pch += strlen(pch);
	}
	*pch = NULL;
	return pbuf;
} 


