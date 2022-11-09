/* ctci.h - MVS-Xinu CTCI header */

#ifndef H_CTCI_H
#define H_CTCI_H			// only generate header once
//------------------------------------------------------------------------------ctci.h

//  ----------------------------------------------------------------------------
//  Hercules CTCI buffers
//  ----------------------------------------------------------------------------

//  The following 2 structs describes the buffer when we issue 
//  either a READ CCW or WRITE CCW against a CTCI device.
//  For CTCI devices, there is only one block per buffer
//  Thanks to Jim Pierson for clearing up the buffer layout.

//  A) ctcihdr.offfset: 2 byte offset to next block offset
//  B) 6 byte segment header (ctcseghdr)
//  C) variable length IP frame, 
//  Optionally followed by more segments, each with their own
//  segment header & IP frame
//  D) Final ctcihdr.offset (offset = 0x0000)

#define	CTCI_BH_FINAL	0x0000		// final CTCIHDR offset value

struct	ctcihdr	{
	UHWORD		offset;		// offset to next block offset
};
typedef	struct ctcihdr	ctcihdr;

#define	CTCI_SH_FRAMTYP	0x0800		// IPv4 CTCI seg hdr frame type
#define	CTCI_SH_RESV	0x0000		// CTCI seg hdr reserved field

struct	ctcseghdr	{		// block header
	HWORD		seglen;		// segment length, including length
					// of segment header (6)
	HWORD		frametype;	// frame type; always 0x0800 for IPv4
	HWORD		reserved;	// reserved; 0x0000
};
typedef	struct ctcseghdr	ctcseghdr;

PROCESS	ctci_ioread();				// CTCI I/O read process
PROCESS	ctci_iowrite();				// CTCI I/O write process

//------------------------------------------------------------------------------ctci.h
#endif
