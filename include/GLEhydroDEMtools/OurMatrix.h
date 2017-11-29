/*
 * OurMatrix.h
 *
 *  Created on: 24/ago/2012
 *      Author: andrevb
 */

#ifndef HYDRODEMTOOLS_OURMATRIX_H_
#define HYDRODEMTOOLS_OURMATRIX_H_

#include "LandscapeModelEnums.h"
#include <iostream>
#include <vector>
#include <cstring>
#include <cassert>
#include <cmath>

template<class T>
class OurMatrix {
public:
	//Initialize the size of matrix to ROWS by COLS
	OurMatrix();
	OurMatrix(int nr, int nc);
	OurMatrix(DEMDimensionType, const T[]);
	OurMatrix(DEMDimensionType, const std::vector<T>);
	OurMatrix(int nr, int nc, const std::vector<T>);
	/* Passing by value ensure that defaultValue is a copy and its
	 * life responsability is upon this.
	 */
	explicit OurMatrix(int nr, int nc, T defaultValue);
	OurMatrix(const OurMatrix<T>& a);
	~OurMatrix();
	OurMatrix<T>& operator=(const OurMatrix<T> & a);

	OurMatrix<double> toDouble() const;
	bool operator==(const OurMatrix<T>& a) const;
	bool operator!=(const OurMatrix<T>& a) const;

	/* Returns a reference to the object into matrix at (i,j)
	 */
	const T& getElement(int i, int j) const;
	/* Take value and insert a copy of that object into matrix at (i,j)
	 * (which is now responsible of the resource)
	 */
	void setElement(const T& value, int i, int j);
	/* Sum each element of another matrix to the elements of this (they should have
	 * equal dimensions). Returns false if dimensions aren't the same.
	 */
	bool sumMatrix(const OurMatrix<T>& a);
	/* Sum the value contained in a to the elements of this (they should have
	 * equal dimensions). Returns false if dimensions aren't the same.
	 */
	bool sumMatrix(const T& a);
	/* Product of each element of another matrix to the elements of this
	 * (they should have equal dimensions - returns false in that case).
	 */
	bool productMatrixElByEl(const OurMatrix<T>& a);
	bool productMatrixElByEl(const T& a);
	bool divisionMatrixElByEl(const OurMatrix<T>& a);

	bool squareRootElByEl(); // square root (only if T is a float)
	bool powerElByEl(int power);
	/* returns the sum of all elements: requires T.operator+=
	 */
	T sumAll();

	/* Mean value of values contained in the matrix */
	double meanOfEls();
	/* Variance of values contained in the matrix, as the second central
	 * moment (called the biased sample variance).
	 */
	double varianceOfEls();
	/**
	 * if (el is smaller than threshold) then set el to valueToSet
	 */
	bool ifThenSet(const T& threshold, const T& valueToSet);

	DEMDimensionType getDimension() const;
	void readMatrix(); //Reading elements from the keyboard for the matrix
#ifdef DEBUG
	void printMatrix() const; //Prints the elements of a matrix
public:
#else
private:
#endif
	/**
	 * 0 1 2
	 * 3 4 5
	 * 6 7 8
	 */
	T* data;
	long int nr;
	long int nc;
	long int numberOfZero;
	bool checkDimension(const OurMatrix<T>& a); // check if dimension of a is the same as this.
};

namespace OurMatrixInternal {
// square root element by element - partial specialization outside class scope
template<class T> inline bool squareRootElByEl_part(OurMatrix<T>& a) {
	return false;
}

// specialization for float
template<> inline bool squareRootElByEl_part<double>(OurMatrix<double>& a) {
	double b;
	for (unsigned int i = 0; i < a.getDimension().n_rows; i++) {
		for (unsigned int j = 0; j < a.getDimension().n_columns; j++) {
			b = (sqrt(a.getElement(i, j)));
			a.setElement(b, i, j);
		}
	}
	return true;
}

template<class T> inline OurMatrix<double> toDouble(const OurMatrix<T>& a) {
	DEMDimensionType dim = a.getDimension();
	OurMatrix<double> newMatrix(dim.n_rows, dim.n_columns);
	for (unsigned int i = 0; i < a.getDimension().n_rows; i++) {
		for (unsigned int j = 0; j < a.getDimension().n_columns; j++) {
			newMatrix.setElement(((double) a.getElement(i, j)), i, j);
		}
	}
	return newMatrix;
}
template<> inline OurMatrix<double> toDouble<double>(
		const OurMatrix<double>& a) {
	return OurMatrix<double>(a);
}
}

//Constructor
// this methods relies on the fact that T() returns a value which is
// considered in the program as a Zero (that means, equal or below zero)
template<class T>
OurMatrix<T>::OurMatrix() :
		nr(10), nc(10), numberOfZero(nr * nc) {
	data = new T[nr * nc];
	for (int i = 0; i < nr * nc; i++) {
		data[i] = 0;
	}
}

