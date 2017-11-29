/*
 * EEE.cpp
 *
 *  Created on: Feb 12, 2013
 *      Author: andrevb
 */

#include "EEL.h"
#include "DEM.h"
#include "OurMatrix.h"
#include "Utilities/LogFileProducer.h"
#include <string>

EEL::EEL(DEM& d) :
		Stepcost(), xt(&d) {
}

double EEL::getStepcost() {
	if (getStepcost_prot() == STEPCOST_INIT_VALUE) {
		bool success = false;
		/* local copy of discharge */
		OurMatrix<double> energyExpenditure(xt->getDischarge());
		// apply threshold
		double temp = DISCHARGE_IDENTITY_VALUE;
		energyExpenditure.ifThenSet(xt->getDischargeThreshold(), temp);
		/* evaluation of stepcost */
		if (energyExpenditure.squareRootElByEl()) {
			if (energyExpenditure.productMatrixElByEl(xt->getFlowLength())) {
				setStepcost(energyExpenditure.varianceOfEls());
				success = true;
			}
		}
		if (!success) {
			LogFileProducer::Instance()->log(
					std::string("Error while evaluating EEL stepcost"));
		}
	}
	return getStepcost_prot();
}
