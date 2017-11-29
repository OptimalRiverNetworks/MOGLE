/*
 * LandscapeModel.cpp
 *
 *  Created on: 27/ago/2012
 *      Author: lordmzn
 */
#include "LandscapeModel.h"
#include "DEMParam.h"
#include "Mask.h"
#include "TEE.h"
#include "EEE.h"
#include "EE.h"
#include "TSP.h"
#include "EEL.h"
#include "SH.h"
#include "Utilities/TXTFileParser.h"
#include "Utilities/LogFileProducer.h"
#include <yaml-cpp/yaml.h>
#include <cstddef>
#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>

LandscapeModel::LandscapeModel() :
		params() {
	baseDEM = NULL;
	mask = NULL;
	attemptedDEM = NULL;
}

LandscapeModel::~LandscapeModel() {
	for (unsigned int obj_idx = 0; obj_idx < params.typeOfObjectives.size();
			obj_idx++) {
		delete objOfBaseDEM[params.typeOfObjectives[obj_idx]];
		delete objOfAttemptedDEM[params.typeOfObjectives[obj_idx]];
	}
	delete baseDEM;
	delete attemptedDEM;
	delete mask;
}

bool LandscapeModel::initializeModel(char* filename) {
	// read parameters file
	params = YAML::LoadFile(filename).as<DEMParam>();
	if (!params.isValid()) {
		LogFileProducer::Instance()->log(
				std::string("Fail to load parameter - aborting."));
		return false;
	}
	LogFileProducer::Instance()->log(std::string("Parameters loaded."));
	LogFileProducer::Instance()->log(params.toString());
	// create the firstDEM
	baseDEM = new DEM(&params);
	// evaluate it
	std::stringstream msg;
	baseDEM->evaluateHydroNet();
	for (unsigned int obj_idx = 0; obj_idx < params.typeOfObjectives.size();
			obj_idx++) {
		switch (params.typeOfObjectives[obj_idx]) {
		case (GLE::Stepcost::TEE_COST): {
			objOfBaseDEM[GLE::Stepcost::TEE_COST] = new TEE(*baseDEM);
			msg << "TEE = "
					<< objOfBaseDEM[GLE::Stepcost::TEE_COST]->getStepcost()
					<< ", "; // just to evaluate it
			break;
		}
		case (GLE::Stepcost::EEE_COST): {
			objOfBaseDEM[GLE::Stepcost::EEE_COST] = new EEE(*baseDEM);
			msg << "EEE = "
					<< objOfBaseDEM[GLE::Stepcost::EEE_COST]->getStepcost()
					<< ", "; // just to evaluate it
			break;
		}
		case (GLE::Stepcost::EE_COST): {
			objOfBaseDEM[GLE::Stepcost::EE_COST] = new EE(*baseDEM);
			msg << "EE = "
					<< objOfBaseDEM[GLE::Stepcost::EE_COST]->getStepcost()
					<< ", "; // just to evaluate it
			break;
		}
		case (GLE::Stepcost::TSP_COST): {
			objOfBaseDEM[GLE::Stepcost::TSP_COST] = new TSP(*baseDEM);
			msg << "TSP = "
					<< objOfBaseDEM[GLE::Stepcost::TSP_COST]->getStepcost()
					<< ", "; // just to evaluate it
			break;
		}
		case (GLE::Stepcost::EEL_COST): {
			objOfBaseDEM[GLE::Stepcost::EEL_COST] = new EEL(*baseDEM);
			msg << "EEL = "
					<< objOfBaseDEM[GLE::Stepcost::EEL_COST]->getStepcost()
					<< ", "; // just to evaluate it
			break;
		}
		case (GLE::Stepcost::SH_COST): {
			objOfBaseDEM[GLE::Stepcost::SH_COST] = new SH(*baseDEM);
			msg << "SH = "
					<< objOfBaseDEM[GLE::Stepcost::SH_COST]->getStepcost()
					<< ", "; // just to evaluate it
			break;
		}
		}
	}
	LogFileProducer::Instance()->log(std::string("Base DEM: ") + msg.str());
	return true;
}

