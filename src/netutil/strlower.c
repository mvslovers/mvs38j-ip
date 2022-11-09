/* strlower.c - strlower */

#include <ctype.h>	// standard C header

#undef	DEBUG		// this can't possibly have bugs, right?
#include <debug.h> 


/*------------------------------------------------------------------------
 *  strlower	copy src to dst, converting to lower case
 *------------------------------------------------------------------------
 */

char 	*strlower(char *dst, char *src) {

	char	*p;
	int	c;


	for ( ; *p; p++) {
		c = *p;
		*p = tolower(c);
	}
	return dst;
}

