// @otlicense

#pragma once

// OpenTwin header
#include "OTSystem/FileSystem/TemporaryFile.h"
#include "OTCore/CoreTypes.h"

// std header
#include <list>
#include <optional>

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
		TemporaryFile m_file;
		NewModelStateInfo m_newEntities;
		const NewModelStateInfo& getNewEntities() const { return m_newEntities; };

		static void log(const ot::StyledTextBuilder& _message);
		static void log(const std::string& _message);
		static void logWarning(const std::string& _message);
		static void logError(const std::string& _message);

		MDF4Parser() = delete;
		MDF4Parser(TemporaryFile&& _file, const std::list<std::string>& _existingMDFFileEntities);
		bool parse();

		static void initializeLogging();

	};
}