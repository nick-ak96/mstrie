//
//  configurator.cpp
//  configurator
//
//  Created by Mikita Akulich on 27/01/2019.
//  Copyright © 2019 Mikita Akulich. All rights reserved.
//

#include "configurator.hpp"

Configurator::Configurator(const std::string &config_file) {
    this->config = std::make_shared<Config>();
    
    std::ifstream ifile;
    ifile.open(config_file);
    if (!ifile.is_open()) {
        throw ConfigurationException("Could not open configuration file "+ config_file);
    }
    std::string line;
    std::stringstream content;
    content<<ifile.rdbuf();
    ifile.close();
    
    this->load_configuration(content);
}

// -----------------------------------------------------------------------------------------------

void Configurator::load_configuration(std::stringstream &content) {
    std::vector<std::string> groups;
    std::string line;
    uint line_cnt = 1;
    long level_cur = 0;
    
    while (std::getline(content, line)) {
        // check if line is a comment or empty
        if (line.compare("") == 0 || line.front() == '#') continue;
        
        // get parameter level
        level_cur = line.find_first_not_of("\t");
        
        // trim line
        line = trim(line);
        
        // check if line is empty
        if (line.compare("") == 0) continue;
        
        // identifier is a group
        if (line.back() == ':') {
            // set group
            if (groups.size() < level_cur + 1) {
                groups.resize(level_cur + 1);
            }
            groups[level_cur] = line;
        }
        // identifier is a parameter
        else {
            auto split_position = line.find_first_of("=");
            std::string parameter_name = trim(line.substr(0, split_position - 1));
            auto par_val_start = line.find_first_of("\"", split_position + 1);
            auto par_val_end = line.find_first_of("\"", par_val_start + 1);
            std::string parameter_value = line.substr(par_val_start + 1, par_val_end - par_val_start - 1);
            
            // create parameter identifier
            std::string parameter_identifier = "";
            for (int i = 0; i < level_cur; i++) {
                parameter_identifier.append(groups[i]);
            }
            parameter_identifier.append(parameter_name);
            
            // set value to configuration
            this->set_value(parameter_identifier, parameter_value);
        }
        line_cnt++;
    }
}

// -----------------------------------------------------------------------------------------------

std::string Configurator::trim(const std::string& str)
{
    const auto str_begin = str.find_first_not_of(" \t");
    
    if (str_begin == std::string::npos)
        return ""; // no content
    
    const auto str_end = str.find_last_not_of(" \t");
    return str.substr(str_begin, str_end - str_begin + 1);
}

// -----------------------------------------------------------------------------------------------

std::vector<std::string> Configurator::parse_parameter_identifier(const std::string &parameter_identifier) {
    std::vector<std::string> ids;
    std::string el;
    char delimiter = ':';
    std::istringstream tokenStream(parameter_identifier);
    while (std::getline(tokenStream, el, delimiter)) {
        ids.push_back(el);
    }
    return ids;
}

// -----------------------------------------------------------------------------------------------

void Configurator::set_value(const std::string &parameter_identifier, const std::string &parameter_value) {
    auto ids = this->parse_parameter_identifier(parameter_identifier);
    
    std::shared_ptr<Config> current_config_group = this->config;
    size_t n = ids.size();
    for (size_t i = 0; i < n - 1; i++) {
        if (current_config_group->groups.find(ids[i]) == current_config_group->groups.end()) {
            current_config_group->groups[ids[i]] = std::make_shared<Config>();
        }
        current_config_group = current_config_group->groups[ids[i]];
    }
    current_config_group->parameters[ids[n - 1]] = parameter_value;
}
