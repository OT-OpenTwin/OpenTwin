//! @file Configuration.h
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// LDS header
#include "ServiceInformation.h"

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTCore/OTClassHelper.h"

// std header
#include <list>
#include <string>

//! @brief The Configuration holds the configuration for the Local Directory Service.
class Configuration : public ot::Serializable {
public:
	static Configuration& instance(void);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Serialization

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
	void importFromEnvironment(void);

	//! @brief Returns true if the provided service is supported by this LDS
	bool supportsService(const std::string& _serviceName) const;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter/Getter

	void setSupportedServices(const std::list<ServiceInformation>& _services) { m_supportedServices = _services; };
	const std::list<ServiceInformation>& getSupportedServices(void) const { return m_supportedServices; };

	void setLauncherPath(const std::string& _launcherPath) { m_launcherPath = _launcherPath; };
	const std::string getLauncherPath(void) const { return m_launcherPath; };

	void setServicesLibraryPath(const std::string& _path) { m_servicesLibraryPath = _path; };
	const std::string& getServicesLibraryPath(void) const { return m_servicesLibraryPath; };

	void setDefaultMaxCrashRestarts(unsigned int _restarts) { m_defaultMaxCrashRestarts = _restarts; };
	unsigned int getDefaultMaxCrashRestarts(void) const { return m_defaultMaxCrashRestarts; };

	void setDefaultMaxStartupRestarts(unsigned int _restarts) { m_defaultMaxStartupRestarts = _restarts; };
	unsigned int getDefaultMaxStartupRestarts(void) const { return m_defaultMaxStartupRestarts; };

private:
	bool m_configurationImported;
	std::list<ServiceInformation> m_supportedServices;
	std::string m_launcherPath;
	std::string m_servicesLibraryPath;
	unsigned int m_defaultMaxCrashRestarts;
	unsigned int m_defaultMaxStartupRestarts;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Constructor/Destructor

	Configuration();
	virtual ~Configuration();
};