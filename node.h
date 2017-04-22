/* Nathaniel Want (nwqk6)
 * CS4280-E02
 * P3
 * 4/24/2017
 * -----------
 *  node.h
 * --------
 *
 */
#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>
#include "token.h"
using namespace std;

struct node_t {
	string label;
	token_t *token1;
	token_t *token2;
	node_t *child1;	
	node_t *child2;
	node_t *child3;
	node_t *child4;
};

#endif