int LandscapeModel::nextDEM(double controlValues[], double stepcostsValues[],
		double constr[]) {
//	// check the mass constrain
//	constr[0] = checkMassConstrainOnControls(controlValues);
//	if (constr[0] != 0.0) {
//#ifdef DEBUG
//		std::stringstream msg;
//		msg << "Control refused because of mass conservation: " << constr[0];
//		LogFileProducer::Instance()->log(msg.str());
//#endif
//		return 1;
//	}

	// set the values in the mask
	delete mask;
	mask = new Mask(params);
	int validity = mask->setControls(controlValues);
	if (validity != 0) {
		std::stringstream msg;
		msg << validity << " controls are incorrect, the DEM isn't created.";
		LogFileProducer::Instance()->log(msg.str());
		for (unsigned int i = 0; i < params.typeOfObjectives.size(); i++) {
			stepcostsValues[i] = 0.0;
		}
		return -1;
	}
	// create the new DEM and evaluate it
	delete attemptedDEM;
	attemptedDEM = new DEM(*baseDEM, *mask);

	// mass check on the new dem
	constr[0] = checkMassConstrainOnDEM();
	if (constr[0] != 0.0) {
#ifdef DEBUG
		std::stringstream msg;
		msg << "Control refused because of mass conservation: " << constr[0];
		LogFileProducer::Instance()->log(msg.str());
#endif
		return 1;
	}

	attemptedDEM->evaluateHydroNet();
	// call the helper function to create the new stepcosts
	addNewStepcosts(stepcostsValues);
#ifdef DEBUG
	LogFileProducer::Instance()->log(
			std::string("New DEM added and evaluated."));
	std::stringstream msg;
	for (unsigned int i = 0; i < params.typeOfObjectives.size(); i++) {
		msg << GLE::toStr(params.typeOfObjectives[i]) << ": "
				<< stepcostsValues[i] << "; ";
	}
	msg << "Mass conservation: " << constr[0];
	LogFileProducer::Instance()->log(msg.str());
#endif
	return 0;
}

double LandscapeModel::checkMassConstrainOnDEM() const {
	if (params.massConstrainEnabled == true) {
		double massVariation = 0.0;
		massVariation = baseDEM->getMass() - attemptedDEM->getMass();
		massVariation = massVariation / baseDEM->getMass();
		if (massVariation < 0) { // fancy absolute value
			massVariation = massVariation * -1.0;
		}
		massVariation -= params.massTolerance;
		if (massVariation < 0.0) {
			return 0.0;
		}
		return massVariation;
	}
	return 0.0;
}

double LandscapeModel::checkMassConstrainOnControls(std::vector<int>& vals) {
// TODO vals should be in meters!!!
	if (params.massConstrainEnabled == true) {
		double massVariation = 0.0;
		for (std::vector<int>::iterator it = vals.begin(); it != vals.end();
				++it) {
			massVariation += (double) *it; // meters
		}
		if (massVariation < 0) { // fancy absolute value
			massVariation = massVariation * -1.0; // = |sum_{xy} delta_{xy}| [meters]
		}
		massVariation = (massVariation / baseDEM->getMass())
				- params.massTolerance;
		if (massVariation < 0.0) {
			return 0.0;
		}
		return massVariation;
	}
	return 0.0;
}

