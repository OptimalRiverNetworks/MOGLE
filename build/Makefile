# Makefile - creates a library called libGLEhydroDEMtools.a
# 
# This make file assumes it is being run on a Unix-like environment.  Windows
# users can install MinGW/MSYS or Cygwin.
#
# This make file requires the following file system
# root/  build/    Makefile (this one)
#       		   libyaml-cpp.a
#        include/  moeaframework.h
#    	           yaml-cpp/              headers.h
#				   GLEhydroDEMtools/ 	  headers.h
#                                         alglib/     headers.h
#										  Utilities/  headers.h
#
#        src/      GLEhydroDEMtools/src/  sources.cpp
#                                         alglib/     sources.cpp
#                                         Utilities/  sources.cpp
#                  GLEmodel4moea/src/	  source.cpp
#				   MOEAframework/		  source.c
#
# Run 'make' to build the lib
#
#  Created on: 05/mar/2013
#  Updated on: 25/jan/2018
#     Author: lordmzn

# vpath declaration allows make to find the files needed when checking targets
vpath %.h ../include ../include/GLEhydroDEMtools ../include/GLEhydroDEMtools/Utilities ../include/GLEhydroDEMtools/alglib ../include
vpath %.cpp ../src/GLEhydroDEMtools ../src/GLEhydroDEMtools/Utilities ../src/GLEhydroDEMtools/alglib ../src/GLEmodel4moea
vpath %.c ../src/MOEAframework
vpath %.o .

#include paths for the compiler and libraries (that should be on current directory) for the linker 
INCLUDEDIR_GLEHYDRODEM = -I ../include/GLEhydroDEMtools -I ../include/GLEhydroDEMtools/Utilities -I ../include/GLEhydroDEMtools/alglib
INCLUDEDIR_MOEAF = -I ../include
LIBRARIES = -L. -lmoea -lGLEhydroDEMtools -lyaml-cpp

# compiler
CC = g++
# compiler flags for normal use
CFLAGS = -Wall -O3 -c -DDFPLANCHON
LDFLAGS =
# the following is for using gprof or valgrind (comment out one at a time)
#CFLAGS = -g -pg -c -O1 -Wall
#LDFLAGS += -pg

# moeaframework library
HEADERS_MOEAF = moeaframework.h
SOURCES_MOEAF = moeaframework.c
OBJECTS_MOEAF = $(SOURCES_MOEAF:.c=.o)
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

all: GLEmodel4moea libGLEhydroDEMtools.a libmoea.a

###  Executable  ###

GLEmodel4moea: model4moea.o libyaml-cpp.a libGLEhydroDEMtools.a libmoea.a
	$(CC) $(LDFLAGS) -o $@ $< $(LIBRARIES)

model4moea.o: model4moea.cpp $(HEADERS_MOEAF) $(HEADERS_GLEHYDRODEM) $(HEADERS_UTIL) $(HEADERS_ALGLIB)
	$(CC) $(CFLAGS) $(INCLUDEDIR_MOEAF) $(INCLUDEDIR_GLEHYDRODEM) -o $@ $<

###  MOEA framework library  ###

moeaframework.o: moeaframework.c moeaframework.h
	gcc $(CFLAGS) $(INCLUDEDIR_MOEAF) -o moeaframework.o ../src/MOEAframework/moeaframework.c

libmoea.a: $(OBJECTS_MOEAF)
	ar -rcs $@ $^


###  GLEhydroDEMtools library  ###

# alglib part

$(OBJECTS_ALGLIB): $($@:.o=.cpp) $(HEADERS_ALGLIB)
	$(CC) $(CFLAGS) $(INCLUDEDIR_GLEHYDRODEM) -DAE_CPU=AE_INTEL -o $@ ../src/GLEhydroDEMtools/alglib/$(basename $@).cpp

# utilities part

$(OBJECTS_UTIL): $($@:.o=.cpp) $(HEADERS_UTIL)
	$(CC) $(CFLAGS) $(INCLUDEDIR_GLEHYDRODEM) -o $@ ../src/GLEhydroDEMtools/Utilities/$(basename $@).cpp

# main part

$(OBJECTS_GLEHYDRODEM): $($@:.o=.cpp) $(HEADERS_GLEHYDRODEM) $(HEADERS_UTIL) $(HEADERS_ALGLIB)
	$(CC) $(CFLAGS) $(INCLUDEDIR_GLEHYDRODEM) -o $@ ../src/GLEhydroDEMtools/$(basename $@).cpp

# static library

libGLEhydroDEMtools.a: $(OBJECTS_ALGLIB) $(OBJECTS_UTIL) $(OBJECTS_GLEHYDRODEM)
	ar -rcs $@ $^
	
.PHONY: clean
clean:
	rm -f *.o GLEmodel4moea libGLEhydroDEMtools.a libmoea.a

