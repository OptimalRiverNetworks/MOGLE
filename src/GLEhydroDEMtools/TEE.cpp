/*
 * TEE.cpp
 *
 *  Created on: 22/ago/2012
 *      Author: lordmzn
 */

#include "TEE.h"
#include "DEM.h"
#include "OurMatrix.h"
#include "Utilities/LogFileProducer.h"
#include <string>

TEE::TEE(DEM& dem) :
		Stepcost(), xt(&dem) {
}

double TEE::getStepcost() {
	if (getStepcost_prot() == STEPCOST_INIT_VALUE) {
		bool success = false;
		/* local copy of disch */
		OurMatrix<double> totalEnergyExpenditure(xt->getDischarge());
		// apply threshold
		double temp = DISCHARGE_IDENTITY_VALUE;
		totalEnergyExpenditure.ifThenSet(xt->getDischargeThreshold(), temp);
		/* evaluation of stepcost */
		if (totalEnergyExpenditure.squareRootElByEl()) {
			if (totalEnergyExpenditure.productMatrixElByEl(xt->getFlowLength())) {
				setStepcost(totalEnergyExpenditure.sumAll());
				success = true;
			}
		}
		if (!success) {
			LogFileProducer::Instance()->log(
					std::string("Error while evaluating TEE stepcost"));
		}
	}
	return getStepcost_prot();
}
