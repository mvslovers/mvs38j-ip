/* ctcread.h - MVS-Xinu CTC header */

#ifndef H_CTCREAD_H
#define H_CTCREAD_H			// only generate header once

#include <ctc.h>

//------------------------------------------------------------------------------ctcread.h

//  For ctcioread, ctdev.ioword (see ctc.h) points to a ctreadstat which is
//  shared between ctcioread and ctcread.

struct	ctreadstat	{

	//  When len[avail] != 0, buf[avail] points at a filled buffer

	//  When pseg == NULL, no data from buf[avail] has been
	//  returned to ctcread's caller; else pseg points at the
	//  next-to-be-returned segment header

	char	eye[16];		// +0x00 eyecatcher CTCREADSTAT

	BYTE	flag;			// +0x10 spare
	char	reserved[3];		// +0x11 spare
	int	avail;			// +0x14 buf[] & len[] index
	void	*pseg;			// +0x18 ptr to next-to-be-returned segment
	void	*spare;			// +0x1C unused

	void	*buf[CTC_READBUFS];	// +0x20 buffer addr

	int	len[CTC_READBUFS];	// +0x?? length of data in buffer
};
typedef	struct	ctreadstat	ctreadstat;
//	enum	crsenum {			// ctreadstat.flag
//	};

//------------------------------------------------------------------------------ctcread.h
#endif

