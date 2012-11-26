/*
 * Ngram.cpp
 *
 *  Created on: Oct 30, 2012
 *      Author: trevor
 */

/*
 ADJ	adjective	new, good, high, special, big, local
ADV	adverb	really, already, still, early, now
CNJ	conjunction	and, or, but, if, while, although
DET	determiner	the, a, some, most, every, no
EX	existential	there, there's
FW	foreign word	dolce, ersatz, esprit, quo, maitre
MOD	modal verb	will, can, would, may, must, should
N	noun	year, home, costs, time, education
NP	proper noun	Alison, Africa, April, Washington
NUM	number	twenty-four, fourth, 1991, 14:24
PRO	pronoun	he, their, her, its, my, I, us
P	preposition	on, of, at, with, by, into, under
TO	the word to	to
UH	interjection	ah, bang, ha, whee, hmpf, oops
V	verb	is, has, get, do, make, see, run
VD	past tense	said, took, told, made, asked
VG	present participle	making, going, playing, working
VN	past participle	given, taken, begun, sung
WH	wh determiner	who, which, when, what, where, how
 */

#include "Ngram.h"

using namespace std;

Ngram::Ngram(int deg) {
	ngram_degree = deg;
	ngram_count = 0;
	for (int i = 0; i < deg; i++) {
		ngram_counts.push_back(0);
	}
	p_thresh = deg*(-5.0);
}

Ngram::~Ngram() {}

void Ngram::add_ngram(vector<wpos_t> ngram) {
	if (ngram.size() != ngram_degree) {
		cerr << "Vector must be of length degree." << endl;
	} else {
		add_ngram_helper(&ngrams, ngram, 0);
	}
}

void Ngram::add_ngram_helper(ngram_t *ngs, vector<wpos_t> ngram, unsigned int n) {
	ngram_counts[n]++;
	if (n == ngram.size()) {
		if ((*ngs).count == 1) { //a new ngram
			ngram_count++;
		}
		(*ngs).count++;
		return;
	}
	wpos_t pt = ngram[n];
	string s = pt.w;
	string pos = pt.pos;
	if ((*ngs).nxgrams.find(s) != (*ngs).nxgrams.end()) {
		//if found, increment step, set word, set pos
		(*ngs).nxgrams[s].count++;
		(*ngs).nxgrams[s].w = s;
		(*ngs).nxgrams[s].pos = pos;
	} else {
		//does not exist
		ngram_t ng;
		ng.count = 1;
		ng.w = s;
		ng.pos = pos;
		(*ngs).nxgrams[s] = ng;
	}
	add_ngram_helper(&((*ngs).nxgrams[s]), ngram, n+1);
}

void Ngram::print_ngrams() {
	cout << "Printing " << ngram_degree << "-grams... " << ngram_count << endl;
	vector<string> s;
	print_ngrams_helper(&ngrams, s, 0);
}

void Ngram::print_ngrams_helper(ngram_t *ngram, vector<string> s, unsigned int n) {
	if (n == ngram_degree) {
		cout << ngram_degree << "-gram: " << vector_to_string(&s) << " " << get_frequency(&s) << endl;
		return;
	}
	map<string, ngram_t>::iterator itr;
	for (itr = (*ngram).nxgrams.begin(); itr != (*ngram).nxgrams.end(); itr++) {
		string sx = (*itr).first;
		ngram_t ngx = (*itr).second;
		s.push_back(sx);
		print_ngrams_helper(&ngx, s, n+1);
		s.pop_back();
	}
}

void Ngram::write_ngrams(ofstream *f) {
	cout << "Writing " << ngram_degree << "-grams... " << ngram_count << endl;
	if ((*f).is_open()) {
		(*f) << ":" << ngram_degree << "-grams" << endl;

		vector<string> s;
		Ngram::write_ngrams_helper(&(*f), &ngrams, s, 0);

		(*f) << "\n";
	} else {
		cout << "Unable to open file." << endl;
	}
}

void Ngram::write_ngrams_helper(ofstream *f, ngram_t *ngram, vector<string> s, unsigned int n) {
	if (n == ngram_degree) {
		(*f) << vector_to_string(&s) << " " << get_frequency((*ngram).count) << "\n";
		return;
	}
	map<string, ngram_t>::iterator itr;
	for (itr = (*ngram).nxgrams.begin(); itr != (*ngram).nxgrams.end(); itr++) {
		string sx = (*itr).first;
		ngram_t ngx = (*itr).second;
		s.push_back(sx);
		write_ngrams_helper(&(*f), &ngx, s, n+1);
		s.pop_back();
	}
}

