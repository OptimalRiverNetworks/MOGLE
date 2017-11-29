/*
 * DEMParam.h
 *
 *  Created on: 03/gen/2013
 *      Author: lordmzn
 */

#ifndef DEMPARAM_H_
#define DEMPARAM_H_

#include "LandscapeModelEnums.h"
#include <yaml-cpp/yaml.h>
#include <vector>
#include <string>

/**
 * should be made by const field, but initialization is then a problem if
 * a not valid value is passed
 */
class DEMParam {
public:
	// delta applied to the dem
	int controlUpperBound; /**< upper bound for the control values*/
	int controlLowerBound; /**< lower bound for the control values */
	int fixedElevationChange; /**< fixed delta elevation applied by the control = resolution of control */
	int fixedElevationChangeFromOpt; /**< fixed delta elevation applied by the control = resolution of control */
	// control interpolator
	GLE::Interpolator::Types interpolatorType;
	unsigned int number_inputs;
	// if IDW or RBF is chosen
	unsigned int sample_frequence;
	// if RBF is chosen
	unsigned int D_RBF; /**< dimension of each function, normally 2D (lat/long) */
	// if IDW is chosen
	unsigned int neighborhood_size;
	double weighting_exponent;
	// state (DEM) related parameters
	unsigned int number_rows; /**< number of rows in the DEM */
	unsigned int number_cols; /**< number of columns in the DEM */
	// DEM cell parameters
	unsigned int areaOfDEMCell; /**< Area of each cell in km^2 (int cause drained area is int) */
	double lengthOfDEMCell; /**< Length of cell in meter */
	double widthOfDEMCell; /**< Width of cell in meter */
	// initial state (DEM) parameters
	GLE::InputData::Types in; /**< if initial DEM is a generated with one of the provided algorithm or loaded by file. */
	std::string DEMfilename; /**< name of the file from which the initial DEM should be loaded */
	unsigned int basinLength; /**< length of main basin in the WEDGE initial state */
	unsigned int basinWidth; /**< width of main basin in the WEDGE initial state evaluated from the shapeFactor */
	double shapeFactor; /**< width of main basin divided by its length (area of basin divided by square of main channel length) */
	// hydrological parameters
	double runoffCoeff; /**< % of rainfall that becomes flow (0~1) */
	double rainfallExcess; /**< Rainfall excess in mm/hr (0.1mm/hr=876mm/yr) */
	// objectives
	std::vector<GLE::Stepcost::Types> typeOfObjectives; /**< objectives to be evaluated on DEM */
	unsigned int adThreshold; /**< threshold to be applied to Ad for objectives evaluation */
	double dischargeThreshold; /**< same as above in terms of discharge */
	// mass constrain parameters
	bool massConstrainEnabled; /**< as the name says */
	double massTolerance; /**< % of total mass of initDEM allowed to change */
	/**
	 * Empty constructor: initialize each field at its default value
	 */
	DEMParam();
	/**
	 * Check feasibility constrain of the parameters. If a parameter is out of
	 * range, it corrects it.
	 */
	bool isValid();
	std::string toString();

private:
	/**
	 * Check parameter validity and return 0 if they're ok. If they're not valid
	 * try to correct it: if it's possible, return 1 else -1.
	 */
	int areDEMInputSourceAndNumberOfRowsAndColumnsValid();
	int isDEMCellDimensionValid();
	int isRunoffCoeffValid();
	int isRainfallExcessValid();
	int isFixedElevationChangeValid();
	int isMassVariationToleranceValid();
	int areControlBoundsValid();
	int areInterpolatorParametersValid();
	int isAdThresholdValid();
};

// yaml-cpp miss the convert<bool> so...
struct YesNoField {
	bool value;
	YesNoField() :
			value(false) {
	}
	YesNoField(bool a) :
			value(a) {
	}
	inline bool toBool() const {
		return value;
	}
};

