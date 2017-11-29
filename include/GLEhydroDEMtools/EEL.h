/*
 * EEE.cpp
 *
 *  Created on: Feb 12, 2013
 *      Author: andrevb
 */

#ifndef HYDRODEMTOOLS_EEL_H_
#define HYDRODEMTOOLS_EEL_H_

#include "Stepcost.h"
class DEM;

/* Represents the implementation of the Energy Expenditure at any link of the network stepcost,
 * jointly with TEE objective, evaluable over a given DEM.
 * Implements the generic data type Stepcost.
 */
class EEL: public Stepcost {
public:
	/* initialize *dem to *xt and stepcostValue to 0 */
	explicit EEL(DEM& xt);
	/**
	 * Evaluates and retrieves the stepcost value. It uses the info
	 * contained in *dem. Requires Discharge and Slope
	 */
	double getStepcost();

private:
	/* Not a responsibility of *this
	 */
	DEM* xt;
};

#endif /* HYDRODEMTOOLS_EEL_H_ */

