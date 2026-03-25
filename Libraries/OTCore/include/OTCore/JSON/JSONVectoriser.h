//@otlicense
#pragma once
#include "OTCore/JSON/JSON.h"
#include <string>
#include <list>
#include "OTCore/CoreAPIExport.h"

class OT_CORE_API_EXPORT JSONVectoriser
{
public:
	static void vectorise(const ot::JsonValue& _value, std::list<std::string>& _allEntries, const std::string& _nameBase);
	static const ot::JsonValue& getValue(const ot::JsonDocument& _structure, const std::string& _fieldName);

private:
	static const std::string m_separator;
};
