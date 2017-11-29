/*
 * TEE.h
 *
 *  Created on: 22/ago/2012
 *      Author: lordmzn
 */

#ifndef HYDRODEMTOOLS_TEE_H_
#define HYDRODEMTOOLS_TEE_H_

#include "Stepcost.h"
class DEM;

/* Represents the Total Energy Expenditure stepcost evaluable over a
 * given DEM. Implements the generic data type Stepcost.
 */
class TEE: public Stepcost {
public:
	/* initialize *dem to *xt and stepcostValue to 0 */
	explicit TEE(DEM& xt);
	/**
	 * Evaluate the stepcostValue. Set stepcostValue field thanks to
	 * the info contained in *dem; requires discharge and flowLength
	 */
	double getStepcost();

private:
	/* Not a responsibility for *this
	 */
	DEM* xt;
};

#endif /* HYDRODEMTOOLS_TEE_H_ */
