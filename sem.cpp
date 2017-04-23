/* Nathaniel Want (nwqk6)
 * CS4280-E02
 * P4
 * 4/24/2017
 * -----------
 *  sem.cpp
 * --------
 * The implemenation for functions 
 * pertaining to handling the static semantics in the 
 * back end of the compiler
 */
#include <iostream>
#include <vector>
#include "node.h"
#include "errors.h"
#include "ST.h"
#include "token.h"
using namespace std;

static ST STV = ST();								// the symbol table "stack" container
static vector<int> varsCount;				// counters for variable counts in each scope. Last element in vector is for current scope.

/**isBlock
 * -------
 * determine if the node a block node
 *
 * inputs:
 * node....the node we are checking
 *
 * returns true if the provided node is a block, else false.
 */
static bool isBlock(node_t* node) {
	return node->label == "<block>";
}


/**isOrUnderVars
 * -------
 * determine if the node is a Vars node, or under a Vars node
 *
 * inputs:
 * node....the node we are checking
 *
 * returns true if the provided node is a Vars node or under a Vars node, else false.
 */
static bool inOrUnderVars(node_t* node) {
	return node->label == "<vars>" || node->label == "<mVars>";
}


/**insert
 * -------
 * attempt to insert an id the symbol table. Throws error if the same token instance
 * has already been declared in this scope or if the stack overflows.
 *
 * inputs:
 * idtk...the IDtk to insert into the symbol table
 */
static void insert(token_t* idtk) {
	if (varsCount.back() == 0                        // have 0 variables been declared in current block (or program if not yet in block)?
				|| STV.find(*idtk) == -1                   // is this variable undeclared in the entire program?
        || STV.find(*idtk) >= varsCount.back()) {  // is this variable undeclared in the current block (scope) or program if not yet in block?
		STV.push(*idtk);														   // yes to one or move of the above; ok. add to symbol table and increase the var count for this scope.
		varsCount.back()++;
	} else 																					 // no to all; throw error and quit. Illegal static semantics
		semError(*idtk, "Variable already declared in same scope."); 
}


/**verify
 * -------
 * Verify that an IDtk instance has already been declared in program. Throws
 * error if the IDtk instance has not been declared in the program.
 *
 * inputs:
 * idtk...the IDtk to verify
 */
static void verify(token_t* idtk) {
	if (STV.find(*idtk) == -1)										// has this token been declared at some scope of the program?
		semError(*idtk, "Variable is undeclared");	// no; throw error and quit. Illegal static semantics.	
																								// else; yes. return without error.
}


/**handleToken
 * -------
 * Handle a token by verifying any static semantics necessary or 
 * storing any information contained in this token neceesary for 
 * verifying static semantics in the rest of the program. Throw 
 * any errors if static semantics have been violated if/when
 * they occur.
 *
 * inputs:
 * tk...the token to handle
 */
static void handleToken(token_t* tk, bool underVars) {
	if (tk == NULL || tk->tokenID != IDtk)  	// do we need to check static semantics for this token?
		return;																	// no; do nothing.
	else if (underVars)												// yes; are we in or under a Vars node?
		insert(tk);															// yes; attempt to add token to symbol table 
	else 
		verify(tk);															// no; make sure this token exists in the symbol table
}


/**processNode
 *------------
 * verify static semantics for a node by handling all tokens in the node. 
 * Throw any errors that may occur in the process. 
 *
 * inputs:
 * node...the node to process.
 */
static void processNode(node_t* node) {
	handleToken(node->token1, inOrUnderVars(node));
	handleToken(node->token2, inOrUnderVars(node));
}


/**processSubTree
 *----------------
 * verify static sematics all of the node in the (sub)tree.
 * 
 * inputs:
 * root....the root of the (sub)tree to process
 */
static void processSubTree(node_t* root) {
	if (root == NULL)															// is there anything to process?
			return;																		// no; do nothing.

	if (isBlock(root)) {       										// have we entered a new block?
		varsCount.push_back(0);  										// yes; start a new varible count for this scope.
	}
	
	// process in left-to-right (PreOrder) traversal
	processNode(root);
	processSubTree(root->child1);
	processSubTree(root->child2);
	processSubTree(root->child3);
	processSubTree(root->child4);

	if (isBlock(root)) {													// are we leaving a block? (reverting to a higher scope)
		for (int i = 0; i < varsCount.back(); i++)	// yes; remove variables for scope from symbol table and end scope variable count.
			STV.pop();								
		varsCount.pop_back();
	}
}


/**sem
 *----
 * Confirm that a parse tree has not violated any static sematics. Throw an error and raise a SIGINT
 * signal if an errors occur. 
 * 
 * inputs:
 * root....the root of the parse tree to verify the static sematics for. 
 */
void sem(node_t* root) {
	varsCount.push_back(0);  // start variable count for global variables
	processSubTree(root);
}		

