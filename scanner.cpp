/* Nathaniel Want (nwqk6)
 * CS4280-E02
 * P4
 * 4/24/2017
 * -----------
 * scanner.cpp
 * --------
 * implementation of the scanner module. Uses an
 * instance of a Scanner class to perform scans when called. 
 */
#include <fstream>
#include <iostream>
#include "scanner.h"
#include "scannerclass.h"
#include "token.h"

static Scanner s = Scanner(); // the internal scanner module 

/**scanner
 * -------
 * get the next token from the provided input	
 *	
 * inputs:
 * input...the stream to get the next token from
 * 
 * returns the next token from the input stream
 */
token_t scanner(ifstream& input) {
	return s.getToken(input);
}

