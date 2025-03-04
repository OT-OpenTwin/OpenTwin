//! @file OTEnvInfo.h
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTCore/CoreAPIExport.h"
#include "OTCore/SystemServiceConfigInfo.h"

// std header
#include <string>

namespace ot {

	class OT_CORE_API_EXPORT OTEnvInfo {
	public:
		enum InformationModeFlag {
			NoRunFlags               = 0 << 0,
			Environment              = 1 << 0,
			MongoDBConfig            = 1 << 1,
			MongoDBConfigFileContent = 1 << 2
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
			std::string authorizationPort;
			std::string certificatePath;
			std::string downloadPort;
			std::string gssPort;
			std::string lssPort;
			std::string gdsPort;
			std::string ldsPort;
			std::string mongoDBAdress;
			std::string mongoServicesAddress;
		};
		EnvData m_envData;

		struct MongoData {
			bool dataSet;
			SystemServiceConfigInfo info;
			std::string configFileContent;
		};
		MongoData m_mongoData;

		void resetData(void);

		void gatherEnvData(void);
		bool gatherMongoDBData(void);
		void gatherMongoDBConfigFileContent(void);
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::OTEnvInfo::InformationModeFlag)