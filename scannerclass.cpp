/*
 Nathaniel Want (nwqk6)
 * CS4280-E02
 * P3
 * 4/24/2017
 * -----------
 *  scannerclass.cpp
 * --------
 * the implementation of the scanner class for the scanner module
 */
#include <fstream>
#include <map>
#include <stdlib.h>
#include "errors.h"
#include "token.h"
#include "scannerclass.h"
using namespace std;

/* the final state maps. these are used to determine the token value when a final state has been reached */
static map<string, tokenId_t> finalStates = {
	{"@", IDtk},
	{"#", INTtk},
	{"=", EQtk},
	{"[", LBRAKtk},
	{"]", RBRAKtk},
	{"{", LBRACEtk},
	{"}", RBRACEtk},
	{";", SEMItk},
	{":", COLtk},
	{".", DOTtk},
	{",", COMMAtk},
	{"/", FStk},
	{"(", LPARtk},
	{")", RPARtk},
	{"+", PLUStk},
	{"-", MINUStk},
	{"*", STARtk},
	{"_", EOFtk},
	{"<=<", LEFTtk},
	{">=>", RIGHTtk},
	{"!!", DEXtk},
	{"BEGIN", BEGINtk},
	{"END", ENDtk},
	{"IF", IFtk},
	{"FOR", FORtk},
	{"VOID", VOIDtk},
	{"VAR", VARtk},
	{"RETURN", RETURNtk},
	{"INPUT", INPUTtk},
	{"OUTPUT", OUTPUTtk}
};
/*--------------------------------------------------------*/

/*----------- table rows (intermediary states) ------------*/
/* these are row indexes for the 2D FSA table */
static map<string, int> states = {
	{"s1", 0},
	{"s2", 1},
	{"s3", 2},
	{"s4", 3},
	{"s5", 4},
	{"s6", 5},
	{"s7", 6},
	{"s8", 7},
	{"s9", 8}
};

static const int INITIAL_STATE = 0;
static const int NUM_STATES = 9;
/*-------------------------------------------*/

/*--------------- table columns (alphabet chars) ----------------------------*/
/* these are column indexes for the 2D FSA table */
static map<string, int> chars = {
	{"letter", 0},
	{"digit", 1},
	{"=", 2},
	{"<", 3},
	{">", 4},
	{"!", 5},
	{"[", 6},
	{"]", 7},
	{"{", 8},
	{"}", 9},
	{";", 10},
	{":", 11},
	{".", 12},
	{",", 13},
	{"/", 14},
	{"(", 15},
	{")", 16},
	{"+", 17},
	{"-", 18},
	{"*", 19},
	{"WS", 20},
	{"$", 21},
	{"EOF", 22}
};
static const int NUM_CHARS = 23; /* the number of characters allowed in this language (whitespace counted as 1 character)*/
/*-----------------------------------------------------------------------*/

/*------------------------ errors ---------------------------------------*/
static map<string, string> errors = {
	{"e0", "Invalid character found. Only alphanumeric characters, whitespace and the following characters are permitted: { ! * = < > [ ] { } ( ) : ; . , + - * / EOF }"},
	{"e1", "Comments must end with whitepace"},
	{"e2", "Tokens that start with '<' may only be followed by a '=' character."},
	{"e3", "Tokens that start with '<=' may only be followed by a '<' character."},
	{"e4", "Tokens that start with '>' may only be followed by a '=' character."},
	{"e5", "Tokens that start with '>=' may only be followed by a '>' character"},
	{"e6", "Tokens that start with '!' may only be followed by a '!' character'"}
};
/*-----------------------------------------------------------------------*/