string Ngram::generate_seed(string seed, unsigned int l) {
	vector<wpos_t> text;
	wpos_t sdata;
	sdata.w = seed;
	sdata.pos = "";
	text.push_back(sdata);

	while (true) {
	//for (unsigned int i = 0; i < l; i++) {
		//get likely next words
		//chose most probable
		//append
		w_data_t next = get_next_ngrams(text)[0];
		wpos_t nextwp;
		nextwp.w = next.w;
		nextwp.pos = next.pos;
		text.push_back(nextwp);
		if (nextwp.w == "</s>")
			break;
	//}
	}
	string s = "";
	for (unsigned int a = 0; a < text.size(); a++) {
		s += text[a].w + " ";
	}
	return s;
}

vector <Ngram::w_data_t> Ngram::get_next_ngrams(vector<wpos_t> ngram, int size) {
	vector<w_data_t> result;
	/* suppose I have a sentence
	 *
	 * then I want to find the last N words that
	 * are linked in data structure
	 *
	 * if (suppose -> I -> have -> a -> sentence) is not linked
	 * try (I -> have -> a -> sentence) if not
	 * try (have -> a -> sentence) if not
	 * try (a -> sentence) if not
	 * try (sentence...)
	 * if not (sentence...) then there is something wrong
	 * because sentence came out of the previous one
	 */
	//cout << "Finding longest path [ngram]." << endl;
	//ngram_t ng = find_largest_path(&ngram);
	ngram_t ng = ngrams.nxgrams[ngram[ngram.size()-1].w];
	//cout << "Longest path found [ngram]." << endl;
	//now ng is at the deepest level of the ngram structs
	map<string, ngram_t>::iterator itr;
	for (itr = ng.nxgrams.begin(); itr != ng.nxgrams.end(); itr++) {
		//loop through all next ngram_t
		w_data_t ng_result;
		ng_result.pos = (*itr).second.pos;
		ng_result.w = (*itr).second.w;
		//TODO: what frequency do I want here?
		ng_result.freq = get_frequency((*itr).second.count);
		result.push_back(ng_result);
	}
	//cout << "Vector size: " << result.size() << ", trying to access: " << size << endl;
	sort(result.begin(), result.end(), ngram_result_cmp);
	return result;
}

Ngram::ngram_t Ngram::find_largest_path(vector<wpos_t> *ngram) {
	unsigned int d;
	if ((*ngram).size() < ngram_degree)
		d = (*ngram).size();
	else
		d = ngram_degree-1;

	ngram_t ng;
	ng.count = -1;

	cout << "Checking ngrams from max depth: " << d << endl;
	while (d >= 0) {
		ngram_t ng_test = ngrams;
		bool found = true;
		for (unsigned int a = d; a > 0; a--) {
			//cout << "a = " << a << endl;
			string w = (*ngram)[(*ngram).size()-a].w;
			cout << "Checking [" << w << "]..." << endl;
			if (ng_test.nxgrams.find(w) != ng_test.nxgrams.end()) {
				ng_test = ng_test.nxgrams[w];
				cout << "[" << w << "] found." << endl;
			} else {
				cout << "[" << w << "] not found." << endl;
				found = false;
				break;
			}
		}
		if (found) {
			cout << "Ngram result found! " << endl;
			return ng_test;
		}
		d--;
	}
	return ng;
}

bool Ngram::ngram_result_cmp(const w_data_t &a, const w_data_t &b) {
	return a.freq > b.freq;
}

string Ngram::generate(string s1, string s2) {
	//find greatest probable path from s1 to s2
	//need to search tree of structs starting from s1 for s2
	ngram_t ng = ngrams;
	if (ng.nxgrams.find(s1) != ng.nxgrams.end()) {
		cout << "s1 [" << s1 << "] found." << endl;
		ng = ng.nxgrams[s1];
		//traverse all looking for s2;
		vector<string> result_vec;
		vector<generate_result_t> return_vec;
		result_vec.push_back(s1);
		generate_search(s1, s2, result_vec, &return_vec, 0, ng, 0);
		cout << "Search finished." << endl;
		if (return_vec.size() == 0) {
			return "No results found.";
		}
		double max_p = 0;
		vector<string> max_vec;
		for (unsigned int i = 0; i < return_vec.size(); i++) {
			generate_result_t pr = return_vec[i];
			cout << "result: " << vector_to_string(&(pr.result)) << " [" << pr.p << "]" << endl;
			if (i == 0 || pr.p > max_p) {
				cout << "Setting max: " << pr.p << endl;
				max_p = pr.p;
				max_vec = pr.result;
			}
		}
		return vector_to_string(&max_vec);
	} else {
		cerr << "s1 [" << s1 << "] does not exist." << endl;
		return "";
	}
}

