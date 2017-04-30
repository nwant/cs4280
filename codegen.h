/* Nathaniel Want (nwqk6)
 * CS4280-E02
 * P4
 * 4/24/2017
 * -----------
 * codegen.h
 * --------
 * The declarations/prototypes for functions 
 * pertaining to code generation in the compiler back end
 */
#ifndef CODEGEN_H
#define CODEGEN_H

#include <fstream>
#include "node.h"

void codegen(const node_t* root, ofstream& os);

#endif
