//! @file OTEnvInfo.h
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTCore/CoreAPIExport.h"

// std header
#include <string>

namespace ot {

	class OT_CORE_API_EXPORT OTEnvInfo {
	public:
		enum InformationModeFlag {
			NoRunFlags = 0 << 0,
			Environment = 1 << 0,
			MongoDBConfig = 1 << 1
		};
		typedef Flags<InformationModeFlag> InformationModeFlags;

		OTEnvInfo();

		void gatherInformation(const InformationModeFlags& _mode);

		void logCurrentInformation(void);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private

	private:
		struct EnvData {
			bool dataSet;
			std::string adminPort;
		};
		EnvData m_envData;

		void resetData(void);

		void gatherEnvData(void);
		void gatherMongoDBData(void);
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::OTEnvInfo::InformationModeFlag)