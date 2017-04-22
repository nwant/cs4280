/* Nathaniel Want (nwqk6)
 * CS4280-E02
 * P3
 * 4/24/2017
 * -----------
 *  parser.h
 * --------
 * declaration/interface for the parser module 
 */
#ifndef PARSER_H
#define PARSER_H

#include <fstream>
#include "node.h"
using namespace std;

//extern ifstream *input;
node_t* parser(ifstream& input);
#endif
