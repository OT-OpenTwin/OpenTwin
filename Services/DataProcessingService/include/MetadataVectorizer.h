//@otlicense
#pragma once
#include "OTCore/JSON/JSON.h"
#include <string>
#include <list>

class MetadataVectorizer
{
public:
	static void vectorize(const ot::JsonValue& _value, std::list<std::string>& _allEntries, const std::string& _nameBase);
	static const ot::JsonValue& getValue(const ot::JsonDocument& _structure, const std::string& _fieldName);

private:
	static const std::string m_separator;
};
