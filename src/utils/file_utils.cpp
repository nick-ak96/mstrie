//
//  file_utils.cpp
//  mstrie
//
//  Created by Mikita Akulich on 25/03/2018.
//  Copyright © 2018 Mikita Akulich. All rights reserved.
//

#include <string>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include "file_utils.hpp"


bool FileUtils::file_exists(const std::string &file_path){
	struct stat buf;
	return stat(file_path.c_str(), &buf) != -1;
}

// -----------------------------------------------------------------------------------------------

void FileUtils::write_file(const std::string &file_path, const std::string &content) {
	std::ofstream ofile;
	/* Setting exceptions for a file to be thrown */
	ofile.exceptions( std::ofstream::failbit | std::ofstream::badbit );
	/* Open the file */
	ofile.open(file_path);
	if (!ofile.is_open()) {
		throw std::runtime_error("ERROR: File "+file_path+" can't be opened.");
	}
	ofile<<content;
	ofile.close();
}

// -----------------------------------------------------------------------------------------------

std::string FileUtils::read_from_file(const std::string &file_path) {
	std::ifstream ifile;
	/* Setting exceptions for a file to be thrown */
	ifile.exceptions( std::ifstream::badbit );
	/* Open the file */
	ifile.open(file_path);
	if (!ifile.is_open()) {
		throw std::runtime_error("ERROR: File "+file_path+" can't be opened.");
	}
	
	std::stringstream content;
	content<<ifile.rdbuf();
	ifile.close();
	return content.str();
}

// -----------------------------------------------------------------------------------------------

std::string get_file_extension(const std::string &file_path) {
	// find part of path that is after '.'
	auto st_pos = file_path.find_last_of(".");
	
	// no extension or the '.' is the last character
	if (st_pos == std::string::npos || st_pos == file_path.length() - 1) {
		return "";
	}
	std::string extension = file_path.substr(st_pos+1);
	// there is no extension, and the '.' was found in the name of the folder
	if (extension.find_first_of("/\\") != std::string::npos) {
		return "";
	}
	else {
		return extension;
	}
}

// -----------------------------------------------------------------------------------------------

bool FileUtils::check_file_extension(const std::string &file_path, const std::string &extension_to_check) {
	std::string file_extension = get_file_extension(file_path);
	return (file_extension.compare(extension_to_check) == 0);
}
