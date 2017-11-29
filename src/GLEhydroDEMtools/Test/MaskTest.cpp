/*
 * Mask_PM1Zero_Test.h
 *
 *  Created on: 26/nov/2012
 *      Author: lordmzn
 */
#include <gtest/gtest.h>
#include "../LandscapeModelEnums.h"
#include "../DEMParam.h"
#include "../Mask.h"

namespace GLEhydroDEMtoolsTests {

class MaskTest: public ::testing::Test {
public:
	MaskTest() :
			nr_(9), nc_(9){
	}

	int nr_;
	int nc_;
};

void setIWDMatrixOn7x7with2(OurMatrix<int>& a) {
	a.setElement(62, 0, 0); a.setElement( 86, 0, 1); a.setElement(90, 0, 2); a.setElement( 92, 0, 3); a.setElement(90, 0, 4); a.setElement( 86, 0, 5); a.setElement(62, 0, 6);
	a.setElement(86, 1, 0); a.setElement(100, 1, 1); a.setElement(95, 1, 2); a.setElement(100, 1, 3); a.setElement(95, 1, 4); a.setElement(100, 1, 5); a.setElement(86, 1, 6);
	a.setElement(90, 2, 0); a.setElement( 95, 2, 1); a.setElement(95, 2, 2); a.setElement( 96, 2, 3); a.setElement(95, 2, 4); a.setElement( 95, 2, 5); a.setElement(90, 2, 6);
	a.setElement(92, 3, 0); a.setElement(100, 3, 1); a.setElement(96, 3, 2); a.setElement(100, 3, 3); a.setElement(96, 3, 4); a.setElement(100, 3, 5); a.setElement(92, 3, 6);
	a.setElement(90, 4, 0); a.setElement( 95, 4, 1); a.setElement(95, 4, 2); a.setElement( 96, 4, 3); a.setElement(95, 4, 4); a.setElement( 95, 4, 5); a.setElement(90, 4, 6);
	a.setElement(86, 5, 0); a.setElement(100, 5, 1); a.setElement(95, 5, 2); a.setElement(100, 5, 3); a.setElement(95, 5, 4); a.setElement(100, 5, 5); a.setElement(86, 5, 6);
	a.setElement(62, 6, 0); a.setElement( 86, 6, 1); a.setElement(90, 6, 2); a.setElement( 92, 6, 3); a.setElement(90, 6, 4); a.setElement( 86, 6, 5); a.setElement(62, 6, 6);
}
void setRBFMLMatrixOn7x7with2(OurMatrix<int>& a) {
	a.setElement(96, 0, 0); a.setElement( 98, 0, 1); a.setElement( 99, 0, 2); a.setElement( 99, 0, 3); a.setElement( 99, 0, 4); a.setElement( 98, 0, 5); a.setElement(96, 0, 6);
	a.setElement(98, 1, 0); a.setElement(100, 1, 1); a.setElement(100, 1, 2); a.setElement(100, 1, 3); a.setElement(100, 1, 4); a.setElement(100, 1, 5); a.setElement(98, 1, 6);
	a.setElement(99, 2, 0); a.setElement(100, 2, 1); a.setElement(100, 2, 2); a.setElement(100, 2, 3); a.setElement(100, 2, 4); a.setElement(100, 2, 5); a.setElement(99, 2, 6);
	a.setElement(99, 3, 0); a.setElement(100, 3, 1); a.setElement(100, 3, 2); a.setElement(100, 3, 3); a.setElement(100, 3, 4); a.setElement(100, 3, 5); a.setElement(99, 3, 6);
	a.setElement(99, 4, 0); a.setElement(100, 4, 1); a.setElement(100, 4, 2); a.setElement(100, 4, 3); a.setElement(100, 4, 4); a.setElement(100, 4, 5); a.setElement(99, 4, 6);
	a.setElement(98, 5, 0); a.setElement(100, 5, 1); a.setElement(100, 5, 2); a.setElement(100, 5, 3); a.setElement(100, 5, 4); a.setElement(100, 5, 5); a.setElement(98, 5, 6);
	a.setElement(96, 6, 0); a.setElement( 98, 6, 1); a.setElement( 99, 6, 2); a.setElement( 99, 6, 3); a.setElement( 99, 6, 4); a.setElement( 98, 6, 5); a.setElement(96, 6, 6);
}

TEST_F(MaskTest, setGetControlsInterpNONE) {
	double fakeValues[49];
	for (unsigned int i = 0; i < 49; i++) {
		fakeValues[i] = 2.0;
	}
	DEMParam p;
	p.number_rows = nr_;
	p.number_cols = nc_;
	p.controlUpperBound = 3;
	p.interpolatorType = GLE::Interpolator::NONE;
	p.isValid();
//	std::cout << "validityOfParam=" << (p.isValid() ? "good" : "bad") << std::endl;
//	std::cout << "#input" << p.number_inputs << std::endl;
	Mask m(p);
	EXPECT_EQ(0, m.setControls(fakeValues)) << "false negative wrong value recognition";
//	m.getControls().printMatrix();
	OurMatrix<int> a(7, 7, 2);
	ASSERT_EQ(a, m.getControls())<< "wrong values initialization";
	fakeValues[32] = -2.0;
//	m.setControls(fakeValues); m.getControls().printMatrix();
	EXPECT_EQ(1, m.setControls(fakeValues))<< "doesn't recognize the invalid value";
//	m.getControls().printMatrix();
	ASSERT_EQ(a, m.getControls())<< "accept invalid values when initializing";
}

TEST_F(MaskTest, operatorEquals) {
	DEMParam p;
	p.number_rows = nr_;
	p.number_cols = nc_;
	p.interpolatorType = GLE::Interpolator::NONE;
	p.isValid();
	Mask m(p);
	double fakeValues[49];
	for (unsigned int i = 0; i < 49; i++) {
		fakeValues[i] = 2.0;
	}
	Mask n(p);
	n.setControls(fakeValues);
	m = n;
	ASSERT_EQ(m.getControls(), n.getControls());
}

TEST_F(MaskTest, setGetControlsInterpIWD) {
	double fakeValues[9];
	for (unsigned int i = 0; i < 9; i++) {
		fakeValues[i] = 2.0;
	}
	DEMParam p;
	p.number_rows = nr_;
	p.number_cols = nc_;
	p.controlUpperBound = 3;
	p.interpolatorType = GLE::Interpolator::INVERSE_WEIGHTED_DISTANCE;
	p.sample_frequence = 2;
	p.isValid();
//	std::cout << "validityOfParam=" << (p.isValid() ? "good" : "bad") << std::endl;
//	std::cout << "#input" << p.number_inputs << std::endl;
	Mask m(p);
	EXPECT_EQ(0, m.setControls(fakeValues)) << "false negative wrong value recognition";
//	m.getControls().printMatrix();
	OurMatrix<int> a(7, 7, 2);
	setIWDMatrixOn7x7with2(a);
	ASSERT_EQ(a, m.getControls())<< "wrong values initialization";
	fakeValues[4] = 12.0;
//	m.setControls(fakeValues);
//	m.getControls().printMatrix();
	EXPECT_EQ(1, m.setControls(fakeValues))<< "doesn't recognize the invalid value";
//	m.getControls().printMatrix();
	ASSERT_EQ(a, m.getControls())<< "accept invalid values when initializing";
}

TEST_F(MaskTest, setGetControlsInterpRBFML) {
	DEMParam p;
	p.number_rows = nr_;
	p.number_cols = nc_;
	p.controlUpperBound = 3;
	p.fixedElevationChange = 100;
	p.interpolatorType = GLE::Interpolator::RBF_ML_ON_DEM;
	p.sample_frequence = 2;
	p.D_RBF = 2;
	p.isValid();
	double fakeValues[9];
	for (unsigned int i = 0; i < 9; i++) {
		fakeValues[i] = 2.0;
	}
	Mask m(p);
	EXPECT_EQ(0, m.setControls(fakeValues)) << "false invalid value recognition";
//	m.getControls().printMatrix();
	OurMatrix<int> a(7, 7, 2);
	setRBFMLMatrixOn7x7with2(a);
	ASSERT_EQ(a, m.getControls())<< "wrong values initialization";
	fakeValues[4] = 12.0;
//	m.setControls(fakeValues);
//	m.getControls().printMatrix();
	EXPECT_EQ(1, m.setControls(fakeValues))<< "doesn't recognize the invalid value";
//	m.getControls().printMatrix();
	ASSERT_EQ(a, m.getControls())<< "accept invalid values when initializing";
	fakeValues[4] = 2.0;

	p.number_rows = 19;
	p.number_cols = 19;
	p.controlUpperBound = 10;
	p.controlLowerBound = 1;
	p.fixedElevationChange = 100;
	p.interpolatorType = GLE::Interpolator::RBF_ML_ON_DEM;
	p.sample_frequence = 4;
	p.D_RBF = 2;
	p.isValid();
	Mask n(p);

	double fakeValues16[16];
	fakeValues16[0] = 2.0;
	fakeValues16[1] = 3.0;
	fakeValues16[2] = 2.5;
	fakeValues16[3] = 2.0;
	fakeValues16[4] = 2.5;
	fakeValues16[5] = 5.0;
	fakeValues16[6] = 4.0;
	fakeValues16[7] = 2.5;
	fakeValues16[8] = 3.0;
	fakeValues16[9] = 7.0;
	fakeValues16[10] = 5.5;
	fakeValues16[11] = 2.9;
	fakeValues16[12] = 3.5;
	fakeValues16[13] = 6.0;
	fakeValues16[14] = 4.0;
	fakeValues16[15] = 2.5;
	EXPECT_EQ(0, n.setControls(fakeValues16)) << "false invalid value recognition";
//	n.getControls().printMatrix();
}

} /* namespace GLEhydroDEMtoolsTests */
