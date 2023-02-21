//
//  configurator.hpp
//  configurator
//
//  Created by Mikita Akulich on 27/01/2019.
//  Copyright © 2019 Mikita Akulich. All rights reserved.
//

#ifndef configurator_hpp
#define configurator_hpp

/* The classes below are exported */

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <exception>
#include <fstream>
#include <memory>

#pragma GCC visibility push(default)

class Configurator {
    
    class Config {
    public:
        std::map<std::string, std::shared_ptr<Config>> groups;
        std::map<std::string, std::string> parameters;
    };
    
    std::shared_ptr<Config> config;
    
    std::vector<std::string> parse_parameter_identifier(const std::string &parameter_identifier);
    
    // loan configuration into groups and parameters
    void load_configuration(std::stringstream &content);
    
    // set parameter value
    void set_value(const std::string &parameter_identifier, const std::string &parameter_value);
    
    std::string trim(const std::string& str);
    
public:
    Configurator(const std::string &config_file);
    
    // custom exception
    class ConfigurationException : public std::exception {
    private:
        const std::string _message;
    public:
        ConfigurationException(const std::string &message)
        : std::exception(), _message(message) { }
        
        virtual const char* what() const throw() {
            return _message.c_str();
        }
    };
    
    template <typename T>
    T convert_to(const std::string &str) {
        std::istringstream ss(str);
        T result;
        ss >> result;
        return result;
    }
    
    template <typename T>
    T get_value(const std::string &parameter_identifier){
        auto ids = parse_parameter_identifier(parameter_identifier);
        std::shared_ptr<Config> current_config_group = config;
        size_t n = ids.size();
        for (size_t i = 0; i < n - 1; i++) {
            if (current_config_group->groups.find(ids[i]) == current_config_group->groups.end()) {
                throw Configurator::ConfigurationException("Could not find configuration group " + ids[i]);
            }
            current_config_group = current_config_group->groups[ids[i]];
        }
        if (current_config_group->parameters.find(ids[n - 1]) == current_config_group->parameters.end())
            throw Configurator::ConfigurationException("Could not find configuration parameter: " + ids[n - 1]);
        return convert_to<T>(current_config_group->parameters[ids[n - 1]]);
    }
};

#pragma GCC visibility pop

#endif /* configurator_hpp */
