/*
 * Mask.cpp
 *
 *  Created on: 28/dic/2012
 *      Author: lordmzn
 */

#include "Mask.h"
#include "Utilities/LogFileProducer.h"
#include <cmath>
#include <string>

Mask::Mask(DEMParam& p) :
		controlValues(p.number_rows - 2, p.number_cols - 2), param(p), input_norm(
				NULL), input_helper(NULL) {
	switch (param.interpolatorType) {
	case GLE::Interpolator::NONE: {
		break;
	}
	case GLE::Interpolator::RBF_ML_ON_DEM: {
		// length of rescaledParams is param.number_inputs
		// each row will contain 1 input_point (i.e. control)
		// first column is x coordinate, second is y, third is the value
		unsigned int idx = 0;
		rescaledParams = new alglib::real_2d_array();
		rescaledParams->setlength(param.number_inputs, 3);
		for (unsigned int row = 0;
				row < (param.number_rows - 2) / param.sample_frequence; row++) {
			for (unsigned int col = 0;
					col < (param.number_cols - 2) / param.sample_frequence; col++) {
				(*rescaledParams)(idx, 0) = (row + 1) * param.sample_frequence;
				(*rescaledParams)(idx, 1) = (col + 1) * param.sample_frequence;
				idx++;
			}
		}
		// these two vectors contains coordinates at which el should
		// be estimated
		rowsOfPointToEstimate = new alglib::real_1d_array();
		rowsOfPointToEstimate->setlength(param.number_rows - 2);
		for (unsigned int row = 0; row < param.number_rows - 2; row++) {
			(*rowsOfPointToEstimate)(row) = row + 1;
		}
		colsOfPointToEstimate = new alglib::real_1d_array();
		colsOfPointToEstimate->setlength(param.number_cols - 2);
		for (unsigned int col = 0; col < param.number_cols - 2; col++) {
			(*colsOfPointToEstimate)(col) = col + 1;
		}
		// rbfcreate
		rbfModel = new alglib::rbfmodel();
		reporter = new alglib::rbfreport();
		alglib::rbfcreate(param.D_RBF, 1, *rbfModel);
		// rbfsetalgomultilayer
		// it's simplified cause it's a regular squared grid
		double rBase = 2 * param.sample_frequence * (1 + sqrt(2));
		double temp = log(
				2 * rBase / (0.5 * param.sample_frequence * (1 + sqrt(2))))
				/ log(2);
		long int NLayers =
				temp > 0.0 ? floor(temp + 0.5) : ceil(temp - 0.5) + 2;
		alglib::rbfsetalgomultilayer(*rbfModel, rBase, NLayers);
		// rbfsetzeroterm = no polynomial base
		alglib::rbfsetzeroterm(*rbfModel);
		break;
	}
	case GLE::Interpolator::INVERSE_WEIGHTED_DISTANCE: {
		input_helper = new OurMatrix<unsigned int>(param.number_inputs, 2, 0);
		unsigned int idx = 0;
		for (unsigned int row = 0;
				row < (param.number_rows - 2) / param.sample_frequence; row++) {
			for (unsigned int col = 0;
					col < (param.number_cols - 2) / param.sample_frequence; col++) {
				input_helper->setElement((row + 1) * param.sample_frequence, idx, 0);
				input_helper->setElement((col + 1) * param.sample_frequence, idx, 1);
				idx++;
			}
		}
		break;
	}
	case GLE::Interpolator::IDW_ON_CONTROLS: {
		input_helper = new OurMatrix<unsigned int>(param.number_inputs, 2,
				0);
		unsigned int idx = 0;
		for (unsigned int row = 0;
				row < (param.number_rows - 2) / param.sample_frequence; row++) {
			for (unsigned int col = 0;
					col < (param.number_cols - 2) / param.sample_frequence;
					col++) {
				input_helper->setElement((row + 1) * param.sample_frequence,
						idx, 0);
				input_helper->setElement((col + 1) * param.sample_frequence,
						idx, 1);
				idx++;
			}
		}
		break;
	}
	}
}

Mask::~Mask() {
	delete input_norm;
	delete input_helper;
}

Mask& Mask::operator =(const Mask & a) {
	// check self assignment
	if (this == &a) {
		return *this;
	}
	// deallocate memory used by this: none
	// copy each field
	controlValues = a.controlValues;
	param = a.param;
	return *this;
}

