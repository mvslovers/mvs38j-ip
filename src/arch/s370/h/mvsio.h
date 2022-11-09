/* mvsio.h - MVS38j I/O control blocks */
#ifndef H_MVSIO_H
#define H_MVSIO_H			// only generate header once

struct	mvsiob	{			// MVS IOB control block: IEZIOB
	char	iobflag1;
	char	iobflag2;
	char	iobsens0;
	char	iobsens1;
	union	{
		char	cc;		// POST completion code
		void	*addr;		// ECB address
	} ecb;
	char	iobflag3;
				// 7 low-order bytes of CSW
	char	ccwaddr[3];		// CCW address
	char	unitstat;		// unit status
	char	chanstat;		// channel status
	unsigned short	residual;	// residual byte count

	union	{
		char	iobsiocc;
		void	*iobstart;
	} ccw;

	void	*iobdcbpt;
	void	*iobrestr;
	char	iobincam[2];
	char	ioberrct[2];
	char	slop[32];		// I'm somewhat uncertain about how
					// much IOB is really needed for a CTC;
					// this should be _way_ more than enough
					// and it makes the mvsiob length an
					// integral number of doublewords
					// sizeof(mvsiob) == 0x40 == 64
};
typedef	struct	mvsiob	mvsiob;

struct	ccw0	{			// S/370 Channel Command Word (Format0)
	union	{
		BYTE	cmd;		// channel command code
		void	*addr;		// data address
	} req;
	BYTE	flag;			// flags
	BYTE	zeroes;			// byte containing 0x00
	USHORT	count;			// # bytes to be transferred
};
typedef	struct ccw0	ccw0;

//  The MVS DCB control block varies in size and content offset depending
//  upon the target device type; this dcb layout conforms to MVSCTC expectations

struct	mvsctcdcb	{		// MVS DCB control block for CTC devices
	char	dcb[96];		// larger than necessary but at least
					// as much as needed and an integral
					// number of doublewords
					// sizeof(mvsctcdcb) == 96 == 0x60
};
typedef	struct mvsctcdcb	mvsctcdcb;

#endif


