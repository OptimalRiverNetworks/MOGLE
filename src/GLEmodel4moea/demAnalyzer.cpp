/*
 * demAnalyzer.cpp
 *
 *  Created on: 24/gen/2013
 *      Author: lordmzn
 */
#include <LandscapeModel.h>
#include <LandscapeModelEnums.h>
#include <Utilities/LogFileProducer.h>
#include <Utilities/TXTFileParser.h>
#include <sstream>
#include <vector>

bool readInputs(std::vector<double>& inputs, char* file) {
	TXTFileParser reader(" ", GLE::MATRIX_LINE);
	if (!reader.parseByLine(std::string(file))) {
		return false;
	}
	std::vector<std::string> tokens = reader.getTokens();
	if (tokens.size() != inputs.size()) {
		return false;
	}
	std::vector<double>::iterator it_int = inputs.begin();
	for (std::vector<std::string>::iterator it_str = tokens.begin();
			it_str != tokens.end(); ++it_str) {
		std::stringstream(*it_str) >> *it_int;
		++it_int;
	}
	return true;
}

/* expects a txt file with parameters as first argument */
int main(int argc, char* argv[]) {
	// initialize logfile
	LogFileProducer::Instance();
	if (argc < 3) {
		LogFileProducer::Instance()->log(std::string("Too few arguments"));
		LogFileProducer::Instance()->closeLogFile();
		delete LogFileProducer::Instance();
		return -1;
	}
	//init model
	LandscapeModel model;
	model.initializeModel(argv[1]);
	// init vars nextDEM
	unsigned int nvars = model.getNumberOfControls();
	unsigned int nobjs = model.getNumberOfObj();
	unsigned int nconstr = model.getNumberOfConstrain();
	double vars[nvars];
	double constr[nconstr];
	double objs[nobjs];
	std::vector<double> formattedInputs(nvars);

	if (!(readInputs(formattedInputs, argv[2]))) {
		LogFileProducer::Instance()->log(std::string("Failed to parse input"));
		return -1;
	}
	for (unsigned int i=0; i<nvars; i++){
		vars[i]=formattedInputs[i];
	}

	model.nextDEM(vars, objs, constr);

	// saving
	GLE::ObjectData::Types o[6];
	o[0] = GLE::ObjectData::DEM_T;
	o[1] = GLE::ObjectData::DRAINED_AREA;
	o[2] = GLE::ObjectData::FLOW_DIRECTION;
	o[3] = GLE::ObjectData::FLOW_LENGTH;
	o[4] = GLE::ObjectData::SLOPE;
	o[5] = GLE::ObjectData::DISCHARGE;
	if (model.saveDataToFile(o, 6, GLE::LAST_OPT)) {
		LogFileProducer::Instance()->log(
				std::string("evolvedDEM and its hydronet saved successfully"));
	}

	// closing
	LogFileProducer::Instance()->closeLogFile();
	delete LogFileProducer::Instance();
	return 0;
}