unsigned int Mask::setControls(double vals[]) {
	// check validity
	int count_invalid = checkValidity(vals);
	if (count_invalid == 0) {
		// format controls and set them
		switch (param.interpolatorType) {
		case GLE::Interpolator::NONE: {
			int controls[param.number_inputs];
			for (unsigned int i = 0; i < param.number_inputs; i++) {
				controls[i] =
						(vals[i] > 0.0) ?
								floor(vals[i] + 0.5) : ceil(vals[i] - 0.5);
			}
			controlValues = OurMatrix<int>(controlValues.getDimension(),
					controls);
			break;
		}
		case GLE::Interpolator::RBF_ML_ON_DEM: {
			int controls[(param.number_cols - 2) * (param.number_rows - 2)];
			interpolateControlsWithRBFML(vals, controls);
			controlValues = OurMatrix<int>(controlValues.getDimension(),
					controls);
			break;
		}
		case GLE::Interpolator::INVERSE_WEIGHTED_DISTANCE: {
			int controls[(param.number_cols - 2) * (param.number_rows - 2)];
			interpolateControlsWithIDW_NoWindow(vals, controls);
			controlValues = OurMatrix<int>(controlValues.getDimension(),
						controls);
			break;
		}
		case GLE::Interpolator::IDW_ON_CONTROLS: {
			int controls[(param.number_cols - 2) * (param.number_rows - 2)];
			interpolateControlsWithIDW_NoWindow(vals, controls);
			controlValues = OurMatrix<int>(controlValues.getDimension(),
						controls);
			break;
		}
		}
	}
	return count_invalid;
}

const OurMatrix<int>& Mask::getControls() const {
	return controlValues;
}

unsigned int Mask::checkValidity(const double vals[]) {
	int count_invalid = 0;
	// check if number of controls is corrected: not possible
	switch (param.interpolatorType) {
	case GLE::Interpolator::NONE:
	case GLE::Interpolator::RBF_ML_ON_DEM:
	case GLE::Interpolator::INVERSE_WEIGHTED_DISTANCE:
	case GLE::Interpolator::IDW_ON_CONTROLS: {
		// check if controls are within the bounds
		for (unsigned int i = 0; i < param.number_inputs; i++) {
			if (vals[i] < param.controlLowerBound
					|| vals[i] > param.controlUpperBound) {
				count_invalid++;
			}
		}
		break;
	}
	}
	return count_invalid;
}

void Mask::interpolateControlsWithRBFML(const double params[], int out[]) {
	// rescale params to fEC = 2 and add params to rescaledParams
	for (unsigned int i = 0; i < param.number_inputs; i++) {
		(*rescaledParams)(i, 2) = params[i]
				* (double) param.fixedElevationChangeFromOpt
				/ (double) param.fixedElevationChange;
	}
	// rbfsetpoints(params)
	alglib::rbfsetpoints(*rbfModel, *rescaledParams, param.number_inputs);
	// rbfbuildmodel
	alglib::rbfbuildmodel(*rbfModel, *reporter);
	if (reporter->terminationtype != 1) {
		std::stringstream msg;
		msg << "RBF-ML failed with alglib::reporter termination type "
				<< reporter->terminationtype << ".";
		LogFileProducer::Instance()->log(msg.str());
	}
	// rbfgridcalc2
	alglib::real_2d_array result;
	result.setlength(rowsOfPointToEstimate->length(),
			colsOfPointToEstimate->length());
	alglib::rbfgridcalc2(*rbfModel, *rowsOfPointToEstimate,
			param.number_rows - 2, *colsOfPointToEstimate,
			param.number_cols - 2, result);
	// roundresult
	unsigned int idx_out = 0;
	double temp;
	for (unsigned int row = 0; row < param.number_rows - 2; row++) {
		for (unsigned int col = 0; col < param.number_cols - 2; col++) {
			temp = result(row, col);
			out[idx_out] = (temp > 0.0) ? floor(temp + 0.5) : ceil(temp - 0.5);
			idx_out++;
		}
	}
}

