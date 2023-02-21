//
//  mstrie.cpp
//  mstrie
//
//  Created by Mikita Akulich on 25/03/2018.
//  Copyright © 2018 Mikita Akulich. All rights reserved.
//

#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>
#include <ctime>

#include "mstrie.hpp"

/* ------------------------------------------------------------------
 * Converter
 * ------------------------------------------------------------------
 */
std::vector<uint> MstrieStructure::str_to_num(const std::string &token){
	std::vector<uint> v (_settings->alphabet, 0);
	if (!token.compare("*")) {
		return v;
	}
	std::string el;
	char delimiter = ',';
	std::istringstream tokenStream(token);
	while (std::getline(tokenStream, el, delimiter)) {
		auto el_int = std::stoi(el);
		if (el_int < 0) {
			throw MstrieException("Token cannot have negative values.");
		}
		if (el_int > _settings->alphabet - 1) {
			throw MstrieException("Token cannot have values greater than alphabet size.");
		}
		v[el_int]++;
	}
	return v;
}

// -----------------------------------------------------------------------------------------------

std::string MstrieStructure::num_to_str(const std::vector<uint> &v) {
	std::string s;
	for (size_t ch = 0; ch < v.size(); ch++) {
		for (int i = 0; i<v[ch]; i++) {
			s += std::to_string(ch) + ",";
		}
	}
	return s.substr(0, s.size() -1);
}

// ===============================================================================================
// ===============================================================================================

/* ------------------------------------------------------------------
 * Constructors/Destructors
 * ------------------------------------------------------------------
 */
MstrieSettings::MstrieSettings(uint alphabet, uint max_multiplicity, const std::string &index_path)
: alphabet(alphabet),
max_multiplicity(max_multiplicity),
index_path(index_path) {};

// -----------------------------------------------------------------------------------------------

MstrieStructure::MstrieStructure(const MstrieSettings &settings)
: _root(std::make_shared<MstrieNode>(settings.max_multiplicity)),
_dummy(std::make_shared<MstrieNode>(0)),
_settings(std::make_unique<MstrieSettings>(settings)){
	statistics = std::make_unique<MstrieStats>();
}

// -----------------------------------------------------------------------------------------------

MstrieNode::MstrieNode(const uint max_multiplicity) {
	this->mult_switch = std::make_shared<std::vector<std::shared_ptr<MstrieNode>>>(max_multiplicity+1, nullptr);
}

// -----------------------------------------------------------------------------------------------

MstrieStats::MstrieStats(const std::string &units){
	// the root node and dummy are always present when mstrie structure is present
	total_number_of_nodes = 2;
	total_number_of_multisets = 0;
	last_query_traversed_nodes = 0;
	last_query_time_taken = 0;
	last_query_name = "";
	time_units = units;
}

// ===============================================================================================
// ===============================================================================================

void MstrieStructure::mstrie_insert(const std::vector<uint> &sv_input)
{
	std::shared_ptr<MstrieNode> root_p = std::move(_root);
	int i = 0;
	try {
		/* Go down to leaf level */
		while (i<_settings->alphabet-1) {
			/* Node already exists */
			if (root_p->mult_switch->at(sv_input[i]) != nullptr) {
				root_p = root_p->mult_switch->at(sv_input[i]);
			}
			/* Insert a new node */
			else {
				root_p->mult_switch->at(sv_input[i]) = std::make_shared<MstrieNode>(_settings->max_multiplicity);
				root_p = root_p->mult_switch->at(sv_input[i]);
				statistics->total_number_of_nodes++;
			}
			++i;
		}
		/* Set pointer in leaf node to acceptor */
		if (root_p->mult_switch->at(sv_input[i]) != _dummy) {
			statistics->total_number_of_multisets++;
			root_p->mult_switch->at(sv_input[i]) = _dummy;
		}
	} catch (std::exception &e) {
		throw std::runtime_error("Insertion failed: " + std::string(e.what()));
	}
	return;
}

// -----------------------------------------------------------------------------------------------

