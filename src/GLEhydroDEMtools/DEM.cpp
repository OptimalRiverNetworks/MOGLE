/*
 * DEM.cpp
 *
 *  Created on: Aug 24, 2012
 *      Author: andrevb
 */

#include "DEM.h"
#include "Mask.h"
#include "HydroNet.h"
#include "Utilities/TXTFileParser.h"
#include "Utilities/LogFileProducer.h"
#include "Cell.h"
#include <string.h>
#include <sstream>
#include <cmath>

/* --------- DEM::CONSTRUCTORS ------------- */
DEM::DEM() :
		demParam(NULL), elevationData(), mass(), hydroNet(NULL) {
}
DEM::DEM(DEMParam const * p) :
		elevationData(p->number_rows, p->number_cols), hydroNet(NULL) {
	demParam = p;
	if (!initElevationData()) {
		/* TODO sth in case of failure (or prevent that case) */
		LogFileProducer::Instance()->log(
				std::string("Initialization of elevation data failed"));

	}
	evaluateMass();
}

DEM::DEM(const DEM& dem, const Mask& ut) :
		elevationData(dem.demParam->number_rows, dem.demParam->number_cols, 0), hydroNet(
				NULL) {
	demParam = dem.demParam;
	if (demParam->interpolatorType
			== GLE::Interpolator::INVERSE_WEIGHTED_DISTANCE) {
		int val;
		for (unsigned int i = 1; i < demParam->number_rows - 1; i++) {
			for (unsigned int j = 1; j < demParam->number_cols - 1; j++) {
				val = ut.getControls().getElement(i - 1, j - 1)
								* demParam->fixedElevationChange;
				if (val < 0) {
					val = 0;
				}
				elevationData.setElement(val, i, j);
			}
		}
	} else {
		int val;
		for (unsigned int i = 1; i < demParam->number_rows - 1; i++) {
			for (unsigned int j = 1; j < demParam->number_cols - 1; j++) {
				val = dem.elevationData.getElement(i, j)
						+ (ut.getControls().getElement(i - 1, j - 1))
								* demParam->fixedElevationChange;
				if (val < 0) {
					val = 0;
				}
				elevationData.setElement(val, i, j);
			}
		}
	}
#ifdef DFPLANCHON
	adjustDepressionsPlanchon();
#else
	adjustDepressions();
#endif
	evaluateMass();
}

DEM::DEM(const DEM& d) :
		elevationData(d.elevationData), mass(d.mass) {
	demParam = d.demParam;
	hydroNet = d.hydroNet;
}

DEM::~DEM() {
	delete hydroNet;
}

DEM& DEM::operator=(const DEM & a) {
	// check self assignment
	if (this == &a) {
		return *this;
	}
	// deallocate memory used by this
	delete hydroNet;
	// fill every field
	demParam = a.demParam; // same demParam obj
	elevationData = a.elevationData; // real copy
	mass = a.mass;
	if (a.hydroNet != NULL) { // deep copy
		hydroNet = new HydroNet(*a.hydroNet);
	}
	return *this;
}

/* ----------- DEM::METHODS ------------------ */
void DEM::evaluateHydroNet() {
	//Evaluate hydrological network process
	hydroNet = new HydroNet(this);
}

#ifdef DFPLANCHON
/* DEPRESSION FILLING: Planchon 2001 version*/

void DEM::adjustDepressionsPlanchon() {
	OurMatrix<int> transientSurface = DFPlanchonStage1();
	DFPlanchonStage2(transientSurface);
	elevationData = transientSurface;
}

OurMatrix<int> DEM::DFPlanchonStage1() {
//	W(c) initialisation
	OurMatrix<int> transientSurface(demParam->number_rows,
			demParam->number_cols, hugeNumber);
	std::vector<Cell> borderVector;
	Cell c(*demParam, 0, 0);
	c.borderFinder(borderVector);
	for (std::vector<Cell>::iterator it = borderVector.begin();
			it != borderVector.end(); ++it) {
		transientSurface.setElement(elevationData.getElement(it->x, it->y),
				it->x, it->y);
	}
	return transientSurface;
}

