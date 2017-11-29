/*
 * HydroNet.cpp
 *
 *  Created on: 27/nov/2012
 *      Author: lordmzn
 */

#include "HydroNet.h"
#include "Utilities/LogFileProducer.h"
#include <cstdlib>

HydroNet::HydroNet(DEM* d) :
		dem(d), stx(d->demParam->number_rows, d->demParam->number_cols), sty(
				d->demParam->number_rows, d->demParam->number_cols), drainedArea(
				d->demParam->number_rows, d->demParam->number_cols), slope(
				d->demParam->number_rows, d->demParam->number_cols), flowLength(
				d->demParam->number_rows, d->demParam->number_cols), discharge(
				NULL) {
	evaluateFlowDirection();
}

HydroNet::HydroNet(const HydroNet& h) :
		dem(h.dem), stx(h.stx), sty(h.sty), drainedArea(h.drainedArea), slope(
				h.slope), flowLength(h.flowLength), discharge(h.discharge) {
}

HydroNet::~HydroNet() {
	delete discharge;
}

HydroNet& HydroNet::operator=(const HydroNet& h) {
	// check self assignment
	if (this == &h) {
		return *this;
	}
	// deallocate memory used by this
	delete discharge;
	// copy every field
	dem = h.dem; // shallow copy is ok here
	stx = h.stx;
	sty = h.sty;
	drainedArea = h.drainedArea;
	slope = h.slope;
	flowLength = h.flowLength;
	discharge = h.discharge;
	return *this;
}

const OurMatrix<unsigned int>& HydroNet::getDrainedArea() const {
	return drainedArea;
}
const OurMatrix<int>& HydroNet::getStx() const {
	return stx;
}
const OurMatrix<int>& HydroNet::getSty() const {
	return sty;
}
const OurMatrix<double>& HydroNet::getSlope() const {
	return slope;
}
const OurMatrix<double>& HydroNet::getFlowLength() const {
	return flowLength;
}
const OurMatrix<double>& HydroNet::getDischarge() {
	if (discharge == NULL) {
		discharge = new OurMatrix<double>(drainedArea.toDouble());
		double a = (dem->demParam->rainfallExcess)
				* (dem->demParam->runoffCoeff) * (dem->demParam->areaOfDEMCell)
				/ 3.6;
		if (!discharge->productMatrixElByEl(a)) {
			LogFileProducer::Instance()->log(
					std::string(
							"Unable to evaluate the discharge. Drained area is used."));
		}
	}
	return *discharge;
}

