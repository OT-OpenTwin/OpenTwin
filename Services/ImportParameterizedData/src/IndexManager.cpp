/*****************************************************************//**
 * \file   IndexManager.cpp
 * \brief  Oversees all parameter and quantity indices. 
 *			Both are being abstracted as P0, P1, ... respectively Q0, Q1, ...
 *			The IndexManager takes care that new parameter and quantities get the next available index assigned.
 *			The IndexManager also takes care of the indices of each parametervalue, respectively quantityvalue.
 * 
 * \author Wagner
 * \date   June 2023
 *********************************************************************/
#include "IndexManager.h"
#include "OpenTwinCore/TypeNames.h"


IndexManager::IndexManager(std::list<std::shared_ptr<EntityMeasurementMetadata>> existingMetadataEntities, const std::string& nameField, const std::string& dataTypeField, const std::string& valueField)
	:_nameField(nameField), _valueField(valueField), _typeField(dataTypeField)
{
	if (existingMetadataEntities.size() != 0)
	{
		Documentation::INSTANCE()->AddToDocumentation("Already existing metadata are ignored.\n");
		Documentation::INSTANCE()->AddToDocumentation("Subsequently, none of the following MSMDs, parameter and quantities are created nor altered.\n");
		Documentation::INSTANCE()->AddToDocumentation("MSMDs:\n");
		StoreAllMSMDs(existingMetadataEntities);
		Documentation::INSTANCE()->AddToDocumentation("Parameter:\n");
		StoreAllParameter(existingMetadataEntities);
		Documentation::INSTANCE()->AddToDocumentation("Quantities:\n");
		StoreAllQuantities(existingMetadataEntities);
	}
}

void IndexManager::StoreAllMSMDs(std::list<std::shared_ptr<EntityMeasurementMetadata>> existingMetadataEntities)
{
	for (auto& msmd : existingMetadataEntities)
	{
		std::string msmdName = msmd->getName();
		msmdName = msmdName.substr(msmdName.find_last_of('/')+1, msmdName.size());
		_takenMetadataNames.insert(msmdName);
		Documentation::INSTANCE()->AddToDocumentation(msmdName + "\n");
	}
}

void IndexManager::StoreAllParameter(std::list<std::shared_ptr<EntityMeasurementMetadata>> existingMetadataEntities)
{
	std::string parameterDocumentName = (*existingMetadataEntities.begin())->getParameterDocumentName();
	for (auto existingMetadata : existingMetadataEntities)
	{
		std::vector<std::string> allParameterNames = existingMetadata->getAllParameterDocumentNames();
		for (const std::string& parameterName : allParameterNames)
		{
			std::string subDocumentName = parameterName.substr(parameterName.find(parameterDocumentName), parameterName.size());
			std::string::difference_type n = std::count(subDocumentName.begin(), subDocumentName.end(), '/');
			//getAllParameterDocumentNames also returns all subDocuments and the superdocument of the parameter. Only the parameter are further analysed:
			if (n != 1)
			{
				continue;
			}

			std::string parameterAbbreviation = subDocumentName.substr(subDocumentName.find(_parameterAbbreviationBase), subDocumentName.size());
			std::string parameterIndex = parameterAbbreviation.substr(parameterAbbreviation.find(_parameterAbbreviationBase) + _parameterAbbreviationBase.size(), parameterAbbreviation.size());
			_takenParameterIndices.insert(std::stoi(parameterIndex));

			auto document = existingMetadata->getDocument(parameterName);
			auto allStringFieldsByName = document->getStringFields();
			if (allStringFieldsByName->find(_nameField) == allStringFieldsByName->end())
			{
				assert(0);
			}
			std::string parameterName = *allStringFieldsByName->find(_nameField)->second.begin();
			auto allInt32FieldsByName = document->getInt32Fields();
			auto allInt64FieldsByName = document->getInt64Fields();
			auto allDoubleFieldsByName = document->getDoubleFields();

			Documentation::INSTANCE()->AddToDocumentation(parameterAbbreviation + ": " + parameterName + " ");
			if (allStringFieldsByName->find(_valueField) != allStringFieldsByName->end())
			{
				_stringParameterByName[parameterName].uniqueValues = allStringFieldsByName->find(_valueField)->second;
				_stringParameterByName[parameterName].parameterName = parameterName;
				_stringParameterByName[parameterName].parameterAbbreviation = parameterAbbreviation;
				Documentation::INSTANCE()->AddToDocumentation("with " +	std::to_string(_stringParameterByName[parameterName].uniqueValues.size()) + " string values.\n");
			}
			else if (allInt32FieldsByName->find(_valueField) != allInt32FieldsByName->end())
			{
				_int32ParameterByName[parameterName].uniqueValues = allInt32FieldsByName->find(_valueField)->second;
				_int32ParameterByName[parameterName].parameterName = parameterName;
				_int32ParameterByName[parameterName].parameterAbbreviation = parameterAbbreviation;
				Documentation::INSTANCE()->AddToDocumentation("with " + std::to_string(_int32ParameterByName[parameterName].uniqueValues.size()) + " int32 values.\n");
			}
			else if (allInt64FieldsByName->find(_valueField) != allInt64FieldsByName->end())
			{
				_int64ParameterByName[parameterName].uniqueValues = allInt64FieldsByName->find(_valueField)->second;
				_int64ParameterByName[parameterName].parameterName = parameterName;
				_int64ParameterByName[parameterName].parameterAbbreviation = parameterAbbreviation;
				Documentation::INSTANCE()->AddToDocumentation("with " + std::to_string(_int64ParameterByName[parameterName].uniqueValues.size()) + " int64 values.\n");
			}
			else if (allDoubleFieldsByName->find(_valueField) != allDoubleFieldsByName->end())
			{
				_doubleParameterByName[parameterName].uniqueValues = allDoubleFieldsByName->find(_valueField)->second;
				_doubleParameterByName[parameterName].parameterName = parameterName;
				_doubleParameterByName[parameterName].parameterAbbreviation = parameterAbbreviation;
				Documentation::INSTANCE()->AddToDocumentation("with " + std::to_string(_doubleParameterByName[parameterName].uniqueValues.size()) + " double values.\n");
			}
			
		}
	}
}

