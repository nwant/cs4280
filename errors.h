/* Nathaniel Want (nwqk6)
 * CS4280-E02
 * P3
 * 4/24/2017
 * -----------
 *  errors.h
 * --------
 * The declarations/prototypes for functions 
 * pertaining to handling the errors generated in the compiler
 */
#ifndef ERRORS_H
#define ERRORS_H

#include <string>
#include "token.h"
using namespace std;

void scanError(const int lineNumber, const string error, const string details);
void parseError(const string expected, const token_t received);   
void semError(const token_t tk, const string msg);

#endif
