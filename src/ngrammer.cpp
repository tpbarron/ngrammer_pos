//============================================================================
// Name        : ngrammer.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

/*
 * 43000 - 42999
 * 433000 - 414756
 * 828000 - 757171
 * 4 - 680236
 *
 * needed to add /usr/include/python2.7 to included path
 * and libpython2.7.so to linked files
 *
 */

#include <iostream>
#include <fstream>
#include <stdio.h>
#include "Ngram.h"
#include "Pos.h"
#include "Tokenizer.h"
#include "Template.h"
#include <stdio.h>
#include <string.h>
#include <sstream>

using namespace std;

const unsigned int N = 5;
const string IN_FILE = "all_docs_cleaned.txt";
const string IN_TEST_FILE = "test_doc.txt";
const string IN_TEST_FILE2 = "test_doc_minimal.txt";
const string IN_FILE_TEST_RESULTS = "test_results_sample.txt";
const string IN_FILE_TEST_RESULTS2 = "test_results_sample2.txt";
const string IN_FILE_TEST_RESULTS_HANDPICKED = "test_results_10_handpicked_sentences.txt";
const string IN_FILE_TEST_RESULTS_HANDPICKED_KEYWORDS = "test_results_10_handpicked_strategic_keywords.txt";

const string OUT_FILE_NGRAM = "ngram_out.txt";
const string OUT_FILE_POS = "pos_out.txt";
const string OUT_FILE_TEST_RESULTS_50 = "test_results.txt";
const string OUT_FILE_TEST_RESULTS_100 = "test_results100.txt";
const string OUT_FILE_TEST_RESULTS_FULL = "test_results_full.txt";
const string OUT_FILE_TEST_RESULTS_HANDPICKED = "test_results_10_handpicked_sentences_prob.txt";
const string OUT_FILE_TEST_RESULTS_HANDPICKED_KEYWORDS = "test_results_10_handpicked_strategic_keywords_prob.txt";

const unsigned int CNN_FILE_COUNT = 1445;
const unsigned int ECONOMIST_FILE_COUNT = 1827;
const string CNN_FILE_DIRECTORY = "crawl_files_cnn/";
const string ECONOMIST_FILE_DIRECTORY = "crawl_files_economist/";

struct ngram_pos_result_t {
	vector<Ngram::wpos_t> result;
	double p;
};

struct key_word {
	int len_to_next;
	int ind;
	string word;
	string pos;
};

void init_tmpl();
void generate_sentence_probabilities();
void read_files();
void read_file(const string f);
void add_paragraph(vector< vector<Ngram::wpos_t> > *paragraph);
int get_pos_id(vector<vector<Ngram::wpos_t> > paragraph, unsigned int sen);
void write_file_ngram(const string f);
void write_file_pos(const string f);
void test_generator();
void generate_strategically(ofstream *ofs, vector<Ngram::wpos_t> *sentence);
void generate_naively(ofstream *ofs, vector<Ngram::wpos_t> *sentence);
vector<vector<Ngram::wpos_t> > get_test_sentences();
vector<key_word> choose_key_words(vector<Ngram::wpos_t> sentence);
static bool key_word_cmp(key_word a, key_word b);
string generate_spath(vector<key_word> key_words);
string generate_spath_pos(vector<key_word> key_words);
void generator_loop();
void generate_text();
vector<Ngram::wpos_t> generate_from_ngram_pos(Ngram::wpos_t seed);
/*vector<Ngram::wpos_t> generate_shortest_path_pos_list(Ngram::wpos_t s1, Ngram::wpos_t s2, int l);
void generate_from_ngram_pos_minimum_list(Ngram::wpos_t s1, Ngram::wpos_t s2,
		double p, vector<Ngram::wpos_t> result,
		vector<ngram_pos_result_t> *result_list,
		unsigned int depth, unsigned int len);
static bool cmp_ngram_pos_result(ngram_pos_result_t a, ngram_pos_result_t b);*/
ngram_pos_result_t generate_shortest_path_pos(Ngram::wpos_t s1, Ngram::wpos_t s2, int l);
void generate_from_ngram_pos_minimum(Ngram::wpos_t s1, Ngram::wpos_t s2,
		double p, ngram_pos_result_t *result, vector<Ngram::wpos_t> cur_result,
		double *max_p, unsigned int depth, unsigned int len);
