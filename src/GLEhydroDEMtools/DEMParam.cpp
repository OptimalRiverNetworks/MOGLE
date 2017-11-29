/*
 * DEMParam.cpp
 *
 *  Created on: 03/gen/2013
 *      Author: lordmzn
 */
#include "DEMParam.h"
#include "Utilities/LogFileProducer.h"
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>

// in initialization list only non simple types (like vector)
DEMParam::DEMParam() :
		DEMfilename(""), typeOfObjectives(1) {
	// delta applied to the dem
	controlUpperBound = 1;
	controlLowerBound = -1;
	fixedElevationChange = 100;
	fixedElevationChangeFromOpt = 100;
	// control interpolator
	interpolatorType = GLE::Interpolator::NONE;
	number_inputs = 9*9;
	D_RBF = 2;
	sample_frequence = 5;
	neighborhood_size = 15;
	weighting_exponent = 2;
	// state (DEM) related parameters
	number_rows = 11;
	number_cols = 11;
	// DEM cell parameters
	areaOfDEMCell = 1;
	lengthOfDEMCell = 1000;
	widthOfDEMCell = 1000;
	// initial state (DEM) parameters
	in = GLE::InputData::PYRAMID;
	basinLength = 50;
	basinWidth = 50;
	shapeFactor = 1;
	// hydrological parameters
	runoffCoeff = 1.0;
	rainfallExcess = 0.1;
	// objective
	typeOfObjectives.push_back(GLE::Stepcost::TEE_COST);
	adThreshold = 1;
	dischargeThreshold = 0.0;
	// mass constrain parameters
	massConstrainEnabled = false;
	massTolerance = 0.001;
}

bool DEMParam::isValid() {
	int validity[9];
	validity[0] = areDEMInputSourceAndNumberOfRowsAndColumnsValid();
	validity[1] = isDEMCellDimensionValid();
	validity[2] = isRainfallExcessValid();
	validity[3] = isRunoffCoeffValid();
	validity[4] = isMassVariationToleranceValid();
	validity[5] = isFixedElevationChangeValid();
	validity[6] = areControlBoundsValid();
	validity[7] = areInterpolatorParametersValid();
	validity[8] = isAdThresholdValid();
	bool fail = false;
	for (int i = 0; i < 9; i++) {
		if (validity[i] < 0) {
			return false;
		} else if (validity[i] > 0) {
			fail = true;
		}
	}
	if (fail) {
		LogFileProducer::Instance()->log(
				std::string(
						"Some of the parameters in the given file were wrong - using default ones."));
	}
	return true;
}

int DEMParam::areDEMInputSourceAndNumberOfRowsAndColumnsValid() {
	int success = -1;
	// check if dimension are too low to proceed
	if (number_cols < 0 || number_rows < 0 || number_cols * number_rows < 10) {
		return -1;
	}
	if (in == GLE::InputData::PYRAMID) {
		// check if dimension are ok to build a pyramid
		if (number_rows == number_cols) {
			if (number_rows % 2 == 1) {
				success = 0;
			} else {
				number_cols = number_rows = number_rows + 1;
				LogFileProducer::Instance()->log(
						std::string("set nc = nr = nr+1;"));
				success = 1;
			}
		} else {
			if (number_rows % 2 == 1) {
				number_cols = number_rows;
				LogFileProducer::Instance()->log(
						std::string("set nc = nr;"));
				success = 1;
			} else if (number_cols % 2 == 1) {
				LogFileProducer::Instance()->log(
						std::string("set nr = nc;"));
				number_rows = number_cols;
				success = 1;
			} else {
				return -1;
			}
		}
	} else if (in == GLE::InputData::DATAFILE) {
		if (DEMfilename.compare(std::string("")) != 0) {
			success = 0;
		} else {
			LogFileProducer::Instance()->log(
					std::string(
							"A filename is required to load baseDEM' elevations."));
			return -1;
		}
	} else if (in == GLE::InputData::WEDGE) {
		if (basinLength > 10 && shapeFactor > 0.25) {
			double temp = (double) basinLength * shapeFactor;
			basinWidth = temp > 0.0 ? floor(temp + 0.5) : ceil(temp - 0.5);
			temp = basinLength * 1.2;
			// add +2 for the border
			number_rows = (temp > 0.0 ? floor(temp + 0.5) : ceil(temp - 0.5)) + 2;
			temp = basinWidth * 1.2;
			number_cols = (temp > 0.0 ? floor(temp + 0.5) : ceil(temp - 0.5)) + 2;
			success = 0;
		} else {
			LogFileProducer::Instance()->log(
					std::string(
							"The basin length and shape factor required to "
									"build the wedge aren't valid."));
			return -1;
		}
	} else {
		LogFileProducer::Instance()->log(
				std::string("Wrong DEM input source."));
	}
	return success;
}

