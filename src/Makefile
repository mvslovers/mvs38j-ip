#
#  Makefile for the MVS38j version of Xinu
#

# NOTE NOTE NOTE	ADDING SOURCE SUBDIRECTORIES
#
#	Update xinu/build/macros.mk
#	Add ${dir} and ${dirOBJ} to XINUSRC, XINUOBJ, S370SRC, S370OBJ
#	Add targets (behind all: tagline)
#	Add subdir to 'make depend' list in this file (after depend: tagline)
#	Add build prototype (behind conf.c conf.h: tagline)
#	If pure S/370 assembler, manually add object targets at bottom of this file
#
# NOTE NOTE NOTE

# Pull common Xinu make macros into this Makefile

include ../build/macros.mk

# tell make to look for object decks in object deck repository
# (keeps source tree less cluttered ... there are a lot of object decks).

VPATH = {$BUILD}/obj

# source and object name definitions created by 'make -f compile/Makefile'
# (which runs mvs38j-genall) invoked by mvs38j-build outside this Makefile
# (run before this Makefile is used)

INCMK = compile/inc.mk

# The following 'make include' pulls in the source and object names
# created by mvs38j-genall

include ${INCMK}

# XINUSRC aggregates the lists of Xinu source elements which we pulled in from
# the 'include ${INCMK}' above.  Several of these macros have null definitions
# due to their source not currently being present in the source tree.

XINUSRC = ${ARP} ${COM} ${DEBUG} ${DGRAM} \
	${ICMP} ${IGMP} ${IP} ${KBM} ${LIB} ${NET} ${NETAPP} ${NETUTIL} \
	${OSPF} ${RFS} ${RIP} ${ROUTE} ${SNMP} ${SYS} ${TCP} ${TCPD} \
	${TTY} ${UDP} ${USER}

# XINUOBJ aggregates the lists of Xinu object elements which we pulled in from
# the 'include ${INCMK}' above.

XINUOBJ = ${ARPOBJ} ${COMOBJ} ${DEBUGOBJ} ${DGRAMOBJ} \
	${ICMPOBJ} ${IGMPOBJ} ${IPOBJ} ${KBMOBJ} ${LIBOBJ} ${NETOBJ} ${NETAPPOBJ} ${NETUTILOBJ} \
	${OSPFOBJ} ${RFSOBJ} ${RIPOBJ} ${ROUTEOBJ} ${SNMPOBJ} ${SYSOBJ} \
	${TCPOBJ} ${TCPDOBJ} ${TTYOBJ} ${UDPOBJ} ${USEROBJ}

# S370SRC aggregates the list of S/370 source code built with Systems/C and Systems/ASM.

S370SRC = ${CLOCK} ${CONSOLE} ${CTC} ${DEVICE} ${ETHER} ${GLOBAL} \
	${KERNEL} ${MEMORY} ${MVS} ${MVSASM} ${PROCESS} \
	${QUEUE} ${SEM} ${TEMP}

# S370OBJ aggregates the lists of MVS object elements which we pulled in from
# the 'include ${INCMK}' above.

S370OBJ = ${CLOCKOBJ} ${CONSOLEOBJ} ${CTCOBJ} ${DEVICEOBJ} ${ETHEROBJ} ${GLOBALOBJ} \
	${KERNELOBJ} ${MEMORYOBJ} ${MVSOBJ} ${MVSASMOBJ} ${PROCESSOBJ} \
	${QUEUEOBJ} ${SEMOBJ} ${TEMPOBJ}

# S370IGNORE lists the S/370 source not addressed by this makefile.
# Currently, the source specified here is ignored since the MVS build side
# takes care of assembling and linking this code.

S370IGNORE = ${STCP} ${U370}

# BUILDOBJ defines the object decks to build.

BUILDOBJ = ${XINUOBJ} ${S370OBJ}

# The all: tag defines the list of elements we want to make sure are current
# based on their previously defined dependencies (which we pull in via the
# ${INCMK} file created by mvs38j-genall).  Thus all the object decks listed
# by BUILDOBJ are checked, and created if not current (or don't exist).

