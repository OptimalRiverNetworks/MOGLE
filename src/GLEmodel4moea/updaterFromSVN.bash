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
module load gcc
make
if [ -e "GLEmodel4moea" ]
then
	cp GLEmodel4moea ../
	echo "GLEmodel4moea updated"
fi
cd ..
