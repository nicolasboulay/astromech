#****************************************************************************
#
# Makefile for TinyXml test.
# Lee Thomason
# www.grinninglizard.com
#
# This is a GNU make (gmake) makefile
#****************************************************************************

# DEBUG can be set to YES to include debugging info, or NO otherwise
DEBUG          := YES

# PROFILE can be set to YES to include profiling info, or NO otherwise
PROFILE        := NO

# TINYXML_USE_STL can be used to turn on STL support. NO, then STL
# will not be used. YES will include the STL files.
TINYXML_USE_STL := YES

#****************************************************************************

CC     := gcc
CXX    := g++
LD     := g++
AR     := ar rc
RANLIB := ranlib

DEBUG_CFLAGS     := -Wall -Wno-format -g -DDEBUG
RELEASE_CFLAGS   := -Wall -Wno-unknown-pragmas -Wno-format -O2

LIBS		 :=

DEBUG_CXXFLAGS   := ${DEBUG_CFLAGS} 
RELEASE_CXXFLAGS := ${RELEASE_CFLAGS}

DEBUG_LDFLAGS    := -g
RELEASE_LDFLAGS  :=

ifeq (YES, ${DEBUG})
   CFLAGS       := ${DEBUG_CFLAGS}
   CXXFLAGS     := ${DEBUG_CXXFLAGS}
   LDFLAGS      := ${DEBUG_LDFLAGS}
else
   CFLAGS       := ${RELEASE_CFLAGS}
   CXXFLAGS     := ${RELEASE_CXXFLAGS}
   LDFLAGS      := ${RELEASE_LDFLAGS}
endif

ifeq (YES, ${PROFILE})
   CFLAGS   := ${CFLAGS} -pg -O3
   CXXFLAGS := ${CXXFLAGS} -pg -O3
   LDFLAGS  := ${LDFLAGS} -pg
endif

#****************************************************************************
# Preprocessor directives
#****************************************************************************

ifeq (YES, ${TINYXML_USE_STL})
  DEFS := -DTIXML_USE_STL
else
  DEFS :=
endif

#****************************************************************************
# Include paths
#****************************************************************************

#INCS := -I/usr/include/g++-2 -I/usr/local/include
#INCS := -I/home/pif/simulateur/tinyxml -I/home/pif/simulateur/trace -I/home/pif/simulateur/util
INCS := -I./tinyxml -I./trace -I./util

#****************************************************************************
# Makefile code common to all platforms
#****************************************************************************

CFLAGS   := ${CFLAGS}   ${DEFS}
CXXFLAGS := ${CXXFLAGS} ${DEFS}

#****************************************************************************
# Targets of the build
#****************************************************************************

OUTPUT := simulation

all: ${OUTPUT}


#****************************************************************************
# Source files
#****************************************************************************

SRCS := \
	srcSimu/simulation.cpp\
	srcSimu/module.cpp\
	srcSimu/algo.cpp\
	srcSimu/guidageConstant.cpp\
	srcSimu/algoPC.cpp\
	srcSimu/algoPICs.cpp\
	srcSimu/comSeriePC_PIC.cpp\
	srcSimu/effecteur.cpp\
	srcSimu/chassis.cpp\
	srcSimu/dynamiquePassive.cpp\
	srcSimu/moteur.cpp\
	srcSimu/senseur.cpp\
	srcSimu/contour.cpp\
	srcSimu/codeuse.cpp\
	srcSimu/camera.cpp\
	srcSimu/systeme.cpp\
	srcSimu/algoPlanifTraj.cpp\
	srcSimu/obstacles.cpp\
	srcSimu/main.cpp\
	tinyxml/tinystr.cpp\
	tinyxml/tinyxml.cpp\
	tinyxml/tinyxmlerror.cpp\
	tinyxml/tinyxmlparser.cpp\
	util/geom2D.cpp\
	util/conversion.cpp\
	util/bit.cpp\
	trace/trace.cpp

# Add on the sources for libraries
SRCS := ${SRCS}

OBJS := $(addsuffix .o,$(basename ${SRCS}))

#****************************************************************************
# Output
#****************************************************************************

${OUTPUT}: ${OBJS}
	${LD} -o $@ ${LDFLAGS} ${OBJS} ${LIBS} ${EXTRA_LIBS}

#****************************************************************************
# common rules
#****************************************************************************

# Rules for compiling source files to object files
%.o : %.cpp
	${CXX} -c ${CXXFLAGS} ${INCS} $< -o $@

%.o : %.c
	${CC} -c ${CFLAGS} ${INCS} $< -o $@

dist:
	bash makedistlinux

clean:
	-rm -f core ${OBJS} ${OUTPUT}