all:		${BUILDOBJ}
arp:		${ARPOBJ}
clock:		${CLOCKOBJ}
com:		${COMOBJ}
config:		${CONFIGOBJ}
console:	${CONSOLEOBJ}
ctc:		${CTCOBJ}
debug:		${DEBUGOBJ}
device:		${DEVICEOBJ}
dgram:		${DGRAMOBJ}
ether:		${ETHEROBJ}
global:		${GLOBALOBJ}
icmp:		${ICMPOBJ}
igmp:		${IGMPOBJ}
ip:		${IPOBJ}
kbm:		${KBMOBJ}
kernel:		${KERNELOBJ}
lib:		${LIBOBJ}
memory:		${MEMORYOBJ}
mvs:		${MVSOBJ}
mvsasm:		${MVSASMOBJ}
net:		${NETOBJ}
netapp:		${NETAPPOBJ}
netutil:	${NETUTILOBJ}
ospf:		${OSPFOBJ}
process:	${PROCESSOBJ}
queue:		${QUEUEOBJ}
rfs:		${RFSOBJ}
rip:		${RIPOBJ}
route:		${ROUTEOBJ}
sem:		${SEMOBJ}
snmp:		${SNMPOBJ}
sys:		${SYSOBJ}
tcp:		${TCPOBJ}
tcpd:		${TCPDOBJ}
temp:		${TEMPOBJ}
tty:		${TTYOBJ}
udp:		${UDPOBJ}
user:		${USEROBJ}
stcp:		${STCPOBJ}
u370:		${U370OBJ}

install: 
	${PLINK} ${BUILDOBJ}

