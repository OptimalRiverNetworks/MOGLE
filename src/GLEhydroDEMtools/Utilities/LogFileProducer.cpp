/*
 * LogFileProducer.cpp
 *
 *  Created on: 14/set/2012
 *      Author: lordmzn
 */
#include "LogFileProducer.h"
#include <sstream>
#include <ctime>
#include <iostream>

LogFileProducer* LogFileProducer::m_pInstance = NULL;

LogFileProducer* LogFileProducer::Instance() {
	std::stringstream str2;
	time_t rawtime;
	time(&rawtime);
	str2 << &rawtime;
	return Instance(str2.str());
}

LogFileProducer* LogFileProducer::Instance(std::string fileId) {
	if (!m_pInstance) { // Only allow one instance of class to be generated.
		m_pInstance = new LogFileProducer("GLEprogress." + std::string(fileId) + ".log");
	}
	return m_pInstance;
}

LogFileProducer::LogFileProducer(std::string filename) {
	startClock = clock();
	time(&startTime);
	if (!logFile.openFile(filename)) {
		std::cerr << "Error in opening logFile! Unable to log anything"
				<< std::endl;
	} else {
		log(std::string("Start log file"));
	}
}

bool LogFileProducer::closeLogFile() {
	log(std::string("Closing log"));
	if (logFile.closeFile()) {
		return true;
	}
	return false;
}

void LogFileProducer::log(const std::string msg) {
	std::ostringstream strTime("");
	std::stringstream str2;
	time_t rawtime;
	time(&rawtime);
	str2 << ctime(&rawtime);
	strTime << str2.str().substr(0, str2.str().length() - 1) << ", ("
			<< (clock() - startClock) << " clock)";
	std::string formattedMsg;
	formattedMsg.append(strTime.str());
	formattedMsg.append(":\t");
	formattedMsg.append(msg);
	logFile.writeLine(formattedMsg);
}

void LogFileProducer::log(char* msg) {
	log(std::string(msg));
}

std::string LogFileProducer::getTimeElapsed() const {
	time_t rawtime;
	time(&rawtime);
	rawtime = rawtime - startTime;
	std::stringstream str2;
	str2 << ctime(&rawtime);
	return str2.str().substr(0, str2.str().length() - 1);
}
