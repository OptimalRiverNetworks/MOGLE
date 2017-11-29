/*
 * TXTFileWriter.h
 *
 *  Created on: 14/set/2012
 *      Author: lordmzn
 */

#ifndef TXTFILEWRITER_H_
#define TXTFILEWRITER_H_

#include "../OurMatrix.h"
#include <string>
#include <sstream>
#include <fstream>

class TXTFileWriter {
public:
	TXTFileWriter();
	bool openFile(std::string filename);
	bool writeLine(std::string line);
	template<typename T>
	bool writeMatrix(const OurMatrix<T>& matrix);
	bool closeFile();

private:
	const std::string fileName;
	std::ofstream file;
};

template<typename T>
bool TXTFileWriter::writeMatrix(const OurMatrix<T>& matrix) {
	if (!file.good()) {
		return false;
	}
	int nr = matrix.getDimension().n_rows;
	int nc = matrix.getDimension().n_columns;
	for (int j = 0; j < nr; j++) {
		std::stringstream row;
		for (int i = 0; i < nc; i++) {
			row << matrix.getElement(i, j) << " ";
		}
		writeLine(row.str());
	}
	return true;
}

#endif /* TXTFILEWRITER_H_ */
