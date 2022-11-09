/* name.h - fopen */

/* Constants that define the name mapping table sizes */

#define	NAMPLEN		32		/* max size of a name prefix	*/
#define	NAMRLEN		32		/* max size of a replacement	*/
#define	NAMLEN		80		/* maximum size of a file name	*/
#define	NNAMES		14		/* number of prefix definitions	*/

/* Definition of the name prefix table that defines all name mappings */

// jmm - added typedef to following struct
typedef struct	nament	{		/* definition of prefix mapping	*/
	char	npre[NAMPLEN];		/* prefix of a name		*/
	char	nrepl[NAMRLEN];		/* replacement for that prefix	*/
	int	ndev;			/* device for this prefix	*/
};

// jmm - added typedef to following struct to prevent duplicate 
//	symbol definition at MVS link type for Nam

typedef struct	nam	{		/* all name space variables	*/
	int	nnames;			/* number of entries in nametab	*/
	struct	nament	nametab[NNAMES];/* actual table of mappings	*/
} NAM;					// jmm - changed from Nam

#ifndef	NAMESPACE
#define	NAMESPACE	SYSERR
#endif

#define	fopen(n,m)	open(NAMESPACE, n, m)

