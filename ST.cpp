/* Nathaniel Want (nwqk6)
 * CS4280-E02
 * P3
 * 4/24/2017
 * -----------
 *  ST.cpp
 * --------
 *  Contains the implementation for the symbol table container 
 *
 */
#include <string>
#include <vector>
#include "errors.h"
#include "ST.h"
#include "sem.h"
#include "token.h"
using namespace std;

static const int STACK_LIMIT = 100;   // the max size the symbol table can grow.


/**ST::push
 *---------
 * Push a token on top of the symbol table. Throw an error if any
 * semantical errors occur.
 * 
 * inputs:
 * tk.....the token to add to the symbol table.
 */
void ST::push(token_t tk) {
	if (this->stack.size() + 1 > STACK_LIMIT)
		semError(tk, "Stack overflow. Programs cannot exceed more than " + to_string(STACK_LIMIT) + " items.");
	
	this->stack.push_back(*copyToken(tk));	
}


/**ST::pop
 *---------
 * Remove the top token from the symbol table.
 */
void ST::pop() {
	this->stack.pop_back();
}


/**ST::find
 *---------
 * Look (from top to bottom) for an token in the symbol table and return its distance of
 * the token's first occurance from the top of the stack or -1 if no match is found.
 *
 * inputs:
 * tk....the token to look for in the symbol table.
 * 
 * Returns the location of the token from the in the symbol table or -1 if no match is found.
 */
int ST::find(const token_t tk) {
	// find the first instance of this token instance, starting from the top of the stack
	//for (int i=stack.size() - 1; i >= 0; i--)
	for (int i=0; i < this->stack.size(); i++)
		if (this->stack.at(i).tokenID == tk.tokenID && this->stack.at(i).tokenInstance == tk.tokenInstance)
			return i;

	return -1;
}

