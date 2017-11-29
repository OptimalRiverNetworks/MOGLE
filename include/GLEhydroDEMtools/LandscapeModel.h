#include "LandscapeModelEnums.h"
#include "DEMParam.h"
#include "DEM.h"
#include "Mask.h"
#include "Stepcost.h"
#include <vector>
#include <map>

/* forward declaration */
class LogFileProducer;
class TXTFileWriter;

/**
 * @class LandscapeModel
 * @brief Represent a step in the evolution of a DEM. Contains the state
 * at x_0, named baseDEM and its stepcosts in objOfBaseDEM. Using nextDEM()
 * can be populated with a new state x_1, obtained by applying the control
 * mask. The new DEM (attemptedDEM) has stepcosts in objOfAttemptedDEM just
 * like the baseDEM. A new call of nextDEM() will overwrite the previous
 * attempt.
 */
class LandscapeModel {
public:
	/* empty constructor: does nothing but initializing fields to zero
	 * and opening the logfile// bad initializer - fields are meaningless
	 * (TODO inner class to contain the stuff?)*/
	LandscapeModel();
	~LandscapeModel();
	/**
	 * Initialize a new Model instance. Requires a file source for its
	 * parameters. Initialize the lists that will contain Control/
	 * State/Stepcosts and DEMparam to the given parameters which are
	 * supposed to be constant throughout the simulation.
	 */
	bool initializeModel(char* parameterFilename);
	/**
	 * Takes the controlValues which is an array of int, check the mass
	 * constraint and return false if it's not verified, else it creates a
	 * new Mask object, initialize it with these values and adds it
	 * to Masks vector. Then calls stateTransition on the latest DEM with that
	 * mask to create a new DEM based on it and adds it to the dem list. Calls
	 * on the new dem the evaluateHydroNet() and then create the required
	 * stepcost, adds them to the right list and returns a copy of the
	 * value of each.
	 * @param vector of controls
	 * @param array into which it writes the stepcost values
	 * @param array into which it writes the value of the constrain violation
	 * @return 1 if the new DEM isn't created because the mass constrain
	 * isn't verified, -1 if the new DEM isn't created because the controls
	 * aren't correct; 0 if the new DEM is correctly created and evaluated.
	 */
	int nextDEM(double controls[], double stepcosts[], double constr[]);

	/**
	 *  Use a TXTFileWriter to save the objs in a file.
	 */
	bool saveDataToFile(GLE::ObjectData::Types obj[], int nobj,
			GLE::SaveDataOption opt);
	DEMDimensionType getDEMDimension() const;
	unsigned int getNumberOfControls() const;
	unsigned int getNumberOfObj() const;
	unsigned int getNumberOfConstrain() const;
	CoupleOfInt getControlBounds() const;

#ifdef DEBUG
public:
#else
private:
#endif
	void addNewStepcosts(double stepcostsValues[]);
	bool saveDataToFileService(GLE::ObjectData::Types obj, TXTFileWriter& file,
			std::string &filenamePrefix, DEM& demToSave);
	bool checkHydroNet(DEM&) const;
	/**
	 * Check the constancy of the mass of the terrain modeled. Check if the
	 * control that will be applied to the DEM varies its mass more than a
	 * fixed threshold, defined as massTolerance in the parameters.
	 * @param the values of the control that is going to be applied.
	 * @return 0 if the mass is going to change less than the threshold, otherwise
	 * return the amount of change above the threshold (weighted by the mass).
	 */
	double checkMassConstrainOnControls(std::vector<int>&);

	double checkMassConstrainOnDEM() const;

	DEMParam params;
	DEM* baseDEM;
	// container of pointer in order to let the compiler correctly use polymorphism
	std::map<GLE::Stepcost::Types, Stepcost*> objOfBaseDEM;
	DEM* attemptedDEM;
	Mask* mask;
	std::map<GLE::Stepcost::Types, Stepcost*> objOfAttemptedDEM;
};
