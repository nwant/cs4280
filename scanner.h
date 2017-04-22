/* Nathaniel Want (nwqk6)
 * CS4280-E02
 * P3
 * 4/24/2017
 * -----------
 * scanner.h
 * --------
 * declaration/interface for the scanner module
 */
#ifndef SCANNER_H
#define SCANNER_H

#include <fstream>
#include "token.h"

token_t scanner(ifstream& input);

#endif
