// @otlicense
// File: main.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// std header
#include <list>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

bool hasValidExtension(const std::string& _filename) {
	const std::list<std::string> validExtensions = {
		".h", ".hpp", ".c", ".cpp"
	};

	for (const std::string& ext : validExtensions) {
		if (_filename.size() >= ext.size() &&
			_filename.compare(_filename.size() - ext.size(), ext.size(), ext) == 0) {
			return true;
		}
	}

	return false;
}

int main(int _argc, char* _argv[]) {
    if (_argc != 2) {
        std::cout << "Usage: " << _argv[0] << " <filename>" << std::endl;
        return 1;
    }

    std::ifstream file(_argv[1]);
    if (!file.is_open()) {
        std::cout << "Failed to open file: " << _argv[1] << std::endl;
        return 1;
    }

    std::string line;
    unsigned long long totalAdded = 0;
    unsigned long long totalRemoved = 0;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string addedStr, removedStr, path;

        if (!(iss >> addedStr >> removedStr >> path)) {
            // skip invalid lines
            continue;
        }

        if (!hasValidExtension(path)) {
            continue;
        }

        try {
            int added = (addedStr == "-" ? 0 : std::stoi(addedStr));
            int removed = (removedStr == "-" ? 0 : std::stoi(removedStr));
            totalAdded += added;
            totalRemoved += removed;
        }
        catch (...) {
            // skip lines with invalid number formats
            continue;
        }
    }

    std::cout << "Lines added:   " << totalAdded << std::endl;
    std::cout << "Lines removed: " << totalRemoved << std::endl;
    std::cout << "Total changes: " << (totalAdded + totalRemoved) << std::endl;

    return 0;
}