void DEM::DFPlanchonStage2(OurMatrix<int>& transientSurface) {
	Cell c(*demParam, 0, 0);
	bool somethingDone = true;
	std::vector<Cell> neighboursVector;
	int depth = 0;
	std::vector<Cell> borderVector;

	c.borderFinder(borderVector);
	for (std::vector<Cell>::iterator it = borderVector.begin();
			it != borderVector.end(); ++it) {
				dryUpwardCell(*it, transientSurface, depth);
	}
	while (somethingDone == true) {
		for (CellExplore::directions dir = CellExplore::fromNWtoRight;
				dir != CellExplore::fromSEtoUp;
				dir = CellExplore::directions(dir + 1)) {
			c.initializeDirections(dir);
			somethingDone = false;
			do {
				if (transientSurface.getElement(c.x, c.y)
						> elevationData.getElement(c.x, c.y)) {
					c.neighboursFinder(neighboursVector);
					for (std::vector<Cell>::iterator it =
							neighboursVector.begin();
							it != neighboursVector.end(); ++it) {
						if ((elevationData.getElement(c.x, c.y)
								>= transientSurface.getElement(it->x, it->y)
										+ epsilon)
								&& (transientSurface.getElement(it->x, it->y)
										!= INT_MAX)) {
							transientSurface.setElement(
									elevationData.getElement(c.x, c.y), c.x,
									c.y);
							somethingDone = true;
							dryUpwardCell(c, transientSurface, depth);
							break;
						} else if (transientSurface.getElement(c.x, c.y)
								> transientSurface.getElement(it->x, it->y)
										+ epsilon&& transientSurface.getElement(it->x, it->y)!=INT_MAX) {
							transientSurface.setElement(
									transientSurface.getElement(it->x, it->y)
											+ epsilon, c.x, c.y);
							somethingDone = true;
						}

					}
				}
			} while (c.goToNextCell(dir) == true);
		}
	}
}

int DEM::dryUpwardCell(Cell c, OurMatrix<int>& transientSurface, int depth) {
	depth = depth + 1;
	if (depth > maxDepth) {
		depth = depth - 1;
		return depth;
	}
	std::vector<Cell> neighboursVector;
	c.neighboursFinder(neighboursVector);
//	std::cout<< "---cell " << c.x << ", " << c.y << " has " << neighboursVector.size() << " neighbors " << std::endl;
	for (std::vector<Cell>::iterator it = neighboursVector.begin();
			it != neighboursVector.end(); ++it) {
		if (transientSurface.getElement(it->x, it->y) == hugeNumber) {
			if (elevationData.getElement(it->x, it->y)
					>= transientSurface.getElement(c.x, c.y) + epsilon) {
				transientSurface.setElement(
						elevationData.getElement(it->x, it->y), it->x, it->y);
				depth = dryUpwardCell(*it, transientSurface, depth);
			}
		}
	}
	depth = depth - 1;
	return depth;
}

#endif
#ifndef DFPLANCHON
void DEM::adjustDepressions() {
	int nr = 0;
	int nc = 0;
	depressionData test;
	depressionData test2nd;

	nr = demParam->number_rows; // # of rows
	nc = demParam->number_cols;// # of columns

	int lowJ = 0;// lower limit of J
	int upJ = 0;// upper limit of J
	int lowI = nr - 1;// lower limit of I
	int upI = 0;// upper limit of I

	for (int j = 1; j < nc - 1; j++) {
		for (int i = 1; i < nr - 1; i++) {
			test = findDepressions(i, j);
			/*If the lowest element of the 8-cells boundary of the current element is higher or equal
			 * to the current element itself, it calls depressionFilling function*/
			if (test.hasDepression == true) {

#ifdef DEBUG
				std::cout
				<< "---------------------------------------------------- "
				<< std::endl;

				std::cout
				<< "DEM is depressed. Starting with depression filling."
				<< std::endl;
				std::cout
				<< "---------------------------------------------------- "
				<< std::endl;
#endif

				/*First depression filling*/
				depressionFilling(test, i, j);

				/*Second depression filling*/
				//Setting limits of I and J
				if (lowJ == 0) {
					lowJ = j - 1;
				};
				upJ = j + 1;
				if (i - 1 < lowI) {
					lowI = i - 1;
				};
				if (upI < i + 1) {
					upI = i + 1;
				};

				//Setting condition of depression
				bool isDepressed = true;
				//Setting temporary limits
				int lowJtemp = lowJ;
				int upJtemp = upJ;
				int lowItemp = lowI;
				int upItemp = upI;
				//Setting counters
				int I = 0;
				int J = 0;
				//Starting second depression filling cycle
				while (isDepressed == true) {
					//The cycle terminates when there are no more depressions, (isDepressed==false) condition
					lowJ = lowJtemp;
					upJ = upJtemp;
					lowI = lowItemp;
					upI = upItemp;
					//Adjusting border conditions
					if (lowJ == 0) {
						lowJ = 1;
					};
					if (upJ == nc - 1) {
						upJ = nc - 2;
					};
					if (lowI == 0) {
						lowI = 1;
					};
					if (upI == nr - 1) {
						upI = nr - 2;
					};
					isDepressed = false;
					for (J = lowJ; J <= upJ; J++) {
						for (I = lowI; I <= upI; I++) {
							test2nd = findDepressions(I, J); //Finds depressions in the boundary
							if (test2nd.hasDepression == true) {
								depressionFilling(test2nd, I, J); //Fills depressions in the boundary
								isDepressed = true;//It found a depression, so the cycle goes on
								//std::cout << I << J << std::endl;
								//Setting temporary border conditions again
								if (J == lowJtemp) {
									lowJtemp = J - 1;
								};
								if (J == upJtemp) {
									upJtemp = J + 1;
								};
								if (I == lowItemp) {
									lowItemp = I - 1;
								};
								if (I == upJtemp) {
									lowItemp = I + 1;
								};
							};
						};
					};
				}
			};
		};
	}

#ifdef DEBUG
	std::cout << "---------------------------------------------------- "
	<< std::endl;
	std::cout << "Completed depression filling." << std::endl;
	std::cout << "---------------------------------------------------- "
	<< std::endl;
#endif
}

