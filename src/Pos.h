/*
 * Ngram.h
 *
 *  Created on: Oct 30, 2012
 *      Author: trevor
 */

#ifndef POS_H_
#define POS_H_

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <math.h>
#include <algorithm>
#include "Tokenizer.h"
#include "Ngram.h"

using namespace std;

class Pos {

public:

	struct pos_result_t {
		string pos;
		double freq;
	};

	Pos(const int deg);
	~Pos();

	//void add_ngram(vector<string> ngram);
	void add_pos(vector<Ngram::wpos_t> pgram);
	double get_frequency(vector<string> *pgram);
	double get_frequency(int n, int count);
	double get_frequency(int count);
	int get_pgram_count();
	void write_pgrams(ofstream *f);
	vector<Pos::pos_result_t> get_next_pos(vector<Ngram::wpos_t> pgram, int size=10);

	/*string generate_seed(vector<string> seed, int l);
	string generate(string s1, string s2);
	string generate_shortest_path(string s1, string s2, unsigned int len);
	double gram_probability(vector<string> ngram, unsigned int g);
	double string_probability(string sentence, unsigned int g);
	void set_p_thresh(double pt);*/

private:

	/*struct pair_t {
		vector<string> result;
		double p;
	};

	struct ng_pair_t {
		string w;
		double p;
	};*/

	struct pgram_t {
		int count;
		string pos;
		map<string, pgram_t> pxgrams;
	};

	unsigned int pgram_degree;
	unsigned int pgram_count;
	vector<int> pgram_counts;
	double p_thresh;
	struct pgram_t pgrams;

	void add_pgram_helper(pgram_t *ngs, vector<Ngram::wpos_t> pgram, unsigned int n);
	void write_pgrams_helper(ofstream *f, pgram_t *pgrams, vector<string> s, unsigned int n);
	/*void generate_search(string s1, string s2,
			vector<string> result_vec, vector<pair_t> *return_vec,
			double p, pgram_t ng, unsigned int depth);
	void generate_shortest_path_search(string s1, string s2, unsigned int len,
			vector<string> path, vector<pair_t> *paths, double p, unsigned int d);
	static bool pgram_pair_cmp(const ng_pair_t &a, const ng_pair_t &b);
	void add_to_sorted_list(vector<ng_pair_t> *level_sort, ng_pair_t *npr);
	void set_base_pthresh(int len);*/
	pgram_t find_largest_path(vector<Ngram::wpos_t> *pgram);
	static bool pos_result_cmp(const pos_result_t &a, const pos_result_t &b);
	string vector_to_string(vector<string> *s);

};

#endif /* POS_H_ */