/*------------------------------ FSA table ---------------------------------------------------------------------------------------------------------------------------------*/
static string fsaTable [NUM_STATES][NUM_CHARS] = {
/*    L      D      =      <      >     !       [      ]      {      }      ;      :      .      ,      /      (      )      +      -      *     WS      $     EOF  */
	{  "s2", "s3",   "=",  "s5",  "s7",  "s9",   "[",   "]",   "{",   "}",   ";",   ":",   ".",   ",",   "/",   "(",   ")",   "+",   "-",   "*",  "s1",  "s4",   "_"}, /*s1*/
	{  "s2", "s2",   "@",   "@",   "@",   "@",   "@",   "@",   "@",   "@",   "@",   "@",   "@",   "@",   "@",   "@",   "@",   "@",   "@",   "@",   "@",   "@",   "@"}, /*s2*/
  {   "#", "s3",   "#",   "#",   "#",   "#",   "#",   "#",   "#",   "#",   "#",   "#",   "#",   "#",   "#",   "#",   "#",   "#",   "#",   "#",   "#",   "#",   "#"}, /*s3*/
	{  "s4", "s4",  "s4",  "s4",  "s4",  "s4",  "s4",  "s4",  "s4",  "s4",  "s4",  "s4",  "s4",  "s4",  "s4",  "s4",  "s4",  "s4",  "s4",  "s4",  "s1",  "s4",  "e1"}, /*s4*/
	{  "e2", "e2",  "s6",  "e2",  "e2",  "e2",  "e2",  "e2",  "e2",  "e2",  "e2",  "e2",  "e2",  "e2",  "e2",  "e2",  "e2",  "e2",  "e2",  "e2",  "e2",  "e2",  "e2"}, /*s5*/
	{  "e3", "e3",  "e3", "<=<",  "e3",  "e3",  "e3",  "e3",  "e3",  "e3",  "e3",  "e3",  "e3",  "e3",  "e3",  "e3",  "e3",  "e3",  "e3",  "e3",  "e3",  "e3",  "e3"}, /*s6*/
	{  "e4", "e4",  "s8",  "e4",  "e4",  "e4",  "e4",  "e4",  "e4",  "e4",  "e4",  "e4",  "e4",  "e4",  "e4",  "e4",  "e4",  "e4",  "e4",  "e4",  "e4",  "e4",  "e4"}, /*s7*/
	{  "e5", "e5",  "e5",  "e5", ">=>",  "e5",  "e5",  "e5",  "e5",  "e5",  "e5",  "e5",  "e5",  "e5",  "e5",  "e5",  "e5",  "e5",  "e5",  "e5",  "e5",  "e5",  "e5"}, /*s8*/
	{  "e6", "e6",  "e6",  "e6",  "e6",  "!!",  "e6",  "e6",  "e6",  "e6",  "e6",  "e6",  "e6",  "e6",  "e6",  "e6",  "e6",  "e6",  "e6",  "e6",  "e6",  "e6",  "e6"}  /*s9*/
};
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


/*------------------------------------- helper functions  ----------------------------------------*/
/** isAppendable
 * ------------
 * should the character represented, as referenced by the state and character index
 * of the FSA table, be appended to the token instance string for this token?
 * 
 * inputs:
 * state...the state (index) we are currently in
 * charIdx.the character (index) we are now encountering.
 * 
 * returns true if this character should be appended, else false  
 */
static bool isAppendable(const int state, const int charIdx) {
	return states["s4"] != state 
		&& charIdx != chars["WS"] 
		&& charIdx != chars["EOF"] 
		&& charIdx != chars["$"];
}


/**isError
 * -------
 * is this state not allowed (aka, is this an error state)?
 *
 * inputs:
 * state...the state (fsa table value) we are currently in
 * 
 * return true if this is an error state, else false
 */
static bool isError(const string state) {
	return state.at(0) == 'e';
}


/**isFinal
 *---------
 * is this a final state?
 * inputs:
 * state...the state (fsa table value) we are currently in
 *
 * return true if this is a final state, else false
 */
static bool isFinal(const string state) {
	return state.at(0) != 's' && !isError(state);
}
/*----------------------------------------------------------------------------------------------------------*/


/*------------------------------------- Scanner class implemenation ----------------------------------------*/


/**Scanner::Scanner()
 * the constructor for the Scanner class
 */
Scanner::Scanner() {
	this->lineNumber = 1;
	this->thisChar = '\0';
	this->nextChar = '\0';
	this->approachingNewline=false;
}

/**Scanner::getToken 
 * use a driver-like implemenation to utilize the FSA table to scan in the input for the next token
 *
 * inputs:
 * input...the input stream to get the next token
 *
 * returns the next token from the input stream
 */
