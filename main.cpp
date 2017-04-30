/* Nathaniel Want (nwqk6)
 * CS4280-E02
 * P4
 * 4/24/2017
 * ----------
 * main.cpp
 * ---------
 * the main application for P1. Run a test on the scanner module using testScanner.
 */
#include <csignal>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "node.h"
#include "parser.h"
#include "codegen.h"
using namespace std;

static const string EXE = "comp"; // the executable name
static const string EXT_IN = ".4280E02";     // expected file extention for input data
static const string EXT_OUT = ".asm";
static const string DEFAULT_OUT = "out.asm";
static char* tempfile; 										// the filename of the temporary file 
static ifstream input;
static ofstream output;

static void cleanUpThenDie(int signum);
static void copyData(const char* inputFp, const char* outputFp);
static void fileOkOrDie(const char* fp); 
static void keyboardToFile(const char* outputFp); 

int main(int argc, char* argv[]) {
	char *tempfp; // tempfile file path. used write all input into before building the tree.
	string inputFp, outputFp;
	// create a temp file to store all input needed for the tree	
 	mkstemp(tempfp);	
	// store this filename in global variable in case we have errors and have to failsafe
	tempfile = (char*)malloc(sizeof(char) * (strlen(tempfp)));
	strcpy(tempfile, tempfp);
	
	// where is the input coming from?
	if (argc == 1) { // the keyboard / directed file stream
		keyboardToFile(tempfp);
		outputFp = DEFAULT_OUT;
	} else if (argc == 2) { // a file
		outputFp = inputFp = argv[1];
		inputFp += EXT_IN;
		outputFp += EXT_OUT;
		copyData(inputFp.c_str(), tempfp);	
	} else { // unknown: error
		cout << "Exceeded number of valid arguments. Proper usage: " << EXE << " [file] where file has a " << EXT_IN << " extention." << endl;
		exit(1);
	}
	
	// register the signalhandler. Each module will raise this signal after 
	// producing an error to the user if/when an error occurs.
	signal(SIGINT, cleanUpThenDie); 
	
	// execute front end.
	input.open(tempfp);
	node_t* root = parser(input);	
	input.close();	
	
	// execute back end.
	output.open(outputFp.c_str()); 
	codegen(root, output);	
	cout << "Static Semantics Passed!" << endl;
	output.close();

	// clean up
	remove(tempfp);

	return 0;
}


/**cleanUpThenDie
 *--------------
 * clean up all resources and exit the program. 
 *
 * inputs:
 * signum...the number of the signal raised.
 */
static void cleanUpThenDie(int signum) {
	input.close();
	output.close();
	remove(tempfile);
	free(tempfile);
	exit(signum);
}

/* fileOkOrDie
 * -----------
 *  Determine is a file is accessible. Provide error message and quit program if file is not ok.
 *
 *  Inputs:
 *  fp....the filepath for the file name
 */
static void fileOkOrDie(const char* fp) {
	ifstream file;
	file.open(fp);
	bool fileIsGood = file.good();
	file.close();
	if (!fileIsGood) {
		cout << "Could not read data from file \"" << fp << "\". Please make sure this file exists and is accessable to you." << endl;
		exit(1);
	}	
}


/* keyboardToFile 
 * -----------
 * Copy contents from stdin to a file until EOF is reached.
 *
 *  Inputs:
 *  outputFp....the file path for the file to write contents from stdin into.
 */
static void keyboardToFile(const char* outputFp) {
	char c;
	ofstream outfile;

	outfile.open(outputFp);
	while (cin.get(c)) {
		outfile << c;
	}
	outfile.close();
}


/* copyData 
 * -----------
 * Copy contents from one file to another file. Program will quit if there are 
 * any issues reading from the input file. 
 *
 *  Inputs:
 *  inputFp....the file path for the file to read from
 *  outputFp...the file path for the file to write to 
 */
static void copyData(const char* inputFp, const char* outputFp) {
	char c;
	ifstream infile;
	ofstream outfile;

	fileOkOrDie(inputFp);
	
	infile.open(inputFp);	
	outfile.open(outputFp);
	while (infile.get(c)) {
		outfile << c;
	}
	infile.close();
	outfile.close();
}

