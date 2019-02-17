//
//  main.cpp
//  mstrie
//
//  Created by Mikita Akulich on 25/03/2018.
//  Copyright © 2018 Mikita Akulich. All rights reserved.
//


#include <iostream>
#include "cli/cli.hpp"
#include "benchmark/benchmark.hpp"
#include "lib/configurator.hpp"


int main(int argc, const char * argv[]) {
	if (argc != 2) {
		std::cerr<<"No configuration file specified."<<std::endl;
		return EXIT_FAILURE;
	}
	
	// create config
	std::string configuration_file(argv[1]);
	
	try {
		Configurator config = Configurator(configuration_file);
		std::string run_mode = config.get_value<std::string>("run_mode");
		
		std::cout<<"Mstrie 0.1"<<std::endl;
		if (run_mode.compare("benchmark") == 0) {
			std::cout<<"Running benchmark..."<<std::endl;
			std::unique_ptr<Benchmark> benchmark = std::make_unique<Benchmark>(config);
			benchmark->run();
			std::cout<<"Done."<<std::endl;
		}
		else if (run_mode.compare("cli") == 0) {
			std::cout<<"Type \"help\" for more information."<<std::endl;
			
			std::unique_ptr<Cli> cli = std::make_unique<Cli>(config, config.get_value<std::string>("default_mstrie_name"));
			cli->command_loop();
		}
		else {
			std::cerr<<"Unknown run mode for mstrie."<<std::endl;
			return EXIT_FAILURE;
		}
	} catch (Configurator::ConfigurationException &e) {
		std::cerr<<"Configuration exception: "<<std::string(e.what())<<std::endl;
		return EXIT_FAILURE;
	} catch (std::exception &e) {
		std::cerr<<"Unhandled exception occured: "<<std::string(e.what())<<std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
