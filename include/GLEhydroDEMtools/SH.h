/*
 * SH.h
 *
 *  Created on: 22/ago/2012
 *      Author: lordmzn
 *  Modified on: 23/ago/2012 by andrea.cominola
 */

#ifndef HYDRODEMTOOLS_SH_H_
#define HYDRODEMTOOLS_SH_H_

#include "Stepcost.h"
class DEM;

/* Represents the ratio between slopes and elevations.
 * Implements the generic data type Stepcost.
 */
class SH: public Stepcost {
public:
	/* initialize *dem to *xt and stepcostValue to 0 */
	explicit SH(DEM& xt);
	/**
	 * Evaluates and retrieves the stepcost value. It uses the info
	 * contained in *dem. Requires Slope and Elevations.
	 */
	double getStepcost();

private:
	/* Not a responsibility of *this
	 */
	DEM* xt;
};

#endif /* HYDRODEMTOOLS_SH_H_ */

