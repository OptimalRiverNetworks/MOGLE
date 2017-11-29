/*
 * HydroNetTest.cpp
 *
 *  Created on: 27/nov/2012
 *      Author: lordmzn
 */

#include <gtest/gtest.h>
#include "../HydroNet.h"
#include "../LandscapeModelEnums.h"
#include "../DEM.h"

namespace GLEhydroDEMtoolsTests {
class HydroNetTest: public ::testing::Test {
public:
	HydroNetTest() :
			d(&p) {
		p.areaOfDEMCell = 1;
		p.lengthOfDEMCell = 1;
		p.rainfallExcess = 1.0;
		p.fixedElevationChange = 10;
	}
	DEMParam p;
	DEM d;
};

TEST_F(HydroNetTest, ConstructorsTest) {
	HydroNet h(&d);
	HydroNet g(h);
	ASSERT_FALSE(&g==&h)<< "they're the same object";
	g = h;
	ASSERT_FALSE(&g==&h)<< "they're the same object";
}

TEST_F(HydroNetTest, GettersTest) {
	HydroNet h(&d);
	OurMatrix<unsigned int> temp1 = h.getDrainedArea();
	ASSERT_EQ(d.getElevationData().getDimension().n_columns, temp1.getDimension().n_columns);
	ASSERT_EQ(d.getElevationData().getDimension().n_rows, temp1.getDimension().n_rows);
	OurMatrix<int> temp = h.getStx();
	ASSERT_EQ(d.getElevationData().getDimension().n_columns, temp.getDimension().n_columns);
	ASSERT_EQ(d.getElevationData().getDimension().n_rows, temp.getDimension().n_rows);
	temp = h.getSty();
	ASSERT_EQ(d.getElevationData().getDimension().n_columns, temp.getDimension().n_columns);
	ASSERT_EQ(d.getElevationData().getDimension().n_rows, temp.getDimension().n_rows);
	OurMatrix<double> tem = h.getSlope();
	ASSERT_EQ(d.getElevationData().getDimension().n_columns, tem.getDimension().n_columns);
	ASSERT_EQ(d.getElevationData().getDimension().n_rows, tem.getDimension().n_rows);
	tem = h.getFlowLength();
	ASSERT_EQ(d.getElevationData().getDimension().n_columns, tem.getDimension().n_columns);
	ASSERT_EQ(d.getElevationData().getDimension().n_rows, tem.getDimension().n_rows);
	tem = h.getDischarge();
	ASSERT_EQ(d.getElevationData().getDimension().n_columns, tem.getDimension().n_columns);
	ASSERT_EQ(d.getElevationData().getDimension().n_rows, tem.getDimension().n_rows);
}

TEST_F(HydroNetTest, HydrologicalNetworkTest) {
	HydroNet h(&d);
	unsigned int i, j;

	// no cell is without direction
	for (i = 1; i < d.getElevationData().getDimension().n_rows - 1; i++) {
		for (j = 1; j < d.getElevationData().getDimension().n_columns - 1; j++) {
			ASSERT_FALSE(
					(h.getStx().getElement(i,j)==0) && (h.getSty().getElement(i,j)==0))<< "cell (" << i << "," << j << ") has no direction";
		}
	}

	// all cell are connected to at least one cell on the contour
	// locate top and save in i1, j1, el1
	OurMatrix<bool> isChecked(11, 11, false);
	unsigned int i1, j1, i2, j2, c1, c2;
	int el1;
	c1 = 0; // this count the number of cell visited, upper bound of the next while
	// min = nr * nc, max = 2 * nr * nc (probably lower)
	c2 = 1; // this counts how many cells have to be visited yet
	while (c2 > 0
			&& c1 < (2 * d.getElevationData().getDimension().n_rows * d.getElevationData().getDimension().n_columns)) {
		el1 = 0;
		for (i2 = 1; i2 < d.getElevationData().getDimension().n_rows - 1; i2++) {
			for (j2 = 1; j2 < d.getElevationData().getDimension().n_columns - 1; j2++) {
				if ((el1 < d.getElevationData().getElement(i2, j2))
						&& (isChecked.getElement(i2, j2) == false)) {
					el1 = d.getElevationData().getElement(i2, j2);
					i1 = i2;
					j1 = j2;
				}
			}
		}
		c1++; // count the top found and mark it
		isChecked.setElement(true, i1, j1);
		// start from top (i1,j1) and go toward the outlet
		i2 = i1 + h.getStx().getElement(i1, j1);
		j2 = j1 + h.getSty().getElement(i1, j1);
		isChecked.setElement(true, i2, j2);
		c1++; // count the i2,j2 cell
		while (c1 < d.getElevationData().getDimension().n_columns
				&& d.getElevationData().getElement(i2, j2) != 0) {
			i1 = i2;
			j1 = j2;
			i2 += h.getStx().getElement(i1, j1);
			j2 += h.getSty().getElement(i1, j1);
			isChecked.setElement(true, i2, j2);
			c1++; // count the new i2,j2 cell
		}
		//count how many cells have to be visited yet
		c2 = 0;
		for (i2 = 1; i2 < d.getElevationData().getDimension().n_rows - 1; i2++) {
			for (j2 = 1; j2 < d.getElevationData().getDimension().n_columns - 1; j2++) {
				if (isChecked.getElement(i2, j2) == false) {
					c2++;
				}
			}
		}
	}
	// if true, each cell is connected to the contour
	ASSERT_EQ(0, c2) << "a cell is not connected to the contour";

	//each cell but the contour has a flow length >= length of DEMcell
	for (i = 1; i < d.getElevationData().getDimension().n_rows - 1; i++) {
		for (j = 1; j < d.getElevationData().getDimension().n_columns - 1; j++) {
			EXPECT_TRUE(h.getFlowLength().getElement(i,j) >= p.lengthOfDEMCell) << "a cell not in the contour has a flow length < length of DEMcell";
		}
	}

	//each cell but the contour has a slope > 0
	for (i = 1; i < d.getElevationData().getDimension().n_rows - 1; i++) {
		for (j = 1; j < d.getElevationData().getDimension().n_columns - 1; j++) {
			EXPECT_TRUE(h.getSlope().getElement(i,j) > 0) << "a cell not in the contour has a slope =< 0";
		}
	}

	//each cell but the contour has a drainageArea >= areaOfCell (at least, it drains itself)
	for (i = 1; i < d.getElevationData().getDimension().n_rows - 1; i++) {
		for (j = 1; j < d.getElevationData().getDimension().n_columns - 1; j++) {
			EXPECT_TRUE(h.getDrainedArea().getElement(i,j) >= p.areaOfDEMCell) << "a cell not in the contour has a drainageArea < 1";
		}
	}

	// if there're no lakes, the sum of drained area of the cells next
	// to the contour is equal to the number of cell in the dem (contour excluded)
	double sum = 0.0;
	for (i = 1; i < d.getElevationData().getDimension().n_rows - 1; i++) {
		sum += h.getDrainedArea().getElement(i, 1);
		sum += h.getDrainedArea().getElement(i, d.getElevationData().getDimension().n_columns - 2);
	}
	for (j = 2; j < d.getElevationData().getDimension().n_columns - 2; j++) {
		sum += h.getDrainedArea().getElement(1, j);
		sum += h.getDrainedArea().getElement(d.getElevationData().getDimension().n_rows - 2, j);
	}
	sum = sum / p.areaOfDEMCell;
	double ncells = (d.getElevationData().getDimension().n_columns - 2)
			* (d.getElevationData().getDimension().n_rows - 2);
	EXPECT_DOUBLE_EQ(ncells, sum) << "the sum of drained area of the cells next to the contour isn't equal to the number of cell in the dem (contour excluded)";
}

} /* namespace GLEhydroDEMtoolsTests */

