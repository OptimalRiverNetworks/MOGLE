/*
 * OurMatrixTest.cpp
 *
 *  Created on: 23/nov/2012
 *      Author: lordmzn
 */
#include <gtest/gtest.h>
#include <math.h>
#include "../OurMatrix.h"

namespace GLEhydroDEMtoolsTests {

class OurMatrixTest: public ::testing::Test {
public:
	OurMatrixTest() :
			a(10, 10) {
	}

	OurMatrix<int> a;
};

TEST_F(OurMatrixTest, ConstructorsTestOnInt) {
	OurMatrix<int> z;
	OurMatrix<int> b(a);
	ASSERT_EQ(b, a)<< "Copy constructor not working";
	ASSERT_EQ(a, z)<< "Void constructor not working";
	for (int i = 0; i < b.getDimension().n_rows; i++) {
		for (int j = 0; j < b.getDimension().n_columns; j++) {
			b.setElement(b.getElement(i, j) + 1, i, j);
		}
	}
	EXPECT_EQ(1,b.data[0]) << "get/set Element wrong";
	OurMatrix<int> c(10, 10, 1);
	ASSERT_EQ(0, b.numberOfZero) << "wrong update of numberOfZero in setElement";
	ASSERT_EQ(c, b)<< "Default value constructor not working";
	c = b;
	ASSERT_EQ(c, b)<< "operator = not working";
	ASSERT_TRUE(c!=a)<< "operator!= not working";
	std::vector<int> vals(100, 1);
	OurMatrix<int> d(10, 10, vals);
	ASSERT_EQ(c, d)<< "constructor from vector";
}

TEST_F(OurMatrixTest, SumTestOnInt) {
	int one = 1;
	int zero = 0;
	OurMatrix<int> b(10, 10, one);
	OurMatrix<int> c(a);
	c.sumMatrix(b);
	ASSERT_EQ(b,c)<< "sum of zero matrix not working";
	c.sumMatrix(zero);
	ASSERT_EQ(b,c)<< "matrix + const";
}

TEST_F(OurMatrixTest, ProductTestOnInt) {
	int one = 1;
	OurMatrix<int> b(10, 10, one);
	OurMatrix<int> c(b);
	c.productMatrixElByEl(b);
	ASSERT_EQ(b,c)<< "product of ones-matrix not working";
	c.productMatrixElByEl(one);
	ASSERT_EQ(b,c)<< "matrix x const";
}

TEST_F(OurMatrixTest, PowAndSqrtTestOnInt) {
	int one = 1;
	OurMatrix<int> b(10, 10, one);
	OurMatrix<int> c(b);
	c.powerElByEl(one);
	ASSERT_EQ(b,c)<< "matrix ^ 1";
	c.powerElByEl(one + one);
	ASSERT_EQ(b,c)<< "matrix(ones) ^ 2";
	c.squareRootElByEl();
	ASSERT_EQ(b,c)<< "sqrt(matrix(ones))";
	OurMatrix<double> d(20, 20, 2.0);
	if (d.squareRootElByEl()) {
		for (int i = 0; i < d.getDimension().n_columns; i++) {
			for (int j = 0; j < d.getDimension().n_columns; j++) {
				ASSERT_DOUBLE_EQ(sqrt(2), d.getElement(i,j)) << "sqrt matrix of 2";
			}
		}
	}
}

TEST_F(OurMatrixTest, MeanAndVarTestOnInt) {
	int one = 1;
	OurMatrix<int> b(10, 10, one);
	OurMatrix<int> c(b);
	EXPECT_EQ(1,c.meanOfEls()) << "mean of matrix of ones";
	EXPECT_EQ(0,c.varianceOfEls()) << "stddev of values";
}

TEST_F(OurMatrixTest, onesToDouble) {
	int one = 1;
	double one_d = 1;
	double two_d = 2;
	double dotOne = 0.1;
	OurMatrix<int> b(10, 10, one);
	OurMatrix<double> c(10, 10, one_d);
	EXPECT_EQ(c,b.toDouble()) << "convert matrix of int ones into double";
	EXPECT_EQ(c,c.toDouble()) << "convert matrix of doubles into double";
	for (int i = 0; i < 10; i++) {
		c.sumMatrix(dotOne);
	}
	OurMatrix<double> d(10, 10, two_d);
	ASSERT_FALSE(d == c)<< "errors in double representation of 1.1";
	for (int i = 0; i < d.getDimension().n_columns; i++) {
		for (int j = 0; j < d.getDimension().n_columns; j++) {
			EXPECT_DOUBLE_EQ(d.getElement(i,j), c.getElement(i,j));
		}
	}
}

} /* namespace GLEhydroDEMtoolsTests */
