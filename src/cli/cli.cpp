//
//  cli.cpp
//  mstrie
//
//  Created by Mikita Akulich on 25/03/2018.
//  Copyright © 2018 Mikita Akulich. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <typeinfo>
#include "cli.hpp"


Cli::Cli(const Configurator &config, const std::string &default_manager_name) :
help_text("\nMstrie commands:\n"
	"\t help\n"
			"\t\t displays this dialog with commands description.\n"
	"\n\t configure\n"
			"\t\t loads or creates a new Multiset-trie structure using configuration parameters\n"
			"\t\t such as maximal allowed multiset multiplicity, maximal allowed multiset alphabet size\n"
			"\t\t and file path for storage.\n"
	"\n\t flush\n"
			"\t\t saves the Multiset-trie structure into configured file and destroys the instance.\n"
	"\n\t search < <= | = | >= > <word>\n"
			"\t\t gives an answer wheather there is a matching found similar to word. The type\n"
			"\t\t of matching can be specified: '=' - exact matching; '<=' - submultiset matching;\n"
			"\t\t '>=' - supermultiset matching.\n"
	"\n\t retrieve < <= | >= > <word | *> [limit]\n"
			"\t\t retrieves the matched results similar to word or * = empty string. The type of\n"
			"\t\t matching can be specified: '<=' - submultiset matching; '>=' - supermultiset matching.\n"
			"\t\t The limit parameter sets the offset limit for the multiplicity changes during search.\n"
	"\n\t update < - | + > <word>\n"
			"\t\t update the Multiset-trie structure with word. The types of update: '-' - word removal;\n"
			"\t\t '+' - word insertion.\n"
	"\n\t stats_<all | total | last>\n"
			"\t\t print statistics of the Multiset-trie structure. All - print both total and last\n"
			"\t\t statistics; total - print the total number of nodes and the total number of multisets\n"
			"\t\t in Multiset-trie; last - print the name, the time and the number of nodes traversed for\n"
			"\t\t the last performed query.\n"
	"\n\t exit\n"
			"\t\t perform flush command and exit the mstrie program.\n"),
f_mapper(std::map<std::string, std::function<void(Cli&, const std::vector<std::string>&)> > {
	{"help",				Cli::Tasks::display_help},
	{"configure",		Cli::Tasks::manager_configure},
	{"managers",		Cli::Tasks::manager_print},
	{"save",				Cli::Tasks::save_index},
	{"flush",				Cli::Tasks::flush_index},
	{"exit",				Cli::Tasks::exit},
	{"search",			Cli::Tasks::search_query},
	{"update",			Cli::Tasks::update_query},
	{"retrieve",		Cli::Tasks::retrieve_query},
	{"stats_all",		Cli::Tasks::stats_full},
	{"stats_total",	Cli::Tasks::stats_total},
	{"stats_last",	Cli::Tasks::stats_last}
}),
default_manager(default_manager_name) {
	this->current_manager = "";
	this->manager = std::map<std::string, std::unique_ptr<MstrieManager>>();
	this->do_loop = true;
	this->config = std::make_unique<Configurator>(config);
}

// -----------------------------------------------------------------------------------------------

void Cli::command_loop() {
	do {
		std::cout<<this->current_manager<<"> ";
		try {
			auto command_input = read_command();
			if (command_input.size() < 1) continue;
			exec_task(command_input);
		} catch (MstrieStructure::MstrieException &e) {
			print_exception(e);
		} catch (Configurator::ConfigurationException &e) {
			print_exception(e);
		} catch (std::exception &e) {
			print_exception(e);
		} catch (...) {
			print_message("Unknown exception. Exiting the program...");
			do_loop = false;
		}
	} while(do_loop);
}

// -----------------------------------------------------------------------------------------------

std::vector<std::string> Cli::read_command(){
	std::string input_command;
	std::vector<std::string> argv;
	std::getline(std::cin, input_command, '\n');
	if (!input_command.compare("")) {
		return argv;
	}
	// parse command into arguments
	std::stringstream input_stream (input_command);
	int argc=0;
	std::string arg;
	while (input_stream >> arg) {
		argv.push_back(arg);
		++argc;
	}
	if (argv.size() > 0) {
		if (f_mapper.find(argv[0]) == f_mapper.end()) {
			throw std::runtime_error("Unknown command: " + argv[0]);
		}
	}
	return argv;
}

// -----------------------------------------------------------------------------------------------

