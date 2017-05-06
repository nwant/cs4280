/* Nathaniel Want (nwqk6)
 * CS4280-E02
 * P4
 * 5/9/2017
 * -----------
 *  codegen.cpp
 * --------
 * the implementation for all backend functionality
 * of the compiler as well as statical semantics verification.
 */
#include <fstream>
#include <string>
#include <vector>
#include "codegen.h"
#include "errors.h"
#include "node.h"
#include "ST.h"
#include "SysStack.h"
#include "token.h"
using namespace std;

typedef enum { VAR, LABEL } nameType;								   									// name request types
typedef enum { LT, LTEQ, GT, GTEQ, EQ, NOTEQ } roType; 									// relational operator types
typedef enum { VAR_TO_ACC, VAR_TO_IO, ACC_TO_VAR, IO_TO_VAR } dataflow;	// variable dataflow directions
static ST STV = ST();																										// the symbol table for global vars
static SysStack sysStack = SysStack();				 													// the system stack for local vars
static int varCount = 0;																								// # of temporary variables declared
static int labelCount = 0;																							// # of labels created
static bool declaringGlobals = true;																		// flag to to determine if globals vars are still being declared

/*********************** helper function prototypes **********************/
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
static roType processRO(const node_t*);
static void processAssign(const node_t*, ofstream&);
static void verifyVar(const token_t*);
static void insertVar(const token_t*, ofstream&);
static bool isGlobal(const token_t*);
static int sysStackOffset(const token_t*);
static string newName(nameType);
static void printDataflow(const token_t*, dataflow, ofstream&);
static void printBranchInstructs(roType, string, ofstream&);
static void printVars(ofstream&);
/*----------------------------------------------------------------------*/


/**codegen
 * -------
 * translate a parse tree into the target language (asm).
 * 
 * inputs:
 * root....the root of the parse tree.
 * os......the output filestream to generate the target language to.
 */
void codegen(const node_t* root, ofstream& os) {
	// do we have a parse tree?	
	if (root == NULL)
		return;									// no; do nothing.

	processProgram(root, os); // yes; translate the parse tree into asm 
	os << "STOP" << endl;			// include required stop command to target
	printVars(os);						// declare all local and temp variables to target
}


/**processProgram
 * -------
 * perform all semantical checks and target code generation for a program
 * node and all other child nodes (recursively).
 * 
 * inputs:
 * programNode...the program node to process
 * os......the output filestream to generate the target language to.
 */
static void processProgram(const node_t* programNode, ofstream& os) {
	processVars(programNode->child1, os);  // process global vars
	declaringGlobals = false;							 // all globals now declared
	processBlock(programNode->child2, os); 
}


/**processBlock
 * -------
 * perform all semantical checks and target code generation for a block 
 * node and all other child nodes (recursively).
 * 
 * inputs:
 * blockNode...the block node to process
 * os......the output filestream to generate the target language to.
 */
static void processBlock(const node_t* blockNode, ofstream& os) {
	sysStack.addLevel();

	processVars(blockNode->child1, os);
	processStats(blockNode->child2, os);

	for (int i=0; i < sysStack.varsInLevel(); i++)
		os << "POP" << endl;

	sysStack.removeLevel();
}


/**processVars
 * -------
 * perform all semantical checks and target code generation for a vars 
 * node and all other child nodes (recursively).
 * 
 * inputs:
 * varsNode...the vars node to process
 * os......the output filestream to generate the target language to.
 */
static void processVars(const node_t* varsNode, ofstream& os) {
	if (varsNode == NULL) {										// case: <vars> -> empty
		return;
	} else {																	// case: <vars> -> id <mvars>
		insertVar(varsNode->token1, os);
		processMvars(varsNode->child1, os);
	}
}


/**processMvars
 * -------
 * perform all semantical checks and target code generation for a mvars 
 * node and all other child nodes (recursively).
 * 
 * inputs:
 * mvarsNode...the mvars node to process
 * os......the output filestream to generate the target language to.
 */
static void processMvars(const node_t* mvarsNode, ofstream& os) {
	if (mvarsNode == NULL)										// case: <mvars> -> empty
		return;
	else {																		// case: <mvars> -> id <mvars>
		insertVar(mvarsNode->token1, os);
		processMvars(mvarsNode->child1, os);	
	}
}