void Ngram::generate_search(string s1, string s2,
		vector<string> result_vec, vector<generate_result_t> *return_vec,
		double p, ngram_t ng, unsigned int depth) {

	//cout << "Search: p = " << p << " depth = " << depth << endl;
	if (depth == ngram_degree) {
		return;
	}

	map<string, ngram_t>::iterator itr;
	for (itr = ng.nxgrams.begin(); itr != ng.nxgrams.end(); itr++) {
		string s = (*itr).first;
		ngram_t ngx = (*itr).second;
		double px = get_frequency(ngx.count);

		//copy array so not pass by ref
		vector<string> res_vec;
		for (unsigned int n = 0; n < result_vec.size(); n++) {
			res_vec.push_back(result_vec[n]);
		}
		res_vec.push_back(s);

		if (s == s2) {
			generate_result_t pair;
			pair.result = res_vec;
			pair.p = p+px;
			(*return_vec).push_back(pair);
		} else {
			generate_search(s1, s2, res_vec, &(*return_vec), p+px, ngx, depth+1);
		}
	}
}


Ngram::generate_result_t Ngram::generate_shortest_path(string s1, string s2, unsigned int len) {
	//reset p_thresh based on length and number of bigrams
	set_base_pthresh(len);
	generate_result_t result;
	if (ngrams.nxgrams.find(s1) == ngrams.nxgrams.end()) {
		cout << "s1 [" << s1 << "] not found." << endl;
		return result;
	}

	vector<string> cur_path;
	double p = 0, max_p = 0;
	unsigned int d = 0;
	cur_path.push_back(s1);
	generate_shortest_path_search(s1, s2, &result, cur_path, p, &max_p, d, len);

	return result;
}

void Ngram::generate_shortest_path_search(string s1, string s2,
		generate_result_t *result, vector<string> cur_path, double p, double *max_p,
		unsigned int d, unsigned int len) {
	cout << "Generating shortest path..." << "[" << p << "] [" << vector_to_string(&cur_path) << "]" << endl;
	if (s1 == s2 && cur_path.size() == len) {
		cout << "Match found!" << endl;
		if (p > (*max_p) || (*max_p) == 0) {
			cout << "New best path! " << vector_to_string(&cur_path) << " [" << (*max_p) << "] [" << p << "]" << endl;
			(*max_p) = p;
			(*result).p = p;
			(*result).result = cur_path;
		}
		return;
	}
	if (cur_path.size() == len) {
		cout << "Ngram reached length limit." << endl;
		return;
	}
	if (p < p_thresh) {
		cout << "Probability too low." << endl;
		return;
	}
	if (ngrams.nxgrams.find(s1) == ngrams.nxgrams.end()) {
		cerr << "s1 [" << s1 << "] at depth [" << d <<"] not found." << endl;
		return;
	}

	//iterate through next level
	//sort into list based on prob
	//then call next level with most probable first
	ngram_t ng = ngrams.nxgrams[s1];
	cout << "map size: " << ng.nxgrams.size() << endl;
	map<string, ngram_t>::iterator itr;
	vector<ng_pair_t> level_sort;
	for (itr = ng.nxgrams.begin(); itr != ng.nxgrams.end(); itr++) {
		ng_pair_t npr;
		string s1_a = (*itr).first;
		vector<string> bigram;
		bigram.push_back(s1);
		bigram.push_back(s1_a);
		double prob = get_frequency(2, (*itr).second.count); //get_frequency(&bigram);
		npr.w = s1_a;
		npr.p = prob;
		level_sort.push_back(npr);
	}
	sort(level_sort.begin(), level_sort.end(), ngram_pair_cmp);

	for (unsigned int n = 0; n < level_sort.size(); n++) {
		cout << "Recursive call... [" << n << "]." << endl;
		ng_pair_t nprx = level_sort[n];
		//if p is already to low stop
		if (!p+nprx.p < (*max_p)) {
			cur_path.push_back(nprx.w);
			generate_shortest_path_search(nprx.w, s2, &(*result), cur_path, p+nprx.p, &(*max_p), d+1, len);
			cur_path.pop_back();
		} else {
			cout << "Prob too low, stopping call [ngram]." << endl;
		}
	}
}


bool Ngram::ngram_pair_cmp(const ng_pair_t &a, const ng_pair_t &b) {
	return a.p > b.p;
}

bool Ngram::generate_result_cmp(generate_result_t a, generate_result_t b) {
	return a.p > b.p;
}