void Cli::exec_task(const std::vector<std::string> &command){
	try {
		f_mapper.at(command[0])(*this, command);
	} catch (std::exception &e) {
		print_exception(e);
	}
}

// -----------------------------------------------------------------------------------------------

void Cli::print_message(const std::string &message){
	std::cout<<message<<std::endl;
}

// -----------------------------------------------------------------------------------------------

void Cli::print_exception(std::exception &e, int level){
	std::cerr<<std::string(level, ' ')<<"error: "<<e.what()<<std::endl;
	try {
		std::rethrow_if_nested(e);
	} catch (std::exception &e) {
		print_exception(e, level+1);
	} catch (...) {
		std::cerr<<"Unknown exception occured while printing an exception."<<std::endl;
	}
}

// ===============================================================================================
// ===============================================================================================

/* ------------------------------------------------------------------
 * Tasks for configuration and destuction
 * ------------------------------------------------------------------
 */
void Cli::Tasks::manager_configure(Cli &cli, const std::vector<std::string> &argv){
	try {
		std::string manager_name;
		if (argv.size() == 1) { // default mstrie
			manager_name = cli.default_manager;
		}
		else if (argv.size() == 2) { // specific manager
			manager_name = argv[1];
		}
		else {
			throw std::runtime_error("Unexpected number of arguments, 1 or none are expected.");
		}
		
		// if manager does not exist, create a new one
		if (cli.manager.find(manager_name) == cli.manager.end()) {
			cli.manager.insert(std::make_pair(manager_name, nullptr));
		}
		
		// configure manager
		if (cli.manager.at(manager_name) != nullptr && cli.manager.at(manager_name)->index_exists()) {
			cli.current_manager = manager_name;
		}
		else {
			MstrieSettings settings = MstrieSettings(
																							 cli.config->get_value<uint>(manager_name + ":alphabet_length"),
																							 cli.config->get_value<uint>(manager_name + ":max_multiplicity"),
																							 cli.config->get_value<std::string>(manager_name + ":mstrie_path")
																							 );
			cli.manager.at(manager_name) = std::make_unique<MstrieManager>(settings);
			try {
				cli.manager.at(manager_name)->init_index();
			} catch (std::exception &e) {
				cli.manager.at(manager_name) = nullptr;
				cli.manager.erase(manager_name);
				throw;
			}
			cli.current_manager = manager_name;
		}
	} catch (std::exception &e) {
		throw;
	}
}

// -----------------------------------------------------------------------------------------------

void Cli::Tasks::manager_print(Cli &cli, const std::vector<std::string> &argv) {
	try {
		if (cli.current_manager.compare("") == 0) {
			cli.print_message("No current manager.");
		}
		else {
			cli.print_message("Current manager: " + cli.current_manager);
		}
		if (cli.manager.size() > 0) {
			cli.print_message("All configured managers:");
			for (auto &manager : cli.manager) {
				cli.print_message("\t" + manager.first);
			}
		}
		else {
			cli.print_message("No configured managers found.");
		}
	} catch (std::exception &e) {
		throw;
	}
}

// -----------------------------------------------------------------------------------------------

void Cli::Tasks::save_index(Cli &cli, const std::vector<std::string> &argv){
	try {
		for (auto &index : cli.manager) {
			if (index.second != nullptr && index.second->index_exists()) {
				index.second->flush_index(false);
			}
		}
	} catch (std::exception &e) {
		throw;
	}
}

// -----------------------------------------------------------------------------------------------

void Cli::Tasks::flush_index(Cli &cli, const std::vector<std::string> &argv){
	try {
		std::string manager_name;
		if (argv.size() == 1) {
			manager_name = cli.current_manager;
		}
		else if (argv.size() == 2) {
			manager_name = argv[1];
		}
		else {
			throw std::runtime_error("Unexpected number of arguments, 1 or none are expected.");
		}
		if (cli.manager.find(manager_name) == cli.manager.end()) {
			throw std::runtime_error("Manager '" + manager_name + "' does not exist.");
		}
		if (cli.manager.at(cli.current_manager) != nullptr && cli.manager.at(cli.current_manager)->index_exists()) {
			cli.manager.at(cli.current_manager)->flush_index(true);
			cli.manager.erase(cli.current_manager);
			cli.current_manager = "";
		}
	} catch (std::exception &e) {
		throw;
	}
}

// -----------------------------------------------------------------------------------------------

