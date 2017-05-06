/* Nathaniel Want (nwqk6)
 * CS4280-E02
 * P4
 * 5/9/2017
 * -----------
 * codegen.h
 * --------
 * the prototype declarations for all backend functionality
 * of the compiler as well as statical semantics verification.
 */
#ifndef CODEGEN_H
#define CODEGEN_H

#include <fstream>
#include "node.h"

void codegen(const node_t* root, ofstream& os);

#endif
