/* Nathaniel Want (nwqk6)
 * CS4280-E02
 * P4
 * 4/24/2017
 * -----------
 *  codegen.cpp
 * --------
 */
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "node.h"
#include "errors.h"
#include "ST.h"
#include "SysStack.h"
#include "token.h"
#include "codegen.h"
using namespace std;

typedef enum { VAR, LABEL } nameType; 
typedef enum { LT, LTEQ, GT, GTEQ, EQ, NOTEQ } roType;
typedef enum {VAR_TO_ACC, VAR_TO_IO, ACC_TO_VAR, IO_TO_VAR } dataflow;
static ST STV = ST();														// the symbol table for  
static SysStack sysStack = SysStack();				 	// the symbol table "stack" container
static int varCount = 0;
static int labelCount = 0;
static bool declaringGlobals = true;

/*------------------- helper function prototypes ----------------------*/
static void processProgram(const node_t*, ofstream&);
static void processVars(const node_t*, ofstream&);
static void processMvars(const node_t*, ofstream&);
static void processBlock(const node_t*, ofstream&);
static void processExpr(const node_t*, ofstream&);
static void processM(const node_t*, ofstream&);
static void processT(const node_t*, ofstream&);
static void processF(const node_t*, ofstream&);
static void processR(const node_t*, ofstream&);
static void processStats(const node_t*, ofstream&);
static void processMStat(const node_t*, ofstream&);
static void processStat(const node_t*, ofstream&);
static void processIn(const node_t*, ofstream&);
static void processOut(const node_t*, ofstream&);
static void processIf(const node_t*, ofstream&);
static void processLoop(const node_t*, ofstream&);
static roType processRO(const node_t*, ofstream&);
static void processAssign(const node_t*, ofstream&);
static void printVars(ofstream&);
static void verify(const token_t*);
static void insert(const token_t*, ofstream&);
static bool isGlobal(const token_t*);
static int sysStackOffset(const token_t*);
static string newName(nameType);
static void printDataflow(const token_t*, dataflow, ofstream&);
static void printBranchInstructs(roType, string, ofstream&);
/*----------------------------------------------------------------------*/

void codegen(const node_t* root, ofstream& os) {
	if (root == NULL)
		return;

	processProgram(root, os);
	os << "STOP" << endl;
	printVars(os);
}


static void processProgram(const node_t* programNode, ofstream& os) {
	processVars(programNode->child1, os);
	declaringGlobals = false;
	processBlock(programNode->child2, os);
}

static void processBlock(const node_t* blockNode, ofstream& os) {
	sysStack.addLevel();

	processVars(blockNode->child1, os);
	processStats(blockNode->child2, os);

	for (int i=0; i < sysStack.varsInLevel(); i++)
		os << "POP" << endl;

	sysStack.removeLevel();
}

static void processVars(const node_t* varsNode, ofstream& os) {
	if (varsNode == NULL) {										// case: <vars> -> empty
		return;
	} else {																	// case: <vars> -> id <mvars>
		insert(varsNode->token1, os);
		processMvars(varsNode->child1, os);
	}
}

static void processMvars(const node_t* mvarsNode, ofstream& os) {
	if (mvarsNode == NULL)										// case: <mvars> -> empty
		return;
	else {																		// case: <mvars> -> id <mvars>
		insert(mvarsNode->token1, os);
		processMvars(mvarsNode->child1, os);	
	}
}


static void processExpr(const node_t* exprNode, ofstream& os) {
	if (exprNode->token1 == NULL) {					 // case: <expr> -> <M>	
		processM(exprNode->child1, os);				 // 	ACC <- result <M>	
	} else {																 // case: <expr> -> <M> + <expr>
		processExpr(exprNode->child2, os);		 //		ACC <- result <expr>
		string temp = newName(VAR);
		os << "STORE " << temp << endl;				 //		temp <- ACC (<expr>)
		processM(exprNode->child1, os);				 // 	ACC <- result <M>
		os << "ADD " << temp << endl;					 // 	ACC <- ACC (<M>) + temp (<expr>)
	}
}



static void processM(const node_t* mNode, ofstream& os) {
	if (mNode->token1 == NULL) {						 // case: <M> -> <T>
		processT(mNode->child1, os);					 //		ACC <- result <T>
	} else {																 // case: <M> -> <T> - <M>
		processM(mNode->child2, os);					 //		ACC <- result <M>
		string temp = newName(VAR);						
		os << "STORE " << temp << endl;				 //		temp <- ACC (<M>)
		processT(mNode->child1, os);					 // 	ACC <- result <T>
		os << "SUB " << temp << endl;					 // 	ACC <- ACC (<T>) - temp (<M>)	
	}
}


