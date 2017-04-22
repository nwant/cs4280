/* Nathaniel Want (nwqk6)
 * CS4280-E02
 * P3
 * 4/24/2017
 * -----------
 *  token.h
 * --------
 *  Contains the definition for a token and token related functions
 *
 */
#ifndef TOKEN_H
#define TOKEN_H

#include <string>
using namespace std;

// token ids
typedef enum {
	IDtk,      // identifier
	INTtk,     // integer
	EOFtk,		 // EOF
	BEGINtk,	 // BEGIN keyword
	ENDtk,		 // END keyword
	IFtk,			 // IF keyword
	INPUTtk,   // INPUT keyword
	FORtk,		 // FOR keyword
	VARtk,     // VAR keyword
	VOIDtk,    // VOID keyword
	RETURNtk,  // RETURN keyword
	OUTPUTtk,  // OUTPUT keyword
	EQtk,      // = operator/delimiter
	LEFTtk,    // <=< operator/delimiter
	RIGHTtk,   // >=> operator/delimiter
	DEXtk,     // !! operator/delimiter
	COLtk,     // : operator/delimiter
	PLUStk,    // + operator/delimiter
	MINUStk,   // - operator/delimiter
	LPARtk,    // ( operator/delimiter
	RPARtk,    // ) operator/delimiter
	COMMAtk,   // , operator/delimiter
	LBRACEtk,  // { operator/delimiter
	RBRACEtk,  // } operator/delimiter
	SEMItk,    // ; operator/delimiter
	RBRAKtk,   // [ operator/delimiter
	LBRAKtk,   // ] operator/delimiter
	STARtk,		 // * operator/delimiter
	FStk,			 // / operator/delimiter
	DOTtk, 		 // . operator/delimiter
} tokenId_t;

// token structure
typedef struct {
	tokenId_t tokenID;
	string tokenInstance;	
	int lineNumber;
} token_t; 

string getTokenName(token_t);
token_t* copyToken(const token_t);

#endif
