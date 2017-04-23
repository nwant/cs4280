/* Nathaniel Want (nwqk6)
 * CS4280-E02
 * P4
 * 4/24/2017
 * -----------
 *  ST.h
 * --------
 *  Contains the definition for the symbol table container 
 *
 */
#ifndef ST_H
#define ST_H

#include <vector>
#include "token.h"
using namespace std;

class ST {
	public:
		//void insert(token_t tk);
	//	bool verify(const token_t tk);
		void push(const token_t tk);
		void pop(void);
		int find(const token_t tk);	
	private:
		vector<token_t> stack;
};

#endif

