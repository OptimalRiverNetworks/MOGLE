#include <moeaframework.h>
#include <LandscapeModel.h>
#include <LandscapeModelEnums.h>
#include <Utilities/LogFileProducer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <vector>
#include <cmath>

static const int REPORTING_FREQUENCY = 1000; // reports/NFE

/* expects a txt file with parameters as first argument and an optional identier as second */
int main(int argc, char* argv[]) {
	// initialize logfile
	if (argc < 2) {
		LogFileProducer::Instance();
		LogFileProducer::Instance()->log(std::string("Too few arguments"));
		LogFileProducer::Instance()->closeLogFile();
		delete LogFileProducer::Instance();
		return EXIT_FAILURE;
	}
	if (argc > 2) {
		LogFileProducer::Instance(std::string(argv[2]));
	} else {
		LogFileProducer::Instance();
	}
	// init model
	LandscapeModel model;
	model.initializeModel(argv[1]);
	// init vars for the moea framework
	unsigned int nvars = model.getNumberOfControls();
	unsigned int nobjs = model.getNumberOfObj();
	unsigned int nconstr = model.getNumberOfConstrain();
	double vars[nvars];
	double constr[nconstr];
	double objs[nobjs];

	int result;
	unsigned long int tryCount = 0;
	unsigned long int wrongMass = 0;
	unsigned long int invalidCount = 0;
#ifdef INPUT_DEBUG
	std::vector<std::vector<double> > inputs;
	int c = 0;
#endif
	// wait for the optimal controls ;{D
	MOEA_Init(nobjs, nconstr);
	while (MOEA_Next_solution() == MOEA_SUCCESS) {
		MOEA_Read_doubles(nvars, vars);
#ifdef INPUT_DEBUG
		inputs.push_back(std::vector<double>(nvars));
		for (unsigned int i = 0; i < nvars; i++) {
			inputs[c][i] = vars[i];
		}
		c++;
#endif
		result = model.nextDEM(vars, objs, constr);
		// update counters
		if (result == 1) {
			wrongMass++;
		} else if (result == -1) {
			invalidCount++;
		}
		tryCount++;
		// reports if needed
		if (tryCount % REPORTING_FREQUENCY == 0) {
			std::stringstream msg;
			msg << tryCount << " evaluated; " << invalidCount
					<< " wrong controls; " << wrongMass
					<< " violate mass constrain.";
			LogFileProducer::Instance()->log(msg.str());
		}
		// write results to the MOEA framework
		MOEA_Write(objs, constr);
	}
	// closing
	MOEA_Terminate();
#ifdef INPUT_DEBUG
	std::stringstream msg1;
	for (int i = 0; i < c; i++) {
		msg1 << "Control #" << i << std::endl;
		for (unsigned int j = 0; j < nvars; j++) {
			msg1 << inputs[i][j] << " ";
		}
		msg1 << std::endl;
	}
	LogFileProducer::Instance()->log(msg1.str());
#endif
	std::stringstream msg2;
	msg2 << tryCount << " evaluated; " << invalidCount << " wrong controls; "
			<< wrongMass << " violate mass constrain.";
	LogFileProducer::Instance()->log(msg2.str());
	std::stringstream msg3;
	msg3 << "Feasible controls: " << tryCount - invalidCount - wrongMass
			<< " and feasibility: "
			<< ((tryCount - invalidCount - wrongMass) / tryCount * 100) << "%. "
			<< LogFileProducer::Instance()->getTimeElapsed()
			<< " time elapsed. ";
	LogFileProducer::Instance()->log(msg3.str());
	LogFileProducer::Instance()->closeLogFile();
	delete LogFileProducer::Instance();
	return EXIT_SUCCESS;
}