/**processExpr
 * -------
 * perform all semantical checks and target code generation for a expr 
 * node and all other child nodes (recursively).
 * 
 * inputs:
 * exprNode...the expr node to process
 * os......the output filestream to generate the target language to.
 */
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


/**processM
 * -------
 * perform all semantical checks and target code generation for a M
 * node and all other child nodes (recursively).
 * 
 * inputs:
 * mNode...the M node to process
 * os......the output filestream to generate the target language to.
 */
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


/**processT
 * -------
 * perform all semantical checks and target code generation for a T 
 * node and all other child nodes (recursively).
 * 
 * inputs:
 * tNode...the T node to process
 * os......the output filestream to generate the target language to.
 */
static void processT(const node_t* tNode, ofstream& os) {
	if (tNode->token1 == NULL) {							// case: <T> -> <F>	
		processF(tNode->child1, os); 						// 	ACC <- result <F>
  } else {																	// case: <T> -> <F> *|/ <T>
		processT(tNode->child2, os); 						// 	ACC <- result <T>
		string temp = newName(VAR);
		os << "STORE " << temp << endl;					// 	temp <- ACC (<T>)
		processF(tNode->child1, os); 						// 	ACC <- result <F>
		if (tNode->token1->tokenID == STARtk)  	// 	case: <T> -> <F> * <T>
			os << "MULT " << temp << endl;				// 		ACC <- ACC (<F>) * temp (<T>)
		else																		// 	case: <T> -> <F> / <T> 
			os << "DIV " << temp << endl;					//		ACC <- ACC (<F>) / temp (<T>)
	}
}


/**processF
 * -------
 * perform all semantical checks and target code generation for a F 
 * node and all other child nodes (recursively).
 * 
 * inputs:
 * fNode...the F node to process
 * os......the output filestream to generate the target language to.
 */
static void processF(const node_t* fNode, ofstream& os) {
	if (fNode->token1 == NULL) { 			// case: <F> -> <R>
		processR(fNode->child1, os);	 	// 	ACC <- <R> result
	} else { 										 			// case: <F> -> - <F>
		processF(fNode->child1, os);   	// 	ACC <- <F> result
		os << "MULT -1" << endl;  			// 	ACC <- ACC (<F>) * -1
	}
}


/**processR
 * -------
 * perform all semantical checks and target code generation for a R
 * node and all other child nodes (recursively).
 * 
 * inputs:
 * rNode...the R node to process
 * os......the output filestream to generate the target language to.
 */
static void processR(const node_t* rNode, ofstream& os) {
	if (rNode->token1 == NULL) { 															// case: <R> -> <expr>
		processExpr(rNode->child1, os); 												// 	ACC <- <expr> result
	} else {																								 	// case: <R> -> id|#
		if (rNode->token1->tokenID == IDtk) {
			verifyVars(rNode->token1);
			printDataflow(rNode->token1, VAR_TO_ACC, os);						// ACC <- id
		} else {
			os << "LOAD " << rNode->token1->tokenInstance << endl; 	// 	ACC <- #
		}
	}
}


/**processStats
 * -------
 * perform all semantical checks and target code generation for a stats
 * node and all other child nodes (recursively).
 * 
 * inputs:
 * statsNode...the stats node to process
 * os......the output filestream to generate the target language to.
 */
static void processStats(const node_t* statsNode, ofstream& os) {
	processStat(statsNode->child1, os);
	processMStat(statsNode->child2, os);
}


/**processMstat
 * -------
 * perform all semantical checks and target code generation for a mStat
 * node and all other child nodes (recursively).
 * 
 * inputs:
 * mStatNode...the mStat node to process
 * os......the output filestream to generate the target language to.
 */
static void processMStat(const node_t* mStatNode, ofstream& os) {
	if (mStatNode == NULL) {								// case: <mStat> -> empty
		return;
	} else {															  // case: <mStat> -> <stat> : <mStat>
		processStat(mStatNode->child1, os);
		processMStat(mStatNode->child2, os);
	}
}


/**processStat
 * -------
 * perform all semantical checks and target code generation for a stat
 * node and all other child nodes (recursively).
 * 
 * inputs:
 * statNode...the stat node to process
 * os......the output filestream to generate the target language to.
 */
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