void delete_files();
string num_to_str(int n);
string vector_to_string(vector<Ngram::wpos_t> *s);




Tokenizer tokenizer;
Ngram ngram(N);
Pos pgram(N);
Template tmpl;

int main() {

	//delete_files();
	//init_tmpl();
	read_files();
	generate_sentence_probabilities();
	//generator_loop();
	//test_generator();
	//write_file_ngram(OUT_FILE_NGRAM);
	//write_file_pos(OUT_FILE_POS);

	cout << "Finalmente he terminado!\n";

	return 0;
}

void generate_sentence_probabilities() {
	ofstream ofs(OUT_FILE_TEST_RESULTS_HANDPICKED_KEYWORDS.c_str());
	ifstream ifs(IN_FILE_TEST_RESULTS_HANDPICKED_KEYWORDS.c_str());
	if (ofs.is_open() && ifs.is_open()) {
		string s;
		while (getline(ifs, s)) {
			if (s == "\n") {
				ofs << endl;
			} else {
				double p = ngram.string_probability(s, 2);
				ofs << s << " " << p << endl;
			}
		}
		ifs.close();
		ofs.close();
	} else {
		cout << "Can't open files.." << endl;
	}
}

void init_tmpl() {
	cout << "Adding template types..." << endl;
	tmpl.add_type("<company_name>", "companies.txt");
	tmpl.add_type("<stock_symbol>", "stock_symbols.txt");
	//tmpl.add_type("<number>", "number.txt", true);
	//tmpl.add_type("<website>", "url.txt", true);
	//TODO: need regex
	tmpl.add_type("<stock_price>", "stock_price.txt");
}

void read_files() {
	cout << "Reading data files..." << endl;
	//string f = CNN_FILE_DIRECTORY + "cnn0_pos.txt";
	//read_file(f);
	for (unsigned int c = 0; c < CNN_FILE_COUNT; c++) {
		string f = CNN_FILE_DIRECTORY + "cnn"+num_to_str(c)+"_pos.txt";
		cout << "Reading " << f << " ..." << endl;
		read_file(f);
	}
	/*for (unsigned int e = 0; e < ECONOMIST_FILE_COUNT; e++) {
		string f = ECONOMIST_FILE_DIRECTORY + "economist"+num_to_str(e)+".txt";
		cout << "Reading " << f << " ..." << endl;
		read_file(f);
	}*/
}

/*
 * The layout of the text file is very important
 * <p>
 * <s> sentence 1 </s>
 * <s> sentence 2 </s>
 * ....
 * </p>
 * \n
 * <p> [next paragraph] </p>
 * \n
 * <p> [next paragraph] </p>
 * ....
 */
void read_file(const string f) {
	ifstream ifs(f.c_str());
	if (ifs.is_open()) {
		string s;
		vector<Ngram::wpos_t> pos_vec;
		vector< vector<Ngram::wpos_t> > paragraph;
		while (getline(ifs, s)) {
			if (!tokenizer.blank_line(s)) {
				//add pairs of Ngram::pos_t to pos_vec
				//split sentence on whitespace
				//split each segment on |
				//add first as word, second as pos
				vector<string> word_pos_pairs = tokenizer.tokenize(s);
				for (unsigned int w = 0; w < word_pos_pairs.size(); w++) {
					string wp_pair = word_pos_pairs[w];
					int ind;
					if ((ind = wp_pair.find("|")) != 0) {
						string word = wp_pair.substr(0, ind);
						string pos = wp_pair.substr(ind+1);
						Ngram::wpos_t wp;
						wp.w = word;
						wp.pos = pos;
						pos_vec.push_back(wp);
					} else {
						cout << "char | not found." << endl;
					}
				}
				paragraph.push_back(pos_vec);
				pos_vec.clear();
			} else {
				//blank line
				if (!paragraph.empty()) {
					add_paragraph(&paragraph);
					paragraph.clear();
				}
			}
		}
		ifs.close();
	} else {
		cout << "Could not open file:  " << f << endl;
	}
}

