# Variable definitions.

PLATFORM	= $(shell uname)
CC		= gcc
OBJDIR		= ./objs
TESTS		= ./tests
INCLUDES	= ./include
SRC		= ./src
INCFLAGS	:= -I${INCLUDES}
#OPTFLAGS	:= -Os -DNDEBUG
OPTFLAGS	:= -g
CCFLAGS		:= -c ${OPTFLAGS} -Wall -std=c99

# The list of objects to include in the library


LIBEIOBJS	:= 	${OBJDIR}/ei_application.o ${OBJDIR}/ei_geometrymanager.o ${OBJDIR}/ei_widget.o ${OBJDIR}/ei_event.o ${OBJDIR}/ei_widgetclass.o ${OBJDIR}/ei_classes.o ${OBJDIR}/ei_geometryclasses.o ${OBJDIR}/ei_eventmanager.o ${OBJDIR}/ei_globs.o ${OBJDIR}/ei_entry.o 




# Platform specific definitions (OS X, Linux, Windows)

ifeq (${PLATFORM},Darwin)

	# Building for Mac OS X
	
	PLATDIR		= _osx
	INCFLAGS	:= ${INCFLAGS} -I/opt/local/include -I/opt/local/include/SDL
	LINK		= ${CC}
	LIBEI		= ${OBJDIR}/libei.a
	LIBEIBASE	= ${PLATDIR}/libeibase.a
	LIBS		= ${LIBEIBASE} -L/opt/local/lib -lfreeimage -lSDL -lSDL_ttf -lSDL_gfx -framework AppKit

else
	ifeq (${PLATFORM},Linux)

	# Building for Linux
	
	PLATDIR		= _x11
	INCFLAGS	:= ${INCFLAGS} -I/usr/include/SDL
	LINK		= ${CC}
	LIBEI		= ${OBJDIR}/libei.a
#	ARCH	        = 32
	ARCH	        = 64
	LIBEIBASE	= ${PLATDIR}/libeibase${ARCH}.a
	LIBS		= ${LIBEIBASE} -L${PLATDIR} -lSDL -lSDL_ttf -lSDL_gfx${ARCH} -lfreeimage${ARCH} -lm -lstdc++

	else

	# Building for Windows
	
	PLATDIR		= _win
	INCFLAGS	:= ${INCFLAGS} -I/usr/include -I/usr/include/SDL -I/usr/local/include/SDL
	LINK		= ${CC} -mwindows -mno-cygwin
	LIBEI		= ${OBJDIR}/libei.lib
	LIBEIBASE	= ${PLATDIR}/libeibase.lib
	LIBS		= -lmingw32 ${LIBEIBASE} /usr/lib/SDL_ttf.lib /usr/local/lib/libSDL_gfx.a -L/usr/lib -lSDL -lFreeImage

	endif
endif



# Main target of the makefile. To build specific targets, call "make <target_name>"

TARGETS		=	${LIBEI} \
			minimal frame frame_button button hello_world \
			toplevel puzzle minesweeper two048 parsing multiple_widget \
			entry two_toplevel
all : ${TARGETS}



########## Test-programs

${OBJDIR}/ei_%.o : ${SRC}/ei_%.c ${INCLUDES}/ei_%.h
	${CC} ${CCFLAGS} ${INCFLAGS} -o $@ $<

# minimal

minimal : ${OBJDIR}/minimal.o ${LIBEIBASE}
	${LINK} -o minimal ${OBJDIR}/minimal.o ${LIBS}

${OBJDIR}/minimal.o : ${TESTS}/minimal.c
	${CC} ${CCFLAGS} ${INCFLAGS} ${TESTS}/minimal.c -o ${OBJDIR}/minimal.o

# frame

frame : ${OBJDIR}/frame.o ${LIBEIOBJS} ${LIBEIBASE} ${LIBEI}
	${LINK} -o frame ${OBJDIR}/frame.o ${LIBEIOBJS} ${LIBEI} ${LIBS}

${OBJDIR}/frame.o : ${TESTS}/frame.c
	${CC} ${CCFLAGS} ${INCFLAGS} ${TESTS}/frame.c -o ${OBJDIR}/frame.o

# multiple_widget

multiple_widget : ${OBJDIR}/multiple_widget.o ${LIBEIOBJS} ${LIBEIBASE} ${LIBEI}
	${LINK} -o multiple_widget ${OBJDIR}/multiple_widget.o ${LIBEIOBJS} ${LIBEI} ${LIBS}

${OBJDIR}/multiple_widget.o : ${TESTS}/multiple_widget.c
	${CC} ${CCFLAGS} ${INCFLAGS} ${TESTS}/multiple_widget.c -o ${OBJDIR}/multiple_widget.o

# frame_button

frame_button : ${OBJDIR}/frame_button.o ${LIBEIOBJS} ${LIBEIBASE} ${LIBEI}
	${LINK} -o frame_button ${OBJDIR}/frame_button.o ${LIBEIOBJS} ${LIBEI} ${LIBS}