/**processIn
 * -------
 * perform all semantical checks and target code generation for a in 
 * node and all other child nodes (recursively).
 * 
 * inputs:
 * inNode...the in node to process
 * os......the output filestream to generate the target language to.
 */
static void processIn(const node_t* inNode, ofstream& os) {
	verifyVars(inNode->token1);
	printDataflow(inNode->token1, IO_TO_VAR, os);
}


/**processOut
 * -------
 * perform all semantical checks and target code generation for a out 
 * node and all other child nodes (recursively).
 * 
 * inputs:
 * outNode...the out node to process
 * os......the output filestream to generate the target language to.
 */
static void processOut(const node_t* outNode, ofstream& os) {
	processExpr(outNode->child1, os); 		// ACC <- <expr> result

	string temp = newName(VAR);
	os << "STORE " << temp << endl; 			// temp <- ACC (<expr>)
	os << "WRITE " << temp << endl;				// output <- temp (<expr>)
}


/**processIf
 * -------
 * perform all semantical checks and target code generation for a if 
 * node and all other child nodes (recursively).
 * 
 * inputs:
 * ifNode...the if node to process
 * os......the output filestream to generate the target language to.
 */
static void processIf(const node_t* ifNode, ofstream& os) {
	// evaluate condition to determine if we branch or not
	processExpr(ifNode->child3, os);			      // ACC <- result (right <expr>)
	
	string temp = newName(VAR);
	os << "STORE " << temp << endl; 			      // temp <- ACC (right <expr>)
	processExpr(ifNode->child1, os);			      // ACC <- result (left <expr>)
	os << "SUB " << temp << endl;					      // ACC <- ACC (left <expr>) - temp (right <expr>)
	
	// create for branching when condition is not satisified
	string label = newName(LABEL);				
	roType ro = processRO(ifNode->child2);	
	printBranchInstructs(ro, label, os);

	// write all instructions to target to be exectuted if condition is satisfied
	processBlock(ifNode->child4, os);							

	// print label to be jumped to when condition is not satisifed
	os << label << ": NOOP" << endl;
}


/**processLoop
 * -------
 * perform all semantical checks and target code generation for a loop 
 * node and all other child nodes (recursively).
 * 
 * inputs:
 * loopNode...the loop node to process
 * os......the output filestream to generate the target language to.
 */
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
	roType ro = processRO(ifNode->child2);	
	printBranchInstructs(ro, endLabel, os);

	// generate code to be executed when conidition is true
	processBlock(ifNode->child4, os);							

	// unconditionally branch to beginning of the loop 
	os << "BR " << startLabel << endl;

	// print label to be jumped to when condition is not satisifed
	os << endLabel << ": NOOP" << endl;
}


/**processAssign
 * -------
 * perform all semantical checks and target code generation for a loop 
 * node and all other child nodes (recursively).
 * 
 * inputs:
 * loopNode...the loop node to process
 * os......the output filestream to generate the target language to.
 */
static void processAssign(const node_t* assignNode, ofstream& os) {
	processExpr(assignNode->child1, os); 												 // ACC <- <expr> result
	verifyVars(assignNode->token1);
	printDataflow(assignNode->token1, ACC_TO_VAR, os);
}


/**processRO
 * -------
 * determine the RO type represented in an RO node
 * 
 * inputs:
 * roNode...the RO node to inspect 
 *
 * returns the ro type of the RO node
 */
static roType processRO(const node_t* roNode) {
	if (roNode->token1->tokenID == LEFTtk) 
		return (roNode->token2 == NULL) ? LT : LTEQ;
	else if (roNode->token1->tokenID == RIGHTtk)
		return (roNode->token2 == NULL) ? GT : GTEQ;
	else if (roNode->token1->tokenID == EQtk)
		return EQ;
	else if (roNode->token1->tokenID == DEXtk)
		return NOTEQ;
}


/**insertVar
 * -------
 * attempt to insert an id the symbol table (if global) or push onto the system stack
 * (if local).. Throws error if the same token instance has already been declared in 
 * this scope or if the stack overflows. Throws an error if static semantics has been 
 * violated.
 *
 * inputs:
 * idtk...the IDtk to insert into the symbol table
 * os......the output filestream to generate the target language to.
 */
