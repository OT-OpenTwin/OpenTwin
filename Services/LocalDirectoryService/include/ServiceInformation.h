#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTCore/Serializable.h"

// C++ header
#include <string>

class ServiceInformation : public ot::Serializable {
public:
	OT_PROPERTY_REF(std::string, name, setName, name);
	OT_PROPERTY_REF(std::string, type, setType, type);
	OT_PROPERTY(unsigned int, maxCrashRestarts, setMaxCrashRestarts, maxCrashRestarts);
	OT_PROPERTY(unsigned int, maxStartupRestarts, setMaxStartupRestarts, maxStartupRestarts);

	ServiceInformation();
	ServiceInformation(const std::string& _name, const std::string& _type);
	ServiceInformation(const ServiceInformation& _other);
	virtual ~ServiceInformation();

	ServiceInformation& operator = (const ServiceInformation& _other);
	bool operator == (const ServiceInformation& _other) const;
	bool operator != (const ServiceInformation& _other) const;

	//! @brief Add the object contents to the provided JSON object
	//! @param _object Json object reference
	//! @param _allocator Allocator
	virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

	//! @brief Will set the object contents from the provided JSON object
	//! @param _object The JSON object containing the information
	//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
	virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

};