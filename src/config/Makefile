#
# Make the Xinu configuration program, which is run in the hosting 
# environment (Linux, Windows, etc.) to build config/conf.h
# and config/conf.c (copied to user/conf.c, h/conf.h)
#

DEFS=
CFLAGS = -g ${DEFS}

all:		config

config:		lex.yy.c y.tab.c
		cc ${CFLAGS} -o config y.tab.c -ll

lex.yy.c:	config.l
		lex config.l

y.tab.c:	config.y
		yacc config.y

install:	config
		./config
		cp conf.c ../user/conf.c
		cp conf.h ../h/conf.h

clean:
		rm -f lex.yy.c y.tab.c core config *~






