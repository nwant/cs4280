/* Nathaniel Want (nwqk6)
 * CS4280-E02
 * P4
 * 4/24/2017
 * -----------
 * parser.cpp
 * --------
 * implementation of the parser module 
 */
#include <csignal>
#include <fstream>
#include <iostream>
#include "errors.h"
#include "node.h"
#include "parser.h"
#include "scanner.h"
#include "token.h"
using namespace std;

static token_t tk;   // the next token to consume

/*---------- local prototypes for non terminals ------------*/
static node_t* Program(ifstream&);
static node_t* Vars(ifstream&);
static node_t* Block(ifstream&);
static node_t* Mvars(ifstream&);
static node_t* Expr(ifstream&);
static node_t* M(ifstream&);
static node_t* T(ifstream&);
static node_t* F(ifstream&);
static node_t* R(ifstream&);
static node_t* Stats(ifstream&);
static node_t* MStat(ifstream&);
static node_t* Stat(ifstream&);
static node_t* In(ifstream&);
static node_t* Out(ifstream&);
static node_t* If(ifstream&);
static node_t* Loop(ifstream&);
static node_t* Assign(ifstream&);
static node_t* RO(ifstream&);
static node_t* genNode(const string);
/*--------------------------------------------------------*/


/**parser
 * -----
 * call the parser module to parse a provided *input stream
 * and return the parse tree for said stream.
 *
 * Inputs:
 * input...the input stream to parse
 *
 * returns the root of the parse tree
 */
node_t* parser(ifstream& input) {
	node_t* root = NULL;
	tk = scanner(input);      

	if (tk.tokenID != EOFtk) { 				// parse as long as this is not an empty file
		root = Program(input);
	} 
		
	if (tk.tokenID != EOFtk)   				// 
		parseError("EOFtk", tk);	
	
	return root;               				// return parse tree
}


/**Program
 *---------
 * representation of the <program> non terminal in the grammar.
 *
 * Inputs:
 * input...the input stream we are parsing
 *
 * returns the parse substree, with node from this function as the root.
 */
node_t* Program(ifstream& input) {									
	node_t* p = genNode("<program>"); // generate program node
	p->child1 = Vars(input);					// process vars
	p->child2 = Block(input);					// process block
	return p;													// return: <vars> <block>
}

/**Vars
 *---------
 * representation of the <vars> non terminal in the grammar.
 *
 * Inputs:
 * input...the input stream we are parsing
 *
 * returns the parse substree, with node from this function as the root.
 */
static node_t* Vars(ifstream& input)  { 
	node_t* v = genNode("<vars>");		// generate vars node
	if (tk.tokenID == VOIDtk) {   		// predict VOIDtk
		tk = scanner(input);        		// consume VOIDtk
		
		if (tk.tokenID == IDtk) {   		// predict IDtk
			v->token1 = copyToken(tk);		// store Identifier
			tk = scanner(input); 					// consume IDtk
			v->child1 = Mvars(input);			// process mVars
			return v;											// return: VOID Idenifier <mvars>	
		} else 
			parseError("IDtk", tk);        
	} else													
		return NULL; 										// return: empty production	
}


/**Block
 *---------
 * representation of the <block> non terminal in the grammar.
 *
 * Inputs:
 * input...the input stream we are parsing
 *
 * returns the parse substree, with node from this function as the root.
 */
static node_t* Block(ifstream& input)  { 
	node_t* b = genNode("<block>"); 		// generate block node
	if (tk.tokenID == BEGINtk) {  			// predict BEGIN
		tk = scanner(input);							// consume BEGIN
		b->child1 = Vars(input);					// process Vars
		if (tk.tokenID == COLtk) {				// predict COLtk
			tk = scanner(input);						// consume COLtk
			b->child2 = Stats(input);				// process Stats
			if (tk.tokenID == ENDtk) {  		// predict END
				tk = scanner(input);					// consume END
				return b;											// return: BEGIN <vars> : <stats> END 					 	
			} else	 
				parseError("ENDtk", tk);
		} else
			parseError("COLtk", tk); 			
	} else 
		parseError("BEGINtk", tk);
}


/**Mvars
 *---------
 * representation of the <mvars> non terminal in the grammar.
 *
 * Inputs:
 * input...the input stream we are parsing
 *
 * returns the parse substree, with node from this function as the root.
 */
static node_t* Mvars(ifstream& input)  {
	node_t* m = genNode("<mVars>");  		// generate mvars node
	if (tk.tokenID == IDtk) {						// predict Identifier
		m->token1 = copyToken(tk);				// store idenifier
		tk = scanner(input);							// consume identifier token
		m->child1 = Mvars(input);							// process mVars
		return m;													// return: Identifer <mvars> 
	} else
		return NULL; 											// return empty production 
}

