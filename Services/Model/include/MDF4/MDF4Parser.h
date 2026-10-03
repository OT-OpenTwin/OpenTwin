// @otlicense

#pragma once

// Model service header
#include "MDF4/MDF4Dataset.h"

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
		//! @brief Parses the given MDF4 file and returns an instance of MDF4Parser containing the parsed data.
		//! @param _file The MDF4 file to parse. If the file was not read before, it will be read during parsing.
		static MDF4Parser parse(ot::TemporaryFile&& _file);

		MDF4Parser(MDF4Parser&& _other) noexcept = default;
		MDF4Parser& operator=(MDF4Parser&& _other) noexcept = default;

		std::list<MDF4Dataset>&& getDatasets() { return std::move(m_datasets); };

	private:
		std::optional<TemporaryFile> m_file;
		std::list<MDF4Dataset> m_datasets;

		static void log(const ot::StyledTextBuilder& _message);
		static void log(const std::string& _message);
		static void logWarning(const std::string& _message);
		static void logError(const std::string& _message);

		MDF4Parser();
		MDF4Parser(TemporaryFile&& _file);
		void parse();

		static void initializeLogging();

	};
}