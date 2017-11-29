/*
 * Mask.h
 *
 *  Created on: 22/dic/2012
 *      Author: lordmzn
 */

#ifndef MASK_H_
#define MASK_H_

#include "OurMatrix.h"
#include "DEMParam.h"
#include "LandscapeModelEnums.h"
#include "alglib/interpolation.h"
#include <vector>
/**
 * Represents the integer control applicable to evolve a DEM through
 * time. Both the bounds are inclusive.
 */
class Mask {
public:
	/**
	 * Constructor of simple control mask. The bounds are set to +1 and
	 * -1, in order to represet
	 */
	explicit Mask(DEMParam&);
	~Mask();
	Mask& operator=(const Mask&);
	const OurMatrix<int>& getControls() const;
	/**
	 * Fill the field controlValues with the input provided. Check
	 * if the values are between the bounds then fills controlValues.
	 * @param the values of the control
	 * @return the number of values not correct: if it's different
	 * from zero, the field controlValues is still full of zero.
	 */
	unsigned int setControls(double[]);
private:
#ifdef DEBUG
public:
#endif
	/**
	 * has dimensions == (dem.nrows - 2) x (dem.ncols - 2)
	 */
	OurMatrix<int> controlValues;
	DEMParam& param;
	// pointer because they're optional
	OurMatrix<double>* input_norm;
	OurMatrix<unsigned int>* input_helper;
	// stuff for rbf multilayer alg lib
	alglib::real_1d_array* rowsOfPointToEstimate;
	alglib::real_1d_array* colsOfPointToEstimate;
	alglib::real_2d_array* rescaledParams;
	alglib::rbfmodel* rbfModel;
	alglib::rbfreport* reporter;

	static const double TOLERANCE_ON_CONTROLS = 0.1;

	unsigned int checkValidity(const double vals[]);
	void interpolateControlsWithRBFML(const double parameters[], int out[]);
	void interpolateControlsWithIDW(const double vals[], int controls[]);
	void interpolateControlsWithIDW_NoWindow(const double vals[], int controls[]);
	double weightingFunctionIDW(int row1, int col1, int row2, int col2);
	double weightingFunctionIDW_NoWindow(int row1, int col1, int row2, int col2);
};

#endif /* MASK_H_ */
