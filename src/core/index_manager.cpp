//
//  index_manager.cpp
//  mstrie
//
//  Created by Mikita Akulich on 25/03/2018.
//  Copyright © 2018 Mikita Akulich. All rights reserved.
//

#include <iostream>
#include <vector>
#include <string>
#include <exception>
#include <chrono>
#include "index_manager.hpp"
#include "../utils/file_utils.hpp"


MstrieManager::MstrieManager(const MstrieSettings &settings) {
	this->mstrie = nullptr;
	this->settings = std::make_unique<MstrieSettings>(settings);
}

// ===============================================================================================
// ===============================================================================================

void MstrieManager::init_index() {
	try {
		create_index();
	} catch (std::exception &e) {
		throw;
	}
}

// -----------------------------------------------------------------------------------------------

void MstrieManager::create_index() {
	try {
		mstrie = std::make_unique<MstrieStructure>(*settings);
		if (FileUtils::file_exists(settings->index_path)) {
			mstrie->load_mstrie(FileUtils::read_from_file(settings->index_path));
		}
		else
			save_index();
	} catch (std::exception &e) {
		mstrie = nullptr;
		throw;
	}
}

// -----------------------------------------------------------------------------------------------

void MstrieManager::save_index() {
	try {
		FileUtils::write_file(settings->index_path, mstrie->retrieve_mstrie());
	} catch (std::exception &e) {
		throw;
	}
}

// -----------------------------------------------------------------------------------------------

void MstrieManager::flush_index(bool destroy) {
	try {
		save_index();
		if (destroy) {
			mstrie = nullptr;
		}
	} catch (std::exception &e) {
		throw;
	}
}

// -----------------------------------------------------------------------------------------------

bool MstrieManager::index_exists() {
	return mstrie != nullptr;
}

// -----------------------------------------------------------------------------------------------

bool MstrieManager::search_query(const std::string &query_type, const std::string &word, int limit){
	try {
		if (query_type.compare("=") == 0) {
			return mstrie->pub_mstrie_search(word);
		}
		else if (query_type.compare("<=") == 0){
			return mstrie->pub_mstrie_subseteq(word, limit);
		}
		else if (query_type.compare(">=") == 0){
			return mstrie->pub_mstrie_superseteq(word, limit);
		}
		else {
			throw MstrieStructure::MstrieException("Unknown search query");
		}
	} catch (std::exception &e) {
		throw;
	}
}

// -----------------------------------------------------------------------------------------------

void MstrieManager::update_query(const std::string &query_type, const std::string &word){
	try {
		if (!query_type.compare("+")) {
			return mstrie->pub_mstrie_insert(word);
		}
		else if (!query_type.compare("-")){
			return mstrie->pub_mstrie_delete(word);
		}
		else {
			throw MstrieStructure::MstrieException("Unknown update query");
		}
	} catch (std::exception &e) {
		throw;
	}
}

// -----------------------------------------------------------------------------------------------

std::string MstrieManager::retrieve_query(const std::string &query_type, const std::string &word, int limit){
	try {
		std::vector<std::string> results = std::vector<std::string>();
		if (query_type.compare("=") == 0){
			if (mstrie->pub_mstrie_search(word))
				return word;
			else
				return "";
		}
		else if (query_type.compare("<=") == 0) {
			return mstrie->pub_mstrie_get_subseteq(word, limit);
		}
		else if (query_type.compare(">=") == 0) {
			return mstrie->pub_mstrie_get_superseteq(word, limit);
		}
		else {
			throw MstrieStructure::MstrieException("Unknown retrieve query");
		}
	} catch (std::exception &e) {
		throw;
	}
}

// -----------------------------------------------------------------------------------------------

std::string MstrieManager::print_full_stats(){
	return mstrie->print_full_stats();
}

// -----------------------------------------------------------------------------------------------

std::string MstrieManager::print_total_stats(){
	return mstrie->print_total_stats();
}

// -----------------------------------------------------------------------------------------------

std::string MstrieManager::print_last_query_stats(){
	return mstrie->print_last_query_stats();
}

// -----------------------------------------------------------------------------------------------

std::string MstrieManager::print_benchmark_stats(){
	return mstrie->print_benchmark_stats();
}
