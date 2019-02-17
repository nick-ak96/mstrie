//
//  file_utils.hpp
//  mstrie
//
//  Created by Mikita Akulich on 25/03/2018.
//  Copyright © 2018 Mikita Akulich. All rights reserved.
//

#ifndef FILE_UTILS_HPP
#define FILE_UTILS_HPP

class FileUtils {
public:
	static bool file_exists(const std::string &file_path);
	static void write_file(const std::string &file_path, const std::string &content);
	static std::string read_from_file(const std::string &file_path);
	static bool check_file_extension(const std::string &file_path, const std::string &extension_to_check);
};
#endif /* FILE_UTILS_HPP */