void IndexManager::StoreAllQuantities(std::list<std::shared_ptr<EntityMeasurementMetadata>> existingMetadataEntities)
{
	std::string quantityDocumentName = (*existingMetadataEntities.begin())->getQuantityDocumentName();
	for (auto& existingMetadata : existingMetadataEntities)
	{
		std::vector<std::string> allQuantityNames = existingMetadata->getAllQuantityDocumentNames();
		for (std::string quantityName : allQuantityNames)
		{
			std::string subDocumentName = quantityName.substr(quantityName.find(quantityDocumentName), quantityName.size());
			std::string::difference_type n = std::count(subDocumentName.begin(), subDocumentName.end(), '/');
	
			//getAllQuantityDocumentNames also returns all subDocuments and the superdocument of the quantity. Only the quantities themselve are further analysed:
			if (n != 1)
			{
				continue;
			}

			auto document = existingMetadata->getDocument(quantityName);
			auto allStringFieldsByName = document->getStringFields();
			std::string quantityName = *allStringFieldsByName->find(_nameField)->second.begin();

			_takenQuantitiesByName[quantityName].quantityName = quantityName;
			_takenQuantitiesByName[quantityName].typeName = *allStringFieldsByName->find(_typeField)->second.begin();

			std::string quantityAbbreviation = subDocumentName.substr(subDocumentName.find(_quantityAbbreviationBase), subDocumentName.size());
			std::string quantityIndex = quantityAbbreviation.substr(quantityAbbreviation.find(_quantityAbbreviationBase) + _quantityAbbreviationBase.size(), quantityAbbreviation.size());
			_takenQuantitiesByName[quantityName].quantityAbbreviation = quantityAbbreviation;
			_takenQuantitiesByName[quantityName].quantityIndex = std::stoi(quantityIndex);
			Documentation::INSTANCE()->AddToDocumentation(quantityAbbreviation + ": " + quantityName + " of type " + _takenQuantitiesByName[quantityName].typeName + "\n");
		}
	}
}

