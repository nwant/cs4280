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

/**ST::insert
 *------------
 * insert a token into the symbol table
 *
 * inputs:
 * tk......the token to insert
 */
void ST::insert(token_t tk) {
	this->table.push_back(tk);
}


/**ST::verify
 *------------
 * verify a token is in the symbol table
 *
 * inputs:
 * tk......the token to verify
 *
 * returns true when the token is in the symbol table, else false
 */
bool ST::verify(token_t tk) {
	for (int i=0; i < this->table.size(); i++)
		if (this->table.at(i).tokenID == tk.tokenID && this->table.at(i).tokenInstance == tk.tokenInstance)
			return true;

	return false;
}


/**ST::getTokens
 *------------
 * get all of the tokens in the symbol table
 *
 * returns all of the tokens in the symbol table
 */
vector<token_t> ST::getTokens() {
	vector<token_t> tokens(this->table);
	return tokens;
}
