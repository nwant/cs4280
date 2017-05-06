/* Nathaniel Want (nwqk6)
 * CS4280-E02
 * P4
 * 5/9/2017
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

node_t* parser(ifstream& input);

#endif
