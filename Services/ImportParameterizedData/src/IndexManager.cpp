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
#include "OTCore/TypeNames.h"


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
			auto allFieldsByName = document->getFields();
			if (allFieldsByName->find(_nameField) == allFieldsByName->end())
			{
				assert(0);
			}
			std::string parameterName = allFieldsByName->find(_nameField)->second.begin()->getConstCharPtr();
			
			Documentation::INSTANCE()->AddToDocumentation(parameterAbbreviation + ": " + parameterName + " ");
			_parameterByName[parameterName].uniqueValues = allFieldsByName->find(_valueField)->second;
			_parameterByName[parameterName].parameterName = parameterName;
			_parameterByName[parameterName].parameterAbbreviation = parameterAbbreviation;
			Documentation::INSTANCE()->AddToDocumentation("with " + std::to_string(_parameterByName[parameterName].uniqueValues.size()) + " string values.\n");
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
			auto allFieldsByName = document->getFields();
			std::string quantityName = allFieldsByName->find(_nameField)->second.begin()->getConstCharPtr();

			_takenQuantitiesByName[quantityName].quantityName = quantityName;
			_takenQuantitiesByName[quantityName].typeName = allFieldsByName->find(_typeField)->second.begin()->getConstCharPtr();

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
	AddMetadataParameterToBundle(*allParameter.getFields(), _parameterByName, parameterBundle);
	
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

	for (auto& field : *allParameter.getFields())
	{
		auto existingParameterEntry = _parameterByName.find(field.first);
		if (existingParameterEntry != _parameterByName.end())
		{
			auto& uniqueValues = existingParameterEntry->second.uniqueValues;
			if(uniqueValues.begin()->getTypeName() != field.second.begin()->getTypeName())
			{
				isConsistent = false;
				errorMessage += CreateErrorText(field.first, uniqueValues.begin()->getTypeName());
			}
		}
	}

	return isConsistent;
}

bool IndexManager::CheckIfAllParameterHaveSameSize(MetadataAssemblyRangeData& allParameter, std::string& errorMessage)
{
	std::map<std::string, uint64_t> parameterNameBySize;
	for (const auto& field : *allParameter.getFields())
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

int32_t IndexManager::GetParameterIndex(const std::string& parameterName, const ot::Variable& value)
{
	const auto parameterValueIndicesByName = _parameterValueIndicesByName.find(parameterName);
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

	AddNewQuantities(*allParameter.getFields(), newQuantities);
	
	return newQuantities;
}

void IndexManager::AddMetadataParameterToBundle(const std::map<std::string, std::list<ot::Variable>>& allFields, std::map<std::string, MetadataParameter>& existingMetadata, MetadataParameterBundle& bundle)
{
	for (auto& field : allFields)
	{

		//Get pointer to already existing parameterset (name and abbr.) or create new one if not existing (setting name and abbr.)
		if (existingMetadata.find(field.first) == existingMetadata.end())
		{
			for (auto newValue : field.second)
			{
				existingMetadata[field.first].uniqueValues.push_back(newValue);
				existingMetadata[field.first].selectedValues.push_back(newValue);
			}
			existingMetadata[field.first].uniqueValues.unique();

			existingMetadata[field.first].parameterName = field.first;
			int32_t nextParameterIndex = 1;
			while (_takenParameterIndices.find(nextParameterIndex) != _takenParameterIndices.end())
			{
				nextParameterIndex++;
			}
			_takenParameterIndices.insert(nextParameterIndex);
			existingMetadata[field.first].parameterAbbreviation = _parameterAbbreviationBase + std::to_string(nextParameterIndex);
			bundle.AddMetadataParameter(existingMetadata[field.first]);
		}
		else
		{
			MetadataParameter newParameter;
			MetadataParameter* compareParameter = &existingMetadata[field.first];

			for (auto newValue : field.second)
			{
				newParameter.selectedValues.push_back(newValue);

				bool newValueAlreadyKnown = false;
				for (auto value : compareParameter->uniqueValues)
				{
					if (newValue == value)
					{
						newValueAlreadyKnown = true;
						break;
					}
				}
				if (!newValueAlreadyKnown)
				{
					newParameter.uniqueValues.push_back(newValue);
					compareParameter->uniqueValues.push_back(newValue);
				}
			}
			if (newParameter.uniqueValues.size() != 0)
			{
				newParameter.parameterName = compareParameter->parameterName;
				newParameter.parameterAbbreviation = compareParameter->parameterAbbreviation;
				bundle.AddMetadataParameter(newParameter);
			}
		}
	}
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
	CreateParameterValueIndices(_parameterByName, _parameterValueIndicesByName);
}

void IndexManager::CreateParameterValueIndices(std::map<std::string, MetadataParameter>& parameterByName, std::map<std::string, std::map<ot::Variable, int32_t>>& parameterValueIndices)
{
	for (const auto& parameter : parameterByName)
	{
		auto valuePointer = parameter.second.uniqueValues.begin();
		for (int i = 1; i <= parameter.second.uniqueValues.size(); i++)
		{
			parameterValueIndices[parameter.second.parameterAbbreviation][*valuePointer] = i;
			valuePointer++;
		}
	}
}

void IndexManager::AddNewQuantities(const std::map<std::string, std::list<ot::Variable>>& fields, std::map<std::string, MetadataQuantity*>& newQuantities)
{
	for (auto& field : fields)
	{
		if (_takenQuantitiesByName.find(field.first) != _takenQuantitiesByName.end())
		{
			if (_takenQuantitiesByName[field.first].typeName != field.second.begin()->getTypeName())
			{
				Documentation::INSTANCE()->AddToDocumentation("Quantity " + field.first + " is already used with a different datatype in a previous MSMD\n");
			}
			else
			{
				Documentation::INSTANCE()->AddToDocumentation("Quantity " + field.first + " already exists\n");
			}
		}
		else
		{
			_takenQuantitiesByName[field.first].quantityName = field.first;
			_takenQuantitiesByName[field.first].typeName = field.second.begin()->getTypeName();
			_takenQuantitiesByName[field.first].quantityIndex = GetNextQuantityIndex();
			_takenQuantitiesByName[field.first].quantityAbbreviation = _quantityAbbreviationBase + std::to_string(_takenQuantitiesByName[field.first].quantityIndex);
			newQuantities[_takenQuantitiesByName[field.first].quantityName] = &_takenQuantitiesByName[field.first];
			Documentation::INSTANCE()->AddToDocumentation("Added new Quantity " + field.first + " as " + _takenQuantitiesByName[field.first].quantityAbbreviation + "\n");
		}
	}
}

