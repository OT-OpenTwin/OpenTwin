// @otlicense
// File: ThisComputerInfo.h
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
#include "OTCore/SystemServiceConfigInfo.h"

// std header
#include <string>

namespace ot {

	//! @class ThisComputerInfo
	//! @brief ThisComputerInfo is used to gather all OpenTwin related information on the current machine.
	class OT_CORE_API_EXPORT ThisComputerInfo {
		OT_DECL_NOCOPY(ThisComputerInfo)
		OT_DECL_NOMOVE(ThisComputerInfo)
	public:

		//! @brief Information gathering mode.
		enum InformationModeFlag {
			NoRunFlags               = 0 << 0, //! @brief Default. No information gathered.
			Environment              = 1 << 0, //! @brief Environment information will be read.
			MongoDBConfig            = 1 << 1, //! @brief Mongo DB configuration will be read.
			MongoDBConfigFileContent = 1 << 2, //! @brief Mongo DB file content wil be read. Requires InformationModeFlag::MongoDBConfig to be set.

			GatherAllMode            = Environment | MongoDBConfig | MongoDBConfigFileContent
		};
		typedef Flags<InformationModeFlag> InformationModeFlags;

		//! @brief Environment entry.
		enum EnvironemntEntry {
			EnvAdminPort,                     //! @brief Admin Panel port.
			EnvAuthPort,                      //! @brief Authorization Service (Auth) port.
			EnvCertPath,                      //! @brief Certificate path.
			EnvDownloadPort,                  //! @brief OpenTwin Download port.
			EnvGssPort,                       //! @brief Global Session Service (GSS) port.
			EnvLssPort,                       //! @brief Local Session Service (LSS) port.
			EnvGdsPort,                       //! @brief Global Directory Service (GDS) port.
			EnvLdsPort,                       //! @brief Local Directory Service (LDS) port.
			EnvMessageTimeout,                //! @brief Global message send timeout.
			EnvMongoDBAddress,                //! @brief Mongo DB Address.
			EnvMongoServicesAddress           //! @brief Mongo DB Services Address.
		};

		//! @brief Environment data.
		struct EnvData {
			bool dataSet;                     //! @brief True if data is set (i.e. information was gathered).
			std::string adminPort;            //! @brief Admin Panel port.
			std::string authorizationPort;    //! @brief Authorization Service (Auth) port.
			std::string certificatePath;      //! @brief Certificate path.
			std::string downloadPort;         //! @brief OpenTwin Download port.
			std::string gssPort;              //! @brief Global Session Service (GSS) port.
			std::string lssPort;              //! @brief Local Session Service (LSS) port.
			std::string gdsPort;              //! @brief Global Directory Service (GDS) port.
			std::string ldsPort;              //! @brief Local Directory Service (LDS) port.
			std::string globalMessageTimeout; //! @brief Globally used message send timeout.
			std::string mongoDBAddress;       //! @brief Mongo DB Address.
			std::string mongoServicesAddress; //! @brief Mongo DB Services Address.
		};

		//! @brief Mongo DB data.
		struct MongoData {
			bool dataSet;                     //! @brief True if data is set (i.e. information was gathered).
			SystemServiceConfigInfo info;     //! @brief Mongo DB Service information.
			std::string configFileContent;    //! @brief Mongo DB configuration file content.
		};

		//! @brief Will gather and log the information according to the mode flags provided.
		//! @param _mode Information to gather and log.
		static std::string toInfoString(const InformationModeFlags& _mode);

		//! @brief Reads and returns a single environment entry
		//! @param _entry 
		//! @return 
		static std::string getEnvEntry(EnvironemntEntry _entry);

		//! @brief Constructor.
		//! Will call gather information with the mode provided.
		//! @param _mode Gather mode.
		ThisComputerInfo(const InformationModeFlags& _mode = InformationModeFlag::NoRunFlags);

		//! @brief Fills the current data according to the mode provided.
		//! @param _mode Information gather mode.
		//! @return Reference to this object.
		ThisComputerInfo& gatherInformation(const InformationModeFlags& _mode);

		//! @brief Logs the currently set data.
		//! Will only log information of data that was set previously.
		//! @param _mode Information gather mode.
		//! @return Reference to this object.
		std::string toInfoString(void);

		//! @brief Currently set environment data.
		inline const EnvData& getEnvData(void) const { return m_envData; };

		//! @brief Currently set mongo data.
		inline const MongoData getMongoData(void) const { return m_mongoData; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private

	private:
		EnvData m_envData; //! @brief Environment data.
		MongoData m_mongoData; //! @brief Mongo data.

		//! @brief Gather environment information.
		void gatherEnvData(void);

		//! @brief Gather mongo database configuration. 
		bool gatherMongoDBData(void);

		//! @brief Read the mongo configuration file and store its contents.
		//! Should only be called if ThisComputerInformation::gatherMongoDBData completed successfully.
		void gatherMongoDBConfigFileContent(void);
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::ThisComputerInfo::InformationModeFlag, ot::ThisComputerInfo::InformationModeFlags)