static void processT(const node_t* tNode, ofstream& os) {
	if (tNode->token1 == NULL) {							// case: <T> -> <F>	
		processF(tNode->child1, os); 						// 	ACC <- result <F>
  } else {																	// case: <T> -> <F> *|/ <T>
		processT(tNode->child2, os); 						// 	ACC <- result <T>
		string temp = newName(VAR);
		os << "STORE temp" << endl;							// 	temp <- ACC (<T>)
		processF(tNode->child1, os); 						// 	ACC <- result <F>
		if (tNode->token1->tokenID == STARtk)  	// 	case: <T> -> <F> * <T>
			os << "MULTI " << temp << endl;				// 		ACC <- ACC (<F>) * temp (<T>)
		else																		// 	case: <T> -> <F> / <T> 
			os << "DIV " << temp << endl;					//		ACC <- ACC (<F>) / temp (<T>)
	}
}

static void processF(const node_t* fNode, ofstream& os) {
	if (fNode->token1 == NULL) { 			// case: <F> -> <R>
		processR(fNode->child1, os);	 	// 	ACC <- <R> result
	} else { 										 			// case: <F> -> - <F>
		processF(fNode->child1, os);   	// 	ACC <- <F> result
		os << "MULTI -1" << endl;  			// 	ACC <- ACC (<F>) * -1
	}
}


static void processR(const node_t* rNode, ofstream& os) {
	if (rNode->token1 == NULL) { 															// case: <R> -> <expr>
		processExpr(rNode->child1, os); 												// 	ACC <- <expr> result
	} else {																								 	// case: <R> -> id|#
		if (rNode->token1->tokenID == IDtk) {
			verify(rNode->token1);
			printDataflow(rNode->token1, VAR_TO_ACC, os);						// ACC <- id
		} else {
			os << "LOAD " << rNode->token1->tokenInstance << endl; 	// 	ACC <- #
		}
	}
}

static void processStats(const node_t* statsNode, ofstream& os) {
	processStat(statsNode->child1, os);
	processMStat(statsNode->child2, os);
}


static void processMStat(const node_t* mStatNode, ofstream& os) {
	if (mStatNode == NULL) {								// case: <mStat> -> empty
		return;
	} else {															  // case: <mStat> -> <stat> : <mStat>
		processStat(mStatNode->child1, os);
		processMStat(mStatNode->child2, os);
	}
}


static void processStat(const node_t* statNode, ofstream& os) {
	string childLabel = statNode->child1->label;

	if (childLabel == "<in>")								// case: <stat> -> <in>
		processIn(statNode->child1, os); 			
	else if (childLabel == "<out>")					// case: <stat> -> <out>
		processOut(statNode->child1, os);			
	else if (childLabel == "<block>")				// case: <stat> -> <block>
		processBlock(statNode->child1, os);		
	else if (childLabel == "<if>")					// case: <stat> -> <if>
		processIf(statNode->child1, os);			
	else if (childLabel == "<loop>")				// case: <stat> -> <loop>
		processLoop(statNode->child1, os);		
	else if (childLabel == "<assign>")			// case: <stat> -> <assign>
		processAssign(statNode->child1, os);	
}


static void processIn(const node_t* inNode, ofstream& os) {
	verify(inNode->token1);
	//os << "READ " << inNode->token1->tokenInstance << endl;		// id <- Input I/O
	printDataflow(inNode->token1, IO_TO_VAR, os);
}

static void processOut(const node_t* outNode, ofstream& os) {
	processExpr(outNode->child1, os); 		// ACC <- <expr> result

	string temp = newName(VAR);
	os << "STORE " << temp << endl; 			// temp <- ACC (<expr>)
	os << "WRITE " << temp << endl;				// output <- temp (<expr>)
}

static void processIf(const node_t* ifNode, ofstream& os) {
	// evaluate condition to determine if we branch or not
	processExpr(ifNode->child3, os);			      // ACC <- result (right <expr>)
	
	string temp = newName(VAR);
	os << "STORE " << temp << endl; 			      // temp <- ACC (right <expr>)
	processExpr(ifNode->child1, os);			      // ACC <- result (left <expr>)
	os << "SUB " << temp << endl;					      // ACC <- ACC (left <expr>) - temp (right <expr>)
	
	// create for branching when condition is not satisified
	string label = newName(LABEL);				
	roType ro = processRO(ifNode->child2, os);	
	printBranchInstructs(ro, label, os);

	processBlock(ifNode->child4, os);							

	// print label to be jumped to when condition is not satisifed
	os << label << ": NOOP" << endl;
}

static void processLoop(const node_t* ifNode, ofstream& os) {
	// evaluate condition to determine if we branch or not
	string startLabel = newName(LABEL);
	string endLabel = newName(LABEL);

	// mark the beginning of the loop
	os << startLabel << ": NOOP" << endl;
	processExpr(ifNode->child3, os);			      // ACC <- result (right <expr>)
	
	// evaluate the condition for breaking out of loop
	string temp = newName(VAR);
	os << "STORE " << temp << endl; 			      // temp <- ACC (right <expr>)
	processExpr(ifNode->child1, os);			      // ACC <- result (left <expr>)
	os << "SUB " << temp << endl;					      // ACC <- ACC (left <expr>) - temp (right <expr>)
	
	// create instructions for breaking when condition is no longer satisified
	roType ro = processRO(ifNode->child2, os);	
	printBranchInstructs(ro, endLabel, os);

	processBlock(ifNode->child4, os);							

	// unconditionally branch to 
	os << "BR " << startLabel << endl;

	// print label to be jumped to when condition is not satisifed
	os << endLabel << ": NOOP" << endl;
}

