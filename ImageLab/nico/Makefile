CC=g++
SHELL = /bin/sh
CFLAGSDEBUG= -Wall -pedantic -W -g
CFLAGSOPT=-O3 -fomit-frame-pointer -funroll-all-loops

LDFLAGS= -lm -lpthread -lX11 
CFLAGSHEADER = -I/usr/X11R6/include  -I./.. -L/usr/X11R6/lib

#./fps.cpp            
SOURCES = \
./test_param.cpp            \
./video.cpp

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=test_param

#.SILENT:


all: $(SOURCES) $(EXECUTABLE)

dep :
	$(CC) $(CFLAGSHEADER) $(SOURCES) -MM > dep

include dep

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(CFLAGSHEADER) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGSDEBUG) $(CFLAGSHEADER) -c $< -o $@

clean: 
	find . -name "*.o" -exec rm -f {} \; 
	rm -f $(EXECUTABLE)

release: dep $(SOURCES)
	$(CC)  $(CFLAGSOPT) $(CFLAGSHEADER) $(LDFLAGS) $(SOURCES) -o $(EXECUTABLE)

dist: clean
	tar cvf - ./* | gzip -9 > $(EXECUTABLE).tar.gz
