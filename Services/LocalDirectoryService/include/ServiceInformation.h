#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTCore/Serializable.h"

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
	//! @param _object Json object reference
	//! @param _allocator Allocator
	virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

	//! @brief Will set the object contents from the provided JSON object
	//! @param _object The JSON object containing the information
	//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
	virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

	void setName(const std::string& _name) { m_name = _name; };
	const std::string& name(void) const { return m_name; };

	void setType(const std::string& _type) { m_type = _type; };
	const std::string type(void) const { return m_type; };

	void setMaxCrashRestarts(unsigned int _restarts) { m_maxCrashRestarts = _restarts; };
	unsigned int maxCrashRestarts(void) const { return m_maxCrashRestarts; };

	void setMaxStartupRestarts(unsigned int _restarts) { m_maxStartupRestarts = _restarts; };
	unsigned int maxStartupRestarts(void) const { return m_maxStartupRestarts; };

private:
	std::string m_name;
	std::string m_type;
	unsigned int m_maxCrashRestarts;
	unsigned int m_maxStartupRestarts;
};