// @otlicense
// File: SystemServiceConfigInfo.h
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
#include "OTCore/Serializable.h"

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {

	//! @class SystemServiceConfigInfo
	//! @brief Manages the configuration details of a system service.
	class OT_CORE_API_EXPORT SystemServiceConfigInfo : public Serializable {
	public:
		//! @brief Default constructor.
		SystemServiceConfigInfo();

		//! @brief Constructor that initializes the service name.
		//! @param _serviceName The name of the service.
		SystemServiceConfigInfo(const char* _serviceName);

		//! @brief Add the object contents to the provided JSON object.
		//! @param _object Json object reference to write the data to.
		//! @param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		//! @brief Loads the system service configuration.
		//! @param _serviceName The name of the service to load the configuration for.
		//! @return True if the configuration was loaded successfully, false otherwise.
		bool loadSystemServiceConfig(const char* _serviceName);

		//! @brief Set the service name.
		//! @param _serviceName The service name to set.
		//! @return Reference to the updated service name.
		//! @ref getServiceName
		void setServiceName(const std::string& _serviceName) { m_serviceName = _serviceName; };

		//! @brief Get the service name.
		//! @return The name of the service.
		const std::string& getServiceName(void) const { return m_serviceName; };

		//! @brief Set the display name of the service.
		//! @param _displayName The display name to set.
		void setDisplayName(const std::string& _displayName) { m_displayName = _displayName; };

		//! @brief Get the display name of the service.
		//! @return The display name of the service.
		const std::string& getDisplayName(void) const { return m_displayName; };

		//! @brief Set the binary path of the service.
		//! @param _binaryPath The binary path to set.
		void setBinaryPath(const std::string& _binaryPath) { m_binaryPath = _binaryPath; };

		//! @brief Get the binary path of the service.
		//! @return The binary path of the service.
		const std::string& getBinaryPath(void) const { return m_binaryPath; };

		//! @brief Set the configuration path of the service.
		//! @param _configPath The configuration path to set.
		void setConfigPath(const std::string& _configPath) { m_configPath = _configPath; };

		//! @brief Get the configuration path of the service.
		//! @return The configuration path of the service.
		const std::string& getConfigPath(void) const { return m_configPath; };

		//! @brief Set the service path.
		//! @param _servicePath The service path to set.
		void setServicePath(const std::string& _servicePath) { m_servicePath = _servicePath; };

		//! @brief Get the service path.
		//! @return The service path.
		const std::string& getServicePath(void) const { return m_servicePath; };

		//! @brief Set the start type of the service.
		//! @param _type The start type to set.
		void setStartType(const std::string& _type) { m_startType = _type; };

		//! @brief Get the start type of the service.
		//! @return The start type of the service.
		const std::string& getStartType(void) const { return m_startType; };

	private:
		std::string m_serviceName; //! @brief The name of the service.
		std::string m_displayName; //! @brief The display name of the service.
		std::string m_binaryPath;  //! @brief The binary path of the service.
		std::string m_configPath;  //! @brief The configuration path of the service.
		std::string m_servicePath; //! @brief The service path.
		std::string m_startType;   //! @brief The start type of the service.

	};


} // namespace ot