static void processAssign(const node_t* assignNode, ofstream& os) {
	processExpr(assignNode->child1, os); 												 // ACC <- <expr> result
	verify(assignNode->token1);
	printDataflow(assignNode->token1, ACC_TO_VAR, os);
	//os << "STORE " << assignNode->token1->tokenInstance << endl; // id <- ACC 
}

static roType processRO(const node_t* roNode, ofstream& os) {
	if (roNode->token1->tokenID == LEFTtk) 
		return (roNode->token2 == NULL) ? LT : LTEQ;
	else if (roNode->token1->tokenID == RIGHTtk)
		return (roNode->token2 == NULL) ? GT : GTEQ;
	else if (roNode->token1->tokenID == EQtk)
		return EQ;
	else if (roNode->token1->tokenID == DEXtk)
		return NOTEQ;
}

/**insert
 * -------
 * attempt to insert an id the symbol table. Throws error if the same token instance
 * has already been declared in this scope or if the stack overflows.
 *
 * inputs:
 * idtk...the IDtk to insert into the symbol table
 */
static void insert(const token_t* idtk, ofstream& os) {
	if (declaringGlobals) {
		if (STV.verify(*idtk) == false)
			STV.insert(*idtk);
		else
			semError(*idtk, "Variable already declared in same scope."); 
	} else {
		if (sysStack.varsInLevel() == 0 || !sysStack.isInLevel(*idtk)) {
			sysStack.push(*idtk);
			os << "PUSH" << endl;
		}
		else {																	 // no to all; throw error and quit. Illegal static semantics
			semError(*idtk, "Variable already declared in same scope."); 
		}
	}
}


static void printDataflow(const token_t* idtk, dataflow flow, ofstream& os) {
	int stackOffset = sysStackOffset(idtk);

	switch(flow) {
		case VAR_TO_ACC:
			if (isGlobal(idtk))
				os << "LOAD " << idtk->tokenInstance << endl;
			else
				os << "STACKR " << stackOffset << endl;
			break;
		
		case VAR_TO_IO:
			if (isGlobal(idtk))
				os << "WRITE " << idtk->tokenInstance << endl;
			else {
				os << "STACKR " << stackOffset << endl;
				string temp = newName(VAR);
				os << "STORE " << temp << endl;
				os << "WRITE " << temp << endl;
			}
			break;
		
		case ACC_TO_VAR:
			if (isGlobal(idtk))
				os << "STORE " << idtk->tokenInstance << endl;
			else 
				os << "STACKW " << stackOffset << endl;
			break;

		case IO_TO_VAR:
			if (isGlobal(idtk))
				os << "READ " << idtk->tokenInstance << endl;
			else {
				string temp = newName(VAR);
				os << "READ " << temp << endl;
				os << "LOAD " << temp << endl;
				os << "STACKW " << stackOffset << endl;
			}
			break;

		default:
			break;
	};
}

static bool isGlobal(const token_t* idtk) {
	return STV.verify(*idtk); 
}

static int sysStackOffset(const token_t* idtk) {
	return sysStack.find(*idtk);
}	

/**verify
 * -------
 * Verify that an IDtk instance has already been declared in program. Throws
 * error if the IDtk instance has not been declared in the program.
 *
 * inputs:
 * idtk...the IDtk to verify
 */
static void verify(const token_t* idtk) {
	if (sysStackOffset(idtk) == -1 && !isGlobal(idtk))
		semError(*idtk, "Variable is undeclared.");
}

static void printVars(ofstream& os) {
	// print all global variables
	int i;
	vector<token_t> globals = STV.getTokens();
	for (i=0; i < globals.size(); i++)
		os << globals.at(i).tokenInstance << " 0" << endl;

	// print all used local variables
	for (i=0; i < varCount; i++)
		os << "V" << i << " 0" <<  endl;
}

static string newName(nameType what) {
	return (what == VAR)  
		? "V" + to_string(varCount++) 
		: "L" + to_string(labelCount++);
}

static void printBranchInstructs(roType ro, string outlabel, ofstream& os) {
	if (ro == LT) {													// false: + or 0 
		os << "BRZPOS " << outlabel << endl;
	} else if (ro == LTEQ) {								// false: + 
		os << "BRPOS " << outlabel << endl;
	} else if (ro == GT) {									// false: - or 0 
		os << "BRZNEG " << outlabel << endl;
	} else if (ro == GTEQ) {								// false: - 
		os << "BRNEG " << outlabel << endl;
	} else if (ro == EQ) {								 	// false: + or - 
		os << "BRNEG " << outlabel << endl;
		os << "BRPOS " << outlabel << endl;
  } else if (ro == NOTEQ) {								// false: 0
		os << "BRZERO " << outlabel << endl;	
	}
}

