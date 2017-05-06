/* Nathaniel Want (nwqk6)
 * CS4280-E02
 * P4
 * 5/9/2017
 * -----------
 *  SysStack.cpp
 * --------
 *  Contains the implementation for the virtual "leveled" system stack.
 *  Levels are used as an abstraction to signify boundaries between scopes.
 *
 */
#include <string>
#include <vector>
#include "errors.h"
#include "SysStack.h"
#include "token.h"
using namespace std;

static const int STACK_LIMIT = 100;   // the max size the symbol table can grow.

/* contructor */
SysStack::SysStack() {
	addLevel();		
	this->tos = -1;
}

/**SysStack::addLevel
 *-------------------
 * add a new level to the system stack
 */
void SysStack::addLevel() {
	this->varsCount.push_back(0);
}

/**SysStack::removeLevel
 *-----------------------
 * remove the top level from the system stack. 
 */
void SysStack::removeLevel() {
	for (int i=0; i < varsInLevel(); i++)
		pop();
	this->varsCount.pop_back();
}


/**SysStack::varsInLevel
 *-----------------------
 * get the number of variable in the current (top) level
 *
 * return the nubmer of varialbe in the current (top) level
 */
int SysStack::varsInLevel() {
	return this->varsCount.back();
}


/**SysStack::push
 *---------
 * Push a token on top of the system stack Throw an error if a stack overflow occurs.
 * 
 * inputs:
 * tk.....the token to add to the top of system stack..
 */
void SysStack::push(token_t tk) {
	if (this->stack.size() + 1 > STACK_LIMIT)
		semError(tk, "Stack overflow. Programs cannot exceed more than " + to_string(STACK_LIMIT) + " items.");
	
	this->stack.push_back(*copyToken(tk));	
	this->varsCount.back()++;
	this->tos++;
}


/**SysStack::pop
 *---------
 * Remove the top token system stack.
 */
void SysStack::pop() {
	this->stack.pop_back();
	this->varsCount.back()--;
	this->tos--;
}


/**SysStack::find
 *---------
 * Look (from top to bottom) for an token in the system stack (all levels) and return its distance of
 * the token's first occurance from the top of the stack or -1 if no match is found.
 *
 * inputs:
 * tk....the token to look for in the symbol table.
 * 
 * Returns the location of the token from the in the symbol table or -1 if no match is found.
 */
int SysStack::find(const token_t tk) {
	// find the first instance of this token instance, starting from the top of the stack
	for (int i=0; i < this->stack.size(); i++)
		if (this->stack.at(i).tokenID == tk.tokenID && this->stack.at(i).tokenInstance == tk.tokenInstance)
			return i;

	return -1;
}


/**SysStack::isInLevel
 *---------
 * Determine if a token is in the current (topmost) level of the system stack.
 *
 * inputs:
 * tk....the token to look for current (topmost) level of the system stack.
 * 
 * returns true if the token is the current (topmost) level of the system stack, else false.
 */
bool SysStack::isInLevel(const token_t tk) {
	// compare starting with the top of stack
	for (int i=0; i < varsInLevel(); i++) {
		if (this->stack.at(this->tos - i).tokenID == tk.tokenID && this->stack.at(this->tos - i).tokenInstance == tk.tokenInstance)
			return true;
	}
	
	return false;
}
