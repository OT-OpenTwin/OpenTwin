// @otlicense
// File: SequenceDiaCfgParser.h
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
#include "OTSystem/Flags.h"
#include "OTSystem/IgnoreRules.h"
#include "OTGui/Diagram/SequenceDiaRefCfg.h"
#include "OTGui/Diagram/SequenceDiaFunctionCfg.h"

// std header
#include <map>
#include <list>

namespace ot {

	class OT_GUI_API_EXPORT SequenceDiaCfgParser : public Serializable {
		OT_DECL_NOCOPY(SequenceDiaCfgParser)
		OT_DECL_DEFMOVE(SequenceDiaCfgParser)
	public:
		enum ParseFlag {
			NoFlags   = 0 << 0,
			NoLogging = 1 << 0
		};
		typedef Flags<ParseFlag> ParseFlags;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Constructor / Destructor

		SequenceDiaCfgParser() = default;
		virtual ~SequenceDiaCfgParser() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Serialization

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void clear();

		void parseDirectory(const std::filesystem::path& _rootDirectoryPath, bool _topLevelOnly = true, const std::list<std::string>& _fileExtensionWhitelist = std::list<std::string>(), const IgnoreRules& _ignoreRules = IgnoreRules(), const ParseFlags& _parseFlags = NoFlags);

		size_t getFilesParsed() const { return m_filesParsed; };
		size_t getInvalidFunctions() const { return m_invalidFunctions; };
		size_t getInvalidDiagrams() const { return m_invalidDiagrams; };

		int64_t getTotalRuntime() const { return m_totalRuntime; };
		int64_t getParseTime() const { return m_parseTime; };
		int64_t getValidationTime() const { return m_validationTime; };

		const std::map<std::string, SequenceDiaFunctionCfg>& getSequenceFunctions() const { return m_sequenceFunctions; };
		const std::map<std::string, SequenceDiaRefCfg>& getSequenceDiagrams() const { return m_sequenceDiagrams; };

	private:
		enum class ValidationState : uint8_t {
			Unvisited,
			Visiting,
			Valid,
			Invalid
		};

		struct ValidationInfo {
			ValidationInfo() = delete;
			ValidationInfo(ValidationInfo&) = delete;
			ValidationInfo(ValidationInfo&&) noexcept = delete;
			ValidationInfo& operator=(ValidationInfo&) = delete;
			ValidationInfo& operator=(ValidationInfo&&) noexcept = delete;
			ValidationInfo(size_t _functionCount)
				: graph(_functionCount, {}), state(_functionCount, ValidationState::Unvisited)
			{
				nameToIndex.reserve(_functionCount);
				functions.reserve(_functionCount);
			};
			~ValidationInfo() = default;

			std::unordered_map<std::string, size_t> nameToIndex;
			std::vector<SequenceDiaFunctionCfg*> functions;
			std::vector<std::vector<size_t>> graph;
			std::vector<ValidationState> state;
		};

		// ###########################################################################################################################################################################################################################################################################################################################

		// Helper functions

		std::string lineStr(const std::string& _filePath, size_t _lineNr) const;

		std::list<std::pair<std::string, std::string>> getKeyValuePairs(const std::filesystem::path& _filePath, size_t _lineNr, const std::string& _trimmedContent) const;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Parsing functions

		void parseFile(const std::filesystem::path& _filePath);

		void finalizeFunction(const std::filesystem::path& _filePath, size_t _lineNr, SequenceDiaFunctionCfg&& _function);
		
		void parseFunctionLine(const std::filesystem::path& _filePath, size_t _lineNr, const std::string& _trimmedContent, SequenceDiaFunctionCfg& _function);

		void parseDiagramLine(const std::filesystem::path& _filePath, size_t _lineNr, const std::string& _trimmedContent);

		void parseFunctionCallLine(const std::filesystem::path& _filePath, size_t _lineNr, const std::string& _trimmedContent, SequenceDiaFunctionCfg& _function);

		void parseSelfCallLine(const std::filesystem::path& _filePath, size_t _lineNr, const std::string& _trimmedContent, SequenceDiaFunctionCfg& _function);

		void parseReturnLine(const std::filesystem::path& _filePath, size_t _lineNr, const std::string& _trimmedContent, SequenceDiaFunctionCfg& _function);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Validation

		void validate();
		void validateFunctions();
		inline bool validateNode(ValidationInfo& _info, size_t _index);
		void validateDiagrams();

		ParseFlags m_parseFlags = NoFlags;
		size_t m_filesParsed = 0;
		size_t m_invalidFunctions = 0;
		size_t m_invalidDiagrams = 0;

		int64_t m_totalRuntime = 0;
		int64_t m_parseTime = 0;
		int64_t m_validationTime = 0;

		std::map<std::string, SequenceDiaFunctionCfg> m_sequenceFunctions;
		std::map<std::string, SequenceDiaRefCfg> m_sequenceDiagrams;

	};

}

OT_ADD_FLAG_FUNCTIONS(ot::SequenceDiaCfgParser::ParseFlag, ot::SequenceDiaCfgParser::ParseFlags)