void LandscapeModel::addNewStepcosts(double stepcostsValues[]) {
	for (unsigned int i = 0; i < params.typeOfObjectives.size(); i++) {
		switch (params.typeOfObjectives[i]) {
		case (GLE::Stepcost::TEE_COST): {
			delete objOfAttemptedDEM[GLE::Stepcost::TEE_COST];
			objOfAttemptedDEM[GLE::Stepcost::TEE_COST] = new TEE(*attemptedDEM);
			stepcostsValues[i] =
					objOfAttemptedDEM[GLE::Stepcost::TEE_COST]->getStepcost();
			break;
		}
		case (GLE::Stepcost::EEE_COST): {
			delete objOfAttemptedDEM[GLE::Stepcost::EEE_COST];
			objOfAttemptedDEM[GLE::Stepcost::EEE_COST] = new EEE(*attemptedDEM);
			stepcostsValues[i] =
					objOfAttemptedDEM[GLE::Stepcost::EEE_COST]->getStepcost();
			break;
		}
		case (GLE::Stepcost::EE_COST): {
			delete objOfAttemptedDEM[GLE::Stepcost::EE_COST];
			objOfAttemptedDEM[GLE::Stepcost::EE_COST] = new EE(*attemptedDEM);
			stepcostsValues[i] =
					objOfAttemptedDEM[GLE::Stepcost::EE_COST]->getStepcost();
			break;
		}
		case (GLE::Stepcost::TSP_COST): {
			delete objOfAttemptedDEM[GLE::Stepcost::TSP_COST];
			objOfAttemptedDEM[GLE::Stepcost::TSP_COST] = new TSP(*attemptedDEM);
			stepcostsValues[i] =
					objOfAttemptedDEM[GLE::Stepcost::TSP_COST]->getStepcost();
			break;
		}
		case (GLE::Stepcost::EEL_COST): {
			delete objOfAttemptedDEM[GLE::Stepcost::EEL_COST];
			objOfAttemptedDEM[GLE::Stepcost::EEL_COST] = new EEL(*attemptedDEM);
			stepcostsValues[i] =
					objOfAttemptedDEM[GLE::Stepcost::EEL_COST]->getStepcost();
			break;
		}
		case (GLE::Stepcost::SH_COST): {
			delete objOfAttemptedDEM[GLE::Stepcost::SH_COST];
			objOfAttemptedDEM[GLE::Stepcost::SH_COST] = new SH(*attemptedDEM);
			stepcostsValues[i] =
					objOfAttemptedDEM[GLE::Stepcost::SH_COST]->getStepcost();
			break;
		}
		}
	}
}

DEMDimensionType LandscapeModel::getDEMDimension() const {
	return DEMDimensionType(params.number_cols, params.number_rows);
}

unsigned int LandscapeModel::getNumberOfControls() const {
	return params.number_inputs;
}

unsigned int LandscapeModel::getNumberOfObj() const {
	return params.typeOfObjectives.size();
}

unsigned int LandscapeModel::getNumberOfConstrain() const {
	return 1; // TODO very clever implementation here
}

CoupleOfInt LandscapeModel::getControlBounds() const {
	CoupleOfInt a;
	a.x = params.controlUpperBound;
	a.y = params.controlLowerBound;
	return a;
}

bool LandscapeModel::saveDataToFile(GLE::ObjectData::Types obj[], int nobj,
		GLE::SaveDataOption opt) {
	TXTFileWriter file;
	std::string filenamePrefix;
	bool isSuccessful = false;
	switch (opt) {
	case GLE::BASE_OPT: {
		LogFileProducer::Instance()->log(
				std::string("BASE_OPTION chosen: target DEM is base DEM."));
		filenamePrefix = "base_";
		for (int i = 0; i < nobj; i++) {
			isSuccessful = (saveDataToFileService(obj[i], file, filenamePrefix,
					*baseDEM) && isSuccessful);
		}
		break;
	}
	case GLE::LAST_OPT: {
		LogFileProducer::Instance()->log(
				std::string(
						"LAST_OPTION chosen: target DEM is last attempted DEM."));
		filenamePrefix = "last_";
		for (int i = 0; i < nobj; i++) {
			isSuccessful = (saveDataToFileService(obj[i], file, filenamePrefix,
					*attemptedDEM) && isSuccessful);
		}
		break;
	}
	}
	return isSuccessful;
}

