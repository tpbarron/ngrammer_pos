/*
 * Template.cpp
 *
 *  Created on: Oct 30, 2012
 *      Author: trevor
 */

#include "Template.h"

using namespace std;

Template::Template() {}

Template::~Template() {}

void Template::check(vector<string> *tokens) {
	for (unsigned int i = 0; i < (*tokens).size(); i++) {
		string s = (*tokens)[i];
		for (list<w_type>::iterator it = type_list.begin(); it != type_list.end(); it++) {
			for (unsigned int j = 0; j < (*it).values.size(); j++) {
				string sv = (*it).values[j];
				if (s.compare(sv) == 0) {
					(*tokens)[i] = (*it).type;
				}
			}
		}
	}
}

void Template::add_type(string type, string file) {
	struct w_type p;
	p.file = file;
	p.type = type;
	read_file(file, &p);
	//Wow, I had the next line before the previous one. And it didn't keep the data. Why
	//is does passing &p after adding it to the list not still change that data?
	type_list.push_back(p);
}

void Template::read_file(string f, struct w_type *p) {
	ifstream ifs(f.c_str());
	if (ifs.is_open()) {
		cout << "Reading file: " << f << endl;
		string s;
		vector<string>::iterator it;
		while (getline(ifs, s)) {
			if ((*p).values.size() == 0) {
				(*p).values.push_back(s);
				cout << "Inserting " << (*p).type << ": " << s << endl;
			} else {
				bool inserted = false;
				for (unsigned int i = 0; i < (*p).values.size(); i++) {
					if (s.compare((*p).values[i]) < 0) {
						cout << "Inserting " << (*p).type << ": " << s << endl;
						(*p).values.insert((*p).values.begin()+i, s);
						inserted = true;
					}
				}
				if (!inserted) {
					(*p).values.push_back(s);
					cout << "Inserting " << (*p).type << ": " << s << endl;
				}
			}
		}
		//cout << (*p).values[0] << " " << (*p).values[1] << endl;
	} else {
		cout << "Can not open template file: " << f << endl;
	}
}

