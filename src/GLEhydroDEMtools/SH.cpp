/*
 * SH.cpp
 *
 *  Created on: Aug 23, 2012
 *      Author: andrevb
 */

#include "SH.h"
#include "DEM.h"
#include "OurMatrix.h"
#include "Utilities/LogFileProducer.h"
#include <string>

SH::SH(DEM& d) :
		Stepcost(), xt(&d) {
}

double SH::getStepcost() {
	if (getStepcost_prot() == STEPCOST_INIT_VALUE) {
		bool success = false;
		/* local copy of slope */
		OurMatrix<double> slopeElevationRatio(xt->getSlope());
		// apply threshold (NOPE)
		//double temp = DISCHARGE_IDENTITY_VALUE;
		//slopeElevationRatio.ifThenSet(xt->getDischargeThreshold(), temp);
		/* evaluation of stepcost */
		if (slopeElevationRatio.divisionMatrixElByEl(xt->getElevationData().toDouble())) {
				setStepcost(slopeElevationRatio.varianceOfEls());
				success = true;
		}
		if (!success) {
			LogFileProducer::Instance()->log(
					std::string("Error while evaluating SH stepcost"));
		}
	}
	return getStepcost_prot();
}