/**Expr 
 *--------- 
 * representation of the <expr> non terminal in the grammar.  
 * 
 * Inputs:
 * input...the input stream we are parsing
 *
 * returns the parse substree, with node from this function as the root.
 */ 
static node_t* Expr(ifstream& input)  { 
	node_t* e = genNode("<expr>"); 			// generate expr node
	e->child1 = M(input); 							// process M
	if (tk.tokenID == PLUStk) {					// predict PLUS token
		e->token1 = copyToken(tk);				// store PLUS
		tk = scanner(input);							// consume PLUS
		e->child2 = Expr(input);					// process Expr	
		return e;													// return: <expr> -> <M> + <expr> 
	} else 
		return e;													// return: <expr> -> <M>
}


/**M
 *---------
 * representation of the <M> non terminal in the grammar.
 *
 * Inputs:
 * input...the input stream we are parsing
 *
 * returns the parse substree, with node from this function as the root.
 */
static node_t* M(ifstream& input)  {
	node_t* m = genNode("<M>");     // generate M node
	m->child1 = T(input);						// process T
	if (tk.tokenID == MINUStk) {    // predict MINUStk
		m->token1 =  copyToken(tk);		// store MINUS token
		tk = scanner(input);					// consume MINUStk
		m->child2 = M(input);					// process M
		return m;											// return: <T> - <M>
	} else
		return m; 										// return: <T>	
}


/**T
 *---------
 * representation of the <T> non terminal in the grammar.
 *
 * Inputs:
 * input...the input stream we are parsing
 *
 * returns the parse substree, with node from this function as the root.
 */
static node_t* T(ifstream& input)  {
	node_t* t = genNode("<T>");  			// generate T node 
	t->child1 = F(input);							// process F
	if (tk.tokenID == STARtk) {				// predict STARtk
		t->token1 = copyToken(tk);			// store STAR token
		tk = scanner(input);						// consume STARtk
		t->child2 = T(input);						// process T
		return t;										  	// return: <F> * <T>			
	} else if (tk.tokenID == FStk) {	// predict FStk
		t->token1 = copyToken(tk);			// store FS token
		tk = scanner(input);						// consume FStk
		t->child2 = T(input);						// process T
		return t;												// return: <F> / <T>
	} else
		return t; 											// return: <F>
}

/**F
 *---------
 * representation of the <F> non terminal in the grammar.
 *
 * Inputs:
 * input...the input stream we are parsing
 *
 * returns the parse substree, with node from this function as the root.
 */
static node_t* F(ifstream& input)  { 
	node_t* f = genNode("<F>");				// generate F node
	if (tk.tokenID == MINUStk) {			// predict MINUStk
		f->token1 = copyToken(tk);			// store MINUS token
		tk = scanner(input);						// consume MINUStk
		f->child1 = F(input);						// process F
		return f;												// return: - <F>
	} else {
		f->child1 = R(input);						// process R 
		return f;												// return: <R>
	}
} 

/**R 
 *--------- 
 * representation of the <R> non terminal in the grammar.  
 * 
 * Inputs:
 * input...the input stream we are parsing
 *
 * returns the parse substree, with node from this function as the root.
 */ 
static node_t* R(ifstream& input)  {  
	node_t* r = genNode("<R>");				// generate R node
	if (tk.tokenID == LPARtk) {       // predict LPARtk 
		tk = scanner(input);						// consume LPARtk 
		r->child1 = Expr(input);				// process Expr
		if (tk.tokenID == RPARtk) {			// predict RPARtk
			tk = scanner(input);					// consume RPARtk
			return r;											// return: ( <expr> )										
		}	else
			parseError("RPARtk", tk);
	} else if (tk.tokenID == IDtk) {	// predict IDtk
		r->token1 = copyToken(tk);			// store Identifier
		tk = scanner(input);						// consume IDtk
		return r;												// return: Identifier	
	} else if (tk.tokenID == INTtk) {	// predict INTtk
		r->token1 = copyToken(tk);			// store Number
		tk = scanner(input);						// consume INTtk
		return r;												// return: Number
	} else
		parseError("LPARtk, IDtk, or INTtk", tk);
}


/**Stats
 *---------
 * representation of the <stats> non terminal in the grammar.
 *
 * Inputs:
 * input...the input stream we are parsing
 *
 * returns the parse substree, with node from this function as the root.
 */
