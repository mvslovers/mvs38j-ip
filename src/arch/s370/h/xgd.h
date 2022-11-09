/* xgd.h - mvs-Xinu Global Data */

#ifndef H_XGD_H
#define H_XGD_H			// only generate header once

//  MVS38j TCP/IP subsystem global data (beginning portion only)
//  Note: older version of the DSSSGD macro refer to the SGD@XGD
//  field as SGD@SQH; MVS-Xinu overlays this field until I get a 
//  chance to fix the macro and supporting stcpss00.asm code.
//  ** DSSSGD updated, STCP370 rebuilt 2003-04-02 - jmm

struct	sgd {
	char	sgdid[3];		// C'SGD' - control block id
	char	sgdflg1;		// MVS38j TCP/IP status flags
	void	*sgd_xgd;		// pointer to XGD
};
typedef	struct sgd	sgd;

//  MVS-Xinu global area
//  The beginning of the XGD is reserved for our Channel End Appendage (IGG019X8)
//  Keep in sync with XGDCHE DSECT in IGG019X8

struct	xgd {
	void	*cheappr13;		// most recent R13 from IGG019X8
					// very short term usage (debug)
	void	*ctclink;		// CTC device chain: ctdev
	char	eye[4];			// "XGD"
	void	*spare[5];
}
typedef	struct xgd	xgd;

#if 0
//  Note: following fields removed from xgd, placed in mvs38j.h
//  as extern fields (memlen renamed mvsmemlen)
	char		sysmask;	// S/370 system mask
	int		memlen;		// mvsallocmem memory length
	void		*pmemory;	// mvsallocmem memory addr
#endif /* 0 */

#endif


