void HydroNet::evaluateFlowDirection() {
	/*int initPool = 0;
	 int endPool = 0;
	 int poolSize = 40;
	 int poolPosition = 0;
	 int i = 0;
	 int i2 = 0;
	 int j = 0;
	 int* i_pool;
	 int* j_pool;
	 int* pool;
	 i_pool = new int[poolSize];
	 j_pool = new int[poolSize];
	 pool = new int[poolSize];
	 bool callGD8internalCycle = true;
	 bool callGD8externalCycle = true;

	 while (callGD8externalCycle == true) {
	 while (callGD8internalCycle == true) {
	 if (initPool == 0 && endPool == 0) {
	 for (i = initPool; i < endPool; i++) {
	 if (this->stx.getElement(i_pool[i], j_pool[i]) == 0
	 && this->sty.getElement(i_pool[i], j_pool[i])
	 == 0) {
	 pool[0] = pool[i];
	 j_pool[0] = j_pool[i];
	 i_pool[0] = i_pool[i];
	 initPool = i + 1;
	 if (pool[0] > 0) {
	 GD8(i_pool[1], j_pool[1]);
	 callGD8internalCycle = true;
	 } else {
	 callGD8internalCycle = false;
	 };
	 };
	 }
	 } else {
	 callGD8internalCycle = false;
	 };

	 initPool = 0;
	 endPool = 0;
	 for (i = 0; i <= poolSize; i++) {
	 pool[i] = 0;
	 };

	 for (j = 1; j <= ncols - 2; j++) {
	 for (i = 1; i <= nrows - 2; i++) {
	 if (this->elevationData.getElement(i, j) == 0
	 || this->slope.getElement(i, j) < 0
	 || this->stx.getElement(i, j) != 0
	 || this->sty.getElement(i, j) != 0) {
	 continue;
	 };
	 for (poolPosition = 0; poolPosition < poolSize;
	 poolPosition++) {
	 if (this->elevationData.getElement(i, j)
	 > pool[poolPosition]) {
	 for (i2 = poolSize-1; i2 >= poolPosition; i2--) {
	 pool[i2] = pool[i2 - 1];
	 i_pool[i2] = i_pool[i2 - 1];
	 j_pool[i2] = j_pool[i2 - 1];
	 };
	 pool[poolPosition] = this->elevationData.getElement(
	 i, j);
	 i_pool[poolPosition] = i;
	 j_pool[poolPosition] = j;
	 endPool = std::min(endPool + 1, poolSize);
	 break;
	 };
	 };
	 };
	 };

	 if (pool[0] > 0) {
	 GD8(i_pool[0], j_pool[0]);
	 callGD8externalCycle = true;
	 } else {
	 callGD8externalCycle = false;
	 };
	 };
	 };*/
	int max = 0;
	HydroNetInternal::Point max_p;

	bool callGD8 = true;
	while (callGD8 == true) {
		for (unsigned int j = 1; j < dem->demParam->number_cols - 1; j++) {
			for (unsigned int i = 1; i < dem->demParam->number_rows - 1; i++) {
				if (stx.getElement(i, j) == 0 && sty.getElement(i, j) == 0) {
					if (max < dem->elevationData.getElement(i, j)) {
						max = dem->elevationData.getElement(i, j);
						max_p.x = i;
						max_p.y = j;
					}
				}
			}
		}
		if (max == 0) {
			callGD8 = false;
		} else {
			GD8(max_p);
			max = 0;
		}
	};
	evaluateDrainedArea();
}

void HydroNet::evaluateDrainedArea() {
	HydroNetInternal::Point curr;
	HydroNetInternal::Point next;
	int dAVal;
	int delta = 0;
	bool addOnlyTheDelta;
	// start from center
	for (unsigned int j = 1; j <= dem->demParam->number_cols - 2; j++) {
		for (unsigned int i = 1; i <= dem->demParam->number_rows - 2; i++) {
			// if the cell is a sink, do nothing
			if (dem->elevationData.getElement(i, j) < 1) {
				continue;
			};
			// if the cell has already a drained area, skip
			if (drainedArea.getElement(i, j) > 0) {
				continue;
			}
			curr.x = i;
			curr.y = j;
			addOnlyTheDelta = false;
			//Discharge not calculated
			//Drained area computation
			drainedArea.setElement(dem->demParam->areaOfDEMCell, curr.x,
					curr.y);
			next.x = curr.x + sty.getElement(curr.x, curr.y);
			next.y = curr.y + stx.getElement(curr.x, curr.y);
			while (dem->elevationData.getElement(next.x, next.y) > 0) {
				// if the next elements has been evaluated yet
				if (drainedArea.getElement(next.x, next.y) > 0) {
					// if we're in the intersection
					if (!addOnlyTheDelta) {
						// set the delta
						delta = drainedArea.getElement(curr.x, curr.y);
						addOnlyTheDelta = true;
					}
					// just add the delta
					dAVal = drainedArea.getElement(next.x, next.y) + delta;
				} else {
					// add the dA
					dAVal = drainedArea.getElement(curr.x, curr.y)
							+ dem->demParam->areaOfDEMCell;
				}
				drainedArea.setElement(dAVal, next.x, next.y);
				// update references to cells
				curr.x = next.x;
				curr.y = next.y;
				next.x = curr.x + sty.getElement(curr.x, curr.y);
				next.y = curr.y + stx.getElement(curr.x, curr.y);
			};
		};
	};
}