void MstrieStructure::mstrie_delete(const std::vector<uint> &sv_input) {
	std::shared_ptr<MstrieNode> root_p = std::move(_root);
	std::shared_ptr<MstrieNode> parent = std::move(_root);
	int pos = 0;
	int children;
	try {
		for (int i=0; i<_settings->alphabet; i++) {
			if (root_p->mult_switch->at(sv_input[i]) != nullptr) {
				children = -1;
				for (int j=0; j<=_settings->max_multiplicity; j++) {
					if (root_p->mult_switch->at(j) != nullptr) {
						children++;
					}
				}
				// children is always >= 0, since we can go
				// down throug i-th multiplicity
				if (children != 0) {
					parent = root_p;
					pos = i;
				}
			}
			else {
				throw MstrieException("nothing to delete.");
			}
			root_p = root_p->mult_switch->at(sv_input[i]);
		}
		parent->mult_switch->at(sv_input[pos]) = nullptr;
		
		statistics->total_number_of_nodes -= (_settings->alphabet - pos - 1);
	} catch (std::exception &e) {
		throw std::runtime_error("Deletion failed: " + std::string(e.what()));
	}
	statistics->total_number_of_multisets--;
}

// -----------------------------------------------------------------------------------------------

bool MstrieStructure::mstrie_search(const std::vector<uint> &sv_input) {
	std::shared_ptr<MstrieNode> root_p = std::move(_root);
	int i = 0;
	try {
		while (i<_settings->alphabet) {
			if (root_p->mult_switch->at(sv_input[i]) != nullptr) {
				root_p = root_p->mult_switch->at(sv_input[i]);
				++i;
				statistics->last_query_traversed_nodes++;
			}
			else {
				return false;
			}
		}
	} catch (std::exception &e) {
		throw std::runtime_error("Search failed: " + std::string(e.what()));
	}
	return true;
}

// -----------------------------------------------------------------------------------------------

bool MstrieStructure::mstrie_subseteq(const std::vector<uint> &sv_input, uint limit)
{
	try {
		if (limit > _settings->max_multiplicity) limit = _settings->max_multiplicity;
		std::shared_ptr<MstrieNode> root_p = std::move(_root);
		return mstrie_subseteq_rec(root_p, sv_input, limit, 0);
	} catch (std::exception &e) {
		throw std::runtime_error("Sub multiset existence failed: " + std::string(e.what()));
	}
}
bool MstrieStructure::mstrie_subseteq_rec(const std::shared_ptr<MstrieNode> root, const std::vector<uint> &sv_input, uint limit, uint vcnt){
	statistics->last_query_traversed_nodes++;
	/* Check if we came to acceptor node */
	if (root == _dummy) {
		return true;
	}
	
	/* Find the closest subset */
	for (uint i = sv_input[vcnt], lt = limit; (i >= 0 && lt >=0) ; i--, lt--) {
		if (root->mult_switch->at(i) != nullptr) {
			/* Proceed search on next level */
			if (mstrie_subseteq_rec(root->mult_switch->at(i), sv_input, limit, vcnt+1)) {
				return true;
			}
		}
	}
	return false;
}

// -----------------------------------------------------------------------------------------------

std::queue<std::string> MstrieStructure::mstrie_get_subseteq(const std::vector<uint> &sv_input, uint limit){
	std::vector<uint> sv_out (_settings->alphabet);
	std::queue<std::string> q;
	try {
		if (limit > _settings->max_multiplicity) limit = _settings->max_multiplicity;
		std::shared_ptr<MstrieNode> root_p = std::move(_root);
		mstrie_get_subseteq_rec(root_p, sv_input, sv_out, q, limit, 0);
	} catch (std::exception &e) {
		throw std::runtime_error("Get sub multisets failed: " + std::string(e.what()));
	}
	return q;
}
void MstrieStructure::mstrie_get_subseteq_rec(const std::shared_ptr<MstrieNode> root, const std::vector<uint> &sv_input, std::vector<uint> &sv_output, std::queue<std::string> &str_que, uint limit, uint vcnt)
{
	statistics->last_query_traversed_nodes++;
	/* Check if we came to acceptor node */
	if (root == _dummy) {
		/* Add vector to queue */
		str_que.push(num_to_str(sv_output));
		return;
	}
	
	/* Find the closest subset */
	for (int i = sv_input[vcnt], lt = limit; (i >= 0 && lt >= 0) ; i--, lt--) {
		if (root->mult_switch->at(i) != nullptr) {
			/* Store multiplicity to vector */
			sv_output[vcnt] = i;
			/* Proceed search on next level */
			mstrie_get_subseteq_rec(root->mult_switch->at(i), sv_input, sv_output, str_que, limit, vcnt+1);
		}
	}
	return;
}

