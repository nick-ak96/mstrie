//
//  cli.hpp
//  mstrie
//
//  Created by Mikita Akulich on 25/03/2018.
//  Copyright © 2018 Mikita Akulich. All rights reserved.
//

#ifndef CLI_HPP
#define CLI_HPP

#include <map>
#include <functional>
#include "../core/index_manager.hpp"
#include "../lib/configurator.hpp"


class Cli {
private:
	// config
	std::unique_ptr<Configurator> config;
	// mstrie
	std::map<std::string, std::unique_ptr<MstrieManager>> manager;
	std::string current_manager;
	const std::string default_manager;
	
	bool do_loop;
	const std::string help_text;
	std::vector<std::string> read_command();
	void exec_task(const std::vector<std::string> &command);
	const std::map<std::string, std::function<void(Cli&, const std::vector<std::string>&)> > f_mapper;
	
	// tasks
	class Tasks {
	public:
		static void manager_configure(Cli &cli, const std::vector<std::string> &argv);
		static void manager_print(Cli &cli, const std::vector<std::string> &argv);
		static void flush_index(Cli &cli, const std::vector<std::string> &argv);
		static void save_index(Cli &cli, const std::vector<std::string> &argv);
		static void exit(Cli &cli, const std::vector<std::string> &argv);
		static void search_query(Cli &cli, const std::vector<std::string> &argv);
		static void update_query(Cli &cli, const std::vector<std::string> &argv);
		static void retrieve_query(Cli &cli, const std::vector<std::string> &argv);
		static void stats_full(Cli &cli, const std::vector<std::string> &argv);
		static void stats_total(Cli &cli, const std::vector<std::string> &argv);
		static void stats_last(Cli &cli, const std::vector<std::string> &argv);
		static void display_help(Cli &cli, const std::vector<std::string> &argv);
	};
	
	// utilities
	void print_message(const std::string &message);
	void print_exception(std::exception &e, int level = 0);
public:
	Cli(const Configurator &config, const std::string &default_manager_name);
	void command_loop();
};

#endif /* CLI_HPP */
