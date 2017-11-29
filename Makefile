# Makefile - creates a library called libGLEhydroDEMtools.a
# 
# This make file assumes it is being run on a Unix-like environment.  Windows
# users can install MinGW/MSYS or Cygwin.
#
# This make file requires the following file system
# root/  build/    Makefile (this one)
#       ? libyaml-cpp.a
#        include/  yaml-cpp/  headers.h
#        svn/      GLEhydroDEMtools/src/  alglib/     headers.h
#                                                     sources.cpp
#                                         Utilities/  headers.h
#                                                     sources.cpp
#                                         headers.h
#                                         sources.cpp
#
# Run 'make' to build the lib
#
#  Created on: 05/mar/2013
#     Author: lordmzn

# vpath declaration allows make to find the files needed when checking targets
vpath %.h ../include:../svn/GLEhydroDEMtools/src:../svn/GLEhydroDEMtools/src/Utilities:../svn/GLEhydroDEMtools/src/alglib
vpath %.cpp ../svn/GLEhydroDEMtools/src:../svn/GLEhydroDEMtools/src/Utilities:../svn/GLEhydroDEMtools/src/alglib
vpath %.o .

#include paths for the compiler and libraries (that should be on current directory) for the linker 
INCLUDEDIR_GLEHYDRODEM = -I ../svn/GLEhydroDEMtools/src -I ../include

# compiler
CC = g++
# compiler flags for normal use
CFLAGS = -Wall -O3 -c -DDFPLANCHON
LDFLAGS =
# the following is for using gprof or valgrind (only comment out one at a time)
#CFLAGS = -g -pg -c -O1 -Wall
#LDFLAGS += -pg

# alglib part of the GLEhydroDEMtools
HEADERS_ALGLIB = alglibinternal.h alglibmisc.h ap.h dataanalysis.h integration.h interpolation.h linalg.h optimization.h solvers.h specialfunctions.h statistics.h stdafx.h
SOURCES_ALGLIB = alglibinternal.cpp alglibmisc.cpp ap.cpp dataanalysis.cpp integration.cpp interpolation.cpp linalg.cpp optimization.cpp solvers.cpp specialfunctions.cpp statistics.cpp
OBJECTS_ALGLIB = $(SOURCES_ALGLIB:.cpp=.o)
# utilities part of the GLEhydroDEMtools
HEADERS_UTIL = LogFileProducer.h TXTFileParser.h TXTFileWriter.h
SOURCES_UTIL = LogFileProducer.cpp TXTFileParser.cpp TXTFileWriter.cpp
OBJECTS_UTIL = $(SOURCES_UTIL:.cpp=.o)
# GLEhydroDEMtools library files
HEADERS_GLEHYDRODEM = Cell.h DEM.h DEMParam.h EE.h EEE.h EEL.h HydroNet.h LandscapeModel.h LandscapeModelEnums.h Mask.h OurMatrix.h Stepcost.h SH.h TEE.h TSP.h
SOURCES_GLEHYDRODEM = Cell.cpp DEM.cpp DEMParam.cpp EE.cpp EEE.cpp EEL.cpp HydroNet.cpp LandscapeModel.cpp Mask.cpp SH.cpp TEE.cpp TSP.cpp
OBJECTS_GLEHYDRODEM = $(SOURCES_GLEHYDRODEM:.cpp=.o)

all: libGLEhydroDEMtools.a

###  GLEhydroDEMtools library ###

# alglib part

$(OBJECTS_ALGLIB): $($@:.o=.cpp) $(HEADERS_ALGLIB)
	$(CC) $(CFLAGS) $(INCLUDEDIR_GLEHYDRODEM) -DAE_CPU=AE_INTEL -o $@ ../svn/GLEhydroDEMtools/src/alglib/$(basename $@).cpp

# utilities part

$(OBJECTS_UTIL): $($@:.o=.cpp) $(HEADERS_UTIL)
	$(CC) $(CFLAGS) $(INCLUDEDIR_GLEHYDRODEM) -o $@ ../svn/GLEhydroDEMtools/src/Utilities/$(basename $@).cpp

# main part

$(OBJECTS_GLEHYDRODEM): $($@:.o=.cpp) $(HEADERS_GLEHYDRODEM) $(HEADERS_UTIL) $(HEADERS_ALGLIB)
	$(CC) $(CFLAGS) $(INCLUDEDIR_GLEHYDRODEM) -o $@ ../svn/GLEhydroDEMtools/src/$(basename $@).cpp

# static library

libGLEhydroDEMtools.a: $(OBJECTS_ALGLIB) $(OBJECTS_UTIL) $(OBJECTS_GLEHYDRODEM)
	ar -rcs $@ $^
	
.PHONY: clean
clean:
	rm -f *.o

