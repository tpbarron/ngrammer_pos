/*
 * Template.h
 *
 *  Created on: Oct 30, 2012
 *      Author: trevor
 */

#ifndef TEMPLATE_H_
#define TEMPLATE_H_

#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <vector>

using namespace std;

class Template {

public:

	Template();
	~Template();

	void check(vector<string> * tokens);
	void add_type(string type, string file);

private:

	struct w_type {
		string type;
		string file;
		vector<string> values;
	};

	void read_file(string f, struct w_type *p);
	list<w_type> type_list;

};

#endif /* TEMPLATE_H_ */
