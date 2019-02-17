//
//  benchmark.cpp
//  mstrie
//
//  Created by Mikita Akulich on 25/03/2018.
//  Copyright © 2018 Mikita Akulich. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <istream>
#include <ostream>
#include "benchmark.hpp"

Benchmark::Benchmark(const Configurator &config) {
	this->config = std::make_unique<Configurator>(config);
	auto mstrie = this->config->get_value<std::string>("benchmark:mstrie_name");
	MstrieSettings settings = MstrieSettings(
																					 this->config->get_value<uint>(mstrie + ":alphabet_length"),
																					 this->config->get_value<uint>(mstrie + ":max_multiplicity"),
																					 this->config->get_value<std::string>(mstrie + ":mstrie_path")
																					 );
	this->manager = std::make_unique<MstrieManager>(settings);
}

void Benchmark::run() {
	
	// initialize index
	manager->init_index();
	
	/* process benchmark */
	std::string search_type = config->get_value<std::string>("benchmark:run:type");
	std::string mstrie_query_type;
	if (search_type.compare("exact_search") == 0) {
		mstrie_query_type = "=";
	}
	else if (search_type.compare("subset_search") == 0) {
		mstrie_query_type = "<=";
	}
	else if (search_type.compare("superset_search") == 0) {
		mstrie_query_type = ">=";
	}
	else{
		throw std::runtime_error("Unknown benchmark type: "+ search_type);
	}
	
	/* open test and result files */
	
	std::ifstream test_file;
	std::string test_file_name = config->get_value<std::string>("benchmark:run:test_file");
	/* Setting exceptions for a file to be thrown */
	test_file.exceptions( std::ifstream::badbit );
	/* Open the file */
	test_file.open(test_file_name);
	if (!test_file.is_open()) {
		throw std::runtime_error("ERROR: File "+ test_file_name +" can't be opened.");
	}
	std::ofstream result_file;
	std::string result_file_name = config->get_value<std::string>("benchmark:run:result_file");
	/* Setting exceptions for a file to be thrown */
	result_file.exceptions( std::ofstream::badbit | std::ofstream::failbit );
	/* Open the file */
	result_file.open(result_file_name);
	if (!result_file.is_open()) {
		throw std::runtime_error("ERROR: File "+result_file_name+" can't be opened.");
	}
	
	process(mstrie_query_type, test_file, result_file);
	
	/* close files */
	
	test_file.close();
	result_file.close();
}


void Benchmark::process(const std::string &mstrie_query_type, std::ifstream &ifile, std::ofstream &ofile) {
	// result file header
	ofile<<"test;output;time_μs"<<std::endl;
	
	std::string test;
	while(std::getline(ifile, test)) {
		auto result = manager->retrieve_query(mstrie_query_type, test);
		ofile<<test<<";"<<result<<";"<<manager->print_benchmark_stats();
		ofile<<std::endl;
	}
}
