// @otlicense

// OpenTwin header
#include "OTCore/String.h"
#include "OTCore/JSON/JSONVectoriser.h"

const std::string ot::JSONVectoriser::m_separator = "~";

void ot::JSONVectoriser::vectorise(const JsonValue& _value, std::list<std::string>& _allEntries, const std::string& _nameBase, const VectorisationFlags& _flags)
{
    if (_value.IsObject())
    {
        std::string separator = _nameBase.empty() ? "" : m_separator;
        for (auto& element : _value.GetObject())
        {
            std::string name = _nameBase + separator + element.name.GetString();
            ot::String::removeControlCharacters(name);

            if (element.value.IsObject())
            {
                if (!_flags.has(VectorisationFlag::AddLeavesOnly))
                {
                    _allEntries.push_back(name);
                }

                if (!_flags.has(VectorisationFlag::TopLevelOnly))
                {
                    vectorise(element.value, _allEntries, name, _flags);
                }
            }
            else
            {
				_allEntries.push_back(name);
            }
        }
    }
}

const ot::JsonValue& ot::JSONVectoriser::getValue(const JsonDocument& _structure, const std::string& _fieldName)
{
    std::list<std::string> entries = String::split(_fieldName, m_separator);

    const ot::JsonValue* cur = &_structure;
    for (const std::string& entry : entries)
    {
        if (!cur->IsObject())
        {
            throw std::exception("Failed to access selected metadata.");
        }

        auto obj = cur->GetObject();
        if (!json::exists(obj, entry))
        {
            throw std::exception("Failed to find selected metadata.");
        }

        const JsonValue& value = obj[entry.c_str()];

        if (entry == entries.back()) {
            return value;
        }

        cur = &value;
    }

    throw std::exception("Failed to access selected metadata.");
}