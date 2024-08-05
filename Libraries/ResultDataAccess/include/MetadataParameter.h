#pragma once
#include "OTCore/Variable.h"
#include "OTCore/CoreTypes.h"
#include "MetadataEntry.h"

#include <string>
#include <list>
#include <map>
#include <stdint.h>
#include <memory>

struct __declspec(dllexport) MetadataParameter
{
	MetadataParameter() = default;
	MetadataParameter(const MetadataParameter& _other) = default;
	MetadataParameter(MetadataParameter&& _other) = default;

	/*MetadataParameter(const MetadataParameter& _other)
	: parameterName(_other.parameterName),parameterLabel(_other.parameterLabel),parameterUID(_other.parameterUID),unit(_other.unit),typeName(_other.typeName),values(_other.values),metaData(_other.metaData) {}
	MetadataParameter(MetadataParameter&& _other) noexcept
		: parameterName(std::move(_other.parameterName)), parameterLabel(std::move(_other.parameterLabel)), parameterUID(std::move(_other.parameterUID)), unit(std::move(_other.unit)), typeName(std::move(_other.typeName)), values(std::move(_other.values)), metaData(std::move(_other.metaData)) {}*/
	MetadataParameter& operator=(const MetadataParameter& _other) = default;
	MetadataParameter& operator=(MetadataParameter&& _other) = default;
	//Unique name
	std::string parameterName = "";

	//In case that the unique name is already taken, the name is extended with a number. The original name is kept in parameterLabel.
	std::string parameterLabel = "";
	
	/****************** Are set by the campaign handling class *********************/
	ot::UID parameterUID = 0;
	/******************************************************************************/
	
	std::string unit;
	std::string typeName;
	
	std::list<ot::Variable> values;
	std::map < std::string, std::shared_ptr<MetadataEntry>> metaData;

	const bool operator==(const MetadataParameter& other) const
	{
		const bool equal = this->typeName == other.typeName &&
			this->unit == other.unit &&
			this->parameterName == other.parameterName;
		return equal;
	}

	const bool operator!=(const MetadataParameter& other) const
	{
		return !(*this == other);
	}
};