/*
 * cell.cpp
 *
 *  Created on: Jan 16, 2013
 *      Author: andre
 */

#include "Cell.h"

Cell::Cell(const DEMParam& p, int row, int col) :
		nR(p.number_rows), nC(p.number_cols) {
	x = row;
	y = col;
}

Cell::Cell(const Cell& c, int row, int col) :
		nR(c.nR), nC(c.nC) {
	x = row;
	y = col;
}

Cell::~Cell() {
}

void Cell::initializeDirections(CellExplore::directions dir) {
	switch (dir) {
	case (CellExplore::fromNWtoRight): {
		x = 0;
		y = 0;
		break;
	}
	case (CellExplore::fromSEtoLeft): {
		x = nR - 1;
		y = nC - 1;
		break;
	}
	case (CellExplore::fromNEtoDown): {
		x = 0;
		y = nC - 1;
		break;
	}
	case (CellExplore::fromSWtoUp): {
		x = nR - 1;
		y = 0;
		break;
	}
	case (CellExplore::fromNEtoLeft): {
		x = 0;
		y = nC - 1;
		break;
	}
	case (CellExplore::fromSWtoRight): {
		x = nR - 1;
		y = 0;
		break;
	}
	case (CellExplore::fromNWtoDown): {
		x = 0;
		y = 0;
		break;
	}
	case (CellExplore::fromSEtoUp): {
		x = nR - 1;
		y = nC - 1;
		break;
	}
	}
}

bool Cell::goToNextCell(CellExplore::directions dir) {
	switch (dir) {
	case (CellExplore::fromNWtoRight): {
		y = y + 1;
		if (x < 0 || y < 0 || x > nR - 1 || y > nC - 1) {
			x = x + 1;
			y = y - nC + 1;
			if (x < 0 || y < 0 || x > nR - 1 || y > nC - 1) {
				return false;
			}
		}
		return true;
	}
	case (CellExplore::fromSEtoLeft): {
		y = y - 1;
		if (x < 0 || y < 0 || x > nR - 1 || y > nC - 1) {
			x = x - 1;
			y = y + nC - 1;
			if (x < 0 || y < 0 || x > nR - 1 || y > nC - 1) {
				return false;
			}
		}
		return true;
	}
	case (CellExplore::fromNEtoDown): {
		x = x + 1;
		if (x < 0 || y < 0 || x > nR - 1 || y > nC - 1) {
			x = x - nR + 1;
			y = y - 1;
			if (x < 0 || y < 0 || x > nR - 1 || y > nC - 1) {
				return false;
			}
		}
		return true;
	}
	case (CellExplore::fromSWtoUp): {
		x = x - 1;
		if (x < 0 || y < 0 || x > nR - 1 || y > nC - 1) {
			x = x + nR - 1;
			y = y + 1;
			if (x < 0 || y < 0 || x > nR - 1 || y > nC - 1) {
				return false;
			}
		}
		return true;
	}
	case (CellExplore::fromNEtoLeft): {
		y = y - 1;
		if (x < 0 || y < 0 || x > nR - 1 || y > nC - 1) {
			x = x + 1;
			y = y + nC - 1;
			if (x < 0 || y < 0 || x > nR - 1 || y > nC - 1) {
				return false;
			}
		}
		return true;
	}
	case (CellExplore::fromSWtoRight): {
		y = y + 1;
		if (x < 0 || y < 0 || x > nR - 1 || y > nC - 1) {
			x = x - 1;
			y = y - nC + 1;
			if (x < 0 || y < 0 || x > nR - 1 || y > nC - 1) {
				return false;
			}
		}
		return true;
	}
	case (CellExplore::fromNWtoDown): {
		x = x + 1;
		if (x < 0 || y < 0 || x > nR - 1 || y > nC - 1) {
			x = x - nR + 1;
			y = y + 1;
			if (x < 0 || y < 0 || x > nR - 1 || y > nC - 1) {
				return false;
			}
		}
		return true;
	}
	case (CellExplore::fromSEtoUp): {
		x = x - 1;
		if (x < 0 || y < 0 || x > nR - 1 || y > nC - 1) {
			x = x + nR - 1;
			y = y - 1;
			if (x < 0 || y < 0 || x > nR - 1 || y > nC - 1) {
				return false;
			}
		}
		return true;
	}
	}
}

void Cell::borderFinder(std::vector<Cell>& borderVector) {
	initializeDirections(CellExplore::fromNWtoDown);
	borderVector.push_back(Cell(*this, x, y));
	while (x < nR - 1) {
		goToNextCell(CellExplore::fromNWtoDown);
		borderVector.push_back(Cell(*this, x, y));
	}
	while (y < nC - 1) {
		goToNextCell(CellExplore::fromSWtoRight);
		borderVector.push_back(Cell(*this, x, y));
	}
	while (x > 0) {
		goToNextCell(CellExplore::fromSEtoUp);
		borderVector.push_back(Cell(*this, x, y));
	}
	while (y > 1) {
		goToNextCell(CellExplore::fromNEtoLeft);
		borderVector.push_back(Cell(*this, x, y));
	}
}

void Cell::neighboursFinder(std::vector<Cell>& neighboursVector) {
	neighboursVector.erase(neighboursVector.begin(), neighboursVector.end());
	for (int j = y - 1; j < y + 2; j++) {
		for (int i = x - 1; i < x + 2; i++) {
			if ((i == x && j == y) || i < 0 || j < 0 || i > nR - 1
					|| j > nC - 1) {
				continue;
			}
			neighboursVector.push_back(Cell(*this, i, j));
		}
	}
}