clean: 
	-rm -rf *.xsd *.ln *.errs Makefile.bak *.a a.xsdut core errs pm.xsdut	\
		conf.c conf.h tags typescript* version *.obj38
	rm -f ../build/asm/*.asm
	rm -f ../build/depend/*
	touch ${NEWDEP}
	rm -f ../build/dar/libxinu.a
	rm -f ../build/list/*.list
	rm -f ../build/listc/*.list
	rm -f ../build/log/*.txt
	rm -f ../build/obj/*
	rm -f ../build/plink/*.txt
	rm -f arch/s370/h/*.h~
	rm -f h/*.h~
#	(cd shell; make clean)			# not ready to be built
#						# mkdep freaks on redefines
#						# need to repair shell/Makefile
#						# ... or something

depend:
	rm -f ${DEPFILE} ${NEWDEP}
	touch ${DEPFILE} ${NEWDEP}
	mkdep -f ${DEPFILE} \
		${DEFS} ${INCLUDE} \
		arp/*.c \
		com/*.c \
		config/*.c \
		debug/*.c \
		dgram/*.c \
		icmp/*.c \
		igmp/*.c \
		ip/*.c \
		kbm/*.c \
		lib/*.c \
		net/*.c \
		netapp/*.c \
		netutil/*.c \
		ospf/*.c \
		rfs/*.c \
		rip/*.c \
		route/*.c \
		snmp/*.c \
		sys/*.c \
		tcp/*.c \
		tcpd/*.c \
		tty/*.c \
		udp/*.c \
		user/*.c \
		arch/s370/clock/*.c \
		arch/s370/console/*.c \
		arch/s370/ctc/*.c \
		arch/s370/device/*.c \
		arch/s370/ether/*.c \
		arch/s370/global/*.c \
		arch/s370/kernel/*.c \
		arch/s370/memory/*.c \
		arch/s370/mvs/*.c \
		arch/s370/process/*.c \
		arch/s370/queue/*.c \
		arch/s370/sem/*.c \
		arch/s370/temp/*.c
	${FIXDEP} ${DEPFILE} ${NEWDEP}

#	(cd shell; make depend)			# not ready to be built at 'make depend' time

Configuration:
	(cd config; make clean; make; make install)

conf.h conf.c: Configuration

${ARPOBJ}:
	 ${CC}  arp `basename $@ .xsd`
${CLOCKOBJ}:
	 ${CC}  arch/s370/clock `basename $@ .xsd`
${COMOBJ}:
	 ${CC}  com `basename $@ .xsd`
${CONFIGOBJ}:
	 ${CC}  config `basename $@ .xsd`
${CONSOLEOBJ}:
	 ${CC}  arch/s370/console `basename $@ .xsd`
${CTCOBJ}:
	 ${CC}  arch/s370/ctc `basename $@ .xsd`
${DEBUGOBJ}:
	 ${CC}  debug `basename $@ .xsd`
${DEVICEOBJ}:
	 ${CC}  arch/s370/device `basename $@ .xsd`
${DGRAMOBJ}:
	 ${CC}  dgram `basename $@ .xsd`
${ETHEROBJ}:
	 ${CC}  arch/s370/ether `basename $@ .xsd`
${GLOBALOBJ}:
	 ${CC}  arch/s370/global `basename $@ .xsd`
${ICMPOBJ}:
	 ${CC}  icmp `basename $@ .xsd`
${IGMPOBJ}:
	 ${CC}  igmp `basename $@ .xsd`
${IPOBJ}:
	 ${CC}  ip `basename $@ .xsd`
${KBMOBJ}:
	 ${CC}  kbm `basename $@ .xsd`
${KERNELOBJ}:
	 ${CC}  arch/s370/kernel `basename $@ .xsd`
${LIBOBJ}:
	 ${CC}  lib `basename $@ .xsd`
${MEMORYOBJ}:
	 ${CC}  arch/s370/memory `basename $@ .xsd`
${MVSOBJ}:
	 ${CC}  arch/s370/mvs `basename $@ .xsd`
${MVSASMOBJ}:
	 ${CC}  arch/s370/mvsasm `basename $@ .xsd` asm
${NETOBJ}:
	 ${CC}  net `basename $@ .xsd`
${NETAPPOBJ}:
	 ${CC}  netapp `basename $@ .xsd`
${NETUTILOBJ}:
	 ${CC}  netutil `basename $@ .xsd`
${OSPFOBJ}:
	 ${CC}  ospf `basename $@ .xsd`
${PROCESSOBJ}:
	 ${CC}  arch/s370/process `basename $@ .xsd`
${QUEUEOBJ}:
	 ${CC}  arch/s370/queue `basename $@ .xsd`
${RFSOBJ}:
	 ${CC}  rfs `basename $@ .xsd`
${RIPOBJ}:
	 ${CC}  rip `basename $@ .xsd`
${ROUTEOBJ}:
	 ${CC}  route `basename $@ .xsd`
${SEMOBJ}:
	 ${CC}  arch/s370/sem `basename $@ .xsd`
${SNMPOBJ}:
	 ${CC}  snmp `basename $@ .xsd`
${SYSOBJ}:
	 ${CC}  sys `basename $@ .xsd`
${TCPOBJ}:
	 ${CC}  tcp `basename $@ .xsd`
${TCPDOBJ}:
	 ${CC}  tcpd `basename $@ .xsd`
${TEMPOBJ}:
	 ${CC}  arch/s370/temp `basename $@ .xsd`
${TTYOBJ}:
	 ${CC}  tty `basename $@ .xsd`
${UDPOBJ}:
	 ${CC}  udp `basename $@ .xsd`
${USEROBJ}:
	${CC}	user `basename $@ .xsd`
${STCPOBJ}:
	 ${CC} arch/s370/stcp `basename $@ .obj` asm
${U370OBJ}:
	 ${CC} arch/s370/u370 `basename $@ .obj` asm

#
#  We manually specify *.asm code dependencies, mkdep doesn't seem to cope
#  with S/370 assembler input
#

include ${NEWDEP}
../build/obj/procload.xsd:	arch/s370/mvsasm/procload.asm
../build/obj/procsave.xsd:	arch/s370/mvsasm/procsave.asm
../build/obj/procsw.xsd:	arch/s370/mvsasm/procsw.asm
../build/obj/mvsclkinit.xsd:	arch/s370/mvsasm/mvsclkinit.asm
../build/obj/mvswait.xsd:	arch/s370/mvsasm/mvswait.asm
../build/obj/mvsctc.xsd:	arch/s370/mvsasm/mvsctc.asm
../build/obj/mvsattn.xsd:	arch/s370/mvsasm/mvsattn.asm
../build/obj/mvsiatn.xsd:	arch/s370/mvsasm/mvsiatn.asm
../build/obj/igg019x8.xsd:	arch/s370/mvsasm/igg019x8.asm
../build/obj/igg019y8.xsd:	arch/s370/mvsasm/igg019y8.asm
../build/obj/mvswaitlist.xsd:	arch/s370/mvsasm/mvswaitlist.asm
../build/obj/mvsdoze.xsd:	arch/s370/mvsasm/mvsdoze.asm
../build/obj/mvsevent.xsd:	arch/s370/mvsasm/mvsevent.asm