void add_paragraph(vector< vector<Ngram::wpos_t> > *paragraph) {
	if (!(*paragraph).empty()) {
		//add the data //ngramming by N //POS by N and full sentences
		//cout << "StartParagraph" << endl;
		for (unsigned int sen = 1; sen < (*paragraph).size()-1; sen++) {
			//int pos_id = get_pos_id(*paragraph, sen);
			vector <Ngram::wpos_t> sentence = (*paragraph)[sen];
			pgram.add_pos(sentence);
			//pclass[pos_id].add_pos(sentence);
			if (sentence.size() < N)
				continue;
			for (unsigned int a = 0; a < sentence.size() - (N - 1); a++) {
				vector<Ngram::wpos_t> ng_vec;
				for (unsigned int b = a; b < a+N; b++) {
					ng_vec.push_back(sentence[b]);
				}
				//cout << "ng_vec[" << a << "]: ";
				//for (unsigned int p = 0; p < ng_vec.size(); p++) {
				//	cout << "[" << ng_vec[p].w << "] ";
				//}
				//cout << endl;
				ngram.add_ngram(ng_vec);
				pgram.add_pos(ng_vec);
				//pclass[pos_id].add_pos(ng_vec);
			}

			/*for (unsigned int word = 0; word < sentence.size(); word++) {
				Ngram::wpos_t wp = sentence[word];
				cout << "[" << wp.w << "(" << wp.pos << ")] ";
			}
			cout << endl;*/
		}
		//cout << "EndParagraph" << endl;
		//cout << endl;
	}
}

int get_pos_id(vector<vector<Ngram::wpos_t> > paragraph, unsigned int sen) {
	if (paragraph.size() == 1) {
		//what should I do
		//I'll just add to middle
		return 1;
	} else if (paragraph.size() == 2) {
		//add first and last
		if (sen == 0) {
			return 0;
		} else { // sen = 1
			return 2;
		}
	} else if (paragraph.size() > 2) {
		//first,
		//middle n-2
		//last
		if (sen == 0) {
			return 0;
		} else if (sen == paragraph.size()-1) {
			return 2;
		} else {
			return 1;
		}
	} else {
		cerr << "error determining pos id." << endl;
		return 1;
	}
}

void write_file_ngram(const string f) {
	ofstream out;
	out.open(f.c_str(), ios::app);
	if (out.is_open()) {
		//TODO: write unigrams, bigrams, trigrams, ngrams from ngram
		ngram.write_ngrams(&out);
		out.close();
	} else {
		cout << "Could not open outfile ngrams." << endl;
	}
}

void write_file_pos(const string f) {
	ofstream out;
	out.open(f.c_str(), ios::app);
	if (out.is_open()) {
		pgram.write_pgrams(&out);
		out.close();
	} else {
		cout << "Could not open outfile pos." << endl;
	}
}

vector<vector<Ngram::wpos_t> > get_test_sentences() {
	vector<vector<Ngram::wpos_t> > sentences;
	//read in ~50 sentences from generator files
	ifstream ifs(IN_FILE_TEST_RESULTS2.c_str());
	if (ifs.is_open()) {
		string s;
		while (getline(ifs, s)) {
			s = tokenizer.strip_pos_tags(s);
			cout << s << endl;
			vector<string> word_pos_pairs = tokenizer.tokenize(s);
			vector<Ngram::wpos_t> sen;
			for (unsigned int w = 0; w < word_pos_pairs.size(); w++) {
				string wp_pair = word_pos_pairs[w];
				int ind;
				if ((ind = wp_pair.find("|")) != 0) {
					string word = wp_pair.substr(0, ind);
					string pos = wp_pair.substr(ind+1);
					Ngram::wpos_t wp;
					wp.w = word;
					wp.pos = pos;
					sen.push_back(wp);
				} else {
					cout << "char | not found." << endl;
				}
			}
			sentences.push_back(sen);
		}
		ifs.close();
	} else {
		cout << "Could not open file " << IN_FILE_TEST_RESULTS << endl;
	}
	return sentences;
}

