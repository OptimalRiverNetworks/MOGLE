#!/bin/bash
cd svn
# checking out
svn checkout http://gle-multiobj.googlecode.com/svn/GLEhydroDEMtools GLEhydroDEMtools
svn checkout http://gle-multiobj.googlecode.com/svn/GLEmodel4moea GLEmodel4moea
svn checkout http://gle-multiobj.googlecode.com/svn/MOEAframework MOEAframework
svn checkout http://gle-multiobj.googlecode.com/svn/GLEmodelJava GLEmodelJava
# copying the Jar containing the MOEA launcher into javaLibs
cd GLEmodelJava
if [ -e "GLEmodelMOEAinterface.jar" ]
then
        cp GLEmodelMOEAinterface.jar ../../javaLibs/
	echo "GLEmodelMOEAinterface updated"
fi
cd ../..
#building
cd build
if [ ! -e "Makefile.GLEhydroDEMtools" ]
then
	cp ../svn/GLEhydroDEMtools/src/Makefile Makefile.GLEhydroDEMtools
	echo "Makefile.GLEhydroDEMtools created"
fi
if [ ! -e "Makefile.GLEmodel4moea" ]
then
	cp ../svn/GLEmodel4moea/src/Makefile Makefile.GLEmodel4moea
	echo "Makefile.GLEmodel4moea created"
fi
module load gcc
make -f Makefile.GLEhydroDEMtools
if [ -e "libGLEhydroDEMtools.a" ]
then
	make -f Makefile.GLEmodel4moea
fi
if [ -e "GLEmodel4moea" ]
then
	cp GLEmodel4moea ../
	echo "GLEmodel4moea updated"
fi
cd ..
