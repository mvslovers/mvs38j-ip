
#
#  Common include file for Xinu Makefile's
#

# Note: make can be sensitive to whitespace following the definitions of some 
# of these macros (since we concatenate macro values); be careful.

# path to S/370 source tree (relative to src)

S370ARCH = arch/s370

# path to S/370 build tree (relative to src)

BUILD = ../build

DEFS = 
CFLAGS = 

# MVS38j C header dir

S370INCL = arch/s370/h
S370INCL2 = arch/s370/temph
S370INCL3 = arch/s370/temp

# Dignus Systems/C header dir

DIGINCL = /usr/local/dignus/160/include

#						# give mkdep Dignus Systems/C includes, too
#						# prevents mkdep from using gcc C headers
#						# and complaining endlessly about redefines
INCLUDE = -Ih -I${S370INCL} -I${S370INCL2} -I${S370INCL3} -I${DIGINCL}

# dependency file output from mkdep

DEPFILE = ${BUILD}/depend/olddep

# dependency file rewritten by mvs38j-fixdep

NEWDEP  = ${BUILD}/depend/newdep

# path to MVS38j perl scripts used in Xinu Makefiles

PERL = ${BUILD}/perl

# path and filename of mkdep post-processor

FIXDEP = ${PERL}/mvs38j-fixdep

# path and filename of Systems/C compile and assembly processor

CC = ${PERL}/mvs38j-compile

# path and filename of Systems/C prelinker

PLINK = ${PERL}/mvs38j-plink

# Xinu source tree directories, relative to src
# S/370 assembly code is specified, so that mvs38j-genall
# will create SRC and OBJ definitions in compile/inc.mk
# Currently, the s370/stcp and s370/u370 subdirs are not actually
# in the source tree.

SRCDIR = arp com debug dgram ether icmp igmp ip kbm \
	lib net netapp netutil ospf rfs rip route snmp sys \
	tcp tcpd tty udp user \
	arch/s370/clock \
	arch/s370/console \
	arch/s370/ctc \
	arch/s370/device \
	arch/s370/ether \
	arch/s370/global \
	arch/s370/kernel \
	arch/s370/memory \
	arch/s370/mvs \
	arch/s370/mvsasm \
	arch/s370/process \
	arch/s370/queue \
	arch/s370/sem \
	arch/s370/stcp \
	arch/s370/temp \
	arch/s370/u370

INCMK = compile/inc.mk



