// @otlicense

//! @file SupportedService.h
//! @author Alexander Kuester (alexk95)
//! @date September 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/OTClassHelper.h"

// std header
#include <string>

class SupportedService {
	OT_DECL_DEFCOPY(SupportedService)
	OT_DECL_DEFMOVE(SupportedService)
public:
	SupportedService();
	SupportedService(const std::string& _name, const std::string& _type);
	~SupportedService() {};

	void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const;

	void setName(const std::string& _name) { m_name = _name; };
	const std::string& getName() const { return m_name; };

	void setType(const std::string& _type) { m_type = _type; };
	const std::string& getType() const { return m_type; };

	void setMaxCrashRestarts(unsigned int _restarts) { m_maxCrashRestarts = _restarts; };
	unsigned int getMaxCrashRestarts() const { return m_maxCrashRestarts; };

	void setMaxStartupRestarts(unsigned int _restarts) { m_maxStartupRestarts = _restarts; };
	unsigned int getMaxStartupRestarts() const { return m_maxStartupRestarts; };

private:
	std::string  m_name;
	std::string  m_type;
	unsigned int m_maxCrashRestarts;
	unsigned int m_maxStartupRestarts;
};