vector<key_word> choose_key_words(vector<Ngram::wpos_t> sentence) {
	unsigned int key_count = sentence.size() / 2;
	//find in order the least probable half and the length to the next word
	vector<key_word> key_words;
	vector<unsigned int> used_indexes;
	//until i have key_count keys,
	//find the next lowest prob word
	while (key_words.size() < key_count) {
		double min_freq = 0;
		key_word min_key;
		for (unsigned int i = 0; i < sentence.size(); i++) {
			bool checked = false;
			for (unsigned int j = 0; j < used_indexes.size(); j++) {
				if (i == used_indexes[j]) {
					checked = true;
				}
			}
			if (checked) {
				continue;
			}
			double f = ngram.get_frequency(1, sentence[i].w);
			if (f < min_freq || min_freq == 0) {
				min_freq = f;
				min_key.ind = i;
				min_key.len_to_next = 0;
				min_key.pos = sentence[i].pos;
				min_key.word = sentence[i].w;
			}
		}
		key_words.push_back(min_key);
		used_indexes.push_back(min_key.ind);
	}
	//sort by index
	sort(key_words.begin(), key_words.end(), key_word_cmp);
	//set len_to_next val
	for (unsigned int k = 0; k < key_words.size()-1; k++) {
		key_word k1 = key_words[k];
		key_word k2 = key_words[k+1];
		k1.len_to_next = k2.ind - k1.ind;
		key_words[k] = k1;
		key_words[k+1] = k2;
	}
	//for (unsigned int w = 0; w < key_words.size(); w++) {
	//	cout << key_words[w].word << " " << key_words[w].len_to_next << " " << key_words[w].ind << endl;
	//}
	return key_words;
}

bool key_word_cmp(key_word a, key_word b) {
	return a.ind < b.ind;
}

void test_generator() {
	vector<vector<Ngram::wpos_t> > sentences = get_test_sentences();
	ofstream ofs(OUT_FILE_TEST_RESULTS_FULL.c_str());
	if (ofs.is_open()) {
		//skip tags
		for (unsigned int i = 0; i < sentences.size(); i++) {
			//write normal sentence.
			vector<Ngram::wpos_t> sentence = sentences[i];
			string s_plain = "";
			for (unsigned int a = 0; a < sentence.size(); a++) {
				s_plain += sentence[a].w + " ";
			}
			ofs << s_plain << endl;
			generate_strategically(&ofs, &sentence);
			//generate_naively(&ofs, &sentence);
		}
		ofs.close();
	} else {
		cout << "Could not open file " << OUT_FILE_TEST_RESULTS_FULL << endl;
	}
}

void generate_strategically(ofstream *ofs, vector<Ngram::wpos_t> *sentence) {
	vector<key_word> key_words = choose_key_words((*sentence));
	string spath = generate_spath(key_words);
	(*ofs) << spath << endl;
	string spath_pos = generate_spath_pos(key_words);
	(*ofs) << spath_pos << endl;
	(*ofs) << endl;
}