void HydroNet::GD8(HydroNetInternal::Point start) {
	HydroNetInternal::GD8state state;
	state.reference.x = start.x;
	state.reference.y = start.y;
	state.current.x = start.x;
	state.current.y = start.y;
	state.isNextValid = false;

	// for the first cell primary and secondary are evaluated but the primary is chosen
	primaryDirectionSearch(state.current, true);
	state.secDir = secondaryDirectionSearch(state.current);

	if (dem->elevationData.getElement(
			state.current.x + sty.getElement(state.current.x, state.current.y),
			state.current.y + stx.getElement(state.current.x, state.current.y))
			> 0
			&& (stx.getElement(
					state.current.x
							+ sty.getElement(state.current.x, state.current.y),
					state.current.y
							+ stx.getElement(state.current.x, state.current.y))
					== 0)
			&& (sty.getElement(
					state.current.x
							+ sty.getElement(state.current.x, state.current.y),
					state.current.y
							+ stx.getElement(state.current.x, state.current.y))
					== 0)) {
		state.isNextValid = true;
	};

	while (state.isNextValid == true) {
		state = computeDirection(state);
	};
}

void HydroNet::primaryDirectionSearch(HydroNetInternal::Point curr,
		bool callTurnFirst) {
	double cellDistance = 0; //Distance between the centers of two adjacent cells
	double currentSlope = 0;
	double probabilityOfChange = 0;

	unsigned int i = curr.x - 1;
	unsigned int j = curr.y - 1;

	srand(time(NULL));

	for (j = curr.y - 1; j < curr.y + 2; j++) {
		for (i = curr.x - 1; i < curr.x + 2; i++) {
			if ((i == curr.x) && (j == curr.y)) {
				continue; //The local search is among the 8-cell boundary. The current cell is skipped.
			};
			if ((i == curr.x) || (j == curr.y)) {
				cellDistance = dem->demParam->lengthOfDEMCell; //Vertical/horizontal case
			} else {
				cellDistance = sqrt(2) * dem->demParam->lengthOfDEMCell; //Diagonal case
			};
			currentSlope = (dem->elevationData.getElement(curr.x, curr.y)
					- dem->elevationData.getElement(i, j)) / cellDistance / 100; // TODO centimers vs meters
			/* to be passed as reference, the following has to be lvalue, that in this case
			 * means that they have to be named. useful if we move from int to an object type
			 */
			int j_minus_y = j - curr.y;
			int i_minus_x = i - curr.x;
			if (currentSlope > slope.getElement(curr.x, curr.y)) {
				slope.setElement(currentSlope, curr.x, curr.y);
				flowLength.setElement(cellDistance, curr.x, curr.y);
				stx.setElement(j_minus_y, curr.x, curr.y);
				sty.setElement(i_minus_x, curr.x, curr.y);
			} else if (currentSlope == slope.getElement(curr.x, curr.y) // TODO equality between double
			&& callTurnFirst == true) { // TODO it doesn't make sense
				probabilityOfChange = (double) rand() / (double) RAND_MAX;
				if (probabilityOfChange > 0.5) {
					slope.setElement(currentSlope, curr.x, curr.y);
					flowLength.setElement(cellDistance, curr.x, curr.y);
					stx.setElement(j_minus_y, curr.x, curr.y);
					sty.setElement(i_minus_x, curr.x, curr.y);
				};
			};
		};
	};
}

