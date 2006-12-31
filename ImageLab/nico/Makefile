CC=g++
SHELL = /bin/sh
CFLAGSDEBUG=-O0 -Wall -pedantic -W 
CFLAGSOPT=-O3 -fomit-frame-pointer -funroll-all-loops

LDFLAGS= -lm -lpthread -lX11 
CFLAGSHEADER = -I/usr/X11R6/include  -L/usr/X11R6/lib

SOURCES = \
./tutorial.cpp            \
./video.cpp               

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=tut

#.SILENT:


all: $(SOURCES) $(EXECUTABLE)

dep : $(SOURCES)
	$(CC) $(CFLAGSHEADER) $(SOURCES) -MM > dep

include dep

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGSDEBUG) $(CFLAGSHEADER) -c $< -o $@

clean: 
	find . -name "*.o" -exec rm -f {} \; 
	rm -f $(EXECUTABLE)

release: dep $(SOURCES)
	$(CC)  $(CFLAGSOPT) $(LDFLAGS) $(CFLAGSHEADER) $(SOURCES) -o $(EXECUTABLE)

dist: clean
	tar cvf - ./* | gzip -9 > $(EXECUTABLE).tar.gz