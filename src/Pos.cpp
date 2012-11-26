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

#include "Pos.h"

using namespace std;

Pos::Pos(int deg) {
	pgram_degree = deg;
	pgram_count = 0;
	for (int i = 0; i < deg; i++) {
		pgram_counts.push_back(0);
	}
	p_thresh = deg*(-5.0);
}

Pos::~Pos() {}

void Pos::add_pos(vector<Ngram::wpos_t> pgram) {
	//cout << "Adding pgram... " << endl;
	add_pgram_helper(&pgrams, pgram, 0);
	//cout << "pos: " << pgram_count << endl;
}
void Pos::add_pgram_helper(pgram_t *pgs, vector<Ngram::wpos_t> pgram, unsigned int n) {
	if (n == pgram_counts.size()) {
		pgram_counts.push_back(0);
	}
	pgram_counts[n]++;
	if (n == pgram.size()) {
		//cout << "POS finished." << endl;
		if ((*pgs).count == 1) { //a new ngram
			pgram_count++;
		}
		(*pgs).count++;
		return;
	}
	Ngram::wpos_t pt = pgram[n];
	string pos = pt.pos;
	if ((*pgs).pxgrams.find(pos) != (*pgs).pxgrams.end()) {
		//if found, increment step, set word, set pos
		//cout << "POS found." << endl;
		(*pgs).pxgrams[pos].count++;
		(*pgs).pxgrams[pos].pos = pos;
		add_pgram_helper(&((*pgs).pxgrams[pos]), pgram, n+1);
	} else {
		//does not exist
		//cout << "Creating POS." << endl;
		pgram_t pg;
		pg.count = 1;
		pg.pos = pos;
		(*pgs).pxgrams[pos] = pg;
		add_pgram_helper(&((*pgs).pxgrams[pos]), pgram, n+1);
	}
}

void Pos::write_pgrams(ofstream *f) {
	cout << "Writing pgrams... " << pgram_count << endl;
	if ((*f).is_open()) {
		(*f) << ":ngrams" << endl;
		vector<string> s;
		write_pgrams_helper(&(*f), &pgrams, s, 0);
		(*f) << "\n";
	} else {
		cout << "Unable to open file." << endl;
	}
}

void Pos::write_pgrams_helper(ofstream *f, pgram_t *pgram, vector<string> s, unsigned int n) {
	if ((*pgram).pxgrams.empty()) {
		//no more levels
		cout << (*pgram).pxgrams.empty() << endl;
		cout << vector_to_string(&s) << endl;
		(*f) << vector_to_string(&s) << " " << get_frequency(n, (*pgram).count) << "\n";
		return;
	}
	map<string, pgram_t>::iterator itr;
	for (itr = (*pgram).pxgrams.begin(); itr != (*pgram).pxgrams.end(); itr++) {
		string sx = (*itr).first;
		pgram_t posx = (*itr).second;
		s.push_back(sx);
		write_pgrams_helper(&(*f), &posx, s, n+1);
		s.pop_back();
	}
}

vector<Pos::pos_result_t> Pos::get_next_pos(vector<Ngram::wpos_t> pgram, int size) {
	/* TODO: I have no way to know whether the
	 * entire list of words I have is in my pos
	 * data structure
	 */
	vector<pos_result_t> result;

	//cout << "Finding longest path [pos]." << endl;
	pgram_t pg = pgrams.pxgrams[pgram[pgram.size()-1].pos];
	//pgram_t pg = find_largest_path(&pgram);
	//cout << "Longest path found [pos]." << endl;
	//now ng is at the deepest level of ngram_deg
	map<string, pgram_t>::iterator itr;
	for (itr = pg.pxgrams.begin(); itr != pg.pxgrams.end(); itr++) {
		//loop through all next ngram_t
		pos_result_t pos_result;
		pos_result.pos = (*itr).second.pos;
		pos_result.freq = get_frequency((*itr).second.count);
		//TODO: what frequency do I want here?
		result.push_back(pos_result);
	}

	//sort result by freq
	sort(result.begin(), result.end(), pos_result_cmp);
	return result;
}

