/* Nathaniel Want (nwqk6)
 * CS4280-E02
 * P4
 * 5/9/2017
 * -----------
 *  SysStack.h
 * --------
 *  Contains the definition for the system stack
 *
 */
#ifndef SYSSTACK_H
#define SYSSTACK_H

#include <vector>
#include "token.h"
using namespace std;

class SysStack {
	public:
		SysStack();
		void addLevel();
		void removeLevel();
		void push(const token_t tk);
		void pop(void);
		int find(const token_t tk);	
		int varsInLevel();
		bool isInLevel(const token_t tk);
	private:
		int tos;
		vector<token_t> stack;
		vector<int> varsCount;
};

#endif

