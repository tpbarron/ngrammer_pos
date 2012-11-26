/*
 * Tokenizer.cpp
 *
 *  Created on: Oct 29, 2012
 *      Author: trevor
 */

#include "Tokenizer.h"

using namespace std;

Tokenizer::Tokenizer() {}

Tokenizer::~Tokenizer() {}

string Tokenizer::strip_tags(string t) {
	//strip <s> and </s> tags
	if (t.substr(0, 3) == "<s>") {
		t = t.substr(3);
	}
	if (t.substr(t.length() - 4) == "</s>") {
		t = t.substr(0, t.length() - 4);
	}
	return t;
}

string Tokenizer::strip_pos_tags(string t) {
	//strip <s> and </s> tags
	if (t.substr(0, 12) == "<s>|S_START ") {
		t = t.substr(12);
	}
	if (t.substr(t.length() - 11) == " </s>|S_END") {
		t = t.substr(0, t.length() - 11);
	}
	return t;
}

bool Tokenizer::blank_line(string t) {
	for (unsigned int c = 0; c < t.length(); c++) {
		if (t[c] != ' ' && t[c] != '\t')
			return false;
	}
	return true;
}

vector<string> Tokenizer::tokenize(string t) {
	tokens.clear();

	//create char array
	int tmp = t.size();
	char t_char[tmp];
	for (int a = 0; a <= tmp; a++) {
		t_char[a]=t[a];
	}
	char * pch;
	//cout << "Splitting string '" << t << "' into tokens." << endl;

	pch = strtok(t_char, " ");
	while (pch != NULL) {
		string s = (pch);
		tokens.push_back(s);
		//printf("%s\n",pch);
		pch = strtok (NULL, " ");
	}

	return tokens;
}

