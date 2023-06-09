#pragma once

// OpenTwin header
#include "OpenTwinCore/OTClassHelper.h"
#include "OpenTwinCore/Serializable.h"

// C++ header
#include <string>

class ServiceInformation : public ot::Serializable {
public:
	ServiceInformation();
	ServiceInformation(const std::string& _name, const std::string& _type);
	ServiceInformation(const ServiceInformation& _other);
	virtual ~ServiceInformation();

	ServiceInformation& operator = (const ServiceInformation& _other);
	bool operator == (const ServiceInformation& _other) const;
	bool operator != (const ServiceInformation& _other) const;

	//! @brief Add the object contents to the provided JSON object
	//! @param _document The JSON document (used to get the allocator)
	//! @param _object The JSON object to add the contents to
	virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

	//! @brief Will set the object contents from the provided JSON object
	//! @param _object The JSON object containing the information
	//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
	virtual void setFromJsonObject(OT_rJSON_val& _object) override;

	OT_PROPERTY_GET(std::string, Name);
	OT_PROPERTY_GET(std::string, Type);
	OT_PROPERTY_GET(unsigned int, MaxCrashRestarts);
	OT_PROPERTY_GET(unsigned int, MaxStartupRestarts);
};