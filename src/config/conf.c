/* conf.c (GENERATED FILE; DO NOT EDIT) */

#include <conf.h>

/* device independent I/O switch */

struct	devsw	devtab[NDEVS] = {

/*  Format of entries is:
device-number, device-name,
init, open, close,
read, write, seek,
getc, putc, cntl,
device-csr-address, input-vector, output-vector,
iint-handler, oint-handler, control-block, minor-device,
*/

/*  CONSOLE  is oprcons  */

0, "CONSOLE",
ionull, ionull, ionull,
ioerr, mvsconswrite, ioerr,
ioerr, ioerr, ionull,
(void *)0000000, 0000, 0000,
oprconsiin, oprconsoin, NULLPTR, 0,

/*  ETHER  is eth  */

1, "ETHER",
ctci_init, noopen, noclose,
ctci_read, ctci_write, noseek,
nogetc, noputc, nocntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 0,

/*  ETHER1  is eth  */

2, "ETHER1",
ctci_init, noopen, noclose,
ctci_read, ctci_write, noseek,
nogetc, noputc, nocntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 1,

/*  ETHER2  is eth  */

3, "ETHER2",
ctci_init, noopen, noclose,
ctci_read, ctci_write, noseek,
nogetc, noputc, nocntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 2,

/*  UDP  is dgm  */

4, "UDP",
ionull, dgmopen, noclose,
noread, nowrite, noseek,
nogetc, noputc, dgmcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 0,

/*  DGRAM0  is dg  */

5, "DGRAM0",
dginit, noopen, dgclose,
dgread, dgwrite, noseek,
nogetc, noputc, dgcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 0,

/*  DGRAM1  is dg  */

6, "DGRAM1",
dginit, noopen, dgclose,
dgread, dgwrite, noseek,
nogetc, noputc, dgcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 1,

/*  DGRAM2  is dg  */

7, "DGRAM2",
dginit, noopen, dgclose,
dgread, dgwrite, noseek,
nogetc, noputc, dgcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 2,

/*  DGRAM3  is dg  */

8, "DGRAM3",
dginit, noopen, dgclose,
dgread, dgwrite, noseek,
nogetc, noputc, dgcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 3,

/*  DGRAM4  is dg  */

9, "DGRAM4",
dginit, noopen, dgclose,
dgread, dgwrite, noseek,
nogetc, noputc, dgcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 4,

/*  DGRAM5  is dg  */

10, "DGRAM5",
dginit, noopen, dgclose,
dgread, dgwrite, noseek,
nogetc, noputc, dgcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 5,

/*  DGRAM6  is dg  */

11, "DGRAM6",
dginit, noopen, dgclose,
dgread, dgwrite, noseek,
nogetc, noputc, dgcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 6,

/*  DGRAM7  is dg  */

12, "DGRAM7",
dginit, noopen, dgclose,
dgread, dgwrite, noseek,
nogetc, noputc, dgcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 7,

/*  DGRAM8  is dg  */

13, "DGRAM8",
dginit, noopen, dgclose,
dgread, dgwrite, noseek,
nogetc, noputc, dgcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 8,

/*  DGRAM9  is dg  */

14, "DGRAM9",
dginit, noopen, dgclose,
dgread, dgwrite, noseek,
nogetc, noputc, dgcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 9,

/*  DGRAMA  is dg  */

15, "DGRAMA",
dginit, noopen, dgclose,
dgread, dgwrite, noseek,
nogetc, noputc, dgcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 10,

/*  DGRAMB  is dg  */

16, "DGRAMB",
dginit, noopen, dgclose,
dgread, dgwrite, noseek,
nogetc, noputc, dgcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 11,

/*  DGRAMC  is dg  */

17, "DGRAMC",
dginit, noopen, dgclose,
dgread, dgwrite, noseek,
nogetc, noputc, dgcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 12,

/*  DGRAMD  is dg  */

18, "DGRAMD",
dginit, noopen, dgclose,
dgread, dgwrite, noseek,
nogetc, noputc, dgcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 13,

/*  DGRAME  is dg  */

19, "DGRAME",
dginit, noopen, dgclose,
dgread, dgwrite, noseek,
nogetc, noputc, dgcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 14,

/*  DGRAMF  is dg  */

20, "DGRAMF",
dginit, noopen, dgclose,
dgread, dgwrite, noseek,
nogetc, noputc, dgcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 15,

/*  TCP  is tcpm  */

21, "TCP",
ionull, tcpmopen, noclose,
noread, nowrite, noseek,
nogetc, noputc, tcpmcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 0,

/*  TCP0  is tcp  */

22, "TCP0",
tcpinit, noopen, tcpclose,
tcpread, tcpwrite, noseek,
tcpgetc, tcpputc, tcpcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 0,

/*  TCP1  is tcp  */

23, "TCP1",
tcpinit, noopen, tcpclose,
tcpread, tcpwrite, noseek,
tcpgetc, tcpputc, tcpcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 1,

/*  TCP2  is tcp  */

24, "TCP2",
tcpinit, noopen, tcpclose,
tcpread, tcpwrite, noseek,
tcpgetc, tcpputc, tcpcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 2,

/*  TCP3  is tcp  */

25, "TCP3",
tcpinit, noopen, tcpclose,
tcpread, tcpwrite, noseek,
tcpgetc, tcpputc, tcpcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 3,

/*  TCP4  is tcp  */

26, "TCP4",
tcpinit, noopen, tcpclose,
tcpread, tcpwrite, noseek,
tcpgetc, tcpputc, tcpcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 4,

/*  TCP5  is tcp  */

27, "TCP5",
tcpinit, noopen, tcpclose,
tcpread, tcpwrite, noseek,
tcpgetc, tcpputc, tcpcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 5,

/*  TCP6  is tcp  */

28, "TCP6",
tcpinit, noopen, tcpclose,
tcpread, tcpwrite, noseek,
tcpgetc, tcpputc, tcpcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 6,

/*  TCP7  is tcp  */

29, "TCP7",
tcpinit, noopen, tcpclose,
tcpread, tcpwrite, noseek,
tcpgetc, tcpputc, tcpcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 7,

/*  TCP8  is tcp  */

30, "TCP8",
tcpinit, noopen, tcpclose,
tcpread, tcpwrite, noseek,
tcpgetc, tcpputc, tcpcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 8,

/*  TCP9  is tcp  */

31, "TCP9",
tcpinit, noopen, tcpclose,
tcpread, tcpwrite, noseek,
tcpgetc, tcpputc, tcpcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 9,

/*  TCPA  is tcp  */

32, "TCPA",
tcpinit, noopen, tcpclose,
tcpread, tcpwrite, noseek,
tcpgetc, tcpputc, tcpcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 10,

/*  TCPB  is tcp  */

33, "TCPB",
tcpinit, noopen, tcpclose,
tcpread, tcpwrite, noseek,
tcpgetc, tcpputc, tcpcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 11,

/*  TCPC  is tcp  */

34, "TCPC",
tcpinit, noopen, tcpclose,
tcpread, tcpwrite, noseek,
tcpgetc, tcpputc, tcpcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 12,

/*  TCPD  is tcp  */

35, "TCPD",
tcpinit, noopen, tcpclose,
tcpread, tcpwrite, noseek,
tcpgetc, tcpputc, tcpcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 13,

/*  TCPE  is tcp  */

36, "TCPE",
tcpinit, noopen, tcpclose,
tcpread, tcpwrite, noseek,
tcpgetc, tcpputc, tcpcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 14,

/*  TCPF  is tcp  */

37, "TCPF",
tcpinit, noopen, tcpclose,
tcpread, tcpwrite, noseek,
tcpgetc, tcpputc, tcpcntl,
(void *)0000000, 0000, 0000,
noiint, nooint, NULLPTR, 15
	};
