# Makefile for linux, using GNU make

# note: sometimes a problem when using C++ streams with -O,
# that's why a couple manually entered rules are necessary
# [still true?]
#
# I'm working on making the makefile portable to DOS


# uncomment next line to link in special debugging memory allocator
#MEM=-DMEMDEBUG


# unix
DEBUG= -g
OPT=
WARNINGS= -Wall -pedantic
CXXFLAGS= $(DEBUG) $(WARNINGS) $(OPT)
LDFLAGS= $(OPT)
LDLIBS= -lm -lcurses -ltermcap

O=.o
COPY=cp
EXEC=


OS=	strlwr$(O)	\
	coord$(O)	\
	move$(O)	\
	moveq$(O)	\
	boardcon$(O)	\
	movegen$(O)	\
	static$(O)	\
	statread$(O)	\
	alphabet$(O)	\
	interact$(O)	\
	getopts$(O)	\
	memhand$(O)	\
	display$(O)	\
	main$(O)		


############

all:	wischk$(EXEC) dumchk$(EXEC)

wischk$(EXEC): $(OS)
	$(CXX) -o wischk $(CXXFLAGS) $(OS) $(LDLIBS)

dumchk$(EXEC): wischk$(EXEC)
	$(COPY) wischk$(EXEC) dumchk$(EXEC)

##############

main$(O): main.cpp boardcon.h move.h piece.h globals.h alphabet.h movegen.h coord.h interact.h getopts.h memhand.h alphabet.h static.h display.h dispprim.h
	$(CXX) -c $(DEBUG) $(WARNINGS) -DDISP_CURSES main.cpp

static$(O): static.cpp boardcon.h globals.h movegen.h static.h alphabet.h

statread$(O): statread.cpp static.h
	$(CXX) -c $(DEBUG) $(WARNINGS) statread.cpp

alphabet$(O): alphabet.cpp globals.h boardcon.h move.h movegen.h piece.h alphabet.h static.h

display$(O): display.cpp display.h dispprim.h
	$(CXX) -c $(CXXFLAGS) -DDISP_CURSES display.cpp

movegen$(O): movegen.cpp movegen.h boardcon.h coord.h piece.h moveq.h move.h

moveq$(O): moveq.cpp moveq.h move.h coord.h

coord$(O): coord.cpp coord.h

move$(O): move.cpp move.h coord.h

boardcon$(O): boardcon.cpp boardcon.h move.h coord.h piece.h 

getopts$(O): getopts.cpp getopts.h

strlwr$(O): strlwr.cpp

interact$(O): movegen.h interact.h display.h dispprim.h interact.cpp
	$(CXX) -c $(CXXFLAGS) -DDISP_CURSES interact.cpp

memhand$(O): memhand.h memhand.cpp
	$(CXX) -c $(CXXFLAGS) $(MEM) memhand.cpp

clean:
	rm -f *.o
	rm -f core

zip:	wischk.man
	rm wischk.zip
	cp master.cfg checkers.cfg
	(cd ..; wischk/makezip wischk)

tar:	wischk.man
	cp master.cfg checkers.cfg
	(cd ..; wischk/maketar wischk)

man:	wischk.man

wischk.man: wischk.6
	nroff -man -Tascii wischk.6 > wischk.man