token_t Scanner::getToken(ifstream& input) {
	token_t token; /* the token to return */
	int state = INITIAL_STATE; /* the current state */
	string nextState; /* the next state (according to the FSA table) */
	int nextCharIdx; /* the index for the next char (the column index for the FSA table) */
	string str;	 /* the string to collect the token instance value */
	
	if (this->nextChar != '\0')  // is this the very beginning of the file?
		input.get(this->thisChar); // yes; grab the first character in the file

	do {
		this->nextChar = input.peek(); // lookahead by one character 
		nextCharIdx = getCharIndex(this->nextChar, input);
  	nextState = fsaTable[state][nextCharIdx]; // determine the next state using the FSA

		if (isError(nextState)) {  // have we reached an error state?
			printError(nextState, ""); // yes; throw an error message an exit 
			//input.close();
			//raise(SIGINT);
		}	else if (isFinal(nextState)) { // no; have we reached a final state
			if (nextState == "#" || nextState == "@" || nextState == "_") {  // yes; build the token and return to caller
				if (!isspace(this->nextChar) && (nextState == "#" || nextState == "@"))
					input.unget();
				token = buildToken(nextState, str);
			} else
				token = buildToken(nextState, nextState);
			break;
		} else { // no; we are in an intermediary staete
			state=states[nextState]; // move to next state
			input.get(this->thisChar);  // grab this character
			if (isAppendable(state, nextCharIdx))  // append to the token instance string
				str += this->thisChar;
		}
	} while (!isFinal(nextState));				
	
	return token;
}


/**Scanner::buildToken
 * ----------
 * build a token using the final state and tokenInstance
 * 
 * inputs:
 * finalState.....the final state (FSA table value we are in)
 * tokenInstance..the string value for the token instance (where applicable)
 *
 * returns the token
 */
token_t Scanner::buildToken(const string finalState, const string tokenInstance) {
	token_t token;
	token.lineNumber = this->lineNumber;
	if (finalStates.count(tokenInstance)) {// is this a keyword/operator/delimiter?
		token.tokenID = finalStates[tokenInstance];
	} else {
		token.tokenID = finalStates[finalState];
	}	
	token.tokenInstance = tokenInstance;	
	return token;
}

/**Scanner::getCharIndex
 * get the character index (column index in the FSA) given the provided character
 * 
 * inputs:
 * c.......the character we are looking for the index of
 * input...the input stream provided by the external caller
 * 
 * returns the index corresponding to the provided character
 */
int Scanner::getCharIndex(const char c, ifstream& input) {
	if (this->approachingNewline) {
		this->lineNumber++;
		approachingNewline = false;
  }
	
	if (input.eof() || c == EOF)	
		return chars["EOF"];
	else if (c == '\n') { 
		approachingNewline = true;
		return chars["WS"];		
	} else if (isspace(c))
		return chars["WS"];		
	else if (isalpha(c)) 
		return chars["letter"];		
	else if (isdigit(c)) 
		return chars["digit"];
	else if (c == '$') 
		return chars["$"];
	
	
	switch(c) {
		case '=': return chars["="]; 
		case '<': return chars["<"];
		case '>': return chars[">"];
		case '!': return chars["!"];
		case '[': return chars["["];
		case ']': return chars["]"];
		case '{': return chars["{"];
		case '}': return chars["}"];
		case ';': return chars[";"];
		case ':': return chars[":"];
		case '.': return chars["."];
		case ',': return chars[","];
		case '/': return chars["/"];
		case '(': return chars["("];
		case ')': return chars[")"];
		case '+': return chars["+"];
		case '-': return chars["-"];
		case '*': return chars["*"];
		default:
			string error;
			error.append("Unrecognized character '");
			error.append(&c);
			error.append("'");
			printError("e0", error);
			//input.close();
			//raise(SIGINT);
			break;
	};
}


/**Scanner::handleComments
 *------------------------
 * parse through the current comment until we are no longer in a comment
 *
 * inputs:
 * input...the input, which is assumed to be currently at a comment section.
 */
void Scanner::handleComments(ifstream& input) {
	char c;
	/* scan until we reach the end of the comment */
	while (true) {  
		c = input.peek();
	
		if (c == EOF && input.eof()) {
			printError("e0", "");	
			//input.close();
			//raise(SIGINT);
		} else if (isspace(c)) {		// have we found the end of this comment?
			return; 
		}
		input.get(c);	
	}
}


/**Scanner::printError
 *--------------------
 *
 * inputs:
 * error...the error index for the error maps. This will be the error message printed to the user
 * details.additional details specific to this error.
 */
void Scanner::printError(const string error, const string details) {
	scanError(this->lineNumber, errors[error], details);	
} 
