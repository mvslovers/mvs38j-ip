/* xconfig.c - xfigger */

#include <stdio.h>	// Standard C header
#include <string.h>	// Standard C header
#include <ctype.h>	// Standard C header

#define	DEBUG
#include <debug.h>

#define	MAX_TOKENS	10		// max # tokens / line

char	*tokens[MAX_TOKENS + 2];	// token pointers
					// each token is null terminated
					// last valid token ptr followed by NULL

#define	CONFIG_FILE	"CONFIG"	// MVS-Xinu configuration ddname
#define	SYSIN_LEN	80		// # chars SYSIN DD * 

#define	NOTOKEN		-1		// no token found in keyword array

/*------------------------------------------------------------------------
 * xfigkey	Given array of keyword strings, scan for keyword
 *	pkey	pointer to array of keyword strings
 *		Final pointer is NULL
 *	ptext	pointer to text to be found
 *	Returns token # (equal to keyword index) or NOTOKEN
 *------------------------------------------------------------------------
 */

int	xfigkey(char *pkey, char *ptext) {

	int	i = 0;


	while (pkey) {
		if (strcasecmp((void *)*ptext, (void *)*pkey) == 0) {
			return i;
		}
		i++; pkey++;
	}
	return NOTOKEN;
}

/*------------------------------------------------------------------------
 * tokenize	Given null terminated input line, break each
 *		blank-separated token into null terminated string,
 *		add each token pointer to tokens[]
 *	Returns number of tokens stored in tokens[]
  *------------------------------------------------------------------------
 */

int tokenize(char *pline) {

	int	i = 0;
	char	*pchar;
	char	*ptoken;


	//  Clean input so it only consists of spaces, letters or digits
	//  or these special cases:
	//	:	colon	(ethernet address)
	//	.	period	(IP address)

	for (pchar = pline; *pchar != '\0'; pchar++) {
		if (*pchar == ':') continue;
		if (*pchar == '.') continue;
		if (!isalnum(*pchar)) *pchar = ' ';
	}

	memset(tokens, 0xff, sizeof(tokens));	// assure list NULL terminated
	pchar = pline;
	while (*pchar != '\0') {
		if (*pchar != ' ') {		// found a token
			ptoken = pchar;
			while ( (*pchar != ' ') && (*pchar != '\0') )
				pchar++;	// scan across token
			if (*pchar == '\0') 
				pchar--;	// nullify next pchar++
			else
				*pchar = '\0';	// null terminate token
			tokens[i++] = ptoken;
			tokens[i] = NULL;	// assure NULL terminated
			if (i > MAX_TOKENS) {
				kprintf("more than %d tokens; "
					"excess ignored\n", MAX_TOKENS);
				break;
			}
		}
		pchar++;			// skip blank or null char
	}
//	snap("TOKENIZE tokens", (void *)tokens, sizeof(tokens));
//	snap("TOKENIZE line", (void *)pline, SYSIN_LEN + 1);
//	trace("TOKENIZE found %d tokens\n", i);
	return i;				// # tokens in tokens[]
}

/*------------------------------------------------------------------------
 * xfigline	Given input line determine statement type from initial
 *		token, pass line to statement handler
 *	Lines beginning with * are considered comments and ignored
 *	Lines consisting of no tokens are also ignored
 *	No errors are reflected back to our caller, we parse the whole 
 *	input file to catch as many errors as possible
 *------------------------------------------------------------------------
 */

void xfigline(char *pline) {

	int	i;
	char	*ptoken;


	if (*pline == '*') return;		// ignore comment
	if (tokenize(pline) == 0) return;	// ignore blank line

	ptoken = tokens[0];
	if (strcasecmp("iface", ptoken) == 0) {
		xfig_iface(&tokens);
		return;
	}
	if (strcasecmp("gateway", ptoken) == 0) {
		xfig_gate(&tokens);
		return;
	}
	kprintf("statement not recognized; ignored\n");
	return;
}


/*------------------------------------------------------------------------
 * xfigger	MVS-Xinu configuration 
 *		Reads configuration file, modifies Config
 *------------------------------------------------------------------------
 */

void	xfigger() {

	FILE	*fd;
	char	linebuf[SYSIN_LEN + 1];


	fd = fopen(CONFIG_FILE, "r");
	if (fd == NULL) {
		kprintf("XFIGGER file %s open fail\n", CONFIG_FILE);
		return;			// leave default Config ASIS
	}
	trace("XFIGGER Parsing MVS-Xinu configuration file\n");
	while	(!feof(fd)) {
		dbug(memset(linebuf, 0x000, sizeof(linebuf)));
		if (fgets(linebuf, sizeof(linebuf), fd) == NULL) break;
		kprintf("%s", linebuf);	// linebuf supplies own \n
		xfigline(linebuf);
	}
	trace("XFIGGER Parsed MVS-Xinu configuration file\n");
	fclose(fd);
	return;
}