Pos::pgram_t Pos::find_largest_path(vector<Ngram::wpos_t> *pgram) {
	unsigned int d;
	if ((*pgram).size() < pgram_degree)
		d = (*pgram).size();
	else
		d = pgram_degree-1;

	pgram_t pg;
	pg.count = -1;

	cout << "Checking pgrams from max depth: " << d << endl;
	while (d >= 0) {
		pgram_t pg_test = pgrams;
		bool found = true;
		for (unsigned int a = d; a > 0; a--) {
			//cout << "a = " << a << endl;
			string pos = (*pgram)[(*pgram).size()-a].pos;
			if (pg_test.pxgrams.find(pos) == pg_test.pxgrams.end()) {
				//cout << "Pos not found." << endl;
				found = false;
				break;
			}
			pg_test = pg_test.pxgrams[pos];
		}
		if (found) {
			cout << "Pos result found! " << endl;
			return pg_test;
		}
		d--;
	}
	return pg;
}

bool Pos::pos_result_cmp(const pos_result_t &a, const pos_result_t &b) {
	return a.freq > b.freq;
}
/*
string Ngram::generate_seed(vector<string> seed, int l) {
	vector<string> s = seed;
	string text = "";
	//add initial seed
	for (unsigned int n = 0; n < seed.size(); n++) {
		text += s[n] + " ";
	}

	//add next l words
	for (int i = 0; i < l; i++) {
		ngram_t ng = ngrams;
		for (unsigned int j = 0; j < seed.size(); j++) {
			if (ng.nxgrams.find(s[j]) == ng.nxgrams.end()) {
				cerr << "Seed " << j << " [" << s[j] << "] not found." << endl;
				return text;
			}
			//go in a level
			ng = ng.nxgrams[s[j]];
		}

		//if get here have ng as a ngram_t struct with a map to a
		//final level of structs which point to a struct containing the int value

		//loop through all in map
		string sn;
		int count = -1;
		map<string, ngram_t>::iterator itr;
		for (itr = ng.nxgrams.begin(); itr != ng.nxgrams.end(); itr++) {
			if ((*itr).second.count > count) {
				sn = (*itr).first;
				count = (*itr).second.count;
			}
		}

		//shift seeds
		for (unsigned int a = 0; a < seed.size()-1; a++) {
			s[a] = s[a+1];
		}
		s[seed.size()-1] = sn;

		text += sn + " ";
	}
	return text;
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
		vector<pair_t> return_vec;
		result_vec.push_back(s1);
		generate_search(s1, s2, result_vec, &return_vec, 0, ng, 0);
		cout << "Search finished." << endl;
		if (return_vec.size() == 0) {
			return "No results found.";
		}
		double max_p = 0;
		vector<string> max_vec;
		for (unsigned int i = 0; i < return_vec.size(); i++) {
			pair_t pr = return_vec[i];
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
		vector<string> result_vec, vector<pair_t> *return_vec,
		double p, ngram_t ng, unsigned int depth) {

	//cout << "Search: p = " << p << " depth = " << depth << endl;
	if (depth == ngram_degree) {
		return;
	}

	map<string, ngram_t>::iterator itr;
	for (itr = ng.nxgrams.begin(); itr != ng.nxgrams.end(); itr++) {
		string s = (*itr).first;
		ngram_t ngx = (*itr).second;
		double px = get_frequency(0, ngx.count);

		//copy array so not pass by ref
		vector<string> res_vec;
		for (unsigned int n = 0; n < result_vec.size(); n++) {
			res_vec.push_back(result_vec[n]);
		}
		res_vec.push_back(s);

		if (s == s2) {
			pair_t pair;
			pair.result = res_vec;
			pair.p = p+px;
			(*return_vec).push_back(pair);
		} else {
			generate_search(s1, s2, res_vec, &(*return_vec), p+px, ngx, depth+1);
		}
	}
}

string Ngram::generate_shortest_path(string s1, string s2, unsigned int len) {
	//reset p_thresh based on length and number of bigrams
	set_base_pthresh(len);
	if (ngrams.nxgrams.find(s1) == ngrams.nxgrams.end()) {
		cout << "s1 [" << s1 << "] not found." << endl;
		return "";
	}

	vector<pair_t> paths;
	vector<string> path;
	path.push_back(s1);

	generate_shortest_path_search(s1, s2, len, path, &paths, 0, 0);

	double max_p = 0;
	string res = "";
	for (unsigned int i = 0; i < paths.size(); i++) {
		pair_t pr = paths[i];
		if (pr.p > max_p || max_p == 0) {
			max_p = pr.p;
			res = vector_to_string(&(pr.result));
		}
		cout << "[" << pr.p << "] " << vector_to_string(&(pr.result)) << endl;
	}
	return res;
}

void Ngram::generate_shortest_path_search(string s1, string s2, unsigned int len,
		vector<string> path, vector<pair_t> *paths, double p, unsigned int d) {
	cout << "Generating shortest path..." << "[" << p << "] [" << vector_to_string(&path) << "]" << endl;
	if (s1 == s2 && path.size() == len) {
		cout << "Match found!" << endl;
		pair_t pr;
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
	map<string, ngram_t>::iterator itr;
	vector<ng_pair_t> level_sort;
	for (itr = ng.nxgrams.begin(); itr != ng.nxgrams.end(); itr++) {
		ng_pair_t npr;
		string s1_a = (*itr).first;

		//cout << "Adding s1_a[" << s1_a << "] to ng pair type." << endl;

		vector<string> bigram;
		bigram.push_back(s1);
		bigram.push_back(s1_a);
		double prob = get_frequency(&bigram);

		npr.w = s1_a;
		npr.p = prob;

		level_sort.push_back(npr);
		sort(level_sort.begin(), level_sort.end(), ngram_pair_cmp);

		//add_to_sorted_list(&level_sort, &npr);
	}

	for (unsigned int n = 0; n < level_sort.size(); n++) {
		cout << "Recursive call... [" << n << "]." << endl;
		ng_pair_t nprx = level_sort[n];
		path.push_back(nprx.w);
		generate_shortest_path_search(nprx.w, s2, len, path, &(*paths), p+nprx.p, d+1);
		path.pop_back();
	}
}

bool Ngram::ngram_pair_cmp(const ng_pair_t &a, const ng_pair_t &b) {
	return a.p > b.p;
}

void Ngram::set_base_pthresh(int len) {
	double base_p = log10(1.0/ngram_counts[1]);
	p_thresh = len*base_p;
}

//make this binary insert
void Ngram::add_to_sorted_list(vector<ng_pair_t> *level_sort, ng_pair_t *npr) {
	//add in order
	//cout << "Adding npr[" << (*npr).w << "] [" << (*npr).p << "] to sorted list." << endl;
	if ((*level_sort).size() == 0) {
		//cout << "First element in list." << endl;
		(*level_sort).push_back(*npr);
	} else {
		for (unsigned int i = 0; i < (*level_sort).size(); i++) {
			if ((*npr).p > (*level_sort)[i].p) {
				//cout << "Inserting in middle of list." << endl;
				(*level_sort).insert((*level_sort).begin()+i, *npr);
				return;
			} else {
				if (i == (*level_sort).size()-1) {
					//cout << "Adding to end of list." << endl;
					(*level_sort).push_back(*npr);
					return;
				}
			}
		}
	}
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
	return gram_probability(v, g);
}

void Ngram::set_p_thresh(double pt) {
	p_thresh = pt;
}
*/

double Pos::get_frequency(vector<string> *pgram) {
	pgram_t pg = pgrams;
	unsigned int i = 0;
	while (i < (*pgram).size()) {
		pg = pg.pxgrams[(*pgram)[i]];
		i++;
	}
	return log10((double)pg.count / pgram_counts[(*pgram).size()-1]);
}

double Pos::get_frequency(int count) {
	return log10((double)count / pgram_count);
}

double Pos::get_frequency(int n, int count) {
	return log10((double)count / pgram_counts[n-1]);
}

int Pos::get_pgram_count() {
	return pgram_count;
}

string Pos::vector_to_string(vector<string> *s) {
	string x = "";
	if (!s) {
		cout << "null vector" << endl;
		return "";
	}
	for (unsigned int i = 0; i < (*s).size()-1; i++) {
		x += (*s)[i] + " ";
	}
	x += (*s)[(*s).size()-1];
	return x;
}
