/*
 * EE.h
 *
 *  Created on: 19/nov/2012
 *      Author: lordmzn
 */

#ifndef HYDRODEMTOOLS_EE_H_
#define HYDRODEMTOOLS_EE_H_

#include "Stepcost.h"
class DEM;

class EE: public Stepcost {
public:
	explicit EE(DEM& xt);
	/**
	 * Evaluates and retrieves stepcostValue field.
	 * thanks to the info contained in *dem requires Discharge
	 * and Slope
	 */
	double getStepcost();

private:
	/* Not a responsibility of *this
	 */
	DEM* xt;
};

#endif /* HYDRODEMTOOLS_EE_H_ */
