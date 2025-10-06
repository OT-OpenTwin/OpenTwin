//! @file Configuration.h
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// LDS header
#include "SupportedService.h"

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTCore/OTClassHelper.h"
#include "OTCommunication/LDSDebugInfo.h"

// std header
#include <list>
#include <string>
#include <optional>

//! @brief The Configuration holds the configuration for the Local Directory Service.
class Configuration : public ot::Serializable {
public:
	static Configuration& instance();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Serialization

	void getDebugInformation(ot::LDSDebugInfo& _info);

	//! @brief Add the object contents to the provided JSON object
	//! @param _object Json object reference
	//! @param _allocator Allocator
	virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

	//! @brief Will set the object contents from the provided JSON object
	//! @param _object The JSON object containing the information
	//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
	virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Management

	//! @brief Will load the confiuguration from the environment.
	//! Will exit on error.
	void importFromEnvironment();

	//! @brief Returns the supported service information for the provided service name.
	std::optional<SupportedService> getSupportedService(const std::string& _serviceName) const;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter/Getter

	void setSupportedServices(const std::list<SupportedService>& _services) { m_supportedServices = _services; };
	const std::list<SupportedService>& getSupportedServices() const { return m_supportedServices; };

	void setLauncherPath(const std::string& _launcherPath) { m_launcherPath = _launcherPath; };
	const std::string getLauncherPath() const { return m_launcherPath; };

	void setServicesLibraryPath(const std::string& _path) { m_servicesLibraryPath = _path; };
	const std::string& getServicesLibraryPath() const { return m_servicesLibraryPath; };

	void setDefaultMaxCrashRestarts(unsigned int _restarts) { m_defaultMaxCrashRestarts = _restarts; };
	unsigned int getDefaultMaxCrashRestarts() const { return m_defaultMaxCrashRestarts; };

	void setDefaultMaxStartupRestarts(unsigned int _restarts) { m_defaultMaxStartupRestarts = _restarts; };
	unsigned int getDefaultMaxStartupRestarts() const { return m_defaultMaxStartupRestarts; };

private:
	bool m_configurationImported;
	std::list<SupportedService> m_supportedServices; //! @brief Name Type pairs of supported services
	std::string m_launcherPath;
	std::string m_servicesLibraryPath;
	unsigned int m_defaultMaxCrashRestarts;
	unsigned int m_defaultMaxStartupRestarts;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Constructor/Destructor

	Configuration();
	virtual ~Configuration();
};