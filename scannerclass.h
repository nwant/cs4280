/* Nathaniel Want (nwqk6)
 * CS4280-E02
 * P4
 * 4/24/2017
 * -----------
 *  Scanner.h
 * --------
 *  Contains the definition for the scanner class 
 *
 */
#ifndef SCANNER_CLASS_H
#define SCANNER_CLASS_H

#include <fstream>
#include <string>
#include "token.h"
using namespace std;

class Scanner {
	public:
		Scanner();
		token_t getToken(ifstream& input);
	private:
		int lineNumber;
		char thisChar;
		char nextChar;
		bool approachingNewline;
		token_t buildToken(const string, const string);
		int getCharIndex(const char, ifstream&);
		void handleComments(ifstream&);
		void printError(const string, const string);
};

#endif
