/* Nathaniel Want (nwqk6)
 * CS4280-E02
 * P3
 * 4/24/2017
 * -----------
 * token.h
 * --------
 * implementation for token related functions
 */
#include <map>
#include <string>
#include "token.h"
using namespace std;

static map<tokenId_t, string> tokenNames = {
	{IDtk, "IDtk"},
	{INTtk, "INTtk"},
	{EQtk, "EQtk"},
	{LBRAKtk, "LBRAKtk"},
	{RBRAKtk, "RBRAKtk"},
	{LBRACEtk, "LBRACEtk"},
	{RBRACEtk, "RBRACEtk"},
	{SEMItk, "SEMItk"},
	{COLtk, "COLtk"},
	{DOTtk, "DOTtk"},
	{COMMAtk, "COMMAtk"},
	{FStk, "FStk"},
	{LPARtk, "LPARtk"},
	{RPARtk, "RPARtk"},
	{PLUStk, "PLUStk"},
	{MINUStk, "MINUStk"},
	{STARtk, "STARtk"},
	{EOFtk, "EOFtk"},
	{LEFTtk, "LEFTtk"},
	{RIGHTtk, "RIGHTtk"},
	{DEXtk, "DEXtk"},
	{BEGINtk, "BEGINtk"},
	{ENDtk, "ENDtk"},
	{IFtk, "IFtk"},
	{FORtk, "FORtk"},
	{VOIDtk, "VOIDtk"},
	{VARtk, "VARtk"},
	{RETURNtk, "RETURNtk"},
	{INPUTtk, "INPUTtk"},
	{OUTPUTtk, "OUTPUTtk"}
};


string getTokenName(token_t tk) {
	return tokenNames[tk.tokenID];
}


/**copyToken
 * -----
 * make a copy of a token
 *
 * inputs:
 * token...the token to copy
 * 
 * returns the copy of the provided token
 */
token_t* copyToken(const token_t token) {
	token_t* tokenCopy = new token_t;
  tokenCopy->tokenID = token.tokenID;
	tokenCopy->tokenInstance = token.tokenInstance;
	tokenCopy->lineNumber = token.lineNumber;
	return tokenCopy;
}