void Cli::Tasks::exit(Cli &cli, const std::vector<std::string> &argv){
	try {
		for (auto &manager : cli.manager) {
			if (manager.second != nullptr && manager.second->index_exists()) {
				manager.second->flush_index(true);
			}
		}
		cli.manager.clear();
		cli.do_loop = false;
	} catch (std::exception &e) {
		throw;
	}
}

// -----------------------------------------------------------------------------------------------

void Cli::Tasks::display_help(Cli &cli, const std::vector<std::string> &argv) {
	cli.print_message(cli.help_text);
}

// ===============================================================================================
// ===============================================================================================

/* ------------------------------------------------------------------
 * Tasks for queries
 * ------------------------------------------------------------------
 */
void Cli::Tasks::search_query(Cli &cli, const std::vector<std::string> &argv){
	try {
		if (cli.manager.at(cli.current_manager) != nullptr && cli.manager.at(cli.current_manager)->index_exists()){
			bool result;
			if (argv.size() == 4) { // the limit is specified
				result = cli.manager.at(cli.current_manager)->search_query(argv[1], argv[2], std::stoi(argv[3]));
			}
			else if (argv.size() == 3) { // the limit is ommited
				result = cli.manager.at(cli.current_manager)->search_query(argv[1], argv[2]);
			}
			else {
				throw std::runtime_error("Unexpected number of arguments, 2 expected");
			}
			// print result
			if (result) {
				cli.print_message("Found match to "+ argv[2]);
			}
			else {
				cli.print_message("There is no match to "+ argv[2]);
			}
		}
		else {
			cli.print_message("Index does not exist.");
		}
	} catch (std::exception &e) {
		throw;
	}
}

// -----------------------------------------------------------------------------------------------

void Cli::Tasks::update_query(Cli &cli, const std::vector<std::string> &argv){
	try {
		if (cli.manager.at(cli.current_manager) != nullptr && cli.manager.at(cli.current_manager)->index_exists()){
			if (argv.size() == 3) {
				cli.manager.at(cli.current_manager)->update_query(argv[1], argv[2]);
			}
			else {
				throw std::runtime_error("Unexpected number of arguments, 2 expected.");
			}
		}
		else {
			cli.print_message("Index does not exist.");
		}
	} catch (std::exception &e) {
		throw;
	}
}

// -----------------------------------------------------------------------------------------------

void Cli::Tasks::retrieve_query(Cli &cli, const std::vector<std::string> &argv){
	try {
		if (cli.manager.at(cli.current_manager) != nullptr && cli.manager.at(cli.current_manager)->index_exists()){
			std::string q_out;
			if (argv.size() == 4) { // the limit is specified
				q_out = cli.manager.at(cli.current_manager)->retrieve_query(argv[1], argv[2], std::stoi(argv[3]));
			}
			else if (argv.size() == 3) { // the limit is ommited
				q_out = cli.manager.at(cli.current_manager)->retrieve_query(argv[1], argv[2]);
			}
			else {
				throw std::runtime_error("Unexpected number of arguments, 2 or 3 expected");
			}
			cli.print_message(q_out);
		}
		else {
			cli.print_message("Index does not exist.");
		}
	} catch (std::exception &e) {
		throw;
	}
}

// ===============================================================================================
// ===============================================================================================

/* ------------------------------------------------------------------
 * Tasks for statistics
 * ------------------------------------------------------------------
 */
void Cli::Tasks::stats_full(Cli &cli, const std::vector<std::string> &argv){
	if (cli.manager.at(cli.current_manager) != nullptr && cli.manager.at(cli.current_manager)->index_exists()){
		cli.print_message(cli.manager.at(cli.current_manager)->print_full_stats());
	}
	else {
		cli.print_message("Index does not exist.");
	}
}

// -----------------------------------------------------------------------------------------------

void Cli::Tasks::stats_total(Cli &cli, const std::vector<std::string> &argv){
	if (cli.manager.at(cli.current_manager) != nullptr && cli.manager.at(cli.current_manager)->index_exists()){
		cli.print_message(cli.manager.at(cli.current_manager)->print_total_stats());
	}
	else {
		cli.print_message("Index does not exist.");
	}
}

// -----------------------------------------------------------------------------------------------

void Cli::Tasks::stats_last(Cli &cli, const std::vector<std::string> &argv){
	if (cli.manager.at(cli.current_manager) != nullptr && cli.manager.at(cli.current_manager)->index_exists()){
		cli.print_message(cli.manager.at(cli.current_manager)->print_last_query_stats());
	}
	else {
		cli.print_message("Index does not exist.");
	}
}

