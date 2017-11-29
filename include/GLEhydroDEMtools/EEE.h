/*
 * EEE.h
 *
 *  Created on: 22/ago/2012
 *      Author: lordmzn
 *  Modified on: 23/ago/2012 by andrea.cominola
 */

#ifndef HYDRODEMTOOLS_EEE_H_
#define HYDRODEMTOOLS_EEE_H_

#include "Stepcost.h"
class DEM;

/* Represents the Equal Energy Expenditure stepcost evaluable over a
 * given DEM. Implements the generic data type Stepcost.
 */
class EEE: public Stepcost {
public:
	/* initialize *dem to *xt and stepcostValue to 0 */
	explicit EEE(DEM& xt);
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

#endif /* HYDRODEMTOOLS_EEE_H_ */

