//
//  index_manager.hpp
//  mstrie
//
//  Created by Mikita Akulich on 25/03/2018.
//  Copyright © 2018 Mikita Akulich. All rights reserved.
//

#ifndef INDEX_MANAGER_HPP
#define INDEX_MANAGER_HPP

#include "mstrie.hpp"


/* Manager for MstrieStructure instance */
class MstrieManager {
private:
	std::unique_ptr<MstrieSettings> settings;
	std::unique_ptr<MstrieStructure> mstrie;
	
	void create_index();
	void save_index();
public:
	MstrieManager(const MstrieSettings &settings);
	
	void init_index();
	void flush_index(bool destroy);
	
	/* queries */
	bool search_query(const std::string &query_type, const std::string &word, int limit = -1);
	void update_query(const std::string &query_type, const std::string &word);
	std::string retrieve_query(const std::string &query_type, const std::string &word, int limit = -1);
	std::string print_full_stats();
	std::string print_total_stats();
	std::string print_last_query_stats();
	std::string print_benchmark_stats();
	
	bool index_exists();
};

#endif /* INDEX_MANAGER_HPP */
