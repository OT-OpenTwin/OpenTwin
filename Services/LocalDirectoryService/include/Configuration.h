#pragma once

// Project header
#include "ServiceInformation.h"

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTCore/Serializable.h"

// C++ header
#include <string>
#include <list>

//! Short form to get the configuration instance
#define LDS_CFG Configuration::instance()

class Configuration : public ot::Serializable {
public:
	static Configuration& instance(void);

	//! @brief Add the object contents to the provided JSON object
	//! @param _object Json object reference
	//! @param _allocator Allocator
	virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

	//! @brief Will set the object contents from the provided JSON object
	//! @param _object The JSON object containing the information
	//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
	virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

	//! @brief Will load the confiuguration from the environment
	//! Returns an exit code (use LDS_EXIT_... macros to determine error: ExitCodes.h)
	//! LDS_EXIT_Ok will be returned if no error occured
	int importFromEnvironment(void);

	//! @brief Returns true if the provided service is supported by this LDS
	bool supportsService(const std::string& _serviceName) const;

	void setSupportedServices(const std::list<ServiceInformation>& _services) { m_supportedServices = _services; };
	const std::list<ServiceInformation>& supportedServices(void) const { return m_supportedServices; };

	void setLauncherPath(const std::string& _launcherPath) { m_launcherPath = _launcherPath; };
	const std::string launcherPath(void) const { return m_launcherPath; };

	void setServicesLibraryPath(const std::string& _path) { m_servicesLibraryPath = _path; };
	const std::string& servicesLibraryPath(void) const { return m_servicesLibraryPath; };

	void setDefaultMaxCrashRestarts(unsigned int _restarts) { m_defaultMaxCrashRestarts = _restarts; };
	unsigned int defaultMaxCrashRestarts(void) const { return m_defaultMaxCrashRestarts; };

	void setDefaultMaxStartupRestarts(unsigned int _restarts) { m_defaultMaxStartupRestarts = _restarts; };
	unsigned int defaultMaxStartupRestarts(void) const { return m_defaultMaxStartupRestarts; };

private:
	bool m_configurationImported;
	std::list<ServiceInformation> m_supportedServices;
	std::string m_launcherPath;
	std::string m_servicesLibraryPath;
	unsigned int m_defaultMaxCrashRestarts;
	unsigned int m_defaultMaxStartupRestarts;

	Configuration();
	virtual ~Configuration();
	Configuration(Configuration&) = delete;
	Configuration& operator = (Configuration&) = delete;
};