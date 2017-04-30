/* Nathaniel Want (nwqk6)
 * CS4280-E02
 * P4
 * 5/9/2017
 * -----------
 *  ST.h
 * --------
 *  Contains the definition for the symbol table
 */
#ifndef ST_H
#define ST_H

#include <vector>
#include "token.h"
using namespace std;

class ST {
	public:
 		ST();
		void insert(token_t);
		bool verify(token_t);
	private:
		vector<token_t> table;		
};

#endif
