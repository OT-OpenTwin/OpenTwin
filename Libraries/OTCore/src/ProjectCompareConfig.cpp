// @otlicense

// OpenTwin header
#include "OTCore/ProjectCompareConfig.h"
#include "OTCore/Logging/Logger.h"

std::string ot::ProjectCompareConfig::toString(ProjectCompareConfig::ProjectCompareFlag _flag)
{
	switch (_flag)
	{
	case ProjectCompareFlag::NoFlags: return "";
	case ProjectCompareFlag::CompareProperties: return "CompareProperties";
	case ProjectCompareFlag::CompareData: return "CompareData";
	case ProjectCompareFlag::CompareResults: return "CompareResults";
	default:
		OT_LOG_E("Unknown ProjectCompareConfig::ProjectCompareFlag value: " + std::to_string(static_cast<int>(_flag)));
		return "";
	}
}

ot::ProjectCompareConfig::ProjectCompareFlag ot::ProjectCompareConfig::stringToProjectCompareFlag(const std::string& _flagString)
{
	if (_flagString == toString(ProjectCompareFlag::CompareProperties)) return ProjectCompareFlag::CompareProperties;
	else if (_flagString == toString(ProjectCompareFlag::CompareData)) return ProjectCompareFlag::CompareData;
	else if (_flagString == toString(ProjectCompareFlag::CompareResults)) return ProjectCompareFlag::CompareResults;
	else
	{
		OT_LOG_E("Unknown ProjectCompareConfig flag string \"" + _flagString + "\"");
		return ProjectCompareFlag::NoFlags;
	}
}

std::list<std::string> ot::ProjectCompareConfig::toStringList(const ProjectCompareConfig::ProjectCompareFlags& _flags)
{
	std::list<std::string> result;
	
	for (ProjectCompareFlags flag = ProjectCompareFlag::Iterator_First; flag.toEnum() <= ProjectCompareFlag::Iterator_Last; flag <<= 1)
	{
		if (_flags.has(flag))
		{
			result.push_back(toString(flag.toEnum()));
		}
	}

	return result;
}

ot::ProjectCompareConfig::ProjectCompareFlags ot::ProjectCompareConfig::stringListToProjectCompareFlags(const std::list<std::string>& _flagStrings)
{
	ProjectCompareConfig::ProjectCompareFlags flags = ProjectCompareFlag::NoFlags;
	for (const std::string& flagString : _flagStrings)
	{
		flags |= stringToProjectCompareFlag(flagString);
	}
	return flags;
}

ot::ProjectCompareConfig::ProjectCompareConfig(const ConstJsonObject& _jsonObject)
	: ProjectCompareConfig()
{
	setFromJsonObject(_jsonObject);
}

void ot::ProjectCompareConfig::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const
{
	_jsonObject.AddMember("TargetProjectName", JsonString(m_targetProjectName, _allocator), _allocator);
	_jsonObject.AddMember("TargetProjectVersion", JsonString(m_targetProjectVersion, _allocator), _allocator);
	_jsonObject.AddMember("Flags", JsonArray(toStringList(m_flags), _allocator), _allocator);
}

void ot::ProjectCompareConfig::setFromJsonObject(const ConstJsonObject& _jsonObject)
{
	m_targetProjectName = json::getString(_jsonObject, "TargetProjectName");
	m_targetProjectVersion = json::getString(_jsonObject, "TargetProjectVersion");
	m_flags = stringListToProjectCompareFlags(json::getStringList(_jsonObject, "Flags"));
}
