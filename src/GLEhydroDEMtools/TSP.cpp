/*
 * TSP.cpp
 *
 *  Created on: 19/nov/2012
 *      Author: lordmzn
 */

#include "TSP.h"
#include "DEM.h"
#include "OurMatrix.h"
#include "Utilities/LogFileProducer.h"
#include <string>

TSP::TSP(DEM& dem) :
		Stepcost(), xt(&dem) {
}

double TSP::getStepcost() {
	if (getStepcost_prot() == STEPCOST_INIT_VALUE) {
		bool success = false;
		OurMatrix<double> totalStreamPower(xt->getDischarge());
		// apply threshold
		double temp = DISCHARGE_IDENTITY_VALUE;
		totalStreamPower.ifThenSet(xt->getDischargeThreshold(), temp);
		if (totalStreamPower.productMatrixElByEl(xt->getSlope())) {
			setStepcost(totalStreamPower.sumAll());
			success = true;
		}
		if (!success) {
			LogFileProducer::Instance()->log(
					std::string("Error while evaluating TSP stepcost"));
		}
	}
	return getStepcost_prot();
}