namespace YAML {
template<>
struct convert<YesNoField> {
	static Node encode(const YesNoField& rhs) {
		return rhs.value ? Node("true") : Node("false");
	}
	static bool decode(const Node& node, YesNoField& rhs) {
		if (node.IsNull()) {
			return false;
		}
		if (node.as<std::string>().compare("Yes") == 0
				|| node.as<std::string>().compare("yes") == 0) {
			rhs.value = true;
		} else if (node.as<std::string>().compare("No") == 0
				|| node.as<std::string>().compare("no") == 0) {
			rhs.value = false;
		} else {
			return false;
		}
		return true;
	}
};
template<>
struct convert<DEMParam> {
	static Node encode(const DEMParam& rhs) {
		Node node;
		node[GLE::toStr(GLE::Params::CONTROLS)][GLE::toStr(GLE::Params::BOUNDS)][0] =
				rhs.controlUpperBound;
		node[GLE::toStr(GLE::Params::CONTROLS)][GLE::toStr(GLE::Params::BOUNDS)][1] =
				rhs.controlLowerBound;
		node[GLE::toStr(GLE::Params::CONTROLS)][GLE::toStr(
				GLE::Params::FIXED_EL_CHANGE)] = rhs.fixedElevationChange;
		node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(GLE::Params::DIMENSIONS)][0] =
				rhs.number_rows;
		node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(GLE::Params::DIMENSIONS)][1] =
				rhs.number_cols;
		node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(GLE::Params::CELL)][GLE::toStr(
				GLE::Params::AREA)] = rhs.areaOfDEMCell;
		node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(GLE::Params::CELL)][GLE::toStr(
				GLE::Params::LENGTH)] = rhs.lengthOfDEMCell;
		node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(GLE::Params::CELL)][GLE::toStr(
				GLE::Params::WIDTH)] = rhs.widthOfDEMCell;
		node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(GLE::Params::INITIAL)][GLE::toStr(
				GLE::Params::SOURCE_TYPE)] = GLE::toStr(rhs.in);
		node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(GLE::Params::INITIAL)][GLE::toStr(
				GLE::Params::FILENAME)] = rhs.DEMfilename;
		node[GLE::toStr(GLE::Params::HYDROLOGICAL_PARAMETERS)][GLE::toStr(
				GLE::Params::RAINFALL_EXCESS)] = rhs.rainfallExcess;
		node[GLE::toStr(GLE::Params::HYDROLOGICAL_PARAMETERS)][GLE::toStr(
				GLE::Params::RUNOFF_COEFF)] = rhs.runoffCoeff;
		for (std::vector<GLE::Stepcost::Types>::const_iterator it =
				rhs.typeOfObjectives.begin(); it != rhs.typeOfObjectives.end();
				++it) {
			node[GLE::toStr(GLE::Params::OBJECTIVES)][GLE::toStr(
					GLE::Params::OBJ_TYPES)].push_back(GLE::toStr(*it));
		}
		node[GLE::toStr(GLE::Params::OBJECTIVES)][GLE::toStr(
				GLE::Params::AD_THRESHOLD)] = rhs.adThreshold;
		node[GLE::toStr(GLE::Params::CONSTRAIN)][GLE::toStr(
				GLE::Params::MASS_CONSTRAIN_ENABLED)] = YesNoField(
				rhs.massConstrainEnabled);
		node[GLE::toStr(GLE::Params::CONSTRAIN)][GLE::toStr(
				GLE::Params::MASS_VARIATION_TOLERANCE)] = rhs.massTolerance;
		// interpolator stuff
		node[GLE::toStr(GLE::Params::INTERPOLATOR)][GLE::toStr(
				GLE::Params::INTERPOLATOR_TYPE)] = GLE::toStr(
				rhs.interpolatorType);
		switch (rhs.interpolatorType) {
		case GLE::Interpolator::NONE: {
			break;
		}
		case GLE::Interpolator::RBF_ML_ON_DEM: {
			node[GLE::toStr(GLE::Params::INTERPOLATOR)][GLE::toStr(
					GLE::Params::INTERPOLATOR_PARAMETERS)].push_back(
					rhs.sample_frequence);
			node[GLE::toStr(GLE::Params::INTERPOLATOR)][GLE::toStr(
					GLE::Params::INTERPOLATOR_PARAMETERS)].push_back(rhs.D_RBF);
			break;
		}
		case GLE::Interpolator::INVERSE_WEIGHTED_DISTANCE: {
			node[GLE::toStr(GLE::Params::INTERPOLATOR)][GLE::toStr(
					GLE::Params::INTERPOLATOR_PARAMETERS)].push_back(
					rhs.sample_frequence);
			node[GLE::toStr(GLE::Params::INTERPOLATOR)][GLE::toStr(
					GLE::Params::INTERPOLATOR_PARAMETERS)].push_back(
					rhs.neighborhood_size);
			node[GLE::toStr(GLE::Params::INTERPOLATOR)][GLE::toStr(
					GLE::Params::INTERPOLATOR_PARAMETERS)].push_back(
					rhs.weighting_exponent);
			break;
		}
		case GLE::Interpolator::IDW_ON_CONTROLS: {
			node[GLE::toStr(GLE::Params::INTERPOLATOR)][GLE::toStr(
					GLE::Params::INTERPOLATOR_PARAMETERS)].push_back(
					rhs.sample_frequence);
			node[GLE::toStr(GLE::Params::INTERPOLATOR)][GLE::toStr(
					GLE::Params::INTERPOLATOR_PARAMETERS)].push_back(
					rhs.weighting_exponent);
			break;
		}
		}
		return node;
	}
	static bool decode(const Node& node, DEMParam& rhs) {
		if (!node.IsMap()) {
			return false;
		}
		// reading controls parameters
		if (node[GLE::toStr(GLE::Params::CONTROLS)]) {
			if (node[GLE::toStr(GLE::Params::CONTROLS)][GLE::toStr(
					GLE::Params::BOUNDS)]) {
				if (!node[GLE::toStr(GLE::Params::CONTROLS)][GLE::toStr(
						GLE::Params::BOUNDS)].IsSequence()) {
					return false;
				}
				if (!node[GLE::toStr(GLE::Params::CONTROLS)][GLE::toStr(
						GLE::Params::BOUNDS)] == 2) {
					return false;
				}
				rhs.controlUpperBound =
						node[GLE::toStr(GLE::Params::CONTROLS)][GLE::toStr(
								GLE::Params::BOUNDS)][0].as<int>();
				rhs.controlLowerBound =
						node[GLE::toStr(GLE::Params::CONTROLS)][GLE::toStr(
								GLE::Params::BOUNDS)][1].as<int>();
			}
			if (node[GLE::toStr(GLE::Params::CONTROLS)][GLE::toStr(
					GLE::Params::FIXED_EL_CHANGE)]) {
				rhs.fixedElevationChange = node[GLE::toStr(
						GLE::Params::CONTROLS)][GLE::toStr(
						GLE::Params::FIXED_EL_CHANGE)].as<int>();
			}
		}
		// reading state parameters
		if (node[GLE::toStr(GLE::Params::STATE)]) {
			if (!node[GLE::toStr(GLE::Params::STATE)].IsMap()) {
				return false;
			}
			// reading dem dimensions
			if (node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(
					GLE::Params::DIMENSIONS)]) {
				if (!node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(
						GLE::Params::DIMENSIONS)].IsSequence()) {
					return false;
				}
				if (!node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(
						GLE::Params::DIMENSIONS)] == 2) {
					return false;
				}
				rhs.number_rows =
						node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(
								GLE::Params::DIMENSIONS)][0].as<int>();
				rhs.number_cols =
						node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(
								GLE::Params::DIMENSIONS)][1].as<int>();
			}
			// reading cell dimensions
			if (node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(
					GLE::Params::CELL)]) {
				if (!node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(
						GLE::Params::CELL)].IsMap()) {
					return false;
				}
				if (node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(
						GLE::Params::CELL)][GLE::toStr(GLE::Params::AREA)]) {
					rhs.areaOfDEMCell =
							node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(
									GLE::Params::CELL)][GLE::toStr(
									GLE::Params::AREA)].as<int>();
				}
				if (node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(
						GLE::Params::CELL)][GLE::toStr(GLE::Params::LENGTH)]) {
					rhs.lengthOfDEMCell =
							node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(
									GLE::Params::CELL)][GLE::toStr(
									GLE::Params::LENGTH)].as<double>();
				}
				if (node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(
						GLE::Params::CELL)][GLE::toStr(GLE::Params::WIDTH)]) {
					rhs.widthOfDEMCell =
							node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(
									GLE::Params::CELL)][GLE::toStr(
									GLE::Params::WIDTH)].as<double>();
				}
			}
			// reading initial state
			if (node[GLE::toStr(GLE::Params::STATE)]) {
				if (!node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(
						GLE::Params::INITIAL)].IsMap()) {
					return false;
				}
				if (node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(
						GLE::Params::INITIAL)][GLE::toStr(
						GLE::Params::SOURCE_TYPE)]) {
					bool typeFound = false;
					for (int in_idx = 0; in_idx < GLE::InputData::numberOfTypes;
							in_idx++) {
						if (node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(
								GLE::Params::INITIAL)][GLE::toStr(
								GLE::Params::SOURCE_TYPE)].as<std::string>().compare(
								GLE::InputData::TypesStrings[in_idx]) == 0) {
							rhs.in = (GLE::InputData::Types) in_idx;
							typeFound = true;
							break;
						}
					}
					if (!typeFound) {
						return false;
					}
				}
				if (node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(
						GLE::Params::INITIAL)][GLE::toStr(GLE::Params::FILENAME)]) {
					if (!node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(
							GLE::Params::INITIAL)][GLE::toStr(
							GLE::Params::FILENAME)].IsNull()) {
						rhs.DEMfilename =
								node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(
										GLE::Params::INITIAL)][GLE::toStr(
										GLE::Params::FILENAME)].as<std::string>();
					}
				}
				if (node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(
						GLE::Params::INITIAL)][GLE::toStr(
						GLE::Params::BASIN_LENGTH)]) {
					if (!node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(
							GLE::Params::INITIAL)][GLE::toStr(
							GLE::Params::BASIN_LENGTH)].IsNull()) {
						rhs.basinLength =
								node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(
										GLE::Params::INITIAL)][GLE::toStr(
										GLE::Params::BASIN_LENGTH)].as<int>();
					}
				}
				if (node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(
						GLE::Params::INITIAL)][GLE::toStr(
						GLE::Params::SHAPE_FACTOR)]) {
					if (!node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(
							GLE::Params::INITIAL)][GLE::toStr(
							GLE::Params::SHAPE_FACTOR)].IsNull()) {
						rhs.shapeFactor =
								node[GLE::toStr(GLE::Params::STATE)][GLE::toStr(
										GLE::Params::INITIAL)][GLE::toStr(
										GLE::Params::SHAPE_FACTOR)].as<double>();
					}
				}
			}
		}
		// reading hydrological parameters
		if (node[GLE::toStr(GLE::Params::HYDROLOGICAL_PARAMETERS)]) {
			if (!node[GLE::toStr(GLE::Params::HYDROLOGICAL_PARAMETERS)].IsMap()) {
				return false;
			}
			if (node[GLE::toStr(GLE::Params::HYDROLOGICAL_PARAMETERS)][GLE::toStr(
					GLE::Params::RAINFALL_EXCESS)]) {
				rhs.rainfallExcess = node[GLE::toStr(
						GLE::Params::HYDROLOGICAL_PARAMETERS)][GLE::toStr(
						GLE::Params::RAINFALL_EXCESS)].as<double>();
			}
			if (node[GLE::toStr(GLE::Params::HYDROLOGICAL_PARAMETERS)][GLE::toStr(
					GLE::Params::RUNOFF_COEFF)]) {
				rhs.runoffCoeff = node[GLE::toStr(
						GLE::Params::HYDROLOGICAL_PARAMETERS)][GLE::toStr(
						GLE::Params::RUNOFF_COEFF)].as<double>();
			}
		}
		// reading constrain
		if (node[GLE::toStr(GLE::Params::CONSTRAIN)]) {
			if (!node[GLE::toStr(GLE::Params::CONSTRAIN)].IsMap()) {
				return false;
			}
			if (node[GLE::toStr(GLE::Params::CONSTRAIN)][GLE::toStr(
					GLE::Params::MASS_CONSTRAIN_ENABLED)]) {
				rhs.massConstrainEnabled =
						node[GLE::toStr(GLE::Params::CONSTRAIN)][GLE::toStr(
								GLE::Params::MASS_CONSTRAIN_ENABLED)].as<
								YesNoField>().toBool();
			}
			if (node[GLE::toStr(GLE::Params::CONSTRAIN)][GLE::toStr(
					GLE::Params::MASS_VARIATION_TOLERANCE)]) {
				rhs.massTolerance =
						node[GLE::toStr(GLE::Params::CONSTRAIN)][GLE::toStr(
								GLE::Params::MASS_VARIATION_TOLERANCE)].as<
								double>();
			}
		}
		// reading objective
		if (node[GLE::toStr(GLE::Params::OBJECTIVES)]) {
			if (node[GLE::toStr(GLE::Params::OBJECTIVES)][GLE::toStr(
					GLE::Params::OBJ_TYPES)]) {
				if (!node[GLE::toStr(GLE::Params::OBJECTIVES)][GLE::toStr(
						GLE::Params::OBJ_TYPES)].IsSequence()) {
					return false;
				}
				rhs.typeOfObjectives = std::vector<GLE::Stepcost::Types>(
						node[GLE::toStr(GLE::Params::OBJECTIVES)][GLE::toStr(
								GLE::Params::OBJ_TYPES)].size());
				for (unsigned int i = 0;
						i < node[GLE::toStr(GLE::Params::OBJECTIVES)][GLE::toStr(
										GLE::Params::OBJ_TYPES)].size(); i++) {
					bool typeFound = false;
					for (int obj_idx = 0;
							obj_idx < GLE::Stepcost::numberOfTypes; obj_idx++) {
						if (node[GLE::toStr(GLE::Params::OBJECTIVES)][GLE::toStr(
								GLE::Params::OBJ_TYPES)][i].as<std::string>().compare(
								GLE::Stepcost::TypesStrings[obj_idx]) == 0) {
							rhs.typeOfObjectives[i] =
									(GLE::Stepcost::Types) obj_idx;
							typeFound = true;
							break;
						}
					}
					if (!typeFound) {
						return false;
					}
				}
			}
			if (node[GLE::toStr(GLE::Params::OBJECTIVES)][GLE::toStr(
					GLE::Params::AD_THRESHOLD)]) {
				if (!node[GLE::toStr(GLE::Params::OBJECTIVES)][GLE::toStr(
						GLE::Params::AD_THRESHOLD)].IsNull()) {
					rhs.adThreshold =
							node[GLE::toStr(GLE::Params::OBJECTIVES)][GLE::toStr(
									GLE::Params::AD_THRESHOLD)].as<unsigned int>();
				}
			}
		}
		// reading inteporlator
		if (node[GLE::toStr(GLE::Params::INTERPOLATOR)]) {
			if (!node[GLE::toStr(GLE::Params::INTERPOLATOR)].IsMap()) {
				return false;
			}
			if (node[GLE::toStr(GLE::Params::INTERPOLATOR)][GLE::toStr(
					GLE::Params::INTERPOLATOR_TYPE)]) {
				bool typeFound = false;
				for (unsigned int type_idx = 0;
						type_idx < GLE::Interpolator::numberOfTypes;
						type_idx++) {
					if (node[GLE::toStr(GLE::Params::INTERPOLATOR)][GLE::toStr(
							GLE::Params::INTERPOLATOR_TYPE)].as<std::string>().compare(
							GLE::Interpolator::TypesStrings[type_idx]) == 0) {
						rhs.interpolatorType =
								(GLE::Interpolator::Types) type_idx;
						typeFound = true;
						break;
					}
				}
				if (!typeFound) {
					return false;
				}
				switch (rhs.interpolatorType) {
				case GLE::Interpolator::NONE: {
					break;
				}
				case GLE::Interpolator::RBF_ML_ON_DEM: {
					if (!node[GLE::toStr(GLE::Params::INTERPOLATOR)][GLE::toStr(
							GLE::Params::INTERPOLATOR_PARAMETERS)]) {
						return false;
					}
					if (!node[GLE::toStr(GLE::Params::INTERPOLATOR)][GLE::toStr(
							GLE::Params::INTERPOLATOR_PARAMETERS)].IsSequence()) {
						return false;
					}
					if (!node[GLE::toStr(GLE::Params::INTERPOLATOR)][GLE::toStr(
							GLE::Params::INTERPOLATOR_PARAMETERS)] == 2) {
						return false;
					}
					rhs.sample_frequence = node[GLE::toStr(
							GLE::Params::INTERPOLATOR)][GLE::toStr(
							GLE::Params::INTERPOLATOR_PARAMETERS)][0].as<
							unsigned int>();
					rhs.D_RBF =
							node[GLE::toStr(GLE::Params::INTERPOLATOR)][GLE::toStr(
									GLE::Params::INTERPOLATOR_PARAMETERS)][1].as<
									unsigned int>();
					break;
				}
				case GLE::Interpolator::INVERSE_WEIGHTED_DISTANCE: {
					if (!node[GLE::toStr(GLE::Params::INTERPOLATOR)][GLE::toStr(
							GLE::Params::INTERPOLATOR_PARAMETERS)]) {
						return false;
					}
					if (!node[GLE::toStr(GLE::Params::INTERPOLATOR)][GLE::toStr(
							GLE::Params::INTERPOLATOR_PARAMETERS)].IsSequence()) {
						return false;
					}
					if (!node[GLE::toStr(GLE::Params::INTERPOLATOR)][GLE::toStr(
							GLE::Params::INTERPOLATOR_PARAMETERS)] == 2) {
						return false;
					}
					rhs.sample_frequence = node[GLE::toStr(
							GLE::Params::INTERPOLATOR)][GLE::toStr(
							GLE::Params::INTERPOLATOR_PARAMETERS)][0].as<
							unsigned int>();
					rhs.weighting_exponent =
							node[GLE::toStr(GLE::Params::INTERPOLATOR)][GLE::toStr(
							GLE::Params::INTERPOLATOR_PARAMETERS)][1].as<
							double>();
					break;
				}
				case GLE::Interpolator::IDW_ON_CONTROLS: {
					if (!node[GLE::toStr(GLE::Params::INTERPOLATOR)][GLE::toStr(
							GLE::Params::INTERPOLATOR_PARAMETERS)]) {
						return false;
					}
					if (!node[GLE::toStr(GLE::Params::INTERPOLATOR)][GLE::toStr(
							GLE::Params::INTERPOLATOR_PARAMETERS)].IsSequence()) {
						return false;
					}
					if (!node[GLE::toStr(GLE::Params::INTERPOLATOR)][GLE::toStr(
							GLE::Params::INTERPOLATOR_PARAMETERS)] == 2) {
						return false;
					}
					rhs.sample_frequence = node[GLE::toStr(
							GLE::Params::INTERPOLATOR)][GLE::toStr(
							GLE::Params::INTERPOLATOR_PARAMETERS)][0].as<
							unsigned int>();
					rhs.weighting_exponent =
							node[GLE::toStr(GLE::Params::INTERPOLATOR)][GLE::toStr(
							GLE::Params::INTERPOLATOR_PARAMETERS)][1].as<
							double>();
					break;
				}
				}
			}
		}
		return true;
	}
};
}

#endif /* DEMPARAM_H_ */
