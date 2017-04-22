/* Nathaniel Want (nwqk6)
 * CS4280-E02
 * P3
 * 4/24/2017
 * -----------
 *  errors.cpp
 * --------
 * The declarations/prototypes for functions 
 * pertaining to handling the errors generated in the compiler
 */
#include <iostream>
#include <signal.h>
#include <sstream>
#include <string>
#include "errors.h"
#include "token.h"
using namespace std;

/**scanError
 *--------------------
 *
 * inputs:
 * error...the error index for the error maps. This will be the error message printed to the user
 * details.additional details specific to this error.
 */
void scanError(const int lineNumber, const string error, const string details) {
	stringstream ss;
	ss << "Scanning error: [line# " << lineNumber << "] " << error  << " " << details << endl;
	cout << ss.str();
	raise(SIGINT);
}
 
/**parseError
 * -----
 * throw an parsing error.
 *
 * inputs:
 * expected...the name of the token(s) we were expecting
 * received...the token we received instead of the expected token(s). 
 */
void parseError(const string expected, const token_t received)  {  
	cout << "Parsing error: [line# " << received.lineNumber << "] " << "Expected: "  << expected << ", Received: " << getTokenName(received) << endl;
	raise(SIGINT); // signal to caller that parsing error occurred.
}


void semError(const token_t tk, const string msg) {
	cout << "Semantical error: [line# " << tk.lineNumber << "] | TokenInstance: ";
	cout << tk.tokenInstance <<  " -> " <<  msg << endl;
	raise(SIGINT);
}

