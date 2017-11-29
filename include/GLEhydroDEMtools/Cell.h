/*
 * cell.h
 *
 *  Created on: Jan 16, 2013
 *      Author: andre
 */

#ifndef CELL_H_
#define CELL_H_

#include "DEMParam.h"
#include <vector>

namespace CellExplore {
enum directions {
	fromNWtoRight = 0,
	fromSEtoLeft,
	fromNEtoDown,
	fromSWtoUp,
	fromNEtoLeft,
	fromSWtoRight,
	fromNWtoDown,
	fromSEtoUp
};
}

struct Cell {
	int nR;
	int nC;

	int x;
	int y;

	Cell(const DEMParam& p, int row, int col);
	Cell(const Cell& c, int row, int col);

	~Cell();
	void initializeDirections(CellExplore::directions dir);
	void borderFinder(std::vector<Cell>& borderVector);
	bool goToNextCell(CellExplore::directions dir);
	void neighboursFinder(std::vector<Cell>& neghboursVector);
};

#endif /* CELL_H_ */
