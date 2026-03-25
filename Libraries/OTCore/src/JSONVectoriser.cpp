#include "OTCore/JSON/JSONVectoriser.h"

#include "OTCore/String.h"

const std::string JSONVectoriser::m_separator = "~";

void JSONVectoriser::vectorise(const ot::JsonValue& _value, std::list<std::string>& _allEntries, const std::string& _nameBase)
{
    if (_value.IsObject())
    {
        std::string separator = _nameBase.empty() ? "" : m_separator;
        for (auto& element : _value.GetObject())
        {
            std::string name = _nameBase + separator + element.name.GetString();
            ot::String::removeControlCharacters(name);
            _allEntries.push_back(name);
            if (element.value.IsObject())
            {
                vectorise(element.value, _allEntries, name);
            }
        }
    }
}

const ot::JsonValue& JSONVectoriser::getValue(const ot::JsonDocument& _structure, const std::string& _fieldName)
{
    std::list<std::string> entries = ot::String::split(_fieldName, m_separator);

    const ot::JsonValue* cur = &_structure;
    for (const std::string& entry : entries)
    {
        if (!cur->IsObject())
        {
            throw std::exception("Failed to access selected metadata.");
        }

        auto obj = cur->GetObject();
        if (!ot::json::exists(obj, entry))
        {
            throw std::exception("Failed to find selected metadata.");
        }

        const ot::JsonValue& value = obj[entry.c_str()];

        if (entry == entries.back()) {
            return value;
        }

        cur = &value;
    }

    throw std::exception("Failed to access selected metadata.");
}