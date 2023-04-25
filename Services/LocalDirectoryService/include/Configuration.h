#pragma once

// Project header
#include "ServiceInformation.h"

// OpenTwin header
#include "OpenTwinCore/OTClassHelper.h"
#include "OpenTwinCore/Serializable.h"

// C++ header
#include <string>
#include <list>

//! Short form to get the configuration instance
#define LDS_CFG Configuration::instance()

class Configuration : public ot::Serializable {
public:
	static Configuration& instance(void);

	//! @brief Add the object contents to the provided JSON object
	//! @param _document The JSON document (used to get the allocator)
	//! @param _object The JSON object to add the contents to
	virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

	//! @brief Will set the object contents from the provided JSON object
	//! @param _object The JSON object containing the information
	//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
	virtual void setFromJsonObject(OT_rJSON_val& _object) override;

	//! @brief Will load the confiuguration from the environment
	//! Returns an exit code (use LDS_EXIT_... macros to determine error: ExitCodes.h)
	//! LDS_EXIT_Ok will be returned if no error occured
	int importFromEnvironment(void);

	OT_PROPERTY_GET(std::list<ServiceInformation>, SupportedServices);
	OT_PROPERTY_GET(std::string, LauncherPath);
	OT_PROPERTY_GET(std::string, ServicesLibraryPath);
	OT_PROPERTY_GET(unsigned int, DefaultMaxCrashRestarts);
	OT_PROPERTY_GET(unsigned int, DefaultMaxStartupRestarts);

	//! @brief Returns true if the provided service is supported by this LDS
	bool supportsService(const std::string& _serviceName) const;

private:
	bool		m_configurationImported;

	Configuration();
	virtual ~Configuration();
	Configuration(Configuration&) = delete;
	Configuration& operator = (Configuration&) = delete;
};