int HydroNet::secondaryDirectionSearch(HydroNetInternal::Point curr) {
	int secondaryDirection = 0; //secondaryDirection=1 if clockwise, -1 if counterclockwise
	if ((stx.getElement(curr.x, curr.y) != 0)
			&& (sty.getElement(curr.x, curr.y) != 0)) { //General case
		if ((dem->elevationData.getElement(curr.x, curr.y)
				- dem->elevationData.getElement(curr.x,
						curr.y + stx.getElement(curr.x, curr.y)))
				> (dem->elevationData.getElement(curr.x, curr.y)
						- dem->elevationData.getElement(
								curr.x + sty.getElement(curr.x, curr.y), curr.y))
				&& (dem->elevationData.getElement(curr.x, curr.y)
						- dem->elevationData.getElement(curr.x,
								curr.y + stx.getElement(curr.x, curr.y))) > 0) {
			secondaryDirection = -stx.getElement(curr.x, curr.y)
					* sty.getElement(curr.x, curr.y);
		} else if ((dem->elevationData.getElement(curr.x, curr.y)
				- dem->elevationData.getElement(curr.x,
						curr.y + stx.getElement(curr.x, curr.y)))
				< (dem->elevationData.getElement(curr.x, curr.y)
						- dem->elevationData.getElement(
								curr.x + sty.getElement(curr.x, curr.y), curr.y))
				&& (dem->elevationData.getElement(curr.x, curr.y)
						- dem->elevationData.getElement(
								curr.x + sty.getElement(curr.x, curr.y), curr.y))
						> 0) {
			secondaryDirection = stx.getElement(curr.x, curr.y)
					* sty.getElement(curr.x, curr.y);
		};
	} else if ((stx.getElement(curr.x, curr.y) == 0)
			&& (sty.getElement(curr.x, curr.y) != 0)) { //Case with only sty
		if ((dem->elevationData.getElement(curr.x, curr.y)
				- dem->elevationData.getElement(
						curr.x + sty.getElement(curr.x, curr.y), curr.y + 1))
				> (dem->elevationData.getElement(curr.x, curr.y)
						- dem->elevationData.getElement(
								curr.x + sty.getElement(curr.x, curr.y),
								curr.y - 1))
				&& (dem->elevationData.getElement(curr.x, curr.y)
						- dem->elevationData.getElement(
								curr.x + sty.getElement(curr.x, curr.y),
								curr.y + 1)) > 0) {
			secondaryDirection = -sty.getElement(curr.x, curr.y);
		} else if ((dem->elevationData.getElement(curr.x, curr.y)
				- dem->elevationData.getElement(
						curr.x + sty.getElement(curr.x, curr.y), curr.y + 1))
				< (dem->elevationData.getElement(curr.x, curr.y)
						- dem->elevationData.getElement(
								curr.x + sty.getElement(curr.x, curr.y),
								curr.y - 1))
				&& (dem->elevationData.getElement(curr.x, curr.y)
						- dem->elevationData.getElement(
								curr.x + sty.getElement(curr.x, curr.y),
								curr.y - 1)) > 0) {
			secondaryDirection = sty.getElement(curr.x, curr.y);
		};
	} else if ((stx.getElement(curr.x, curr.y) != 0)
			&& (sty.getElement(curr.x, curr.y) == 0)) { //Case with only stx
		if ((dem->elevationData.getElement(curr.x, curr.y)
				- dem->elevationData.getElement(curr.x + 1,
						curr.y + stx.getElement(curr.x, curr.y)))
				> (dem->elevationData.getElement(curr.x, curr.y)
						- dem->elevationData.getElement(curr.x - 1,
								curr.y + stx.getElement(curr.x, curr.y)))
				&& (dem->elevationData.getElement(curr.x, curr.y)
						- dem->elevationData.getElement(curr.x + 1,
								curr.y + stx.getElement(curr.x, curr.y))) > 0) {
			secondaryDirection = stx.getElement(curr.x, curr.y);
		} else if ((dem->elevationData.getElement(curr.x, curr.y)
				- dem->elevationData.getElement(curr.x + 1,
						curr.y + stx.getElement(curr.x, curr.y)))
				< (dem->elevationData.getElement(curr.x, curr.y)
						- dem->elevationData.getElement(curr.x - 1,
								curr.y + stx.getElement(curr.x, curr.y)))
				&& (dem->elevationData.getElement(curr.x, curr.y)
						- dem->elevationData.getElement(curr.x - 1,
								curr.y + stx.getElement(curr.x, curr.y))) > 0) {
			secondaryDirection = -stx.getElement(curr.x, curr.y);
		};
	};
	return secondaryDirection;
}
HydroNetInternal::GD8state HydroNet::computeDirection(
		HydroNetInternal::GD8state state) {
	HydroNetInternal::Point next;
	int next_secDir = 0;
	next.x = state.current.x + sty.getElement(state.current.x, state.current.y); //downstream cell coordinates
	next.y = state.current.y + stx.getElement(state.current.x, state.current.y);

	double distanceFromRefCell = 0;
	double secondaryDistanceFromRefCell = 0;
	double primaryGradient = 0;
	double secondaryGradient = 0;
	int secondaryCoefficient;

	if (next.x == 1 - 1 || next.x == dem->demParam->number_rows - 1
			|| next.y == 1 - 1 || next.y == dem->demParam->number_cols - 1) {
		state.isNextValid = false;
		return state;
	};
	primaryDirectionSearch(next, false);

	if (slope.getElement(next.x, next.y) <= 0) {
		std::stringstream msg;
		msg << "Depression point found in (" << next.x << "," << next.y
				<< ") after a depression filling";
		LogFileProducer::Instance()->log(msg.str());
		state.isNextValid = false;
		return state;
	};

	distanceFromRefCell = sqrt(
			(pow((double) abs((next.x + sty.getElement(next.x, next.y)- state.reference.x)), 2.0)
					+ pow((double) abs((next.y + stx.getElement(next.x, next.y)- state.reference.y)), 2.0)))
			* dem->demParam->lengthOfDEMCell;

    //General case
	if (stx.getElement(next.x, next.y) != 0
			&& sty.getElement(next.x, next.y) != 0) {
		int deltaHx = dem->elevationData.getElement(next.x, next.y)
				- dem->elevationData.getElement(next.x,
						next.y + stx.getElement(next.x, next.y));
		int deltaHy = dem->elevationData.getElement(next.x, next.y)
				- dem->elevationData.getElement(
						next.x + sty.getElement(next.x, next.y), next.y);
		if ((deltaHx < 1) && (deltaHy < 1)) { //This condition was not present in the previous second direction search.
			state.reference = next;
			state.secDir = 0;
			state.current = next;
			state.isNextValid = true;
			return state;
		};
		if (deltaHx > deltaHy) {
			next_secDir = -(stx.getElement(next.x, next.y))
					* sty.getElement(next.x, next.y);
			if ((next_secDir * state.secDir == 1)
					&& (sty.getElement(next.x, next.y)
							== sty.getElement(state.current.x, state.current.y))
					&& (stx.getElement(next.x, next.y)
							== stx.getElement(state.current.x, state.current.y))) {
				secondaryDistanceFromRefCell = sqrt(
						pow((double) abs((next.x - state.reference.x)), 2)
								+ pow(
										(double) abs((next.y
												+ stx.getElement(next.x, next.y)
												- state.reference.y)), 2))
						* dem->demParam->lengthOfDEMCell;
				secondaryGradient = (dem->elevationData.getElement(
						state.reference.x, state.reference.y)
						- dem->elevationData.getElement(next.x,
								next.y + stx.getElement(next.x, next.y)))
						/ secondaryDistanceFromRefCell;
				primaryGradient = (dem->elevationData.getElement(
						state.reference.x, state.reference.y)
						- dem->elevationData.getElement(
								next.x + sty.getElement(next.x, next.y),
								next.y + stx.getElement(next.x, next.y)))
						/ distanceFromRefCell;

				if (secondaryGradient > primaryGradient) {
					double slopeVal = ((dem->elevationData.getElement(next.x,
							next.y)
							- dem->elevationData.getElement(next.x,
									next.y + stx.getElement(next.x, next.y)))
							/ (dem->demParam->lengthOfDEMCell / 100));
					slope.setElement(slopeVal, next.x, next.y);
					flowLength.setElement(dem->demParam->lengthOfDEMCell,
							next.x, next.y);
					next_secDir = -next_secDir;
					int zero = 0; // ok, this's getting ridiculous
					sty.setElement(zero, next.x, next.y);
				};
			};
		} else if (deltaHx < deltaHy) {
			next_secDir = (stx.getElement(next.x, next.y))
					* sty.getElement(next.x, next.y);
			if ((next_secDir * state.secDir == 1)
					&& (sty.getElement(next.x, next.y)
							== sty.getElement(state.current.x, state.current.y))
					&& (stx.getElement(next.x, next.y)
							== stx.getElement(state.current.x, state.current.y))) {
				secondaryDistanceFromRefCell = sqrt(
						pow(
								(double) abs((next.x
										+ sty.getElement(next.x, next.y)
										- state.reference.x)), 2)
								+ pow((double) abs((next.y - state.reference.y)), 2))
						* dem->demParam->lengthOfDEMCell;
				secondaryGradient =
						(dem->elevationData.getElement(state.reference.x,
								state.reference.y)
								- dem->elevationData.getElement(
										next.x + sty.getElement(next.x, next.y),
										next.y)) / secondaryDistanceFromRefCell;
				primaryGradient = (dem->elevationData.getElement(
						state.reference.x, state.reference.y)
						- dem->elevationData.getElement(
								next.x + sty.getElement(next.x, next.y),
								next.y + stx.getElement(next.x, next.y)))
						/ distanceFromRefCell;

				if (secondaryGradient > primaryGradient) {
					double slopeVal = ((dem->elevationData.getElement(next.x,
							next.y)
							- dem->elevationData.getElement(
									next.x + sty.getElement(next.x, next.y),
									next.y))
							/ (dem->demParam->lengthOfDEMCell / 100));
					slope.setElement(slopeVal, next.x, next.y);
					flowLength.setElement(dem->demParam->lengthOfDEMCell,
							next.x, next.y);
					next_secDir = -next_secDir;
					int zero = 0; // i know, i know ...
					stx.setElement(zero, next.x, next.y);
				};
			};

		} else if (deltaHx == deltaHy) {
			next_secDir = 0;
		};
		state.secDir = next_secDir;
	} else if (stx.getElement(next.x, next.y) == 0
			&& sty.getElement(next.x, next.y) != 0) { //Case with only sty
		int deltaHleft = dem->elevationData.getElement(next.x, next.y)
				- dem->elevationData.getElement(
						next.x + sty.getElement(next.x, next.y), next.y + 1);
		int deltaHright = dem->elevationData.getElement(next.x, next.y)
				- dem->elevationData.getElement(
						next.x + sty.getElement(next.x, next.y), next.y - 1);
		if ((deltaHleft < 1) && (deltaHright < 1)) { //This condition was not present in the previous second direction search.
			state.reference = next;
			state.secDir = 0;
			state.current = next;
			state.isNextValid = true;
			return state;
		};
		next_secDir = secondaryDirectionSearch(next);
		secondaryCoefficient = -next_secDir / sty.getElement(next.x, next.y);
		if ((next_secDir * state.secDir == 1)
				&& (sty.getElement(next.x, next.y)
						== sty.getElement(state.current.x, state.current.y))
				&& (stx.getElement(next.x, next.y)
						== stx.getElement(state.current.x, state.current.y))) {
			secondaryDistanceFromRefCell = sqrt(
					pow(
							(double) abs((next.x + sty.getElement(next.x, next.y)
									- state.reference.x)), 2)
							+ pow(
									(double) abs((next.y + secondaryCoefficient
											- state.reference.y)), 2))
					* dem->demParam->lengthOfDEMCell;
			secondaryGradient = (dem->elevationData.getElement(
					state.reference.x, state.reference.y)
					- dem->elevationData.getElement(
							next.x + sty.getElement(next.x, next.y),
							next.y + secondaryCoefficient))
					/ secondaryDistanceFromRefCell;
			primaryGradient = (dem->elevationData.getElement(state.reference.x,
					state.reference.y)
					- dem->elevationData.getElement(
							next.x + sty.getElement(next.x, next.y), next.y))
					/ distanceFromRefCell;
			if (secondaryGradient > primaryGradient) {
				double slopeVal = ((dem->elevationData.getElement(next.x,
						next.y)
						- dem->elevationData.getElement(
								next.x + sty.getElement(next.x, next.y),
								next.y + secondaryCoefficient))
						/ (sqrt(2) * dem->demParam->lengthOfDEMCell) / 100);
				slope.setElement(slopeVal, next.x, next.y);
				double flowLengthVal = sqrt(2) * dem->demParam->lengthOfDEMCell;
				flowLength.setElement(flowLengthVal, next.x, next.y);
				next_secDir = -next_secDir;
				stx.setElement(secondaryCoefficient, next.x, next.y);
			};
		};
		state.secDir = next_secDir;
		state.current = next;
		state.isNextValid = true;
		return state;
	} else if (stx.getElement(next.x, next.y) != 0
			&& sty.getElement(next.x, next.y) == 0) { //Case with only stx
		int deltaHup = dem->elevationData.getElement(next.x, next.y)
				- dem->elevationData.getElement(next.x + 1,
						next.y + stx.getElement(next.x, next.y));
		int deltaHdown = dem->elevationData.getElement(next.x, next.y)
				- dem->elevationData.getElement(next.x - 1,
						next.y + stx.getElement(next.x, next.y));
		if ((deltaHup < 1) && (deltaHdown < 1)) { //This condition was not present in the previous second direction search.
			state.reference = next;
			state.secDir = 0;
			state.current = next;
			state.isNextValid = true;
			return state;
		};
		next_secDir = secondaryDirectionSearch(next);
		secondaryCoefficient = next_secDir / stx.getElement(next.x, next.y);
		if ((next_secDir * state.secDir == 1)
				&& (stx.getElement(next.x, next.y)
						== stx.getElement(state.current.x, state.current.y))
				&& (sty.getElement(next.x, next.y)
						== sty.getElement(state.current.x, state.current.y))) {
			secondaryDistanceFromRefCell = sqrt(
					pow(
							(double) abs((next.x + secondaryCoefficient
									- state.reference.x)), 2)
							+ pow(
									(double) abs((next.y
											+ stx.getElement(next.x, next.y)
											- state.reference.y)), 2))
					* dem->demParam->lengthOfDEMCell;
			secondaryGradient = (dem->elevationData.getElement(
					state.reference.x, state.reference.y)
					- dem->elevationData.getElement(
							next.x + secondaryCoefficient,
							next.y + stx.getElement(next.x, next.y)))
					/ secondaryDistanceFromRefCell;
			primaryGradient = (dem->elevationData.getElement(state.reference.x,
					state.reference.y)
					- dem->elevationData.getElement(next.x,
							next.y + stx.getElement(next.x, next.y)))
					/ distanceFromRefCell;
			if (secondaryGradient > primaryGradient) {
				double slopeVal = ((dem->elevationData.getElement(next.x,
						next.y)
						- dem->elevationData.getElement(
								next.x + secondaryCoefficient,
								next.y + stx.getElement(next.x, next.y)))
						/ (sqrt(2) * dem->demParam->lengthOfDEMCell) / 100);
				slope.setElement(slopeVal, next.x, next.y);
				// maybe it's due the presence of literals
				double flowLVal = sqrt(2) * dem->demParam->lengthOfDEMCell;
				flowLength.setElement(flowLVal, next.x, next.y);
				next_secDir = -next_secDir;
				sty.setElement(secondaryCoefficient, next.x, next.y);
			};
		};
		state.secDir = next_secDir;
		state.current = next;
		state.isNextValid = true;
		return state;
	};

	state.current = next;

	if (dem->elevationData.getElement(
			state.current.x + sty.getElement(state.current.x, state.current.y),
			state.current.y + stx.getElement(state.current.x, state.current.y))
			> 0
			&& (stx.getElement(
					state.current.x
							+ sty.getElement(state.current.x, state.current.y),
					state.current.y
							+ stx.getElement(state.current.x, state.current.y))
					== 0)
			&& (sty.getElement(
					state.current.x
							+ sty.getElement(state.current.x, state.current.y),
					state.current.y
							+ stx.getElement(state.current.x, state.current.y))
					== 0)) {
		state.isNextValid = true;
	} else {
		state.isNextValid = false;
	};
	return state;
}
