/*
 * Tokenizer.h
 *
 *  Created on: Oct 29, 2012
 *      Author: trevor
 */

#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <sstream>

using namespace std;

class Tokenizer {

public:

	Tokenizer();
	~Tokenizer();

	string strip_tags(string t);
	string strip_pos_tags(string t);
	vector<string> tokenize(string t);
	bool blank_line(string t);

private:
	vector<string> tokens;

};

#endif /* TOKENIZER_H_ */
