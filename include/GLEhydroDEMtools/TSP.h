/*
 * TSP.h
 *
 *  Created on: 19/nov/2012
 *      Author: lordmzn
 */

#ifndef HYDRODEMTOOLS_TSP_H_
#define HYDRODEMTOOLS_TSP_H_

#include "Stepcost.h"
class DEM;

/* Represents the Total Stream Power stepcost evaluable over a
 * given DEM. Implements the generic data type Stepcost.
 */
class TSP: public Stepcost {
public:
	explicit TSP(DEM& xt);
	/**
	 * Evaluate the stepcostValue. Set stepcostValue field thanks to
	 * the info contained in *dem; requires discharge and flowLength
	 */
	double getStepcost();

private:
	/* Not a responsibility of *this
	 */
	DEM* xt;
};

#endif /* HYDRODEMTOOLS_TSP_H_ */
