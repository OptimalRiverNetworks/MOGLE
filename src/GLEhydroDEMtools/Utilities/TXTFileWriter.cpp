/*
 * TXTFileWriter.cpp
 *
 *  Created on: 15/set/2012
 *      Author: lordmzn
 */

#include "TXTFileWriter.h"
#include <sstream>

TXTFileWriter::TXTFileWriter() {

}

bool TXTFileWriter::openFile(std::string filename) {
	file.open(filename.c_str());
	if (file.is_open()) {
		return true;
	}
	return false;
}

bool TXTFileWriter::writeLine(std::string line) {
	if (!file.good()) {
		return false;
	}
	file << line << std::endl;
	return true;
}

bool TXTFileWriter::closeFile() {
	file.close();
	return true;
}

