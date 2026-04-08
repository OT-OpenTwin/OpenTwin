// @otlicense

// OpenTwin header
#include "OTCore/DatasetDependencyInfos.h"

ot::DatasetDependencyInfos::DatasetDependencyInfos(const ConstJsonObject& _jsonObject)
	: DatasetDependencyInfos()
{
	setFromJsonObject(_jsonObject);
}

void ot::DatasetDependencyInfos::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const
{
	JsonArray infosArr;
	for (const auto& info : m_infos)
	{
		infosArr.PushBack(JsonObject(info, _allocator), _allocator);
	}
	_jsonObject.AddMember("Infos", infosArr, _allocator);
}

void ot::DatasetDependencyInfos::setFromJsonObject(const ConstJsonObject& _jsonObject)
{
	m_infos.clear();
	for (const ConstJsonObject& infoObj : json::getObjectList(_jsonObject, "Infos"))
	{
		m_infos.push_back(DatasetDependencyInfo(infoObj));
	}
}

ot::DatasetDependencyInfos ot::DatasetDependencyInfos::findMatchingDependencies(const DatasetDependencyInfos& _otherDependencies)
{
	DatasetDependencyInfos result;

	for (const auto& info : m_infos)
	{
		for (const auto& otherInfo : _otherDependencies.getDependencies())
		{
			{
				if (info.getLabel() == otherInfo.getLabel() && info.getUnit() == otherInfo.getUnit() && info.getValue() == otherInfo.getValue())
				{
					result.addDependency(info);
					break;
				}
			}
		}

	}

	return result;
}

ot::DatasetDependencyInfos ot::DatasetDependencyInfos::findNonMatchingDependencies(const DatasetDependencyInfos& _otherDependencies, bool _onlyExisting)
{
	DatasetDependencyInfos result;

	for (const auto& info : m_infos)
	{
		bool found = false;
		bool isMatch = false;

		for (const auto& otherInfo : _otherDependencies.getDependencies())
		{
			if (info.getLabel() == otherInfo.getLabel())
			{
				found = true;
				if (info.getUnit() == otherInfo.getUnit() && info.getValue() == otherInfo.getValue())
				{
					isMatch = true;
				}
				
				break;
			}
		}

		if ((found || !_onlyExisting) && !isMatch)
		{
			result.addDependency(info);
		}
	}

	return result;
}

bool ot::DatasetDependencyInfos::operator==(const DatasetDependencyInfos& _other) const
{
	if (m_infos.size() != _other.m_infos.size())
	{
		return false;
	}

	for (const auto& info : m_infos)
	{
		bool found = false;
		for (const auto& otherInfo : _other.m_infos)
		{
			if (info == otherInfo)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			return false;
		}
	}

	return true;
}

std::optional<ot::DatasetDependencyInfo> ot::DatasetDependencyInfos::getDependency(const std::string& _label)
{
	for (const auto& info : m_infos)
	{
		if (info.getLabel() == _label)
		{
			return info;
		}
	}

	return std::nullopt;
}

const std::optional<ot::DatasetDependencyInfo> ot::DatasetDependencyInfos::getDependency(const std::string& _label) const
{
	for (const auto& info : m_infos)
	{
		if (info.getLabel() == _label)
		{
			return info;
		}
	}

	return std::nullopt;
}