/*Ngram::generate_result_t Ngram::generate_shortest_path_list(string s1, string s2, unsigned int len) {
	//reset p_thresh based on length and number of bigrams
	set_base_pthresh(len);
	if (ngrams.nxgrams.find(s1) == ngrams.nxgrams.end()) {
		cout << "s1 [" << s1 << "] not found." << endl;
		generate_result_t r;
		r.p = 0;
		return r;
	}

	vector<generate_result_t> paths;
	vector<string> path;
	path.push_back(s1);

	generate_shortest_path_search(s1, s2, len, path, &paths, 0, 0);

	sort(paths.begin(), paths.end(), generate_result_cmp);
	return paths[0];
}


void Ngram::generate_shortest_path_search_list(string s1, string s2, unsigned int len,
		vector<string> path, vector<generate_result_t> *paths, double p, unsigned int d) {
	cout << "Generating shortest path..." << "[" << p << "] [" << vector_to_string(&path) << "]" << endl;
	if (s1 == s2 && path.size() == len) {
		cout << "Match found!" << endl;
		generate_result_t pr;
		pr.p = p;
		pr.result = path;
		(*paths).push_back(pr);
		return;
	}
	if (path.size() == len) {
		cout << "Ngram reached length limit." << endl;
		return;
	}
	if (p < p_thresh) {
		cout << "Probability too low." << endl;
		return;
	}
	if (ngrams.nxgrams.find(s1) == ngrams.nxgrams.end()) {
		cerr << "s1 [" << s1 << "] at depth [" << d <<"] not found." << endl;
		return;
	}

	//iterate through next level
	//sort into list based on prob
	//then call next level with most probable first
	ngram_t ng = ngrams.nxgrams[s1];
	cout << "map size: " << ng.nxgrams.size() << endl;
	map<string, ngram_t>::iterator itr;
	vector<ng_pair_t> level_sort;
	for (itr = ng.nxgrams.begin(); itr != ng.nxgrams.end(); itr++) {
		ng_pair_t npr;
		string s1_a = (*itr).first;

		vector<string> bigram;
		bigram.push_back(s1);
		bigram.push_back(s1_a);
		double prob = get_frequency(&bigram);
		npr.w = s1_a;
		npr.p = prob;

		level_sort.push_back(npr);
	}
	sort(level_sort.begin(), level_sort.end(), ngram_pair_cmp);

	for (unsigned int n = 0; n < level_sort.size(); n++) {
		cout << "Recursive call... [" << n << "]." << endl;
		ng_pair_t nprx = level_sort[n];
		path.push_back(nprx.w);
		generate_shortest_path_search(nprx.w, s2, len, path, &(*paths), p+nprx.p, d+1);
		path.pop_back();
	}
}*/

void Ngram::set_base_pthresh(int len) {
	double base_p = log10(2.0/ngram_counts[1]);
	p_thresh = len*base_p;
}

double Ngram::gram_probability(vector<string> ngram, unsigned int g) {
	double p = 0;
	for (unsigned int i = 0; i < ngram.size()-(g-1); i++) {
		vector<string> gram;
		for (unsigned int j = 0; j < g; j++) {
			gram.push_back(ngram[i+j]);
		}
		p += get_frequency(&gram);
	}
	return p;
}

double Ngram::string_probability(string sentence, unsigned int g) {
	Tokenizer t;
	vector<string> v = t.tokenize(sentence);
	if (g > v.size()) {
		return 0;
	}
	return gram_probability(v, g);
}

void Ngram::set_p_thresh(double pt) {
	p_thresh = pt;
}

double Ngram::get_frequency(vector<string> *ngram) {
	ngram_t ng = ngrams;
	unsigned int i = 0;
	while (i < (*ngram).size()) {
		if (ng.nxgrams.find((*ngram)[i]) == ng.nxgrams.end()) {
			cerr << "key [" << (*ngram)[i] << "] not found... [" << i << "]" << endl;
			return 0;
		} else {
			ng = ng.nxgrams[(*ngram)[i]];
			i++;
		}
	}
	return log10((double)ng.count / ngram_counts[(*ngram).size()-1]);
}

double Ngram::get_frequency(int count) {
	return log10((double)count / ngram_count);
}

double Ngram::get_frequency(int n, int count) {
	return log10((double)count / ngram_counts[n-1]);
}

double Ngram::get_frequency(int n, string w) {
	if (ngrams.nxgrams.find(w) != ngrams.nxgrams.end()) {
		int count = ngrams.nxgrams[w].count;
		return log10((double)count / ngram_counts[n-1]);
	} else {
		return 0;
	}
}

int Ngram::get_ngram_count() {
	return ngram_count;
}

string Ngram::vector_to_string(vector<string> *s) {
	string x = "";
	for (unsigned int i = 0; i < (*s).size()-1; i++) {
		x += (*s)[i] + " ";
	}
	x += (*s)[(*s).size()-1];
	return x;
}
