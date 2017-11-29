/*
 * EE.cpp
 *
 *  Created on: 19/nov/2012
 *      Author: lordmzn
 */

#include "EE.h"
#include "DEM.h"
#include "OurMatrix.h"
#include "Utilities/LogFileProducer.h"
#include <string>

EE::EE(DEM& d) :
		Stepcost(), xt(&d) {
}

double EE::getStepcost() {
	if (getStepcost_prot() == STEPCOST_INIT_VALUE) {
		bool success = false;
		OurMatrix<double> energyExpenditure(xt->getDischarge());
		// apply threshold
		double temp = DISCHARGE_IDENTITY_VALUE;
		energyExpenditure.ifThenSet(xt->getDischargeThreshold(), temp);
		if (energyExpenditure.squareRootElByEl()) {
			if (energyExpenditure.productMatrixElByEl(xt->getSlope())) {
				setStepcost(energyExpenditure.sumAll());
				success = true;
			}
		}
		if (!success) {
			LogFileProducer::Instance()->log(
					std::string("Error while evaluating EE stepcost"));
		}
	}
	return getStepcost_prot();
}

