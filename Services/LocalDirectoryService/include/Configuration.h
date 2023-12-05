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

	OT_PROPERTY_REF(std::list<ServiceInformation>, supportedServices, setSupportedService, supportedServices);
	OT_PROPERTY_REF(std::string, launcherPath, setLauncherPath, launcherPath);
	OT_PROPERTY_REF(std::string, servicesLibraryPath, setServicesLibraryPath, servicesLibraryPath);
	OT_PROPERTY(unsigned int, defaultMaxCrashRestarts, setDefaultMaxCrashRestarts, defaultMaxCrashRestarts);
	OT_PROPERTY(unsigned int, defaultMaxStartupRestarts, setDefaultMaxStartupRestarts, defaultMaxStartupRestarts);

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

private:
	bool m_configurationImported;

	Configuration();
	virtual ~Configuration();
	Configuration(Configuration&) = delete;
	Configuration& operator = (Configuration&) = delete;
};