MetadataParameterBundle IndexManager::CreateMetadataParameter(MetadataAssemblyRangeData& allParameter)
{
	std::string errorMessage;
	bool isConsistent = CheckOnRMDLevelForParameterConsistency(allParameter, errorMessage);
	if (!isConsistent)
	{
		throw std::exception(("Failed to create new set of parameter. A consistency check with parametersets from other MSMDs of the active RMD resulted in the following errors: " + errorMessage).c_str());
	}
	isConsistent = CheckIfAllParameterHaveSameSize(allParameter, errorMessage);
	if (!isConsistent)
	{
		throw std::exception(("Failed to create new set of parameter. Not all parameter have the same number of entries, thus some quantities are not completely defined: " + errorMessage).c_str());
	}

	MetadataParameterBundle parameterBundle;
	AddMetadataParameterToBundle(*allParameter.GetStringFields(), _stringParameterByName, parameterBundle);
	AddMetadataParameterToBundle(*allParameter.GetInt32Fields(), _int32ParameterByName, parameterBundle);
	AddMetadataParameterToBundle(*allParameter.GetInt64Fields(), _int64ParameterByName, parameterBundle);
	AddMetadataParameterToBundle(*allParameter.GetDoubleFields(), _doubleParameterByName, parameterBundle);

	CreateParameterValueIndices();
	return parameterBundle;
}

bool IndexManager::CheckOnRMDLevelForParameterConsistency(MetadataAssemblyRangeData& allParameter, std::string& errorMessage)
{
	bool isConsistent= true;
	errorMessage = "";
	auto CreateErrorText = [](std::string parameterName, std::string type) 
	{ 
		return "Parameter: " + parameterName + " already exists as: " + type + "\n";
	};

	for (auto& field : *allParameter.GetStringFields())
	{
		if (_int32ParameterByName.find(field.first) != _int32ParameterByName.end())
		{
			isConsistent = false;
			errorMessage += CreateErrorText(field.first, "int32");
		}
		if (_int64ParameterByName.find(field.first) != _int64ParameterByName.end())
		{
			isConsistent = false;
			errorMessage += CreateErrorText(field.first, "int64");
		}
		if (_doubleParameterByName.find(field.first) != _doubleParameterByName.end())
		{
			isConsistent = false;
			errorMessage += CreateErrorText(field.first, "double");
		}
	}

	for (auto& field : *allParameter.GetDoubleFields())
	{
		if (_stringParameterByName.find(field.first) != _stringParameterByName.end())
		{
			isConsistent = false;
			errorMessage += CreateErrorText(field.first,"string");
		}
		if (_int32ParameterByName.find(field.first) != _int32ParameterByName.end())
		{
			isConsistent = false;
			errorMessage += CreateErrorText(field.first, "int32");
		}
		if(_int64ParameterByName.find(field.first) != _int64ParameterByName.end())
		{
			isConsistent = false;
			errorMessage += CreateErrorText(field.first, "int64");
		}
	}

	for (auto& field : *allParameter.GetInt32Fields())
	{
		if (_stringParameterByName.find(field.first) != _stringParameterByName.end())
		{
			isConsistent = false;
			errorMessage += CreateErrorText(field.first, "string");
		}
		if (_int64ParameterByName.find(field.first) != _int64ParameterByName.end())
		{
			isConsistent = false;
			errorMessage += CreateErrorText(field.first, "int64");
		}
		if (_doubleParameterByName.find(field.first) != _doubleParameterByName.end())
		{
			isConsistent = false;
			errorMessage += CreateErrorText(field.first, "double");
		}
	}

	for (auto& field : *allParameter.GetInt64Fields())
	{
		if (_stringParameterByName.find(field.first) != _stringParameterByName.end())
		{
			isConsistent = false;
			errorMessage += CreateErrorText(field.first, "string");
		}
		if (_int32ParameterByName.find(field.first) != _int32ParameterByName.end())
		{
			isConsistent = false;
			errorMessage += CreateErrorText(field.first, "int32");
		}
		if (_doubleParameterByName.find(field.first) != _doubleParameterByName.end())
		{
			isConsistent = false;
			errorMessage += CreateErrorText(field.first, "double");
		}
	}

	return isConsistent;
}

