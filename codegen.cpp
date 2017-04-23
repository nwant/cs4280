/* Nathaniel Want (nwqk6)
 * CS4280-E02
 * P4
 * 4/24/2017
 * -----------
 * codegen.cpp
 * --------
 * The declarations/prototypes for functions 
 * pertaining to code generation in the compiler back end
 */
#include <fstream>
#include <string>
#include "node.h"
#include "token.h"
#include "codegen.h"
using namespace std;

/*------------------- helper function prototypes ----------------------*/
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
	processProgram(root, os);	
}


/*------------------------ helper functions ----------------------------*/

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
	processM(exprNode->child1, os);			
	if (exprNode->child1 != NULL) {
																			//TODO: create a temp variable
		os << "STORE temp" << endl;  			//TODO change temp name?
		processExpr(exprNode->child2, os);
		os << "ADD temp" << endl;					//TODO: change temp name? 
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