${OBJDIR}/frame_button.o : ${TESTS}/frame_button.c
	${CC} ${CCFLAGS} ${INCFLAGS} ${TESTS}/frame_button.c -o ${OBJDIR}/frame_button.o

# toplevel

toplevel : ${OBJDIR}/toplevel.o ${LIBEIOBJS} ${LIBEIBASE} ${LIBEI}
	${LINK} -o toplevel ${OBJDIR}/toplevel.o ${LIBEIOBJS} ${LIBEI} ${LIBS}

${OBJDIR}/toplevel.o : ${TESTS}/toplevel.c
	${CC} ${CCFLAGS} ${INCFLAGS} ${TESTS}/toplevel.c -o ${OBJDIR}/toplevel.o

# two_toplevel

two_toplevel : ${OBJDIR}/two_toplevel.o ${LIBEIOBJS} ${LIBEIBASE} ${LIBEI}
	${LINK} -o two_toplevel ${OBJDIR}/two_toplevel.o ${LIBEIOBJS} ${LIBEI} ${LIBS}

${OBJDIR}/two_toplevel.o : ${TESTS}/two_toplevel.c
	${CC} ${CCFLAGS} ${INCFLAGS} ${TESTS}/two_toplevel.c -o ${OBJDIR}/two_toplevel.o

# entry

entry : ${OBJDIR}/entry.o ${LIBEIOBJS} ${LIBEIBASE} ${LIBEI}
	${LINK} -o entry ${OBJDIR}/entry.o ${LIBEIOBJS} ${LIBEI} ${LIBS}

${OBJDIR}/entry.o : ${TESTS}/entry.c
	${CC} ${CCFLAGS} ${INCFLAGS} ${TESTS}/entry.c -o ${OBJDIR}/entry.o

# button

button : ${OBJDIR}/button.o ${LIBEIBASE} ${LIBEI}
	${LINK} -o button ${OBJDIR}/button.o ${LIBEI} ${LIBS}

${OBJDIR}/button.o : ${TESTS}/button.c
	${CC} ${CCFLAGS} ${INCFLAGS} ${TESTS}/button.c -o ${OBJDIR}/button.o

# hello_world

hello_world : ${OBJDIR}/hello_world.o ${LIBEIBASE} ${LIBEI}
	${LINK} -o hello_world ${OBJDIR}/hello_world.o ${LIBEI} ${LIBS}

${OBJDIR}/hello_world.o : ${TESTS}/hello_world.c
	${CC} ${CCFLAGS} ${INCFLAGS} ${TESTS}/hello_world.c -o ${OBJDIR}/hello_world.o

# parsing

parsing : ${OBJDIR}/parsing.o ${LIBEIBASE} ${LIBEI}
	${LINK} -o parsing ${LDFLAGS} ${OBJDIR}/parsing.o ${LIBEI} ${LIBS}

${OBJDIR}/parsing.o : ${TESTS}/parsing.c
	${CC} ${CCFLAGS} ${INCFLAGS} ${TESTS}/parsing.c -o ${OBJDIR}/parsing.o

# puzzle

puzzle : ${OBJDIR}/puzzle.o ${LIBEIBASE} ${LIBEI}
	${LINK} -o puzzle ${OBJDIR}/puzzle.o ${LIBEI} ${LIBS}

${OBJDIR}/puzzle.o : ${TESTS}/puzzle.c
	${CC} ${CCFLAGS} ${INCFLAGS} ${TESTS}/puzzle.c -o ${OBJDIR}/puzzle.o

# two048

two048 : ${OBJDIR}/two048.o ${LIBEIBASE} ${LIBEI}
	${LINK} -o two048 ${OBJDIR}/two048.o ${LIBEI} ${LIBS}

${OBJDIR}/two048.o : ${TESTS}/two048.c
	${CC} ${CCFLAGS} ${INCFLAGS} ${TESTS}/two048.c -o ${OBJDIR}/two048.o

# minesweeper

minesweeper : ${OBJDIR}/minesweeper.o ${LIBEIBASE} ${LIBEI}
	${LINK} -o minesweeper ${OBJDIR}/minesweeper.o ${LIBEI} ${LIBS}

${OBJDIR}/minesweeper.o : ${TESTS}/minesweeper.c
	${CC} ${CCFLAGS} ${INCFLAGS} ${TESTS}/minesweeper.c -o ${OBJDIR}/minesweeper.o

# Building of the library libei

${LIBEI} : ${LIBEIOBJS}
	ar rcs ${LIBEI} ${LIBEIOBJS}



# Building of the doxygen documentation.

doc :
	doxygen docs/doxygen.cfg



# Removing all built files.

clean:
	rm -f ${TARGETS}
	rm -f *.exe
	rm -f ${OBJDIR}/*
