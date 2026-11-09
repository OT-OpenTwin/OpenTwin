// @otlicense
// File: MDF4Parser.h
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
#include "OTSystem/FileSystem/TemporaryFile.h"
#include "OTCore/CoreTypes.h"
#include "OTModelEntities/MDF/EntityMDFFile.h"

// std header
#include <list>
#include <optional>

namespace mdf
{
	class MdfReader;
	class IDataGroup;
}

namespace ot
{

	class MDF4Parser
	{
		OT_DECL_NOCOPY(MDF4Parser)
	public:
		struct MDFParserResult
		{
			NewModelStateInfo newEntities;
		};

		//! @brief Parses the given MDF4 file and returns an instance of MDF4Parser containing the parsed data.
		//! @param _file The MDF4 file to parse. If the file was not read before, it will be read during parsing.
		static NewModelStateInfo parse(ot::TemporaryFile&& _file, const std::list<std::string>& _existingMDFFileEntities);

	private:
		std::list<std::string> m_existingMDFFileEntities;
		std::list<std::string> m_existingFileChildEntities;

		TemporaryFile m_rawFile;

		NewModelStateInfo m_newEntities;
		const NewModelStateInfo& getNewEntities() const { return m_newEntities; };

		std::unique_ptr<EntityMDFFile> m_fileEntity;

		static void log(const ot::StyledTextBuilder& _message);
		static void log(const std::string& _message);
		static void logWarning(const std::string& _message);
		static void logError(const std::string& _message);

		MDF4Parser() = delete;
		MDF4Parser(TemporaryFile&& _file, const std::list<std::string>& _existingMDFFileEntities);
		bool parse();
		bool parse(mdf::MdfReader& _reader, mdf::IDataGroup* _dataGroup);

		void initialize();
		static void initializeLogging();

	};
}