/*
 * HydroNet.h
 *
 *  Created on: 27/nov/2012
 *      Author: lordmzn
 */

#ifndef HYDRONET_H_
#define HYDRONET_H_

#include "DEM.h"

namespace HydroNetInternal {
struct Point {
	unsigned int x;
	unsigned int y;
};
struct GD8state {
	Point reference;
	Point current;
	int secDir;
	bool isNextValid;
};
}
/**
 * Evaluate the hydrological network of a given digital elevation
 * model of a terrain.
 */
class HydroNet {
public:
	/**
	 * The constructor from a given DEM object. Longer
	 * description here.
	 * @param pointer to the DEM object which contains elevation
	 * data.
	 * @param DEMparam object with various needed parameters.
	 */
	explicit HydroNet(DEM*);
	/**
	 * Copy constructor.
	 * @param
	 */
	HydroNet(const HydroNet&);
	/**
	 * Destructor required to delete discharge field
	 */
	~HydroNet();
	/**
	 * Assignment operator
	 */
	HydroNet& operator=(const HydroNet&);

	const OurMatrix<unsigned int>& getDrainedArea() const;
	const OurMatrix<int>& getStx() const;
	const OurMatrix<int>& getSty() const;
	const OurMatrix<double>& getSlope() const;
	const OurMatrix<double>& getFlowLength() const;
	const OurMatrix<double>& getDischarge();

private:
	/* Pointers to const object: the responsibility of their
	 * lifes is not taken by *this, as the ability to make changes.
	 */
	DEM* dem;
	OurMatrix<int> stx;
	OurMatrix<int> sty;
	OurMatrix<unsigned int> drainedArea;
	OurMatrix<double> slope;
	OurMatrix<double> flowLength;
	OurMatrix<double>* discharge;

	/**
	 * Fills all the fields by locating the starting cells from
	 * which GD8() has to be called and then evaluates drained area
	 */
	void evaluateFlowDirection();

	/**
	 * The function evaluates the drained area for each cell. It
	 * is calculated as: number of cells whose flow is drained by
	 * the current cell * area of a single cell.
	 */
	void evaluateDrainedArea();

	/**
	 * It evaluated stx and sty, slope and flow length for
	 * each of the downstream cells, starting from a reference
	 * cell. It calls the functions primariDirectionSearch and
	 * secondaryDirectionSearch, in an iterative way.
	 */
	void GD8(HydroNetInternal::Point);

	/**
	 * It searches in the 8-cell boundary the maximum gradient
	 * and finds the primary direction. It updates:
	 * - maximum slope;
	 * - distance between the two cells with the maximum gradient;
	 * - direction vectors stx and sty.
	 */
	void primaryDirectionSearch(HydroNetInternal::Point, bool callTurnFirst);

	/**
	 * It computes the secondary direction by evaluating the higher
	 * gradient between the current cell and the two which are adjacent
	 * to the cell pointed by the primary direction
	 */
	int secondaryDirectionSearch(HydroNetInternal::Point);

	/**
	 * It decides if the flow directions for a cell are the ones of the
	 * primary direction or the ones given by the secondary direction.
	 * Three criteria are considered for the choice. If:
	 * 1- the current and downstream cell have the same stx and sty;
	 * 2- the current and downstream cell have the same secondary direction;
	 * 3- the gradient between the reference cell and the secondary direction
	 *    cell is higher than the one between the reference cell and the
	 *    primary direction cell.
	 * Then, the secondary direction is chosen. If one of the three
	 * conditions is not satisfied, the primary direction is chosen.
	 */
	HydroNetInternal::GD8state computeDirection(HydroNetInternal::GD8state);
};

#endif /* HYDRONET_H_ */
