//
//  benchmark.hpp
//  mstrie
//
//  Created by Mikita Akulich on 25/03/2018.
//  Copyright © 2018 Mikita Akulich. All rights reserved.
//

#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include "../core/index_manager.hpp"
#include "../lib/configurator.hpp"

class Benchmark {
private:
	std::unique_ptr<Configurator> config;
	std::unique_ptr<MstrieManager> manager;
	void process(const std::string &mstrie_query_type, std::ifstream &ifile, std::ofstream &ofile);
public:
	Benchmark(const Configurator &config);
	void run();
};

#endif /* BENCHMARK_HPP */
