/*
 * EEE.cpp
 *
 *  Created on: Aug 23, 2012
 *      Author: andrevb
 */

#include "EEE.h"
#include "DEM.h"
#include "OurMatrix.h"
#include "Utilities/LogFileProducer.h"
#include <string>

EEE::EEE(DEM& d) :
		Stepcost(), xt(&d) {
}

double EEE::getStepcost() {
	if (getStepcost_prot() == STEPCOST_INIT_VALUE) {
		bool success = false;
		/* local copy of discharge */
		OurMatrix<double> energyExpenditure(xt->getDischarge());
		// apply threshold
		double temp = DISCHARGE_IDENTITY_VALUE;
		energyExpenditure.ifThenSet(xt->getDischargeThreshold(), temp);
		/* evaluation of stepcost */
		if (energyExpenditure.squareRootElByEl()) {
			if (energyExpenditure.productMatrixElByEl(xt->getSlope())) {
				setStepcost(energyExpenditure.varianceOfEls());
				success = true;
			}
		}
		if (!success) {
			LogFileProducer::Instance()->log(
					std::string("Error while evaluating EEE stepcost"));
		}
	}
	return getStepcost_prot();
}
