// @otlicense

//! @file FileSystem.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTSystem/Exception.h"
#include "OTSystem/FileSystem.h"

// std header
#include <filesystem>

namespace ot {
    namespace intern {

        bool matchesExtension(const std::filesystem::path& _path, const std::list<std::string>& _extensions) {
            if (_extensions.empty()) {
                return true; // accept all if no filter
            }

            const std::string ext = _path.extension().string();
            for (const std::string& allowedExt : _extensions) {
                if (ext == allowedExt) {
                    return true;
                }
            }

            return false;
        }

    }
}

std::list<std::string> ot::FileSystem::getDirectories(const std::string& _path, const FileSystemOptions& _options) {
	std::list<std::string> result;

    if (_options & FileSystemOption::Recursive) {
        for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(_path)) {
            if (entry.is_directory()) {
                if (_options & FileSystemOption::GenericFormat) {
                    result.push_back(entry.path().generic_string());
                }
                else {
                    result.push_back(entry.path().string());
                }
                
            }
        }
    }
    else {
        for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(_path)) {
            if (entry.is_directory()) {
                if (_options & FileSystemOption::GenericFormat) {
                    result.push_back(entry.path().generic_string());
                }
                else {
                    result.push_back(entry.path().string());
                }
            }
        }
    }

    return result;
}

std::list<std::string> ot::FileSystem::getFiles(const std::string& _path, const std::list<std::string>& _extensions, const FileSystemOptions& _options) {
    std::list<std::string> result;

    if (_options & FileSystemOption::Recursive) {
        for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(_path)) {
            if (entry.is_regular_file() && intern::matchesExtension(entry.path(), _extensions)) {
                if (_options & FileSystemOption::GenericFormat) {
                    result.push_back(entry.path().generic_string());
                }
                else {
                    result.push_back(entry.path().string());
                }
            }
        }
    }
    else {
        for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(_path)) {
            if (entry.is_regular_file() && intern::matchesExtension(entry.path(), _extensions)) {
                if (_options & FileSystemOption::GenericFormat) {
                    result.push_back(entry.path().generic_string());
                }
                else {
                    result.push_back(entry.path().string());
                }
            }
        }
    }

    return result;
}

std::list<std::string> ot::FileSystem::readLines(const std::string& _filePath, std::ifstream::iostate _exceptionMask) {
    std::list<std::string> lines;
    std::ifstream file(_filePath);

    if (!file.is_open()) {
        throw Exception::FileOpen("Failed to open fil for reading: \"" + _filePath + "\"");
    }

    file.exceptions(_exceptionMask);

    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(std::move(line));
        line.clear();
    }

    return lines;
}