bool IndexManager::CheckIfAllParameterHaveSameSize(MetadataAssemblyRangeData& allParameter, std::string& errorMessage)
{
	std::map<std::string, uint64_t> parameterNameBySize;
	for (const auto& field : *allParameter.GetStringFields())
	{
		parameterNameBySize[field.first] = field.second.size();
	}
	for (const auto& field : *allParameter.GetDoubleFields())
	{
		parameterNameBySize[field.first] = field.second.size();
	}
	for (const auto& field : *allParameter.GetInt32Fields())
	{
		parameterNameBySize[field.first] = field.second.size();
	}
	for (const auto& field : *allParameter.GetInt64Fields())
	{
		parameterNameBySize[field.first] = field.second.size();
	}

	uint64_t size = parameterNameBySize.begin()->second;
	bool sizeIsConsistent = true;
	errorMessage = "";
	for (const auto& parameter : parameterNameBySize)
	{
		errorMessage += ("Parameter: " + parameter.first + " size: " + std::to_string(parameter.second) + "\n");
		if (parameter.second != size)
		{
			sizeIsConsistent = false;
		}
	}
	return sizeIsConsistent;
}

int32_t IndexManager::GetParameterIndex(const std::string& parameterName, const std::string& value)
{
	const auto parameterValueIndicesByName = _stringParameterValueIndicesByName.find(parameterName);
	const auto& parameterValueIndices = parameterValueIndicesByName->second;
	int32_t index = parameterValueIndices.find(value)->second;
	return index;
}

int32_t IndexManager::GetParameterIndex(const std::string& parameterName, const double& value)
{
	const auto parameterValueIndicesByName = _doubleParameterValueIndicesByName.find(parameterName);
	const auto& parameterValueIndices = parameterValueIndicesByName->second;
	int32_t index = parameterValueIndices.find(value)->second;
	return index;
}

int32_t IndexManager::GetParameterIndex(const std::string& parameterName, const int32_t& value)
{
	const auto parameterValueIndicesByName = _int32ParameterValueIndicesByName.find(parameterName);
	const auto& parameterValueIndices = parameterValueIndicesByName->second;
	int32_t index = parameterValueIndices.find(value)->second;
	return index;
}

int32_t IndexManager::GetParameterIndex(const std::string& parameterName, const int64_t& value)
{
	const auto parameterValueIndicesByName = _int64ParameterValueIndicesByName.find(parameterName);
	const auto& parameterValueIndices = parameterValueIndicesByName->second;
	int32_t index = parameterValueIndices.find(value)->second;
	return index;
}

int32_t IndexManager::GetQuantityIndex(const std::string& quantityName)
{
	return _takenQuantitiesByName.find(quantityName)->second.quantityIndex;
}

std::map<std::string, MetadataQuantity*> IndexManager::GetNonExistingQuantityAbbreviationsByName(MetadataAssemblyRangeData& allParameter)
{
	std::map<std::string, MetadataQuantity*> newQuantities;

	AddNewQuantities(*allParameter.GetDoubleFields(), ot::TypeNames::getDoubleTypeName(), newQuantities);
	AddNewQuantities(*allParameter.GetInt32Fields(), ot::TypeNames::getInt32TypeName(), newQuantities);
	AddNewQuantities(*allParameter.GetInt64Fields(), ot::TypeNames::getInt64TypeName(), newQuantities);
	AddNewQuantities(*allParameter.GetStringFields(), ot::TypeNames::getStringTypeName(), newQuantities);

	return newQuantities;
}

int32_t IndexManager::GetNextQuantityIndex()
{
	int32_t index = 1;

	for (int i = 0; i < _takenQuantitiesByName.size() + 1; i++)
	{
		bool existsAllready = false;
		for (auto& quantity : _takenQuantitiesByName)
		{
			if (index == quantity.second.quantityIndex)
			{
				existsAllready = true;
				break;
			}
		}
		if (!existsAllready)
		{
			return index;
		}
		index++;
	}
	return -1;
}

void IndexManager::CreateParameterValueIndices()
{
	CreateParameterValueIndices(_stringParameterByName, _stringParameterValueIndicesByName);
	CreateParameterValueIndices(_doubleParameterByName, _doubleParameterValueIndicesByName);
	CreateParameterValueIndices(_int32ParameterByName, _int32ParameterValueIndicesByName);
	CreateParameterValueIndices(_int64ParameterByName, _int64ParameterValueIndicesByName);
}

