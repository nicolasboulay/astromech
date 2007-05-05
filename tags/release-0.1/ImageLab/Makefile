#--------------------------------------------------------------------------------------
#
#  Makefile  : Makefile for compiling CImg examples on Unix
#
#  Copyright : David Tschumperle (http://www.greyc.ensicaen.fr/~dtschump/)
#
# -----------------------------------------------------------------------------------

# Set correct variables and paths
CIMGVER = 1.17
CC      = g++
X11PATH = /usr/X11R6
CFLAGS  = -Wall -W -ansi -pedantic -ffast-math -I$(X11PATH)/include
LDFLAGS = -L$(X11PATH)/lib -lm -lpthread -lX11

# TINYXML_USE_STL can be used to turn on STL support. NO, then STL
# will not be used. YES will include the STL files.
TINYXML_USE_STL := YES

# Uncomment to enable fast image display, using the XSHM library.
XSHM_FLAGS = -Dcimg_use_xshm -lXext 

# Uncomment to enable screen mode switching, using the XRandr library.
XRANDR_FLAGS = -Dcimg_use_xrandr -lXrandr

# Uncomment to enable native support for PNG image files, using the PNG library.
#PNG_FLAGS = -Dcimg_use_png -lpng -lz

# Uncomment to enable native support for JPEG image files, using the JPEG library.
#JPEG_FLAGS = -Dcimg_use_jpeg -ljpeg

# Uncomment to enable native support for TIFF image files, using the TIFF library.
#TIFF_FLAGS = -Dcimg_use_tiff -ltiff

# Uncomment to enable native support of most classical image file formats, using the Magick++ library.
#MAGICK_FLAGS = -Dcimg_use_magick `Magick++-config --cppflags` `Magick++-config --cxxflags` `Magick++-config --ldflags` `Magick++-config --libs`

#****************************************************************************
# Preprocessor directives
#****************************************************************************

ifeq (YES, ${TINYXML_USE_STL})
  DEFS := -DTIXML_USE_STL
else
  DEFS :=
endif

OPTFLAGS = -O3 $(XSHM_FLAGS) $(XRANDR_FLAGS) $(PNG_FLAGS) $(JPEG_FLAGS) $(TIFF_FLAGS) $(MAGICK_FLAGS)

CIMGFILES = imageLab

# Compilation rules
.cpp:
	@echo
	@echo "** Compiling '$* ($(CIMGVER))' with '`$(CC) -v 2>&1 | grep version`'"
	@echo
	$(CC) -o $* $< $(CFLAGS) $(LDFLAGS) $(ARCHFLAGS) $(DEFS)
menu:
	@echo
	@echo "CImg Library $(CIMGVER) : Examples"
	@echo "-----------------------------"
	@echo "  > dlinux   : Compile for Linux/MacOSX, with debug informations."
	@echo "  > linux    : Compile for Linux/MacOSX, with standard options."
	@echo "  > olinux   : Compile for Linux/MacOSX, with optimizations."
	@echo
	@echo "  > dsolaris : Compile for Solaris, with debug informations."
	@echo "  > solaris  : Compile for Solaris, with standard options."
	@echo "  > osolaris : Compile for Solaris, with optimizations."
	@echo
	@echo "  > dminimal : Compile with minimal dependancies, with debug informations."
	@echo "  > minimal  : Compile with minimal dependancies, with standard options."
	@echo "  > ominimal : Compile with minimal dependancies, with optimizations."
	@echo
	@echo "  > clean    : Clean generated files."
	@echo
	@echo "Choose your option :"
	@read CHOICE; echo; make $$CHOICE; echo; echo "> Next time, you can bypass the menu by typing directly 'make $$CHOICE'"; echo;

all: $(CIMGFILES)

clean:
	rm -f *.exe *~ $(CIMGFILES)

# Linux/Mac OSX targets
linux:
	make all
dlinux:
	make "ARCHFLAGS=-Dcimg_debug=2 -g" all
olinux:
	make "ARCHFLAGS=$(OPTFLAGS)" all

# Sun Solaris targets
solaris:
	make "ARCHFLAGS=-R$(X11PATH)/lib -lrt -lnsl -lsocket" all
dsolaris:
	make "ARCHFLAGS=-Dcimg_debug=2 -g -R$(X11PATH)/lib -lrt -lnsl -lsocket" all
osolaris:
	make "ARCHFLAGS=-R$(X11PATH)/lib -lrt -lnsl -lsocket $OPTFLAGS" all

# targets with minimal dependancies
minimal:
	make "ARCHFLAGS=-Dcimg_display_type=0" all
dminimal:
	make "ARCHFLAGS=-Dcimg_display_type=0 -Dcimg_debug=2 -g" all
ominimal:
	make "ARCHFLAGS=-Dcimg_display_type=0 -O3" all
