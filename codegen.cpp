/* Nathaniel Want (nwqk6)
 * CS4280-E02
 * P4
 * 4/24/2017
 * -----------
 *  codegen.cpp
 * --------
 */
#include <iostream>
#include <vector>
#include "node.h"
#include "errors.h"
#include "ST.h"
#include "token.h"
#include "codegen.h"
using namespace std;

static ST STV = ST();								// the symbol table "stack" container
static vector<int> varsCount;				// counters for variable counts in each scope. Last element in vector is for current scope.

/*------------------- helper function prototypes ----------------------*/
static void processSubTree(const node_t* root, ostream&);
static void processNode(const node_t* node, ostream&);
static void processProgram(const node_t*, ostream&);
static void processVars(const node_t*, ostream&);
static void processBlock(const node_t*, ostream&);
static void processExpr(const node_t*, ostream&);
static void processM(const node_t*, ostream&);
static void processT(const node_t*, ostream&);
static void processF(const node_t*, ostream&);
static void processR(const node_t*, ostream&);
static void processStats(const node_t*, ostream&);
static void processMStat(const node_t*, ostream&);
static void processStat(const node_t*, ostream&);
static void processIn(const node_t*, ostream&);
static void processOut(const node_t*, ostream&);
static void processIf(const node_t*, ostream&);
static void processLoop(const node_t*, ostream&);
static void processRO(const node_t*, ostream&);
static void processAssign(const node_t*, ostream&);
static void printlabel(const string, ostream&);
/*----------------------------------------------------------------------*/

void codegen(node_t* root, ostream& os) {
	varsCount.push_back(0);  // start variable count for global variables
	processSubTree(root, os);
}

static void processProgram(const node_t* programNode, ostream& os) {
	printlabel("PROGRAM", os);
	processVars(programNode->child1, os);
	processBlock(programNode->child2, os);
}

static void processVars(const node_t* varsNode, ostream& os) {
	
}

static void processBlock(const node_t* blockNode, ostream& os) {
	printlabel("BLOCK", os);
	processVars(blockNode->child1, os);
	processBlock(blockNode->child2, os);
}

static void processExpr(const node_t* exprNode, ostream& os) {
	//TODO: verify 
	printlabel("EXPR", os);
}

/**isBlock
 * -------
 * determine if the node a block node
 *
 * inputs:
 * node....the node we are checking
 *
 * returns true if the provided node is a block, else false.
 */
