/*
 * TXTFileParser.h
 *
 *  Created on: 10/set/2012
 *      Author: lordmzn
 */

#ifndef TXTFILEPARSER_H_
#define TXTFILEPARSER_H_

#include "../LandscapeModelEnums.h"
#include <vector>
#include <string>

const int MAX_CHARS_PER_LINE = 1024;
const int MAX_TOKENS_PER_LINE = 512;

class TXTFileParser {
public:
	TXTFileParser(char* delimiter, GLE::ReadingOptions);
	TXTFileParser(const char* const delimiter, GLE::ReadingOptions);
	TXTFileParser(char* delimiter, char wildcard, GLE::ReadingOptions);
	TXTFileParser(const char* const delimiter, char wildcard,
			GLE::ReadingOptions);
	/**
	 * Open the given file and read it line by line. It pass each
	 * line to commonFieldLineParser.
	 */
	bool parseByLine(std::string s); // could become parseByLine(char* filename, ParsingType type);
	void printTokens();
	std::vector<std::string>& getTokens();

private:
	const char* const DELIMITER;
	/**
	 * @param A character used to indicate comment lines.
	 */
	const char WILDCARD;
	const GLE::ReadingOptions option;
	/**
	 * Array to store the tokens read from the file. Tokens are
	 * just string, split according to the parsing method used.
	 */
	std::vector<std::string> tokens;
	/**
	 * Read one line of the file. If it start with WILDCARD it
	 * discard it, else it split it into two string: one equals
	 * line until 1st occurrence of DELIMITER (not included); the
	 * other equals line after DELIMITER (which is not included
	 * neither here).
	 */
	void commonFieldLineParser(std::string line);
	void numericalMatrixLineParser(std::string line);
};

#endif /* TXTFILEPARSER_H_ */