void generate_naively(ofstream *ofs, vector<Ngram::wpos_t> *sentence) {
	// split into every step-th word
	vector<Ngram::wpos_t> key_words;
	unsigned int j = 0, step = 2;
	while (j < (*sentence).size()) {
		cout << (*sentence)[j].w << " ";
		key_words.push_back((*sentence)[j]);
		j+=step;
	}

	// shortest path w/o pos
	string spath = key_words[0].w + " ";
	for (unsigned int n = 0; n < key_words.size()-1; n++) {
		Ngram::generate_result_t path_segment = ngram.generate_shortest_path(key_words[n].w,
				key_words[n+1].w, step+1);
		for (unsigned int p = 1; p < path_segment.result.size(); p++) {
			spath += path_segment.result[p] + " ";
		}
	}
	(*ofs) << spath << endl;

	// shortest path w/ pos
	string spath_pos = key_words[0].w + " ";
	for (unsigned int m = 0; m < key_words.size()-1; m++) {
		Ngram::wpos_t s1;
		Ngram::wpos_t s2;
		s1.w = key_words[m].w;
		s1.pos = key_words[m].pos;
		s2.w = key_words[m+1].w;
		s1.pos = key_words[m+1].pos;

		ngram_pos_result_t path_segment_pos = generate_shortest_path_pos(s1, s2, step+1);
		for (unsigned int p = 1; p < path_segment_pos.result.size(); p++) {
			spath_pos += path_segment_pos.result[p].w + " ";
		}
	}
	(*ofs) << spath_pos << endl;
	(*ofs) << endl;
}

string generate_spath(vector<key_word> key_words) {
	string spath = key_words[0].word + " ";
	for (unsigned int n = 0; n < key_words.size()-1; n++) {
		if (key_words[n].len_to_next == 1) {
			//has been reach from the last call, continue
			continue;
		}
		if (key_words[n].len_to_next == 0) {
			//reached end of key words
			break;
		}

		Ngram::generate_result_t path_segment = ngram.generate_shortest_path(key_words[n].word,
				key_words[n+1].word, key_words[n].len_to_next+1);
		for (unsigned int p = 1; p < path_segment.result.size(); p++) {
			spath += path_segment.result[p] + " ";
		}
	}
	return spath;
}

string generate_spath_pos(vector<key_word> key_words) {
	string spath_pos = key_words[0].word + " ";
	for (unsigned int m = 0; m < key_words.size()-1; m++) {
		if (key_words[m].len_to_next == 1) {
			spath_pos += key_words[m].word + " ";
			continue;
		}
		if (key_words[m].len_to_next == 0) {
			//reached end of key words
			break;
		}

		Ngram::wpos_t s1;
		Ngram::wpos_t s2;
		s1.w = key_words[m].word;
		s1.pos = key_words[m].pos;
		s2.w = key_words[m+1].word;
		s1.pos = key_words[m+1].pos;

		ngram_pos_result_t path_segment_pos = generate_shortest_path_pos(s1, s2,
				key_words[m].len_to_next+1);
		for (unsigned int p = 1; p < path_segment_pos.result.size(); p++) {
			spath_pos += path_segment_pos.result[p].w + " ";
		}
	}
	return spath_pos;
}

void generator_loop() {
	string s;
	while (true) {
		cout << "Generate text (y/n): ";
		cin >> s;
		if (s == "y") {
			generate_text();
		} else {
			return;
		}
	}
}