static node_t* Stats(ifstream& input)  {  
	node_t* s = genNode("<stats>");		// generate Stats node 
	s->child1 = Stat(input);					// process Stat
	if (tk.tokenID == COLtk) {				// predict COLtk
		tk = scanner(input);						// consume COLtk
		s->child2 = MStat(input);				// process MStat
		return s;												// return: <stat> : <mStat>
	} else 
		parseError("COLtk", tk);	
}


/**MStat
 *---------
 * representation of the <mStat> non terminal in the grammar.
 *
 * Inputs:
 * input...the input stream we are parsing
 *
 * returns the parse substree, with node from this function as the root.
 */
static node_t* MStat(ifstream& input)  {  
	node_t* m = genNode("<mStat>");   // generate mStat node
	
	// predict what follows mStat, but don't consume if prediction is correct
	if (tk.tokenID == ENDtk)   				// precict what follows mStat	
		return NULL;										// return: empty production 
	
	m->child1 = Stat(input);					// process Stat
	if (tk.tokenID == COLtk) {   			// predict COLtk
		tk = scanner(input);      			// consume COLtk
		m->child2 = MStat(input);				// process MStat
		return m;												// return: <stat> : <mStat>		
	} else
		parseError("COLtk", tk);
}


/**Stat
 *---------
 * representation of the <stat> non terminal in the grammar.
 *
 * Inputs:
 * input...the input stream we are parsing
 *
 * returns the parse substree, with node from this function as the root.
 */
static node_t* Stat(ifstream& input)  {  
	node_t* s = genNode("<stat>");
	// for each production, predict what follows but don't consume, as prediction will be predicted and consumed by lower levels of grammar
	if (tk.tokenID == INPUTtk) {         	// predict what follows In
		s->child1 = In(input);						 	// process In	
		return s;													 	// return: <in>
	} else if (tk.tokenID == OUTPUTtk) { 	// predict what follows Out
		s->child1 = Out(input);						 	// process Out
		return s;														// return: <out> 
	} else if (tk.tokenID == BEGINtk) {		// predict what follows Block
		s->child1 = Block(input);						// process Block
		return s;														// return: <block>
	} else if (tk.tokenID == IFtk) {			// predict what follows If
		s->child1 = If(input);							// process IF
		return s;														// return: <if>
	} else if (tk.tokenID == FORtk) {			// predict what follows Loop
		s->child1 = Loop(input);						// process Loop				
		return s;														// return: <loop>
	} else if (tk.tokenID == IDtk) {			// predict what follows Assign
		s->child1 = Assign(input);					// process Assign
		return s;														// return: <assign>
	} else
		parseError("INPUTtk, OUTPUTtk, BEGINtk, IFtk, FORtk, or IDtk", tk);
}


/**In
 *---------
 * representation of the <in> non terminal in the grammar.
 *
 * Inputs:
 * input...the input stream we are parsing
 *
 * returns the parse substree, with node from this function as the root.
 */
static node_t* In(ifstream& input)  {  
	node_t* i = genNode("<in>");					// generate In node
	if (tk.tokenID == INPUTtk) {  				// predict INPUTtk
		tk = scanner(input);								// consume INPUTtk
		if (tk.tokenID == IDtk) {						// predict IDtk
			i->token1 = copyToken(tk);				// store Identifier
			tk = scanner(input);							// consume IDtk
			return i;													// return: INPUT Identifier 
		} else
			parseError("IDtk", tk);
	} else
		parseError("INPUTtk", tk);
}


/**Out
 *---------
 * representation of the <out> non terminal in the grammar.
 *
 * Inputs:
 * input...the input stream we are parsing
 *
 * returns the parse substree, with node from this function as the root.
 */
static node_t* Out(ifstream& input)  {  
	node_t* o = genNode("<out>");					// generate Out node
	if (tk.tokenID == OUTPUTtk) {					// predict OUTPUTtk
		tk = scanner(input);								// consume OUTPUTtk
		o->child1 = Expr(input);						// process Expr
		return o;														// return: OUTPUT <expr> 
	} else 
		parseError("OUTPUTtk", tk);
}


/**If
 *---------
 * representation of the <if> non terminal in the grammar.
 *
 * Inputs:
 * input...the input stream we are parsing
 *
 * returns the parse substree, with node from this function as the root.
 */
static node_t* If(ifstream& input)  {  
	node_t* i = genNode("<if>");					// generate If node
	if (tk.tokenID == IFtk) {             // predict IFtk
		tk = scanner(input);								// consume IFtk
		if (tk.tokenID == LPARtk) {					// predict LPARtk
			tk = scanner(input);							// consume LPARtk
			i->child1 = Expr(input);					// process Expr
			i->child2 = RO(input);						// process RO
			i->child3 = Expr(input);					// process Expr
			if (tk.tokenID == RPARtk) {       // predict RPARtk
				tk = scanner(input);						// consume RPARtk
				i->child4 = Block(input);			  // process Block
				return i;												// return: IF ( <expr> <RO> <expr> ) <block>
			} else
				parseError("RPARtk", tk);
		} else
			parseError("LPARtk", tk);
	} else
		parseError("IFtk", tk);
}