template<class T>
OurMatrix<T>::OurMatrix(int nr_a, int nc_a) :
		nr(nr_a), nc(nc_a), numberOfZero(T() <= 0 ? nr * nc : 0) {
	data = new T[nr * nc];
	for (int i = 0; i < nr * nc; i++) {
		data[i] = T();
	}
}

template<class T>
OurMatrix<T>::OurMatrix(int nr_a, int nc_a, T defaultValue) :
		nr(nr_a), nc(nc_a), numberOfZero(defaultValue <= 0 ? nr * nc : 0) {
	data = new T[nr * nc];
	for (int i = 0; i < nr * nc; i++) {
		data[i] = defaultValue;
	}
}

template<class T>
OurMatrix<T>::OurMatrix(DEMDimensionType dim, const T vals[]) :
		nr(dim.n_rows), nc(dim.n_columns), numberOfZero(0) {
	data = new T[nr * nc];
	for (int i = 0; i < nr * nc; i++) {
		data[i] = vals[i];
		if (data[i] <= 0) {
			numberOfZero++;
		}
	}
}

template<class T>
OurMatrix<T>::OurMatrix(DEMDimensionType dim, const std::vector<T> vals) :
		nr(dim.n_rows), nc(dim.n_columns), numberOfZero(0) {
	data = new T[nr * nc];
	for (int i = 0; i < nr * nc; i++) {
		data[i] = vals[i];
		if (data[i] <= 0) {
			numberOfZero++;
		}
	}
}

template<class T>
OurMatrix<T>::OurMatrix(int nr_a, int nc_a, const std::vector<T> vals) :
		nr(nr_a), nc(nc_a), numberOfZero(0) {
	data = new T[nr * nc];
	for (int i = 0; i < nr * nc; i++) {
		data[i] = vals[i];
		if (data[i] <= 0) {
			numberOfZero++;
		}
	}
}

template<class T>
OurMatrix<T>::OurMatrix(const OurMatrix<T>& a) :
		nr(a.nr), nc(a.nc), numberOfZero(a.numberOfZero) {
	data = new T[nr * nc];
	for (int i = 0; i < nr * nc; i++) {
		data[i] = a.data[i];
	}
}

template<class T>
OurMatrix<T>::~OurMatrix() {
	if (data != NULL) {
		delete[] data;
	}
}

template<class T>
OurMatrix<T>& OurMatrix<T>::operator=(const OurMatrix<T> & a) {
	// check self assignment
	if (this == &a) {
		return *this;
	}
	if (nr == a.nr && nc == a.nc) {
		// don't deallocate, no need to do that
		// the chunk pointed by data should already be nr*nc*sizeof(T)
		numberOfZero = a.numberOfZero;
		for (int i = 0; i < nr * nc; i++) {
			data[i] = a.data[i];
		}
		return *this;
	}
	// deallocate memory used by this
	if (data != NULL) {
		delete[] data;
	}
	// copy every field
	data = new T[nr * nc];
	nr = a.nr;
	nc = a.nc;
	numberOfZero = a.numberOfZero;
	for (int i = 0; i < nr * nc; i++) {
		data[i] = a.data[i];
	}
	return *this;
}

template<class T>
bool OurMatrix<T>::operator==(const OurMatrix<T>& a) const {
	// check self
	if (this == &a) {
		return true;
	}
	if (nr != a.nr) {
		return false;
	}
	if (nc != a.nc) {
		return false;
	}
	if (numberOfZero != a.numberOfZero) {
		return false;
	}
	bool res = true;
	for (unsigned int i = 0; i < a.getDimension().n_rows; i++) {
		for (unsigned int j = 0; j < a.getDimension().n_columns; j++) {
			res = res && (getElement(i, j) == a.getElement(i, j));
		}
	}
	for (int i = 0; i < (nr * nc); i++) {
		res = res && (data[i] == a.data[i]);
	}
	return res;
}

template<class T>
bool OurMatrix<T>::operator!=(const OurMatrix<T>& a) const {
	return !(*this == a);
}

template<class T>
OurMatrix<double> OurMatrix<T>::toDouble() const {
	return OurMatrixInternal::toDouble(*this);
}

// returns a reference to the obj into matrix at (i,j)
template<class T>
const T& OurMatrix<T>::getElement(int i, int j) const {
#ifdef DEBUG
	assert(i > -1 && i < nr && j > -1 && j < nc);
#endif
	return data[i * nc + j];
}

//Take the value and insert a copy of it into matrix at (i,j)
template<class T>
void OurMatrix<T>::setElement(const T& value, int i, int j) {
#ifdef DEBUG
	assert(i > -1 && i < nr && j > -1 && j < nc);
#endif
	if (data[i * nc + j] <= 0 && value > 0) {
		numberOfZero--;
	} else if (data[i * nc + j] > 0 && value <= 0) {
		numberOfZero++;
	}
	data[i * nc + j] = value;
}

