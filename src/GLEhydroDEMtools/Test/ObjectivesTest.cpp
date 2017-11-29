/*
 * ObjectivesTest.cpp
 *
 *  Created on: 7/gen/2012
 *      Author: andrevb
 */

#include <gtest/gtest.h>
#include "../DEM.h"
#include "../EE.h"
#include "../TSP.h"
#include "../TEE.h"
#include "../EEE.h"

namespace GLEhydroDEMtoolsTests {

class ObjectivesTest: public ::testing::Test {
public:
	ObjectivesTest() {
		p.areaOfDEMCell = 1;
		p.lengthOfDEMCell = 1000;
		p.rainfallExcess = 0.1;
		p.fixedElevationChange = 100;
	}
	DEMParam p;
};

TEST_F(ObjectivesTest, ObjectivesTest) {
	double EEEsample = 5.728303400920680E-08;
	double EEsample = 7.459333759703739E-02;
	double TSPsample = 1.833333333333334E-02;
	double TEEsample = 18648.334399259344;

	DEM d(&p);
	EEE eee(d);
	EE ee(d);
	TSP tsp(d);
	TEE tee(d);

	d.evaluateHydroNet();

	EXPECT_DOUBLE_EQ(EEEsample, eee.getStepcost()) << "EEE incorrectly evaluated";
	EXPECT_DOUBLE_EQ(EEsample, ee.getStepcost()) << "EE incorrectly evaluated";
	EXPECT_DOUBLE_EQ(TSPsample, tsp.getStepcost()) << "TSP incorrectly evaluated";
	EXPECT_DOUBLE_EQ(TEEsample, tee.getStepcost()) << "TEE incorrectly evaluated";
}
} /* namespace GLEhydroDEMtoolsTests */