void generate_text() {
	string r;
	string seed;
	cout << "(N) ngrams\n"
			"(N2) ngram tree\n"
			"(N3) ngram shortest path\n"
			"(POS) generate from pos\n"
			"(NPOS) generate from ngram and pos\n"
			"(NPOS2) generate min from ngram and pos\n"
			"(P) get probability for text\n"
			"Type: ";
	cin >> r;
	if (r == "N" || r == "n") {
		cout << "Generating from ngrams" << endl;
		string seed;
		cout << "Enter seed: ";
		cin >> seed;
		string n_text = ngram.generate_seed(seed, 20);
		cout << n_text << endl;
	} else if (r == "N2" || r == "n2") {
		cout << "Generating from ngrams tree search" << endl;
		string s1, s2;

		cout << "Enter seed1: ";
		cin >> s1;
		cout << "Enter seed2: ";
		cin >> s2;

		string n_text = ngram.generate(s1, s2);
		cout << n_text << endl;
	} else if (r == "N3" || r == "n3") {
		cout << "Generating from nrams bigrams / shortest path tree search" << endl;
		string s1, s2;
		int l;
		cout << "Enter seed1: ";
		cin >> s1;
		cout << "Enter seed2: ";
		cin >> s2;
		cout << "Enter length: ";
		cin >> l;

		Ngram::generate_result_t result = ngram.generate_shortest_path(s1, s2, l);
		for (unsigned int a = 0; a < result.result.size(); a++) {
			cout << result.result[a] << " ";
		}
		cout << endl;
	} else if (r == "POS" || r == "pos") {
		cout << "Generating from pos..." << endl;

	} else if (r == "NPOS" || r == "npos") {
		cout << "Generating from ngrams and pos..." << endl;
		string w, pos;
		cout << "Enter a starting word: ";
		cin >> w;
		cout << "Enter the POS: ";
		cin >> pos;
		Ngram::wpos_t s;
		s.w = w;
		s.pos = pos;
		vector<Ngram::wpos_t> result = generate_from_ngram_pos(s);
	} else if (r == "NPOS2" || r == "npos2") {
		cout << "Generating minimum from ngrams and pos..." << endl;
		Ngram::wpos_t s1, s2;
		string w1, pos1, w2, pos2;
		cout << "Enter starting word1: ";
		cin >> w1;
		cout << "Enter the POS1: ";
		cin >> pos1;
		s1.w = w1;
		s1.pos = pos1;

		cout << "Enter starting word2: ";
		cin >> w2;
		cout << "Enter the POS1: ";
		cin >> pos2;
		s2.w = w2;
		s2.pos = pos2;

		ngram_pos_result_t result = generate_shortest_path_pos(s1, s2, 2);
		for (unsigned int r = 0; r < result.result.size(); r++) {
			cout << result.result[r].w << " ";
		}
		cout << endl;
	} else if (r == "P" || r == "p") {
		string s;
		string text = "";
		int b;
		cout << "Enter -1 when finished." << endl;
		unsigned int i = 0;
		while (true) {
			cout << "Enter word " << i++ << ": ";
			cin >> s;
			if (s == "-1") {
				break;
			}
			text += s + " ";
			cout << "Current text: " << text << endl;
		}
		cout << "Enter gram degree for prob: ";
		cin >> b;
		cout << "Finding prob for text [" << text << "] with degree [" << b << "]" << endl;
		double p = ngram.string_probability(text, b);
		cout << "Prob of [" << text << "]: " << p << endl;
	}
	cout << endl;
}

vector<Ngram::wpos_t> generate_from_ngram_pos(Ngram::wpos_t seed) {
	string last = "";
	vector<Ngram::wpos_t> result;
	result.push_back(seed);
	while (last != "</s>") { //end of sentence
		cout << vector_to_string(&result) << endl;
		vector <Ngram::w_data_t> ngram_next = ngram.get_next_ngrams(result, 50);
		cout << "Ngrams: ";
		for (unsigned int n = 0; n < ngram_next.size(); n++) {
			cout << "(" << ngram_next[n].w << "[" << ngram_next[n].pos << "|" << ngram_next[n].freq << "]), ";
		}
		cout << endl;
		vector <Pos::pos_result_t> pos_next = pgram.get_next_pos(result, 50);
		cout << "Pos: ";
		for (unsigned int p = 0; p < pos_next.size(); p++) {
			cout << "(" << pos_next[p].pos << "[" << pos_next[p].freq << "]), ";
		}
		cout << endl;
		int max_freq = 0;
		string max_word;
		string max_pos;
		for (unsigned int n = 0; n < ngram_next.size(); n++) {
			for (unsigned int p = 0; p < pos_next.size(); p++) {
				Ngram::w_data_t ng_res = ngram_next[n];
				Pos::pos_result_t pos_res = pos_next[p];
				if (ng_res.pos == pos_res.pos && (max_freq == 0 || (ng_res.freq + pos_res.freq) > max_freq)) {
					max_freq = ng_res.freq + pos_res.freq;
					max_word = ng_res.w;
					cout << "Max word: " << max_word << endl;
					max_pos = pos_res.pos;
				}
			}
		}
		Ngram::wpos_t next;
		next.pos = max_pos;
		next.w = max_word;
		result.push_back(next);
		last = max_word;
		//cout << last << " \n";
	}
	cout << vector_to_string(&result) << endl;
	return result;
}


