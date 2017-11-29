/*
 * LandscapeModelEnums.h
 *
 *  Created on: 27/ago/2012
 *      Author: lordmzn
 */

#ifndef LANDSCAPEMODELENUMS_H
#define LANDSCAPEMODELENUMS_H

#include <string>

namespace GLE {
namespace Params {
enum FileKeys {
	CONTROLS = 0,
	BOUNDS,
	FIXED_EL_CHANGE,
	STATE,
	DIMENSIONS,
	CELL,
	AREA,
	LENGTH,
	WIDTH,
	INITIAL,
	SOURCE_TYPE,
	FILENAME,
	BASIN_LENGTH,
	SHAPE_FACTOR,
	HYDROLOGICAL_PARAMETERS,
	RUNOFF_COEFF,
	RAINFALL_EXCESS,
	OBJECTIVES,
	OBJ_TYPES,
	AD_THRESHOLD,
	CONSTRAIN,
	MASS_CONSTRAIN_ENABLED,
	MASS_VARIATION_TOLERANCE,
	INTERPOLATOR,
	INTERPOLATOR_TYPE,
	INTERPOLATOR_PARAMETERS
};
/* 26 values */
static const std::string FileKeysStrings[] = { "controls", "bounds",
		"fixedElevationChange", "state", "dimensions", "cell", "area", "length",
		"width", "initial", "sourceType", "filename", "basinLength",
		"shapeFactor", "hydrologicalParameters", "runoffCoeff",
		"rainfallExcess", "objective", "types", "drainedAreaThreshold",
		"constrain", "massConstrainEnabled", "massVariationTolerance",
		"interpolator", "type", "parameters" };

//enum Fields {
//	CONTROL_UPPER_BOUND,
//	CONTROL_LOWER_BOUND,
//	FIXED_ELEVATION_CHANGE,
//	FIXED_ELEVATION_CHANGE_FROM_OPT,
//	//	GLE::Interpolator::Types interpolatorType,
//	NUMBER_INPUTS,
//	SAMPLE_FREQUENCE,
//	D_RBF,
//	NEIGHBORHOOD_SIZE,
//	WEIGHTING_EXPONENT,
//	NUMBER_ROWS,
//	NUMBER_COLS,
//	AREA_OF_DEM_CELL,
//	LENGTH_OF_DEM_CELL,
//	WIDTH_OF_DEM_CELL,
//	//	GLE::InputData::Types in,
//	//	std::string DEMfilename,
//	BASIN_LENGTH_F,
//	BASIN_WIDTH_F,
//	SHAPE_FACTOR_F,
//	RUNOFF_COEFF_F,
//	RAINFALL_EXCESS_F,
//	//	std::vector<GLE::Stepcost::Types> typeOfObjectives,
//	// bool massConstrainEnabled,
//	MASS_TOLERANCE
//};

}

namespace Stepcost {
enum Types {
	TEE_COST = 0, EEE_COST, TSP_COST, EE_COST, EEL_COST, SH_COST
};
static const std::string TypesStrings[] = { "TEE", "EEE", "TSP", "EE", "EEL", "SH" };
static const int numberOfTypes = 6;
}

namespace InputData {
enum Types {
	PYRAMID = 0, DATAFILE, WEDGE
};
static const std::string TypesStrings[] = { "PYRAMID", "DATAFILE", "WEDGE" };
static const int numberOfTypes = 3;
}

namespace Interpolator {
enum Types {
	NONE = 0, RBF_ML_ON_DEM, INVERSE_WEIGHTED_DISTANCE, IDW_ON_CONTROLS
};
static const std::string TypesStrings[] = { "none", "RBF-ML", "IDW", "IDW-CONTROLS" };
static const unsigned int numberOfTypes = 4;
}

namespace ObjectData {
enum Types {
	CONTROL = 0,
	DEM_T,
	FLOW_DIRECTION,
	FLOW_LENGTH,
	SLOPE,
	DRAINED_AREA,
	DISCHARGE //, STEPCOST
};
static const std::string TypesStrings[] = { "mask", "dem", "flowDir", "flowLen",
		"slope", "drainedArea", "discharge" };
}

enum SaveDataOption {
	BASE_OPT, LAST_OPT
};
enum ReadingOptions {
	COMMON_FIELD, MATRIX_LINE
};
inline const std::string toStr(Params::FileKeys enumobj) {
	return Params::FileKeysStrings[enumobj];
}
inline const std::string toStr(ObjectData::Types enumobj) {
	return ObjectData::TypesStrings[enumobj];
}
inline const std::string toStr(InputData::Types enumobj) {
	return InputData::TypesStrings[enumobj];
}
inline const std::string toStr(Stepcost::Types enumobj) {
	return Stepcost::TypesStrings[enumobj];
}
inline const std::string toStr(Interpolator::Types enumobj) {
	return Interpolator::TypesStrings[enumobj];
}
} // namespace GLE

struct DEMDimensionType {
	unsigned int n_rows;
	unsigned int n_columns;
	DEMDimensionType() :
			n_rows(0), n_columns(0) {
	}
	DEMDimensionType(unsigned int x, unsigned int y) :
			n_rows(x), n_columns(y) {
	}
	DEMDimensionType(const DEMDimensionType& d) :
			n_rows(d.n_rows), n_columns(d.n_columns) {
	}
};

struct CoupleOfInt {
	int x;
	int y;
};

#endif /* LANDSCAPEMODELENUMS_H */
