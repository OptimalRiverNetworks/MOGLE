/*
 * DEM_intTest.cpp
 *
 *  Created on: 22/dic/2012
 *      Author: lordmzn
 */

#include <gtest/gtest.h>
#include "../DEM.h"
#include "../Mask.h"
#include "../HydroNet.h"

namespace GLEhydroDEMtoolsTests {

class DEMTest: public ::testing::Test {
public:
	DEMTest() {
		p.areaOfDEMCell = 1;
		p.lengthOfDEMCell = 1000;
		p.rainfallExcess = 1.0;
		p.fixedElevationChange = 10;
	}
	DEMParam p;
};

void anotherPyramid(OurMatrix<int>& m) {
	for (int d = 1; d < 5; d++) {
		// d-th row
		for (int i = d; i < 11 - d; i++) {
			m.setElement(400 * d, d, i);
		}
		// d-th last row
		for (int i = d; i < 11 - d; i++) {
			m.setElement(400 * d, 10 - d, i);
		}
		// d-th column
		for (int i = d + 1; i < 10 - d; i++) {
			m.setElement(400 * d, i, d);
		}
		// d-th last column
		for (int i = d + 1; i < 10 - d; i++) {
			m.setElement(400 * d, i, 10 - d);
		}
	}
	m.setElement(400 * 5, 5, 5);
}

TEST_F(DEMTest, ConstructorsTest) {
	DEM e(&p);
	ASSERT_EQ(11, e.getElevationData().getDimension().n_columns);
	ASSERT_EQ(11, e.getElevationData().getDimension().n_rows);
	OurMatrix<int> pyramid(11, 11, 0);
	anotherPyramid(pyramid);
	ASSERT_EQ(pyramid, e.getElevationData())<< "pyramid uncorrectly initialized";
	DEM d(e);
	ASSERT_FALSE(&d==&e);
	ASSERT_EQ(d.getElevationData().getDimension().n_columns,
			e.getElevationData().getDimension().n_columns);
	ASSERT_EQ(d.getElevationData().getDimension().n_rows,
			e.getElevationData().getDimension().n_rows);
}

TEST_F(DEMTest, StateTransitionTest) {
	DEM d(&p);
	Mask control(p);
	double vals[81];
	for (unsigned int i = 0; i < 81; i++) {
		vals[i] = 1.0;
	}
	// set a different control for the top
	vals[(4 * 9) + 4] = -1.0;
	control.setControls(vals);
	DEM e(d, control);
	// they have same dimension (at least)
	ASSERT_EQ(d.getElevationData().getDimension().n_columns, e.getElevationData().getDimension().n_columns)<< "different DEM dimensions";
	ASSERT_EQ(d.getElevationData().getDimension().n_rows, e.getElevationData().getDimension().n_rows)<< "different DEM dimensions";

	OurMatrix<int> pyramid(11, 11, 0);
	anotherPyramid(pyramid);
	pyramid.sumMatrix(10);
	pyramid.setElement(pyramid.getElement(5, 5) - 20, 5, 5);
	for (int i = 0; i < 11; i++) {
		pyramid.setElement(0, 0, i);
		pyramid.setElement(0, 10, i);
	}
	for (int i = 1; i < 10; i++) {
		pyramid.setElement(0, i, 0);
		pyramid.setElement(0, i, 10);
	}
	// now pyramid should equal the new elevation
	ASSERT_FALSE(&d==&e)<< "new and old dem are the same obj";
	ASSERT_FALSE(d.getElevationData() == e.getElevationData())<< "new DEM has elevations = old DEM";
	ASSERT_EQ(pyramid, e.getElevationData())<< "wrong evaluations of elevations in the new DEM";
}

TEST_F(DEMTest, DepressionFillingTest) {
	p.controlLowerBound = -20.0;
	p.controlUpperBound = 20.0;
	p.fixedElevationChange = 900;
//	float massVariationLimit = 0.00001;
//	int massBeforeDF = 0;
//	int massAfterDF = 0;
//	float varMass = 0;

	DEM d(&p);
	Mask depressionMask(p);
	OurMatrix<int> depressionMatrix(11, 11, 0);
	anotherPyramid(depressionMatrix);

	// Set controls for generating single-cell depressions
	depressionMatrix.setElement(depressionMatrix.getElement(5, 5) - 900, 5, 5);
	double vals[81];
	for (unsigned int i = 0; i < 81; i++) {
		vals[i] = 0.0;
	}
	vals[(4 * 9) + 4] = -1.0;
	ASSERT_EQ(0, depressionMask.setControls(vals) )<< "Mask creation failed on single";
	DEM e(d, depressionMask);

//	depressionMatrix.printMatrix();
//	std::cout << "-----base dem---------------" << std::endl;
//	e.getElevationData().printMatrix();
//	std::cout << "-----dem after depression filling---------------"
//			<< std::endl;
//	std::cout << "------------------------------------------------"
//			<< std::endl;

	ASSERT_FALSE(depressionMatrix == e.getElevationData())<< "Depression filling not executed on single";
	ASSERT_FALSE(depressionMatrix.getElement(5,5) == e.getElevationData().getElement(5,5))<< "Depressed element corrected";

//	massBeforeDF = depressionMatrix.sumAll();
//	massAfterDF = e.getElevationData().sumAll();
//	varMass = (massAfterDF - massBeforeDF);
//	varMass = varMass / massBeforeDF;
//	ASSERT_TRUE(varMass<massVariationLimit)<< "Mass conservation constrain not respected";

	HydroNet h(&e);
	for (unsigned int i = 1; i < d.getElevationData().getDimension().n_rows - 1;
			i++) {
		for (unsigned int j = 1;
				j < d.getElevationData().getDimension().n_columns - 1; j++) {
			EXPECT_TRUE(h.getSlope().getElement(i,j) > 0)
					<< "A non-boundary cell has slope<=0";
		}
	}

	// Set controls for generating extended depressed area (more than 1 cell involved)
	p.fixedElevationChange = 1700;

	anotherPyramid(depressionMatrix);
	depressionMatrix.setElement(-100, 4, 4);
	depressionMatrix.setElement(-100, 5, 4);
	depressionMatrix.setElement(-100, 6, 4);

	double valsMultiple[81];
	for (unsigned int i = 0; i < 81; i++) {
		valsMultiple[i] = 0.0;
	}
	valsMultiple[(3 * 9) + 3] = -1.0;
	valsMultiple[(4 * 9) + 3] = -1.0;
	valsMultiple[(5 * 9) + 3] = -1.0;
	Mask depressionMaskMultiple(p);
	ASSERT_EQ(0, depressionMaskMultiple.setControls(valsMultiple) )<< "Mask creation failed in 3x1";
	DEM f(d, depressionMaskMultiple);

//	depressionMatrix.printMatrix();
//	std::cout << "-----base dem---------------" << std::endl;
//	f.getElevationData().printMatrix();
//	std::cout << "-----dem after depression filling---------------"
//			<< std::endl;
//	std::cout << "------------------------------------------------"
//			<< std::endl;

	ASSERT_FALSE(depressionMatrix == f.getElevationData())<< "Depression filling on 3x1 depression not executed";
	ASSERT_FALSE(depressionMatrix.getElement(4,4) == f.getElevationData().getElement(4,4) && (depressionMatrix.getElement(5,4) == f.getElevationData().getElement(5,4) &&
					(depressionMatrix.getElement(6,4) == f.getElevationData().getElement(6,4))))<< "Depressed elements corrected";
//	massBeforeDF = depressionMatrix.sumAll();
//    massAfterDF = f.getElevationData().sumAll();
//	varMass = (massAfterDF - massBeforeDF);
//	varMass = varMass / massBeforeDF;
//	ASSERT_TRUE(varMass<massVariationLimit)<< "Mass conservation constrain not respected";

	HydroNet g(&f);

	for (unsigned int i = 1; i < d.getElevationData().getDimension().n_rows - 1;
			i++) {
		for (unsigned int j = 1;
				j < d.getElevationData().getDimension().n_columns - 1; j++) {
			EXPECT_TRUE(g.getSlope().getElement(i,j) > 0)
					<< "A non-boundary cell has slope<=0";
		}
	}

	anotherPyramid(depressionMatrix);
	depressionMatrix.setElement(-18700, 3, 4);
	depressionMatrix.setElement(-18700, 5, 4);
	depressionMatrix.setElement(-18700, 7, 4);

	for (unsigned int i = 0; i < 81; i++) {
		valsMultiple[i] = 0.0;
	}
	valsMultiple[(2 * 9) + 3] = -11.0;
	valsMultiple[(4 * 9) + 3] = -11.0;
	valsMultiple[(6 * 9) + 3] = -11.0;
	Mask depressionMaskMultiple2(p);
	ASSERT_EQ(-20.0, p.controlLowerBound)<< "DEMParam is wrong";
	ASSERT_EQ(0, depressionMaskMultiple2.setControls(valsMultiple) )<< "Mask creation failed in 1 x 1 x 1";
//	std::cout << "------result without DF------" << std::endl;
//	depressionMatrix.printMatrix();
	DEM ff(d, depressionMaskMultiple2);
//	std::cout << "--------------dem produced------" << std::endl;
//	ff.getElevationData().printMatrix();

	ASSERT_FALSE(depressionMatrix == ff.getElevationData())<< "Depression filling 1 x 1 x 1 depression not executed";
	ASSERT_TRUE(801 == ff.getElevationData().getElement(3,4) && 1201 == ff.getElevationData().getElement(5,4) &&
			801 == ff.getElevationData().getElement(7,4))<< "Depressed elements not corrected";
}

TEST_F(DEMTest, evaluateHydronetTest) {
	DEM d(&p);
	d.evaluateHydroNet();
	HydroNet h(&d);

	ASSERT_EQ(h.getStx(),d.getStx())<< "Stx incorrectly retrieved from DEM d";
	ASSERT_EQ(h.getSty(),d.getSty())<< "Sty incorrectly retrieved from DEM d";
	ASSERT_EQ(h.getDrainedArea(),d.getDrainedArea())<< "Drained area incorrectly retrieved from DEM d";
	ASSERT_EQ(h.getSlope(),d.getSlope())<< "Slope incorrectly retrieved from DEM d";
	ASSERT_EQ(h.getFlowLength(),d.getFlowLength())<< "Flow length incorrectly retrieved from DEM d";
}} /* namespace GLEhydroDEMtoolsTests */