ngram_pos_result_t generate_shortest_path_pos(Ngram::wpos_t s1, Ngram::wpos_t s2, int l) {
	double p = 0, max_p = 0;
	unsigned int d = 0;
	ngram_pos_result_t result;
	vector<Ngram::wpos_t> cur_result;
	cur_result.push_back(s1);
	generate_from_ngram_pos_minimum(s1, s2, p, &result, cur_result, &max_p, d, l);
	return result;
}

void generate_from_ngram_pos_minimum(Ngram::wpos_t s1, Ngram::wpos_t s2,
		double p, ngram_pos_result_t *result, vector<Ngram::wpos_t> cur_result,
		double *max_p, unsigned int depth, unsigned int len) {
	if (s1.w == s2.w && cur_result.size() == len) { //end of sentence
		cout << "Reached end of sentence! I hope it's good!" << endl;
		if (p > (*max_p) || (*max_p) == 0) {
			cout << "New best path! " << vector_to_string(&cur_result) << " [" << max_p << "] [" << p << "]" << endl;
			//if there is a new best path
			(*max_p) = p;
			(*result).p = p;
			(*result).result = cur_result;
		}
		return;
	}
	if (depth == len) {
		cout << "Reached max length." << endl;
		return;
	}
	//if the current prob < getting 2 counts for each word, stop
	if (p < (depth+1) * (ngram.get_frequency(1) + pgram.get_frequency(1))) {
		cout << "Probability too low!" << endl;
		return;
	}

	cout << "Getting next ngrams..." << endl;
	vector <Ngram::w_data_t> ngram_next = ngram.get_next_ngrams(cur_result, -1);
	//for (unsigned int n = 0; n < ngram_next.size(); n++) {
	//	cout << ngram_next[n].w << " ";
	//}
	//cout << endl;
	cout << "Getting next pos..." << endl;
	vector <Pos::pos_result_t> pos_next = pgram.get_next_pos(cur_result, -1);
	//for (unsigned int pg = 0; pg < pos_next.size(); pg++) {
	//	cout << pos_next[pg].pos << " ";
	//}
	//cout << endl;

	vector <Ngram::w_data_t> call_vec;
	cout << "Processing ngrams & pos..." << endl;
	for (unsigned int n = 0; n < ngram_next.size(); n++) {
		for (unsigned int p = 0; p < pos_next.size(); p++) {
			Ngram::w_data_t ng_res = ngram_next[n];
			Pos::pos_result_t pos_res = pos_next[p];
			//cout << "Ngram_pos: " << ng_res.pos << ", Pos_pos: " << pos_res.pos << endl;
			if (ng_res.pos == pos_res.pos) {
				Ngram::w_data_t word_to_test;
				word_to_test.freq = ng_res.freq + pos_res.freq;
				word_to_test.pos = pos_res.pos;
				word_to_test.w = ng_res.w;
				call_vec.push_back(word_to_test);
			}
		}
	}

	cout << "Sorting next call vec..." << endl;
	//sort call vec
	sort(call_vec.begin(), call_vec.end(), ngram.ngram_result_cmp);
	cout << "Calling next..." << endl;
	for (unsigned int c = 0; c < call_vec.size(); c++) {
		Ngram::w_data_t next_call = call_vec[c];
		// if the probability is already to far negative stop.
		if (!p+next_call.freq < (*max_p)) {
			Ngram::wpos_t next_seed;
			next_seed.w = next_call.w;
			next_seed.pos = next_call.pos;
			cur_result.push_back(next_seed);
			generate_from_ngram_pos_minimum(next_seed, s2, p+next_call.freq, &(*result),
					cur_result, &(*max_p), depth+1, len);
			cur_result.pop_back();
		} else {
			cout << "Prob to low.. stopping call [ngram|pos]." << endl;
		}
	}
}