static void insertVar(const token_t* idtk, ofstream& os) {
	if (declaringGlobals) { // global
		// has this variable already been declared globally?
		if (STV.verifyVars(*idtk) == false)	
			STV.insertVar(*idtk);														// no; ok. add to symbol table
		else
			semError(*idtk, "Variable already declared in same scope."); // yes; error   
	} else {   // local
		// is this variable already declared in this scope?
		if (!sysStack.isInLevel(*idtk)) {
			sysStack.push(*idtk);							// no; ok. add to sys stack	
			os << "PUSH" << endl;													
		} else 																	 
			semError(*idtk, "Variable already declared in same scope.");  // yes; error
	}
}


/**isGlobal
 * -------
 * has a variable been declared globally?
 * 
 * inputs:
 * idtk...the variable to check
 *
 * returns true if the variable has been declared globally, else false.
 */
static bool isGlobal(const token_t* idtk) {
	return sysStackOffset(idtk) == -1; 
}


/**sysStackOffset
 * -------
 * get the location of a variable with respect to the top of the 
 * system stack (e.g. 0 = 1st element from top of stack, 1 = 2nd 
 * element from top of stack, etc.). 
 * 
 * inputs:
 * idtk...the variable to check 
 *
 * returns the location of the variable with respect to the top of
 * the stack if the variable exists in the system stack. If the 
 * variable is not found in they system stack, returns -1.
 */
static int sysStackOffset(const token_t* idtk) {
	return sysStack.find(*idtk);
}	


/**verifyVar
 * -------
 * Verify that an IDtk instance has already been declared in program. Throws
 * error if the IDtk instance has not been declared in the program.
 *
 * inputs:
 * idtk...the IDtk to verify
 */
static void verifyVar(const token_t* idtk) {
	if (sysStackOffset(idtk) == -1 && !isGlobal(idtk))
		semError(*idtk, "Variable is undeclared.");
}


/**newName
 * -------
 * generate a new name for a temporary variable or for a label.
 *
 * inputs:
 * what...the type of name to generate (e.g. temp var or label)
 *
 * returns the generated name.
 */
static string newName(nameType what) {
	return (what == VAR)  
		? "V" + to_string(varCount++) 
		: "L" + to_string(labelCount++);
}


/**printBranchInstructs
 * -------
 * print the proper code to target needed to determine branch instructions. 
 * 
 * inputs:
 * ro........the relational operation type for this branch instruction
 * outlabel..the label to jump to when branch's jump conditions are met.
 * os......the output filestream to generate the target language to.
 */
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


/**printDataFlow
 * -------
 * print the proper code to target needed to command the flow of data to or from 
 * a variable either in memory or on the system stack to or from i/o or ACC.
 * 
 * inputs:
 * idtk...the variable that data should be flowing to or from
 * flow...the dataflow direction
 * os......the output filestream to generate the target language to.
 */
static void printDataflow(const token_t* idtk, dataflow flow, ofstream& os) {
	int stackOffset = sysStackOffset(idtk);

	switch(flow) {
		// ACC <- id
		case VAR_TO_ACC:
			if (isGlobal(idtk))
				os << "LOAD " << idtk->tokenInstance << endl;
			else
				os << "STACKR " << stackOffset << endl;
			break;
		
		// output <- ACC <- id
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
		
		// id <- ACC
		case ACC_TO_VAR:
			if (isGlobal(idtk))
				os << "STORE " << idtk->tokenInstance << endl;
			else 
				os << "STACKW " << stackOffset << endl;
			break;
		
		// id <- ACC <- input
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


/**printVars
 * -------
 * print the global and temporary varaibles to the target file. Initalize
 * each variable to 0.
 *
 * inputs:
 * os......the output filestream to generate the target language to.
 */
static void printVars(ofstream& os) { // print all global variables 
	int i;
	vector<token_t> globals = STV.getTokens();
	for (i=0; i < globals.size(); i++)
		os << globals.at(i).tokenInstance << " 0" << endl;

	// print all used local variables
	for (i=0; i < varCount; i++)
		os << "V" << i << " 0" <<  endl;
}