bool LandscapeModel::saveDataToFileService(GLE::ObjectData::Types obj,
		TXTFileWriter& file, std::string& filenamePrefix, DEM& target) {
	bool isSuccessful = false;
	std::string filename;
	switch (obj) {
	case GLE::ObjectData::DEM_T: {
		filename = filenamePrefix + GLE::toStr(GLE::ObjectData::DEM_T)
				+ std::string(".data");
		LogFileProducer::Instance()->log(
				std::string("Saving chosen DEM to ") + filename);
		if (file.openFile(filename)) {
			file.writeMatrix<int>(target.getElevationData());
			file.closeFile();
			isSuccessful = true;
		}
		break;
	}
	case GLE::ObjectData::DRAINED_AREA: {
		if (checkHydroNet(target)) {
			filename = filenamePrefix
					+ GLE::toStr(GLE::ObjectData::DRAINED_AREA)
					+ std::string(".data");
			LogFileProducer::Instance()->log(
					std::string("Saving drainage area to ") + filename);
			if (file.openFile(filename)) {
				file.writeMatrix<unsigned int>(target.getDrainedArea());
				file.closeFile();
				isSuccessful = true;
			}
		} else {
			LogFileProducer::Instance()->log(
					std::string("HydroNet isn't evaluated for ")
							+ filenamePrefix + std::string(" DEM."));
		}
		break;
	}
	case GLE::ObjectData::FLOW_DIRECTION: {
		if (checkHydroNet(target)) {
			filename = filenamePrefix
					+ GLE::toStr(GLE::ObjectData::FLOW_DIRECTION)
					+ std::string("_x.data");
			LogFileProducer::Instance()->log(
					std::string("Saving flow directions of baseDEM to ")
							+ filename);
			if (file.openFile(filename)) {
				file.writeMatrix(target.getStx());
				file.closeFile();
				isSuccessful = true;
			}
			filename = filenamePrefix
					+ GLE::toStr(GLE::ObjectData::FLOW_DIRECTION)
					+ std::string("_y.data");
			LogFileProducer::Instance()->log(
					std::string("Saving flow directions of baseDEM to ")
							+ filename);
			if (file.openFile(filename)) {
				file.writeMatrix(target.getSty());
				file.closeFile();
				isSuccessful = true;
			}
		} else {
			LogFileProducer::Instance()->log(
					std::string("HydroNet isn't evaluated for ")
							+ filenamePrefix + std::string(" DEM."));
		}
		break;
	}
	case GLE::ObjectData::FLOW_LENGTH: {
		if (checkHydroNet(target)) {
			filename = filenamePrefix + GLE::toStr(GLE::ObjectData::FLOW_LENGTH)
					+ std::string(".data");
			LogFileProducer::Instance()->log(
					std::string("Saving flow length of baseDEM to ")
							+ filename);
			if (file.openFile(filename)) {
				file.writeMatrix(target.getFlowLength());
				file.closeFile();
				isSuccessful = true;
			}
		} else {
			LogFileProducer::Instance()->log(
					std::string("HydroNet isn't evaluated for ")
							+ filenamePrefix + std::string(" DEM."));
		}
		break;
	}
	case GLE::ObjectData::SLOPE: {
		if (checkHydroNet(target)) {
			filename = filenamePrefix + GLE::toStr(GLE::ObjectData::SLOPE)
					+ std::string(".data");
			LogFileProducer::Instance()->log(
					std::string("Saving slope of baseDEM to ") + filename);
			if (file.openFile(filename)) {
				file.writeMatrix(target.getSlope());
				file.closeFile();
				isSuccessful = true;
			}
		} else {
			LogFileProducer::Instance()->log(
					std::string("HydroNet isn't evaluated for ")
							+ filenamePrefix + std::string(" DEM."));
		}
		break;
	}
	case GLE::ObjectData::DISCHARGE: {
		if (checkHydroNet(target)) {
			filename = filenamePrefix + GLE::toStr(GLE::ObjectData::DISCHARGE)
					+ std::string(".data");
			LogFileProducer::Instance()->log(
					std::string("Saving discharge of baseDEM to ") + filename);
			if (file.openFile(filename)) {
				file.writeMatrix(target.getDischarge());
				file.closeFile();
				isSuccessful = true;
			}
		} else {
			LogFileProducer::Instance()->log(
					std::string("HydroNet isn't evaluated for ")
							+ filenamePrefix + std::string(" DEM."));
		}
		break;
	}
	case GLE::ObjectData::CONTROL: {
		if (attemptedDEM == &target) {
			filename = filenamePrefix + GLE::toStr(GLE::ObjectData::CONTROL)
					+ std::string(".data");
			LogFileProducer::Instance()->log(
					std::string("Saving control applied to baseDEM to ")
							+ filename);
			if (file.openFile(filename)) {
				file.writeMatrix(mask->getControls());
				file.closeFile();
				isSuccessful = true;
			}
			break;
		} else {
			LogFileProducer::Instance()->log(
					std::string(
							"Cannot save the control that created the baseDEM!"));
		}
		break;
	}
	}
	return isSuccessful;
}

bool LandscapeModel::checkHydroNet(DEM& d) const {
	return d.isHydroNetEvaluated();
}
