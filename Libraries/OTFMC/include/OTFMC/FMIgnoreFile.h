// @otlicense
// File: FMIgnoreFile.h
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

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTFMC/FMConnectorAPIExport.h"

// std header
#include <list>
#include <regex>
#include <filesystem>

namespace ot {

	class FMIgnoreFile {
		OT_DECL_DEFCOPY(FMIgnoreFile)
		OT_DECL_DEFMOVE(FMIgnoreFile)
	public:
		FMIgnoreFile() = default;
		~FMIgnoreFile() = default;

        //! @brief Parse ignore rules from text file (e.g. ".otignore").
        bool parseFromFile(const std::filesystem::path& _filePath);

        //! @brief Parse ignore rules from a string (each line = one rule).
        bool parseFromText(const std::string& _text);

        //! @brief Returns true if the given path is ignored by any rule.
        bool isIgnored(const std::filesystem::path& _path) const;

        //! @brief Clear all rules.
        void clear();

    private:
        std::list<std::regex> m_patterns;

        // ###########################################################################################################################################################################################################################################################################################################################

		// Private: Helper

        static std::string normalizePath(const std::filesystem::path& _path);
        static std::string globToRegex(const std::string& _pattern);
	};
}