/*findDepressions function*/
depressionData DEM::findDepressions(int i, int j) {
	/*It looks if the element (i,j) is depressed by comparing it to its boundary elements.
	 * it returns a structure containing:
	 * -the elevation of the (i,j) element: c_el;
	 * -the elevation of the lowest element of c_el boundary, with its coordinates: b_el(i_b,j_b);
	 * -a boolean value, which is true if a depression was found, false in the opposite case.
	 */
	depressionData test;
	test.c_el = elevationData.getElement(i, j); //current element
	test.b_el = 0;
	test.i_b = 0;
	test.j_b = 0;

	if (test.c_el < 1) { // Checking if current element is lower than 1
		test.hasDepression = false;
		return test;
	};

	/* Finding the lowest element in the boundary of c_el*/
	test.b_el = elevationData.getElement(i, (j - 1)); //searching on the left contiguous element
	test.i_b = i;
	test.j_b = j - 1;

	if (elevationData.getElement(i, j + 1) < test.b_el) { //searching on the right contiguous element
		test.b_el = elevationData.getElement(i, j + 1);
		test.i_b = i;
		test.j_b = j + 1;
	};
	for (int j2 = j - 1; j2 <= j + 1; j2++) {
		if (elevationData.getElement(i - 1, j2) < test.b_el) { //searching in the upper row, among the three contiguous elements
			test.b_el = elevationData.getElement(i - 1, j2);
			test.i_b = i - 1;
			test.j_b = j2;
		}
		if (elevationData.getElement(i + 1, j2) < test.b_el) { //searching in the lower row, among the three contiguous elements
			test.b_el = elevationData.getElement(i + 1, j2);
			test.i_b = i + 1;
			test.j_b = j2;
		}
	}
	if (test.c_el <= test.b_el) {
		test.hasDepression = true;
	} else {
		test.hasDepression = false;
	};
	return test;
}

/* depressionFilling function */
void DEM::depressionFilling(depressionData test, int i, int j) {
	int delta = 0;
	int delta_int = 0;

	delta = test.b_el - test.c_el;
	delta_int = (int) (delta / 2);
	int val = 0;
	if (delta % 2 == 0) {
		val = (test.b_el - delta_int);
		elevationData.setElement(val, test.i_b, test.j_b);
		val = (test.c_el + delta_int + 1);
		elevationData.setElement(val, i, j);
	} else {
		val = (test.b_el - delta_int - 1);
		elevationData.setElement(val, test.i_b, test.j_b);
		val = (test.c_el + delta_int);
		elevationData.setElement(val, i, j);
	};
}
#endif /*not defined DFPLANCHON*/

