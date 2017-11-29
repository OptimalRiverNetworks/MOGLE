/*
 * Stepcost.h
 *
 *  Created on: 22/ago/2012
 *      Author: lordmzn
 */

#ifndef HYDRODEMTOOLS_STEPCOST_H_
#define HYDRODEMTOOLS_STEPCOST_H_

/* Generic data type that represents the stepcost of a dynamic model.
 */
class Stepcost {
private:
	double stepcost;
protected:
	static const double STEPCOST_INIT_VALUE = -2.0;
	static const double DISCHARGE_IDENTITY_VALUE = 0.0;
	void setStepcost(double val) {
		stepcost = val;
	}
	double getStepcost_prot() {
		return stepcost;
	}
public:
	Stepcost() :
			stepcost(STEPCOST_INIT_VALUE) {
	}
	virtual ~Stepcost() {

	}
	/* returns a copy of the value of the stepcost (evaluates it if necessary) */
	virtual double getStepcost() {
		return stepcost;
	}
};

#endif /* HYDRODEMTOOLS_STEPCOST_H_ */
