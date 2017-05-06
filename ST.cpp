/* Nathaniel Want (nwqk6)
 * CS4280-E02
 * P4
 * 5/9/2017
 * -----------
 *  ST.cpp
 * --------
 *  Contains the implementation for the symbol table
 */
#include <string>
#include <vector>
#include "ST.h"
#include "token.h"
using namespace std;

ST::ST() { }

void ST::insert(token_t tk) {
	this->table.push_back(tk);
}


bool ST::verify(token_t tk) {
	for (int i=0; i < this->table.size(); i++)
		if (this->table.at(i).tokenID == tk.tokenID && this->table.at(i).tokenInstance == tk.tokenInstance)
			return true;

	return false;
}

vector<token_t> ST::getTokens() {
	vector<token_t> tokens(this->table);
	return tokens;
}