// -----------------------------------------------------------------------------------------------

bool MstrieStructure::mstrie_superseteq(const std::vector<uint> &sv_input, uint limit)
{
	try {
		if (limit > _settings->max_multiplicity) limit = _settings->max_multiplicity;
		std::shared_ptr<MstrieNode> root_p = std::move(_root);
		return mstrie_superseteq_rec(root_p, sv_input, limit, 0);
	} catch (std::exception &e) {
		throw std::runtime_error("Super multiset existence failed: " + std::string(e.what()));
	}
}
bool MstrieStructure::mstrie_superseteq_rec(const std::shared_ptr<MstrieNode> root, const std::vector<uint> &sv_input, uint limit, uint vcnt) {
	statistics->last_query_traversed_nodes++;
	/* Check if we came to acceptor node */
	if (root == _dummy) {
		return true;
	}
	
	/* Find the closest subset */
	for (int i = sv_input[vcnt], lt = limit; (i <= _settings->max_multiplicity && lt >= 0) ; i++, lt--) {
		if (root->mult_switch->at(i) != nullptr) {
			/* Proceed search on next level */
			if (mstrie_superseteq_rec(root->mult_switch->at(i), sv_input, limit, vcnt+1)) {
				return true;
			}
		}
	}
	return false;
}

// -----------------------------------------------------------------------------------------------

std::queue<std::string> MstrieStructure::mstrie_get_superseteq(const std::vector<uint> &sv_input, uint limit){
	std::vector<uint> sv_out (_settings->alphabet);
	std::queue<std::string> q;
	try {
		if (limit > _settings->max_multiplicity) limit = _settings->max_multiplicity;
		std::shared_ptr<MstrieNode> root_p = std::move(_root);
		mstrie_get_superseteq_rec(root_p, sv_input, sv_out, q, limit, 0);
	} catch (std::exception &e) {
		throw std::runtime_error("Get super multisets failed: " + std::string(e.what()));
	}
	return q;
}
void MstrieStructure::mstrie_get_superseteq_rec(const std::shared_ptr<MstrieNode> root, const std::vector<uint> &sv_input, std::vector<uint> &sv_output, std::queue<std::string> &str_que, uint limit, uint vcnt)
{
	statistics->last_query_traversed_nodes++;
	/* Check if we came to acceptor node */
	if (root == _dummy) {
		/* Add vector to queue */
		str_que.push(num_to_str(sv_output));
		return;
	}
	
	/* Find the closest subset */
	for (int i = sv_input[vcnt], lt = limit; (i <= _settings->max_multiplicity && lt >= 0) ; i++, lt--) {
		if (root->mult_switch->at(i) != nullptr) {
			/* Store multiplicity to vector */
			sv_output[vcnt] = i;
			/* Proceed search on next level */
			mstrie_get_superseteq_rec(root->mult_switch->at(i), sv_input, sv_output, str_que, limit, vcnt+1);
		}
	}
	return;
}


// ===============================================================================================
// ===============================================================================================


/* ------------------------------------------------------------------
 * Read / Write utilities
 * ------------------------------------------------------------------
 */
void MstrieStructure::load_mstrie(const std::string &content) {
	std::stringstream ss(content);
	std::string token, temp;
	std::getline(ss, token, '\n'); // skip signature
	std::getline(ss, token, '\n'); // read the structural parameters
	
	std::stringstream params(token);
	std::getline(params, temp, ' ');
	uint used_max_multiplicity = std::stoi(temp);
	std::getline(params, temp, '\n');
	uint used_alphabet_size = std::stoi(temp);
	
	if (used_alphabet_size != _settings->alphabet || used_max_multiplicity != _settings->max_multiplicity) {
		throw MstrieException("Mstrie parametrization is not correct.\nThe mstrie you are trying to load is parametrized as follows:\n\talphabet_size="+std::to_string(_settings->alphabet)+"\n\tmax_multiplicity="+std::to_string(_settings->max_multiplicity));
	}
	while (std::getline(ss, token, '\n')) {
		auto v = str_to_num(token);
		mstrie_insert(v);
	}
}

// -----------------------------------------------------------------------------------------------

std::string MstrieStructure::retrieve_mstrie(){
	std::vector<uint> v = std::vector<uint> (_settings->alphabet, 0);
	std::queue<std::string> q = mstrie_get_superseteq(v, _settings->max_multiplicity);
	return prepare_mstrie_dump(q);
}