void Mask::interpolateControlsWithIDW(const double vals[], int out[]) {
	unsigned int row = 1;
	unsigned int col = 0;
	double outTemp;
	double weight;
	double sumWeight;
	for (unsigned int idx_controls = 0;
			idx_controls < (param.number_rows - 2) * (param.number_cols - 2);
			idx_controls++) {
		sumWeight = 0.0;
		outTemp = 0.0;
		col++;
		// update the row idx if the row is finished
		if (col > param.number_cols - 2) {
			col = 1;
			row++;
		}
		for (unsigned int idx_input = 0; idx_input < (param.number_inputs + 4);
				idx_input++) {
			// if this point (row, col) is the same as one of the input, don't approximate
			if (input_helper->getElement(idx_input, 0) == row
					&& input_helper->getElement(idx_input, 1) == col) {
				outTemp = vals[idx_input];
				sumWeight = 1.0;
				break;
			}
			weight = weightingFunctionIDW(row, col,
					input_helper->getElement(idx_input, 0),
					input_helper->getElement(idx_input, 1));
			sumWeight += weight;
			outTemp += weight * vals[idx_input];
		}
		outTemp /= sumWeight; //sum_i of weight_i * el_i / sum_i weight_i
		// to gain advantage of more smoothness, outTemp which is
		// lBound < min(vals) <= outTemp <= max(vals) < uBound (with fEC=x)
		// becomes
		// lBound*fECfromOpt/fEC <= outTemp <= uBound*fECfromOpt/fEC
		// lBound is zero
		outTemp = outTemp * (double) param.fixedElevationChangeFromOpt
				/ (double) param.fixedElevationChange;
		out[idx_controls] =
				(outTemp > 0.0) ? floor(outTemp + 0.5) : ceil(outTemp - 0.5);
	}
}

void Mask::interpolateControlsWithIDW_NoWindow(const double vals[], int out[]) {
	unsigned int row = 1;
	unsigned int col = 0;
	double outTemp;
	double weight;
	double sumWeight;
	for (unsigned int idx_controls = 0;
			idx_controls < (param.number_rows - 2) * (param.number_cols - 2);
			idx_controls++) {
		sumWeight = 0.0;
		outTemp = 0.0;
		col++;
		// update the row idx if the row is finished
		if (col > param.number_cols - 2) {
			col = 1;
			row++;
		}
		for (unsigned int idx_input = 0; idx_input < param.number_inputs;
				idx_input++) {
			// if this point (row, col) is the same as one of the input, don't approximate
			if (input_helper->getElement(idx_input, 0) == row
					&& input_helper->getElement(idx_input, 1) == col) {
				outTemp = vals[idx_input];
				sumWeight = 1.0;
				break;
			}
			weight = weightingFunctionIDW_NoWindow(row, col,
					input_helper->getElement(idx_input, 0),
					input_helper->getElement(idx_input, 1));
			sumWeight += weight;
			outTemp += weight * vals[idx_input];
		}
		outTemp /= sumWeight; //sum_i of weight_i * el_i / sum_i weight_i
		// to gain advantage of more smoothness, outTemp which is
		// lBound < min(vals) <= outTemp <= max(vals) < uBound (with fEC=x)
		// becomes
		// lBound*fECfromOpt/fEC <= outTemp <= uBound*fECfromOpt/fEC
		// lBound is zero
		outTemp = outTemp * (double) param.fixedElevationChangeFromOpt
				/ (double) param.fixedElevationChange;
		out[idx_controls] =
				(outTemp > 0.0) ? floor(outTemp + 0.5) : ceil(outTemp - 0.5);
	}
}

double Mask::weightingFunctionIDW(int row1, int col1, int row2, int col2) {
	unsigned int deltaRow = abs(row1 - row2);
	unsigned int deltaCol = abs(col1 - col2);
	if (deltaRow < param.neighborhood_size) {
		if (deltaCol < param.neighborhood_size) {
			double weight;
			if (param.weighting_exponent == 2.0) {
				weight = 1
						/ ((double) ((deltaRow * deltaRow) + (deltaCol * deltaCol)));
			} else {
			weight =  1
					/ pow(sqrt(pow((double) deltaRow, 2) + pow((double) deltaCol, 2)),
							param.weighting_exponent);
			}
			return weight;
		}
	}
	return 0.0;
}

double Mask::weightingFunctionIDW_NoWindow(int row1, int col1, int row2, int col2) {
	unsigned int deltaRow = abs(row1 - row2);
	unsigned int deltaCol = abs(col1 - col2);
	double weight;
	if (param.weighting_exponent == 2.0) {
		weight = 1 / ((double) ((deltaRow * deltaRow) + (deltaCol * deltaCol)));
	} else {
		weight = 1 / pow( sqrt( pow((double) deltaRow, 2) + pow((double) deltaCol, 2)),
						param.weighting_exponent);
	}
	return weight;
}