/**Loop
 *---------
 * representation of the <loop> non terminal in the grammar.
 *
 * Inputs:
 * input...the input stream we are parsing
 *
 * returns the parse substree, with node from this function as the root.
 */
static node_t* Loop(ifstream& input)  {  
	node_t* l = genNode("<loop>");				// generate For node
	if (tk.tokenID == FORtk) { 						// predict FORtk
		tk = scanner(input);								// consume FORtk
		if (tk.tokenID == LPARtk) {					// predict LPARtk
			tk = scanner(input);							// consume LPARtk
			l->child1 = Expr(input);					// process Expr
			l->child2 = RO(input);						// process RO
			l->child3 = Expr(input);					// process Expr
			if (tk.tokenID == RPARtk) {				// predict RPARtk
				tk = scanner(input);						// consume RPARtk
				l->child4 = Block(input);				// process Block
				return l;												// return: FOR ( <expr> <RO> <expr> ) <block>
			} else
				parseError("RPARtk", tk); 
		} else
			parseError("LPARtk", tk);
	} else
		parseError("FORtk", tk);
}


/**Assign
 *---------
 * representation of the <assign> non terminal in the grammar.
 *
 * Inputs:
 * input...the input stream we are parsing
 *
 * returns the parse substree, with node from this function as the root.
 */
static node_t* Assign(ifstream& input)  {  
	node_t* a = genNode("<assign>");			// generate Assign node
	if (tk.tokenID == IDtk) {							// predict IDtk
		a->token1 = copyToken(tk);					// store identifier
		tk = scanner(input);								// consume IDtk
		if (tk.tokenID == EQtk) {						// predict EQtk
			tk = scanner(input);							// consume IDtk
			a->child1 = Expr(input);					// process Expr
			return a;													// return: Identifier = <expr> 
		} else
			parseError("EQtk", tk);
	} else
		parseError("IDtk", tk);
}


/**RO
 *---------
 * representation of the <RO> non terminal in the grammar.
 *
 * Inputs:
 * input...the input stream we are parsing
 *
 * returns the parse substree, with node from this function as the root.
 */
static node_t* RO(ifstream& input)  {  
	node_t* r = genNode("<RO>");					// generate RO node
	if (tk.tokenID == RIGHTtk) {					// predict RIGHTtk
		r->token1 = copyToken(tk);					// store RIGHTtk
		tk = scanner(input);								// consume RIGHTtk
		if (tk.tokenID == EQtk) {						// predict EQtk
			r->token2 = copyToken(tk);				// store EQtk
			tk = scanner(input);							// consume EQtk
			return r;													// return: >=> =
		} else
			return r;													// return: >=>
	} else if (tk.tokenID == LEFTtk) {		// predict LEFTtk
		r->token1 = copyToken(tk);					// store LEFTtk
		tk = scanner(input);								// consume LEFTtk
		if (tk.tokenID == EQtk) {						// predict EQtk
			r->token2 = copyToken(tk);				// store EQtk
			tk = scanner(input);							// consume EQtk
			return r;													// return: <=< =
		} else
			return r;													// return: <=<
	} else if (tk.tokenID == DEXtk) {			// preduct DEXtk
		r->token1 = copyToken(tk);					// store DEXtk
		tk = scanner(input);								// consume DEXtk
		return r;														// return: !!												
	} else if (tk.tokenID == EQtk) {			// predict EQtk
		r->token1 = copyToken(tk);					// store EQtk
		tk = scanner(input);								// consume EQtk
		if (tk.tokenID == EQtk) {						// predict EQtk
			r->token2 = copyToken(tk);				// store EQtk
			tk = scanner(input);							// consume EQtk
			return r;													// return: = =
		} else
			parseError("EQtk", tk);
	} else
		parseError("LEFTtk, RIGHTtk, EQtk, or DEXtk", tk); 
} 


/**genNode
 * -----
 * generate a new parse tree node
 *
 * inputs:
 * label....the label of the node
 * 
 * returns the node, which contains the provided label
 */
static node_t* genNode(const string label) {
	node_t* node = new node_t;
	node->label = label;
	node->token1 = NULL;
	node->token2 = NULL;
	node->child1 = NULL;
	node->child2 = NULL;
	node->child3 = NULL;
	node->child4 = NULL;
	return node;
}

