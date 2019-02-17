//
//  mstrie.hpp
//  mstrie
//
//  Created by Mikita Akulich on 25/03/2018.
//  Copyright © 2018 Mikita Akulich. All rights reserved.
//

#ifndef MSTRIE_HPP
#define MSTRIE_HPP

#include <queue>


/* The class that holds statistics of the mstrie structure */
class MstrieStats {
private:
	template<typename T>
	void calculate_last_query_time();
public:
	int total_number_of_nodes;
	int total_number_of_multisets;
	int last_query_traversed_nodes;
	long last_query_time_taken;
	std::string last_query_name;
	
	std::chrono::system_clock::time_point tp_start;
	std::chrono::system_clock::time_point tp_end;
	
	void set_start_time();
	void set_end_time();
	
	void set_last_query_time();
	std::string time_units;
	
	
	MstrieStats(const std::string &units = "µs"); // micro sec by default
	
	// resets last_query_*
	void reset();
	// creates statistics output for last query
	std::string generate_last_query_stats();
	// creates statistics output for total values
	std::string generate_total_stats();
	// creates statistics output for benchmark
	std::string generate_benchmark_stats();
};

/* The mstrie settings */
class MstrieSettings {
public:
	// the length of alphabet of elements for multisets
	const uint alphabet;
	// the maximal multiplicity of elements for multisets
	const uint max_multiplicity;
	
	const std::string index_path;
	
	MstrieSettings(uint alphabet, uint max_multiplicity, const std::string &index_path);
};

/* The base class for nodes in the mstrie structure */
class MstrieNode
{
public:
	// array with pointers that represent the multiplicity
	// and gives access to corresponding child node
	std::shared_ptr<std::vector<std::shared_ptr<MstrieNode>>> mult_switch;
	
	MstrieNode(const uint max_multiplicity);
};

/* The class for mstrie structure management */
class MstrieStructure {
private:
	// mstrie settings
	const std::unique_ptr<MstrieSettings> _settings;
	// indicator node: multiset acceptor
	const std::shared_ptr<MstrieNode> _dummy;
	// root node of the mstrie structure
	const std::shared_ptr<MstrieNode> _root;
	
	std::unique_ptr<MstrieStats> statistics;
	
	/* private queries */
	// insert
	void mstrie_insert(const std::vector<uint> &sv_input);
	// delete
	void mstrie_delete(const std::vector<uint> &sv_input);
	// search
	bool mstrie_search(const std::vector<uint> &sv_input);
	// exists closest sub
	bool mstrie_subseteq(const std::vector<uint> &sv_input, uint limit);
	bool mstrie_subseteq_rec(const std::shared_ptr<MstrieNode> root, const std::vector<uint> &sv_input, uint limit, uint vcnt);
	// exists closest super
	bool mstrie_superseteq(const std::vector<uint> &sv_input, uint limit);
	bool mstrie_superseteq_rec(const std::shared_ptr<MstrieNode> root, const std::vector<uint> &sv_input, uint limit, uint vcnt);
	// retrieval closest sub
	std::queue<std::string> mstrie_get_subseteq(const std::vector<uint> &sv_input, uint limit);
	void mstrie_get_subseteq_rec(const std::shared_ptr<MstrieNode> root, const std::vector<uint> &sv_input, std::vector<uint> &sv_output, std::queue<std::string> &str_que, uint limit, uint vcnt);
	// retrieval closest super
	std::queue<std::string> mstrie_get_superseteq(const std::vector<uint> &sv_input, uint limit);
	void mstrie_get_superseteq_rec(const std::shared_ptr<MstrieNode> root, const std::vector<uint> &sv_input, std::vector<uint> &sv_output, std::queue<std::string> &str_que, uint limit, uint vcnt);
	
	/* utility functions */
	std::string prepare_mstrie_dump(std::queue<std::string> queue);
	std::string timestamp_string();
	
	std::vector<uint> str_to_num(const std::string &token);
	std::string num_to_str(const std::vector<uint> &v);
public:
	MstrieStructure(const MstrieSettings &settings);
	
	// custom mstrie exception
	class MstrieException : public std::exception {
	private:
		const std::string _message;
	public:
		MstrieException(const std::string &message)
		: std::exception(), _message(message) { }
		
		virtual const char* what() const throw() {
			return _message.c_str();
		}
	};
	
	
	/* read/write functions */
	void load_mstrie(const std::string &content);
	std::string retrieve_mstrie();
	
	/* public queries */
	
	// insert
	void pub_mstrie_insert(const std::string &word);
	// delete
	void pub_mstrie_delete(const std::string &word);
	// search
	bool pub_mstrie_search(const std::string &word);
	// exists closest sub
	bool pub_mstrie_subseteq(const std::string &word);
	bool pub_mstrie_subseteq(const std::string &word, uint limit);
	// exists closest super
	bool pub_mstrie_superseteq(const std::string &word);
	bool pub_mstrie_superseteq(const std::string &word, uint limit);
	// retrieval closest sub
	std::string pub_mstrie_get_subseteq(const std::string &word);
	std::string pub_mstrie_get_subseteq(const std::string &word, uint limit);
	// retrieval closest super
	std::string pub_mstrie_get_superseteq(const std::string &word);
	std::string pub_mstrie_get_superseteq(const std::string &word, uint limit);
	
	std::string print_full_stats();
	std::string print_last_query_stats();
	std::string print_total_stats();
	std::string print_benchmark_stats();
};
#endif /* MSTRIE_HPP */
