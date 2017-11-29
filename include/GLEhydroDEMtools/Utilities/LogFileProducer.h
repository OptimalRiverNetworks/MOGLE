/*
 * LogFileWriter.h
 *
 *  Created on: 14/set/2012
 *      Author: lordmzn
 */

#ifndef LOGFILEPRODUCER_H_
#define LOGFILEPRODUCER_H_

#include "TXTFileWriter.h"
#include <string>

/* to build and mantain the logfile. designed around singleton design pattern */
class LogFileProducer {
public:
	/**
	 * This function is called to create an instance of the class.
	 * Calling the constructor publicly is not allowed. The constructor
	 * is private and is only called by this Instance function.
	 * Being static it's possible to call it without having a LogFileProducer
	 * object.
	 */
	static LogFileProducer* Instance();
	/**
	 * Same as above but specifies a string to be add to the filename
	 */
	static LogFileProducer* Instance(std::string);
	void log(const std::string msg);
	void log(char* msg);
	std::string getTimeElapsed() const;
	bool closeLogFile();

private:
	/* Private so that it can not be called
	 */
	explicit LogFileProducer(std::string);
	/* copy constructor is private too */
	LogFileProducer(LogFileProducer const&) :
			startClock(clock()) {
		time(&startTime);
	}
	/* assignment operator is private too */
	LogFileProducer& operator=(LogFileProducer const& a) {
		LogFileProducer* b = new LogFileProducer(a);
		return *b;
	}
	/**
	 * Global static pointer used to ensure a single instance of the class.
	 */
	static LogFileProducer* m_pInstance;
	TXTFileWriter logFile;
	clock_t startClock;
	time_t startTime;
};

#endif /* LOGFILEPRODUCER_H_ */
