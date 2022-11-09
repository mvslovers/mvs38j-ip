/* ctcwrite.h - MVS-Xinu CTC header */

#ifndef H_CTCWRITE_H
#define H_CTCWRITE_H			// only generate header once

#include <ctc.h>

//------------------------------------------------------------------------------ctcwrite.h

//  For ctci_iowrite, ctdev.ioword (see ctc.h) points to a ctwritestat which is
//  shared between ctci_iowrite and ctci_write.

struct	ctwritestat	{
	char	eye[16];		// +0x00 eyecatcher CTCWRITESTAT
	BYTE	flag;			// +0x10 spare
	char	reserved[3];		// +0x11 spare
	void	*fillbuf;		// +0x14 NULL or @ buffer ctci_write is filling
					//       address of CTCIHDR @ beginning of buffer
	int	pidsleep;		// +0x18 ctci_iowrite pid when sleeping, else zero
	void	*spare;			// +0x1C spare
	void	*filled[CTC_WRITEBUFS];	// +0x20 ptrs to buffers filled by ctci_write
};
typedef	struct	ctwritestat	ctwritestat;

//------------------------------------------------------------------------------ctcwrite.h
#endif