static bool isBlock(const node_t* node) {
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
static bool inOrUnderVars(const node_t* node) {
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
static void processNode(const node_t* node, ostream& os) {
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
static void processSubTree(const node_t* root, ostream& os) {
	if (root == NULL)															// is there anything to process?
			return;																		// no; do nothing.

	if (isBlock(root)) {       										// have we entered a new block?
		varsCount.push_back(0);  										// yes; start a new varible count for this scope.
	}
	
	// process in left-to-right (PreOrder) traversal
	processNode(root, os);
	processSubTree(root->child1, os);
	processSubTree(root->child2, os);
	processSubTree(root->child3, os);
	processSubTree(root->child4, os);

	if (isBlock(root)) {													// are we leaving a block? (reverting to a higher scope)
		for (int i = 0; i < varsCount.back(); i++)	// yes; remove variables for scope from symbol table and end scope variable count.
			STV.pop();								
		varsCount.pop_back();
	}
}


static void processM(const node_t* mNode, ostream& os) {
	//TODO: verify 
	printlabel("M", os);
	processT(mNode->child1, os);
	if (mNode->child1 != NULL) {
																			//TODO: create a temp variable
		os << "STORE temp" << endl;  			//TODO change temp name?
		processM(mNode->child2, os);
		os << "SUB temp" << endl;					//TODO: change temp name? 
	}
}


static void processT(const node_t* tNode, ostream& os) {
	printlabel("T", os);
	if (tNode->token1 == NULL) { 						// case: <T> -> <F> *|/ <T>
		processT(tNode->child2, os);		 			// 	ACC <- <T> result 
		// create a temp variable							//TODO: create a temp variable
		os << "STORE temp" << endl; 					//TODO: change temp name?
		processF(tNode->child1, os);					// 	ACC <- <F> result
		if (tNode->token1->tokenID == STARtk)	// 	case: <T> -> <F> * <T>
			os << "MULTI temp" << endl;					// TODO: change temp name
		else																	// 	case: <T> -> <F> / <T>
			os << "DIV temp" << endl;						// TODO: change temp name?
	} else {										 						// case: <T> -> <T>
		processT(tNode->child1, os);	 				// 	ACC <- <T> result
	}
}

static void processF(const node_t* fNode, ostream& os) {
	printlabel("F", os);
	if (fNode->token1 == NULL) { 			// case: <F> -> <R>
		processR(fNode->child1, os);	 	// 	ACC <- <R> result
	} else { 										 			// case: <F> -> - <F>
		processF(fNode->child1, os);   	// 	ACC <- <F> result
		os << "MULTI -1" << endl;  			// 	ACC <- ACC * -1
	}
}


static void processR(const node_t* rNode, ostream& os) {
	printlabel("R", os);
	if (rNode->token1 == NULL) { 															// case: <R> -> <expr>
		processExpr(rNode->child1, os); 												// 	ACC <- <expr> result
	} else {																								 	// case: <R> -> id|#
		os << "LOAD " << rNode->token1->tokenInstance << endl; 	// 	ACC <- id|#
	}
}

static void processStats(const node_t* statsNode, ostream& os) {
	//TODO: verify
	printlabel("STATS", os);
	processStat(statsNode->child1, os);
	processMStat(statsNode->child2, os);
}


static void processMStat(const node_t* mStatNode, ostream& os) {
	//TODO: verify
	printlabel("MSTATS", os);
	if (mStatNode->child1 != NULL) {
		processStat(mStatNode->child1, os);
		processMStat(mStatNode->child2, os);
	}
}


static void processStat(const node_t* statNode, ostream& os) {
	printlabel("STAT", os);
	string childLabel = statNode->child1->label;

	if (childLabel == "<in>")								// case: <stat> -> <in>
		processIn(statNode->child1, os); 			// 	ACC <- <in> result
	else if (childLabel == "<out>")					// case: <stat> -> <out>
		processOut(statNode->child1, os);			// 	ACC <- <out> result
	else if (childLabel == "<block>")				// case: <stat> -> <block>
		processBlock(statNode->child1, os);		// 	ACC <- <block> result
	else if (childLabel == "<if>")					// case: <stat> -> <if>
		processIf(statNode->child1, os);			// 	ACC <- <if> result
	else if (childLabel == "<loop>")				// case: <stat> -> <loop>
		processLoop(statNode->child1, os);		// 	ACC <- <loop> result
	else if (childLabel == "<assign>")			// case: <stat> -> <assign>
		processAssign(statNode->child1, os);	// 	ACC <- <assign> result
}


static void processIn(const node_t* inNode, ostream& os) {
	printlabel("IN", os);
	os << "READ " << inNode->token1->tokenInstance << endl;		// id <- Input I/O
}

static void processOut(const node_t* outNode, ostream& os) {
	printlabel("OUT", os);
	processExpr(outNode->child1, os); 		// ACC <- <expr> result

	// create instructions to place the expr result (in ACC) to memory 
	//TODO: create a temp variable
	os << "STORE temp" << endl; //TODO: change temp name?
	// create instructions to output this value
	os << "WRITE temp" << endl; //TODO: change temp name?
}

static void processIf(const node_t* ifNode, ostream& os) {

}

static void processLoop(const node_t* ifNode, ostream& os) {

}

static void processAssign(const node_t* assignNode, ostream& os) {
	printlabel("ASSIGN", os);
	processExpr(assignNode->child1, os); 												 // ACC <- <expr> result
	os << "STORE " << assignNode->token1->tokenInstance << endl; // id <- ACC 
}

static void processRO(const node_t* roNode, ostream& os) {

}

static void printlabel(const string label, ostream& os) {
	os << label << ": NOOP" << endl;
}