// -----------------------------------------------------------------------------------------------

std::string MstrieStructure::prepare_mstrie_dump(std::queue<std::string> queue){
	// add timestamp to content
	std::string content = timestamp_string();
	content += '\n';
	content += std::to_string(_settings->max_multiplicity) + ' ' + std::to_string(_settings->alphabet) + '\n';
	while (!queue.empty()) {
		content.append(queue.front());
		content += '\n';
		queue.pop();
	}
	return content;
}

// -----------------------------------------------------------------------------------------------

std::string MstrieStructure::timestamp_string(){
	auto t = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	return std::to_string(t);
}

// ===============================================================================================
// ===============================================================================================

/* ------------------------------------------------------------------
 * Public search queries
 * ------------------------------------------------------------------
 */
bool MstrieStructure::pub_mstrie_search(const std::string &word){
	statistics->reset();
	statistics->last_query_name = "search exact";
	statistics->set_start_time();
	bool result;
	try {
		result = mstrie_search(str_to_num(word));
	} catch (std::exception &e) {
		throw;
	}
	statistics->set_end_time();
	statistics->set_last_query_time();
	return result;
}

// -----------------------------------------------------------------------------------------------

bool MstrieStructure::pub_mstrie_subseteq(const std::string &word) {
	return this->pub_mstrie_subseteq(word, _settings->max_multiplicity);
}
bool MstrieStructure::pub_mstrie_subseteq(const std::string &word, uint limit){
	statistics->reset();
	statistics->last_query_name = "search sub";
	statistics->set_start_time();
	if (limit > _settings->max_multiplicity) limit = _settings->max_multiplicity;
	bool result;
	try {
		result = mstrie_subseteq(str_to_num(word), limit);
	} catch (std::exception &e) {
		throw;
	}
	statistics->set_end_time();
	statistics->set_last_query_time();
	return result;
}

// -----------------------------------------------------------------------------------------------

bool MstrieStructure::pub_mstrie_superseteq(const std::string &word) {
	return this->pub_mstrie_superseteq(word, _settings->max_multiplicity);
}
bool MstrieStructure::pub_mstrie_superseteq(const std::string &word, uint limit){
	statistics->reset();
	statistics->last_query_name = "search sup";
	statistics->set_start_time();
	if (limit > _settings->max_multiplicity) limit = _settings->max_multiplicity;
	bool result;
	try {
		result = mstrie_superseteq(str_to_num(word), limit);
	} catch (std::exception &e) {
		throw;
	}
	statistics->set_end_time();
	statistics->set_last_query_time();
	return result;
}

// ===============================================================================================
// ===============================================================================================

/* ------------------------------------------------------------------
 * Public retrieve queries
 * ------------------------------------------------------------------
 */
std::string MstrieStructure::pub_mstrie_get_subseteq(const std::string &word){
	return this->pub_mstrie_get_subseteq(word, _settings->max_multiplicity);
}
std::string MstrieStructure::pub_mstrie_get_subseteq(const std::string &word, uint limit){
	statistics->reset();
	statistics->last_query_name = "retrieve sub_" + std::to_string(limit);
	statistics->set_start_time();
	if (limit > _settings->max_multiplicity) limit = _settings->max_multiplicity;
	std::string output;
	try {
		auto q = mstrie_get_subseteq(str_to_num(word), limit);
		if (q.empty()){
			output = "";
		}
		else {
			while (!q.empty()) {
				output.append(q.front() + "|");
				q.pop();
			}
			output = output.substr(0, output.size() -1);
		}
	} catch (std::exception &e) {
		throw;
	}
	statistics->set_end_time();
	statistics->set_last_query_time();
	return output;
}

// -----------------------------------------------------------------------------------------------

std::string MstrieStructure::pub_mstrie_get_superseteq(const std::string &word){
	return this->pub_mstrie_get_superseteq(word, _settings->max_multiplicity);
}
std::string MstrieStructure::pub_mstrie_get_superseteq(const std::string &word, uint limit){
	statistics->reset();
	statistics->last_query_name = "retrieve sup_" + std::to_string(limit);
	statistics->set_start_time();
	if (limit > _settings->max_multiplicity) limit = _settings->max_multiplicity;
	std::string output;
	try {
		auto q = mstrie_get_superseteq(str_to_num(word), limit);
		if (q.empty()){
			output = "";
		}
		else {
			while (!q.empty()) {
				output.append(q.front() + "|");
				q.pop();
			}
			output = output.substr(0, output.size() -1);
		}
	} catch (std::exception &e) {
		throw;
	}
	statistics->set_end_time();
	statistics->set_last_query_time();
	return output;
}

