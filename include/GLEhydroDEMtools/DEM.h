/*
 * DEM.h
 *
 *  Created on: 22/ago/2012
 *      Author: lordmzn
 */

#ifndef HYDRODEMTOOLS_DEM_H_
#define HYDRODEMTOOLS_DEM_H_

#include "OurMatrix.h"
#include "DEMParam.h"
#include "LandscapeModelEnums.h"
#include "Cell.h"
#include <climits>

class Mask;
class HydroNet;
//---depressionData STRUCT----
struct depressionData {
	bool hasDepression;
	int c_el; //current element
	int b_el; // lower boundary element
	int i_b; //coordinates of the boundary element
	int j_b;
};

/**
 * Represents a digital elevation model of a terrain. Has a rectangular
 * shape, which dimensions are defined in params. Implements the generic
 * data type State. Also contains datas and methods required to evaluate
 * the hydrological network of a given DEM. It's also able to evaluate
 * the discharge of the network, given a constant precipitation and
 * runoff coefficient.
 */
class DEM /*: public State*/{
public:
	DEM();
	explicit DEM(DEMParam const* p);
	DEM(const DEM&);
	/**
	 * Builds a new DEM from a given DEM and a mask: initialize a new
	 * DEM equal to the parameter passed but for the new elevations.
	 * @param base dem over which applying the mask
	 * @param the mask representing the elevation change
	 */
	DEM(const DEM&, const Mask&);
	~DEM();
	DEM& operator=(const DEM&);
	/**
	 * check if DEM hasDepression=FALSE else calls depressionFilling,
	 * then calls the evaluateHydroNet to get the
	 * hydrological network of this DEM */
	void evaluateHydroNet();
	bool isHydroNetEvaluated() const;

	// getters
	const double getMass() const;
	OurMatrix<int>& getElevationData();
	const OurMatrix<unsigned int>& getDrainedArea() const;
	const OurMatrix<int>& getStx() const;
	const OurMatrix<int>& getSty() const;
	const OurMatrix<double>& getSlope() const;
	const OurMatrix<double>& getFlowLength() const;
	const OurMatrix<double>& getDischarge();

	//
	const double& getDischargeThreshold() const;

	/**
	 * i'm bad
	 */
	friend class HydroNet;

private:
	/* Pointer to const object: responsibility of demParam
	 * life is not taken by *this.
	 */
	DEMParam const* demParam;
	OurMatrix<int> elevationData;
	/**
	 * Elevation step between different rows/cols during initialization of dem PYRAMID and WEDGE
	 * centimeters
	 */
	static const int elStep = 400;
	/**
	 * Value of the minimal positive difference of altitude for Planchon depression filling
	 */
	static const int epsilon = 1;
	/**
	 * Value of the huge number for transientSurface in Planchon depression filling
	 */
	static const int hugeNumber = INT_MAX;
	static const int maxDepth = 2000;
	/**Mass of the terrain modelled. It's the sum of the
	 * elevation values contained in elevationData.
	 */
	double mass; // meters
	/* This is a pointer just because having an hydronet
	 * it's optional, but the responsibility for hydroNet'
	 * life is nonetheless taken by *this.
	 */
	HydroNet* hydroNet;

	/**
	 * Initialize the field elevationData. According to the
	 * input data type, fills each cell with its elevation,
	 * either from file or from the pyramid.
	 * @param type of input
	 * @return true if elevationData now contains the values
	 * indicated.
	 */
	bool initElevationData();
	/**
	 * Evaluates the mass of this DEM.
	 */
	void evaluateMass();

	/**
	 * It scrolls the current DEM element by element and calls
	 * findDepression function in order to verify if the element
	 * is depressed. If the current element is depresses:
	 * - it calls depressionFilling function for a first adjustment;
	 * - it analyze the boundary elements of the adjusted current
	 *   element and goes on calling the depressionFilling function
	 *       for the surrounding elements, until depressions are all adjusted.
	 */
//TODO scommentare ifdef #ifndef DFPLANCHON
	void adjustDepressions();
	depressionData findDepressions(int i, int j);
	/**
	 * Depression filling function
	 * It evaluates delta between the depressed element and the lowest of its boundary as
	 *  b_el - c_el and rounds it to int, then it sets:
	 *  c_el(i,j)= b_el + 0.5(delta);
	 *  b_el=b_el - 0.5(delta);
	 */
	void depressionFilling(depressionData test, int i, int j); // perform depression filling on this DEM

//#elif
	void adjustDepressionsPlanchon();
	int dryUpwardCell(Cell c, OurMatrix<int>& transientSurface, int depth);
	OurMatrix<int> DFPlanchonStage1();
	void DFPlanchonStage2(OurMatrix<int>& transientSurface);
//#endif
	int myRound(double r);
};

#endif /* HYDRODEMTOOLS_DEM_H_ */