//Sum a matrix to this
template<class T>
bool OurMatrix<T>::sumMatrix(const OurMatrix<T>& a) {
	if (checkDimension(a)) {
		numberOfZero = 0;
		for (int i = 0; i < nr * nc; i++) {
			data[i] += a.data[i];
			if (data[i] <= 0) {
				numberOfZero++;
			}
		}
		return true;
	}
	return false;
}

//Sum an element to this
template<class T>
bool OurMatrix<T>::sumMatrix(const T& a) {
	numberOfZero = 0;
	for (int i = 0; i < nr * nc; i++) {
		data[i] += a;
		if (data[i] <= 0) {
			numberOfZero++;
		}
	}
	return true;
}

//Product a matrix to this
template<class T>
bool OurMatrix<T>::productMatrixElByEl(const OurMatrix<T>& a) {
	numberOfZero = 0;
	if (checkDimension(a)) {
		for (int i = 0; i < nr * nc; i++) {
			data[i] *= a.data[i];
			if (data[i] <= 0) {
				numberOfZero++;
			}
		}
		return true;
	}
	return false;
}

//Scalar product
template<class T>
bool OurMatrix<T>::productMatrixElByEl(const T& a) {
	if (a <= 0) {
		numberOfZero = nr * nc;
	}
	for (int i = 0; i < nr * nc; i++) {
		data[i] *= a;
	}
	return true;
}

//Divide a matrix by this
template<class T>
bool OurMatrix<T>::divisionMatrixElByEl(const OurMatrix<T>& a) {
	numberOfZero = 0;
	if (checkDimension(a)) {
		for (int i = 0; i < nr * nc; i++) {
			if (a.data[i] == 0) {
				data[i] = 0;
				numberOfZero++;
			} else {
				data[i] /= a.data[i];
				if (data[i] <= 0) {
					numberOfZero++;
				}
			}
		}
		return true;
	}
	return false;
}

template<class T>
bool OurMatrix<T>::squareRootElByEl() {
	return OurMatrixInternal::squareRootElByEl_part(*this);
}

template<class T>
bool OurMatrix<T>::powerElByEl(int exp) {
	for (int i = 0; i < nr * nc; i++) {
		data[i] = pow(data[i], exp);
	}
	return true;
}

//return the sum of all elements
// TODO partial specialize this one so checks if sum is going overflow
template<class T>
T OurMatrix<T>::sumAll() {
	T sum = 0;
	for (int i = 0; i < nr * nc; i++) {
		sum += data[i];
	}
	return sum;
}

// mean of values
template<class T>
double OurMatrix<T>::meanOfEls() {
	if (numberOfZero >= nr * nc) {
		return 0.0;
	}
	return (sumAll() / (nr * nc - numberOfZero));
}

// variance of values
template<class T>
double OurMatrix<T>::varianceOfEls() {
	if (numberOfZero >= nr * nc) {
		return 0.0;
	}
	double mean(meanOfEls());
	OurMatrix<double> result(this->toDouble());
	result.sumMatrix(-mean);
	for (unsigned int j = 0; j < getDimension().n_columns; j++) {
		for (unsigned int i = 0; i < getDimension().n_columns; i++) {
			if (result.getElement(i, j) == -mean) {
				result.setElement(0, i, j);
			}
		}
	}
	result.powerElByEl(2);

	/* variance */
	return (result.sumAll() / (nr * nc - numberOfZero));
}

template<class T>
bool OurMatrix<T>::ifThenSet(const T& threshold, const T& valueToSet) {
	for (unsigned int i = 0; i < nr; i++) {
		for (unsigned int j = 0; j < nc; j++) {
			if (getElement(i,j) < threshold) {
				setElement(valueToSet, i, j);
			}
		}
	}
	return true;
}

// getter of dimensions.
template<class T>
DEMDimensionType OurMatrix<T>::getDimension() const {
	DEMDimensionType dim(nr, nc);
	return dim;
}

//Reading elements from the keyboard for the matrix
template<class T>
void OurMatrix<T>::readMatrix() {
	T value;
	for (int i = 0; i < nr; i++) {
		for (int j = 0; j < nc; j++) {
			std::cin >> value;
			setElement(value, i, j);
		}
	}
}

#ifdef DEBUG
//Printing elements in the matrix
template<class T>
void OurMatrix<T>::printMatrix() const {
	std::cout << "wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww" << std::endl;
	for (int i = 0; i < nr; i++) {
		for (int j = 0; j < nc; j++) {
			std::cout << getElement(i, j) << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm" << std::endl;
}
#endif

//Check if a has dimension of this
template<class T>
bool OurMatrix<T>::checkDimension(const OurMatrix<T>& a) {
	if ((a.nc == nc) && (a.nr == nr)) {
		return true;
	}
	return false;
}
#endif  /* HYDRODEMTOOLS_OURMATRIX_H_ */