int DEMParam::isDEMCellDimensionValid() {
	if (areaOfDEMCell > 0 && lengthOfDEMCell > 0 && widthOfDEMCell > 0) {
		if (lengthOfDEMCell * widthOfDEMCell != areaOfDEMCell * 1000000) { // TODO double equality
			widthOfDEMCell = ((double) this->areaOfDEMCell * 1000000)
					/ this->lengthOfDEMCell; // TODO meter
			LogFileProducer::Instance()->log(
					std::string(
							"Inequality on param.dimensions cause width recalculation"));
			return 1;
		}
		return 0;
	}
	return -1;
}

int DEMParam::isMassVariationToleranceValid() {
	if (massTolerance < 0) {
		massTolerance = 0.001; // 0.1%
		LogFileProducer::Instance()->log(
				std::string("reset massTol to 0.001;"));
		return 1;
	}
	return 0;
}

int DEMParam::isRunoffCoeffValid() {
	if (runoffCoeff < 0.0 || runoffCoeff > 1.0) {
		runoffCoeff = 1.0; // % of rainfall that becomes flow
		LogFileProducer::Instance()->log(
				std::string("reset runoffCoeff to 1.0;"));
		return 1;
	}
	return 0;
}

int DEMParam::isRainfallExcessValid() {
	if (rainfallExcess < 0.0) {
		rainfallExcess = 0.1; // TODO mm/hr (0.1mm/hr=876mm/yr)
		LogFileProducer::Instance()->log(
				std::string("reset rainfallExc to 0.1;"));
		return 1;
	}
	return 0;
}

int DEMParam::isFixedElevationChangeValid() {
	if (fixedElevationChange < 0) {
		fixedElevationChange = 100; // TODO centimeters
		LogFileProducer::Instance()->log(
				std::string("reset fEC to 100;"));
		return 1;
	}
	return 0;
}

int DEMParam::areControlBoundsValid() {
	if (controlUpperBound < controlLowerBound) {
		controlUpperBound = 1;
		controlLowerBound = -1;
		LogFileProducer::Instance()->log(
				std::string("reset uBound = 1 and lBound = -1;"));
		return 1;
	}
	return 0;
}

int DEMParam::areInterpolatorParametersValid() {
	switch(interpolatorType) {
	case GLE::Interpolator::NONE: {
		number_inputs = (number_cols - 2) * (number_rows - 2);
		return 0;
		break;
	}
	case GLE::Interpolator::RBF_ML_ON_DEM: {
		int res = 0;
		if (sample_frequence < 1 || D_RBF < 1) {
			sample_frequence = 5;
			D_RBF = 2;
			res = 1;
		}
		number_inputs = ((number_rows - 2) / sample_frequence)
				* ((number_cols - 2) / sample_frequence);
		fixedElevationChangeFromOpt = fixedElevationChange;
		fixedElevationChange = 2;
		return res;
		break;
	}
	case GLE::Interpolator::INVERSE_WEIGHTED_DISTANCE: {
		int res = 0;
		if (sample_frequence < 1) {
			sample_frequence = 5;
			res = 1;
		}
		if (neighborhood_size < 1) {
			neighborhood_size = 10;
			res = 1;
		}
		number_inputs = ((number_rows - 2) / sample_frequence)
				* ((number_cols - 2) / sample_frequence);
		fixedElevationChangeFromOpt = fixedElevationChange;
		fixedElevationChange = 2;
		return res;
		break;
	}
	case GLE::Interpolator::IDW_ON_CONTROLS: {
		int res = 0;
		if (sample_frequence < 1) {
			sample_frequence = 5;
			res = 1;
		}
		number_inputs = ((number_rows - 2) / sample_frequence)
				* ((number_cols - 2) / sample_frequence);
		fixedElevationChangeFromOpt = fixedElevationChange;
		fixedElevationChange = 2;
		return res;
		break;
	}
	}
	return -1;
}

int DEMParam::isAdThresholdValid() {
	if (adThreshold == 0) {
		adThreshold = 1;
	}
	dischargeThreshold = adThreshold * rainfallExcess * runoffCoeff
			* areaOfDEMCell / 3.6;
	return 0;
}

std::string DEMParam::toString() {
	YAML::Node node(*this);
	std::stringstream msg;
	msg << node;
	return msg.str();
}
