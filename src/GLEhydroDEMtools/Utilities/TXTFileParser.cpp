/*
 * TXTFileParser.cpp
 *
 *  Created on: 10/set/2012
 *      Author: lordmzn
 */
#include "TXTFileParser.h"
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>

TXTFileParser::TXTFileParser(char* del, GLE::ReadingOptions opt) :
		DELIMITER(del), WILDCARD('#'), option(opt) {
}

TXTFileParser::TXTFileParser(const char* const del, GLE::ReadingOptions opt) :
		DELIMITER(del), WILDCARD('#'), option(opt) {
}

TXTFileParser::TXTFileParser(char* del, char wild, GLE::ReadingOptions opt) :
		DELIMITER(del), WILDCARD(wild), option(opt) {
}

TXTFileParser::TXTFileParser(const char* const del, char wild,
		GLE::ReadingOptions opt) :
		DELIMITER(del), WILDCARD(wild), option(opt) {
}

bool TXTFileParser::parseByLine(std::string s) {
	// create a file-reading object
	std::ifstream inputFile;
	inputFile.open(s.c_str()); // open a file
	if (!inputFile) {
		return false; // exit if file not found
	}
	// read each line of the file
	std::string line;
	while (std::getline(inputFile, line)) {
		if (!line.empty()) {
			if (line.at(0) != WILDCARD) {
				switch (option) {
				case GLE::COMMON_FIELD: {
					commonFieldLineParser(line);
					break;
				}
				case GLE::MATRIX_LINE: {
					numericalMatrixLineParser(line);
					break;
				}
				}
			}
		}
	}
	inputFile.close();
	return true;
}

#ifdef DEBUG
void TXTFileParser::printTokens() {
	for (std::vector<std::string>::size_type i = 0; i < tokens.size(); i++) {
		std::cout << "Token [" << i << "] : " << tokens[i] << std::endl;
	}
	std::cout << std::endl;
}
#endif

std::vector<std::string>& TXTFileParser::getTokens() {
	return tokens;
}

void TXTFileParser::commonFieldLineParser(std::string line) {
	// parse the line into blank-delimited tokens
	size_t startToken = 0;
	startToken = line.find(DELIMITER, startToken);
	tokens.push_back(std::string(line, 0, startToken));
	tokens.push_back(std::string(line, startToken + strlen(DELIMITER)));

}

void TXTFileParser::numericalMatrixLineParser(std::string line) {
	size_t startCharacter = 0;
	size_t endCharacter = line.find(DELIMITER, startCharacter);
	while (endCharacter < line.npos) {
		tokens.push_back(
				std::string(line, startCharacter,
						(endCharacter - startCharacter)));
		startCharacter = endCharacter + strlen(DELIMITER);
		endCharacter = line.find(DELIMITER, startCharacter);
	}
}
