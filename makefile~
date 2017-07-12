#===========================================================================
#
#	A simple Makefile
#
#===========================================================================

# Compiling
CC	      = g++
INCPATH       = -I/usr/include/
CFLAGS=-c -Wall -ansi -std=c99

# Linking
LINK          = g++
LFLAGS        = -Wl,-rpath=libs #-L./libs/ -lViconDataStreamSDK_CPP
LIBS          = -lm -lpthread -L./libs/ -lViconDataStreamSDK_CPP  -lmysqlclient

# Source & Target
SOURCES= ViconUtils.cpp main.cpp  LogUtils.cpp MyProtocol.cpp SQLUtils.cpp MyWayPoint.cpp
EXECUTABLE= ecustControl

#===========================================================================
OBJECTS=$(SOURCES:.c=.o)


all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LFLAGS) -o $(EXECUTABLE) $(OBJECTS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCPATH) $< -o $@

clean: 	
	-rm *.o
	-rm $(EXECUTABLE)
	