// ===============================================================================================
// ===============================================================================================

/* ------------------------------------------------------------------
 * Public update queries
 * ------------------------------------------------------------------
 */
void MstrieStructure::pub_mstrie_insert(const std::string &word){
	statistics->reset();
	statistics->last_query_name = "insert";
	statistics->set_start_time();
	try {
		mstrie_insert(str_to_num(word));
	} catch (std::exception &e) {
		throw;
	}
	statistics->set_end_time();
	statistics->set_last_query_time();
}

// -----------------------------------------------------------------------------------------------

void MstrieStructure::pub_mstrie_delete(const std::string &word){
	statistics->reset();
	statistics->last_query_name = "delete";
	statistics->set_start_time();
	try {
		mstrie_delete(str_to_num(word));
	} catch (std::exception &e) {
		throw;
	}
	statistics->set_end_time();
	statistics->set_last_query_time();
}

// ===============================================================================================
// ===============================================================================================

std::string MstrieStructure::print_full_stats(){
	return statistics->generate_last_query_stats() + statistics->generate_total_stats();
}

// -----------------------------------------------------------------------------------------------

std::string MstrieStructure::print_last_query_stats(){
	return statistics->generate_last_query_stats();
}

// -----------------------------------------------------------------------------------------------

std::string MstrieStructure::print_total_stats(){
	return statistics->generate_total_stats();
}

// -----------------------------------------------------------------------------------------------

std::string MstrieStructure::print_benchmark_stats() {
	return statistics->generate_benchmark_stats();
}

// -----------------------------------------------------------------------------------------------

void MstrieStats::reset(){
	last_query_time_taken = 0;
	last_query_traversed_nodes = 0;
	last_query_name = "";
}

// -----------------------------------------------------------------------------------------------

std::string MstrieStats::generate_last_query_stats(){
	std::string stats;
	stats += "Last query: " + last_query_name;
	stats += "; time: " + std::to_string(last_query_time_taken) + " ms";
	stats += "; nodes: " + std::to_string(last_query_traversed_nodes);
	stats += "\n";
	return stats;
}

// -----------------------------------------------------------------------------------------------

std::string MstrieStats::generate_total_stats(){
	std::string stats;
	stats += "Total nodes: " + std::to_string(total_number_of_nodes);
	stats += "; total multisets: " + std::to_string(total_number_of_multisets);
	//stats += "; space taken: " + std::to_string(sizeof(Mstrie)*total_number_of_nodes);
	stats += "\n";
	return stats;
}

// -----------------------------------------------------------------------------------------------

std::string MstrieStats::generate_benchmark_stats(){
	std::string stats;
	stats += std::to_string(last_query_time_taken);
	//    stats += ";";
	//    stats += std::to_string(last_query_traversed_nodes);
	stats += time_units;
	return stats;
}

// -----------------------------------------------------------------------------------------------

void MstrieStats::set_start_time() {
	this->tp_start = std::chrono::system_clock::now();
}

// -----------------------------------------------------------------------------------------------

void MstrieStats::set_end_time() {
	this->tp_end = std::chrono::system_clock::now();
}

// -----------------------------------------------------------------------------------------------

template<typename T>
void MstrieStats::calculate_last_query_time() {
	this->last_query_time_taken = std::chrono::duration_cast<T>(tp_end - tp_start).count();
}

// -----------------------------------------------------------------------------------------------

void MstrieStats::set_last_query_time() {
	if (time_units.compare("s") == 0) {
		this->calculate_last_query_time<std::chrono::seconds>();
	}
	else if (time_units.compare("ms") == 0) {
		this->calculate_last_query_time<std::chrono::milliseconds>();
	}
	else if (time_units.compare("µs") == 0) {
		this->calculate_last_query_time<std::chrono::microseconds>();
	}
	else if (time_units.compare("ns") == 0) {
		this->calculate_last_query_time<std::chrono::nanoseconds>();
	}
	else {
		throw std::runtime_error("Unknown time units.");
	}
}
