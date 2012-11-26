/*
 * Ngram.h
 *
 *  Created on: Oct 30, 2012
 *      Author: trevor
 */

#ifndef NGRAM_H_
#define NGRAM_H_

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <math.h>
#include <algorithm>
#include "Tokenizer.h"

using namespace std;

class Ngram {

public:

	struct wpos_t {
		string w;
		string pos;
	};

	struct w_data_t {
		string w;
		string pos;
		double freq;
	};

	struct generate_result_t {
		vector<string> result;
		double p;
	};

	Ngram(const int deg);
	~Ngram();

	//void add_ngram(vector<string> ngram);
	void add_ngram(vector<wpos_t> ngram);
	double get_frequency(vector<string> *ngram);
	double get_frequency(int n, int count);
	double get_frequency(int count);
	double get_frequency(int n, string w);
	int get_ngram_count();
	void print_ngrams();
	void write_ngrams(ofstream *f);
	vector <w_data_t> get_next_ngrams(vector<wpos_t> ngram, int size=10);
	string generate_seed(string seed, unsigned int l);
	string generate(string s1, string s2);
	generate_result_t generate_shortest_path(string s1, string s2, unsigned int len);
	double gram_probability(vector<string> ngram, unsigned int g);
	double string_probability(string sentence, unsigned int g);
	void set_p_thresh(double pt);
	static bool ngram_result_cmp(const w_data_t &a, const w_data_t &b);


private:

	struct ngram_t {
		int count;
		string pos;
		string w;
		map<string, ngram_t> nxgrams;
	};

	struct ng_pair_t {
		string w;
		double p;
	};

	unsigned int ngram_degree;
	unsigned int ngram_count;
	vector<int> ngram_counts;
	double p_thresh;
	struct ngram_t ngrams;

	void print_ngrams_helper(ngram_t *ngram, vector<string> s, unsigned int n);
	//void add_ngram_helper(ngram_t *ngs, vector<string> ngram, unsigned int n);
	void add_ngram_helper(ngram_t *ngs, vector<wpos_t> ngram, unsigned int n);
	void write_ngrams_helper(ofstream *f, ngram_t *ngrams, vector<string> s, unsigned int n);
	void generate_search(string s1, string s2,
			vector<string> result_vec, vector<generate_result_t> *return_vec,
			double p, ngram_t ng, unsigned int depth);
	/*void generate_shortest_path_search_list(string s1, string s2, unsigned int len,
			vector<string> path, vector<generate_result_t> *paths, double p, unsigned int d);*/
	void generate_shortest_path_search(string s1, string s2,
			generate_result_t *result, vector<string> cur_path, double p, double *max_p,
			unsigned int d, unsigned int len);
	static bool generate_result_cmp(generate_result_t a, generate_result_t b);
	static bool ngram_pair_cmp(const ng_pair_t &a, const ng_pair_t &b);
	ngram_t find_largest_path(vector<wpos_t> *ngram);
	void set_base_pthresh(int len);
	string vector_to_string(vector<string> *s);

};

#endif /* NGRAM_H_ */