bool DEM::initElevationData() {
	switch (demParam->in) {
	/* initialize this DEMdata with a pyramid like dem
	 * WARNING: works only with square dem*/
	case (GLE::InputData::PYRAMID): {
		if (elevationData.getDimension().n_columns != demParam->number_cols
				|| elevationData.getDimension().n_rows
						!= demParam->number_rows) {
			return false;
		}
		int icr = 0;
		int elVal = 0;
		icr = (demParam->number_rows - 1) / 2;
		for (int i = 0; i < icr + 1; i++) {
			for (int j = i; j < (icr - i) * 2 + i + 1; j++) {
				elVal = i * elStep; // so elVal is an lvalue and i can pass the ref
				elevationData.setElement(elVal, j, i);
				elevationData.setElement(elVal, i, j);
				elevationData.setElement(elVal, j, (icr - i) * 2 + i);
				elevationData.setElement(elVal, (icr - i) * 2 + i, j);
			}
		}
		return true;
		break;
	}
	case (GLE::InputData::WEDGE): {
		if (elevationData.getDimension().n_columns != demParam->number_cols
				|| elevationData.getDimension().n_rows
						!= demParam->number_rows) {
			return false;
		}
		// create the wedge larger surface
		for (unsigned int i = 1; i < demParam->basinLength + 1; i++) {
			for (unsigned int j = 1; j < demParam->number_cols - 1; j++) {
				elevationData.setElement(i * elStep, i, j);
			}
		}
		// creates the back surface
		int delta = demParam->number_cols - demParam->basinWidth;
		int elStepBehind =
				myRound(
						elevationData.getElement(demParam->basinLength, delta)
								/ (demParam->number_rows - 2
										- demParam->basinLength + 1));
		for (unsigned int i = demParam->number_rows - 2; i > demParam->basinLength;
				i--) {
			for (unsigned int j = 1; j < demParam->number_cols - 1; j++) {
				elevationData.setElement(
						elStepBehind * (demParam->number_rows - 1 - i), i, j);
			}
		}
		// adjust the sides
		for (int j = 1; j < delta + 1; j++) {
			for (unsigned int i = 1; i < demParam->number_rows - 1 - j; i++) {
				if (elevationData.getElement(i, j)
						> elevationData.getElement(
								demParam->number_rows - 1 - j, j)) {
					elevationData.setElement(
							elevationData.getElement(
									demParam->number_rows - 1 - j, j), i, j);
					elevationData.setElement(
							elevationData.getElement(
									demParam->number_rows - 1 - j, j), i,
							demParam->number_cols - 1 - j);

				}
			}
		}
		// applying the fixed normal distributed noise
		std::vector<int> vals(demParam->number_rows * demParam->number_cols, 0);
		for (std::vector<int>::iterator it = vals.begin(); it != vals.end();
				++it) {
			*it = rand() % elStep - (elStep / 2);
		}
		// reset border to zero
		OurMatrix<int> vals2(demParam->number_rows, demParam->number_cols,
								vals);
		std::vector<Cell> border;
		Cell upperAngle(*demParam, 0, 0);
		upperAngle.borderFinder(border);
		for (std::vector<Cell>::iterator it = border.begin(); it!= border.end(); ++it) {
			vals2.setElement(0, it->x, it->y);
		}
		elevationData.sumMatrix(vals2);
#ifdef DFPLANCHON
			adjustDepressionsPlanchon();
#endif
#ifndef DFPLANCHON
			adjustDepressions();
#endif
		return true;
		break;
	}
	case (GLE::InputData::DATAFILE): {
		TXTFileParser file(" ", GLE::MATRIX_LINE);
		if (file.parseByLine(demParam->DEMfilename)) {
			std::vector<std::string> t = file.getTokens();
#ifdef DEBUG
			file.printTokens();
#endif
			if (t.size()
					!= (unsigned int) demParam->number_rows
							* demParam->number_cols) {
				std::stringstream error;
				error << "Error while parsing the dem file: " << t.size()
						<< " tokens were found in spite of "
						<< demParam->number_rows * demParam->number_cols
						<< std::endl;
				LogFileProducer::Instance()->log(error.str());
			}
			std::vector<int> elValues(t.size(), 0);
			std::vector<int>::iterator it_int = elValues.begin();
			for (std::vector<std::string>::iterator it = t.begin();
					it != t.end(); ++it) {
				std::stringstream(*it) >> *it_int;
				++it_int;
			}
			elevationData = OurMatrix<int>(demParam->number_rows,
					demParam->number_cols, elValues);
#ifdef DEBUG
			elevationData.printMatrix();
#endif
#ifdef DFPLANCHON
			adjustDepressionsPlanchon();
#endif
#ifndef DFPLANCHON
			adjustDepressions();
#endif
			return true;
		} else {
			// failed to open the file
			std::stringstream error;
			error << "Unable to open file: " << demParam->DEMfilename
					<< std::endl;
			LogFileProducer::Instance()->log(error.str());
		}
		break;
	}
	}
	return false;
}

void DEM::evaluateMass() {
	mass = ((double) elevationData.sumAll()) / 100.0; // TODO mass is in meters (per unit of area)
}

inline int DEM::myRound(double r) {
	return r > 0.0 ? floor(r + 0.5) : ceil(r - 0.5);
}

bool DEM::isHydroNetEvaluated() const {
	return hydroNet != NULL ? true : false;
}

/* -------------- DEM::GETTERS ---------------- */
const double DEM::getMass() const {
	return mass;
}

OurMatrix<int>& DEM::getElevationData() {
	return elevationData;
}

const OurMatrix<int>& DEM::getStx() const {
	return hydroNet->getStx();
}

const OurMatrix<int>& DEM::getSty() const {
	return hydroNet->getSty();
}

const OurMatrix<unsigned int>& DEM::getDrainedArea() const {
	return hydroNet->getDrainedArea();
}

const OurMatrix<double>& DEM::getSlope() const {
	return hydroNet->getSlope();
}

const OurMatrix<double>& DEM::getFlowLength() const {
	return hydroNet->getFlowLength();
}

const OurMatrix<double>& DEM::getDischarge() {
	return hydroNet->getDischarge();
}

const double& DEM::getDischargeThreshold() const {
	return demParam->dischargeThreshold;
}