/*vector<Ngram::wpos_t> generate_shortest_path_pos_list(Ngram::wpos_t s1, Ngram::wpos_t s2, int l) {
	double p = 0;
	unsigned int d = 0;
	vector<Ngram::wpos_t> result;
	vector<ngram_pos_result_t> result_list;
	result.push_back(s1);
	generate_from_ngram_pos_minimum_list(s1, s2, p, result, &result_list, d, l);
	sort(result_list.begin(), result_list.end(), cmp_ngram_pos_result);
	return result_list[0].result;
}

bool cmp_ngram_pos_result(ngram_pos_result_t a, ngram_pos_result_t b) {
	return a.p < b.p;
}

void generate_from_ngram_pos_minimum_list(Ngram::wpos_t s1, Ngram::wpos_t s2,
		double p, vector<Ngram::wpos_t> result,
		vector<ngram_pos_result_t> *result_list,
		unsigned int depth, unsigned int len) {

	//print current vector
	for (unsigned int r = 0; r < result.size(); r++) {
		cout << result[r].w << " ";
	}
	cout << endl;
	if (s1.w == s2.w) { //end of sentence
		cout << "Reached end of sentence! I hope it's good!" << endl;
		ngram_pos_result_t np_res;
		np_res.result = result;
		np_res.p = p;
		(*result_list).push_back(np_res);
		return;
	}
	if (depth == len) {
		cout << "Reached max length." << endl;
		return;
	}
	//if the current prob < getting 2 counts for each word, stop
	if (p < (depth+1) * (ngram.get_frequency(1) + pgram.get_frequency(1))) {
		cout << "Probability too low!" << endl;
		return;
	}

	vector <Ngram::w_data_t> ngram_next = ngram.get_next_ngrams(result, -1);
	vector <Pos::pos_result_t> pos_next = pgram.get_next_pos(result, -1);

	vector <Ngram::w_data_t> call_vec;
	for (unsigned int n = 0; n < ngram_next.size(); n++) {
		for (unsigned int p = 0; p < pos_next.size(); p++) {
			Ngram::w_data_t ng_res = ngram_next[n];
			Pos::pos_result_t pos_res = pos_next[p];
			//cout << "Ngram_pos: " << ng_res.pos << ", Pos_pos: " << pos_res.pos << endl;
			if (ng_res.pos == pos_res.pos) {
				Ngram::w_data_t word_to_test;
				word_to_test.freq = ng_res.freq + pos_res.freq;
				word_to_test.pos = pos_res.pos;
				word_to_test.w = ng_res.w;
				call_vec.push_back(word_to_test);
			}
		}
	}

	//sort call vec
	sort(call_vec.begin(), call_vec.end(), ngram.ngram_result_cmp);
	for (unsigned int c = 0; c < call_vec.size(); c++) {
		Ngram::w_data_t next_call = call_vec[c];
		Ngram::wpos_t next_seed;
		next_seed.w = next_call.w;
		next_seed.pos = next_call.pos;
		result.push_back(next_seed);
		generate_from_ngram_pos_minimum_list(next_seed, s2, p+next_call.freq,
				result, &(*result_list), depth+1, len);
		result.pop_back();
	}
}*/

void delete_files() {
	if (remove(OUT_FILE_NGRAM.c_str()) == 0) {
		cout << OUT_FILE_NGRAM << " deleted\n";
	}
	if (remove(OUT_FILE_POS.c_str()) == 0) {
		cout << OUT_FILE_POS << " deleted\n";
	}
	if (remove(OUT_FILE_TEST_RESULTS_50.c_str()) == 0) {
		cout << OUT_FILE_TEST_RESULTS_50 << " deleted\n";
	}
}

string num_to_str(int n) {
	string r;//string which will contain the result
	stringstream convert; // stringstream used for the conversion
	convert << n;//add the value of Number to the characters in the stream
	r = convert.str();
	return r;
}

string vector_to_string(vector<Ngram::wpos_t> *s) {
	string x = "";
	for (unsigned int i = 0; i < (*s).size(); i++) {
		x += "(" + (*s)[i].w + "[" + (*s)[i].pos + "]) ";
	}
	return x;
}
