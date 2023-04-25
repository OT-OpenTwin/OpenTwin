#pragma once
#include "EntityMeasurementMetadata.h"
#include "MetadataParameterBundle.h"
#include "MetadataAssemblyRangeData.h"
#include "MetadataQuantity.h"
#include <set>
#include <map>

class IndexManager
{
public:
	IndexManager(std::list<std::shared_ptr<EntityMeasurementMetadata>> existingMetadataEntities);

	MetadataParameterBundle CreateMetadataParameter(MetadataAssemblyRangeData& allParameter);
	std::map<std::string, MetadataQuantity*> GetNonExistingQuantityAbbreviationsByName(MetadataAssemblyRangeData& allParameter);
	std::string GetNextAvailableMSMDName();
	bool DoesMSMDAlreadyExist(std::string msmdIndex) { return _takenMetadataNames.find(msmdIndex) != _takenMetadataNames.end(); }
private:
	const std::string _parameterAbbreviationBase = "P_";
	const std::string _quantityAbbreviationBase = "Q_";
	
	std::map<std::string, MetadataParameter<std::string>> _stringParameterByName; 
	std::map<std::string, MetadataParameter<double>> _doubleParameterByName;
	std::map<std::string, MetadataParameter<int32_t>> _int32ParameterByName;
	std::map<std::string, MetadataParameter<int64_t>> _int64ParameterByName;

	std::set<int> _takenParameterIndices; 
	std::set<std::string> _takenMetadataNames; 
	std::map<std::string, MetadataQuantity> _takenQuantitiesByName;
	//Needs to be made exceptionsave. What happens, if the creation of a metadata object reserves an index but failes? 
	//It needs to be released again for the other, parallel running branches!


	void StoreAllParameter(std::list<std::shared_ptr<EntityMeasurementMetadata>> existingMetadataEntities);
	void StoreAllQuantities(std::list<std::shared_ptr<EntityMeasurementMetadata>> existingMetadataEntities);
	void StoreAllMSMDs(std::list<std::shared_ptr<EntityMeasurementMetadata>> existingMetadataEntities);
	bool CheckOnRMDLevelForParameterConsistency(MetadataAssemblyRangeData& allParameter, std::string& errorMessage);
	bool CheckIfAllParameterHaveSameSize(MetadataAssemblyRangeData& allParameter, std::string& errorMessage);
	template <class T>
	void AddMetadataParameterToBundle(const std::map<std::string, std::list<T>>& allFieldsOfTypeT, std::map<std::string, MetadataParameter<T>>& existingMetadata, MetadataParameterBundle& bundle);
	int32_t GetNextQuantityIndex();
	
	template <class T>
	void AddNewQuantities(const std::map<std::string, std::list<T>>& felder, std::string expectedType, std::map<std::string, MetadataQuantity*>& newQuantities, std::string summary);

};

template<class T>
inline void IndexManager::AddMetadataParameterToBundle(const std::map<std::string, std::list<T>>& allFieldsOfTypeT, std::map<std::string, MetadataParameter<T>>& existingMetadata, MetadataParameterBundle& bundle)
{
	for (auto& field : allFieldsOfTypeT)
	{

		//Get pointer to already existing parameterset (name and abbr.) or create new one if not existing (setting name and abbr.)
		if (existingMetadata.find(field.first) == existingMetadata.end())
		{
			for (auto newValue : field.second)
			{
				existingMetadata[field.first].values.push_back(newValue);
			}

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
			MetadataParameter<T> newParameter;
			MetadataParameter<T>* compareParameter = &existingMetadata[field.first];
			
			for (auto newValue : field.second)
			{
				bool newValueAlreadyKnown = false;
				for (auto value : compareParameter->values)
				{
					if (newValue == value)
					{
						newValueAlreadyKnown = true;
						break;
					}
				}
				if (!newValueAlreadyKnown)
				{
					newParameter.values.push_back(newValue);
					compareParameter->values.push_back(newValue);
				}
			}
			if (newParameter.values.size() != 0)
			{
				newParameter.parameterName = compareParameter->parameterName;
				newParameter.parameterAbbreviation = compareParameter->parameterAbbreviation;
				bundle.AddMetadataParameter(newParameter);
			}
		}
	}
}

template<class T>
inline void IndexManager::AddNewQuantities(const std::map<std::string, std::list<T>>& fields, std::string expectedType, std::map<std::string, MetadataQuantity*>& newQuantities, std::string summary)
{
	for (auto& field : fields)
	{
		if (_takenQuantitiesByName.find(field.first) != _takenQuantitiesByName.end())
		{
			if (_takenQuantitiesByName[field.first].typeName != ot::TypeNames::getDoubleTypeName())
			{
				summary += "Quantity " + field.first + " is already used with a different datatype in a previous MSMD\n";
			}
			else
			{
				summary += "Quantity " + field.first + " already exists\n";
			}
		}
		else
		{
			_takenQuantitiesByName[field.first].quantityName = field.first;
			_takenQuantitiesByName[field.first].typeName = ot::TypeNames::getDoubleTypeName();
			_takenQuantitiesByName[field.first].quantityIndex = GetNextQuantityIndex();
			_takenQuantitiesByName[field.first].quantityAbbreviation = _quantityAbbreviationBase + std::to_string(_takenQuantitiesByName[field.first].quantityIndex);
			newQuantities[_takenQuantitiesByName[field.first].quantityName] = &_takenQuantitiesByName[field.first];
			summary += "Added new Quantity " + field.first + " as " + _takenQuantitiesByName[field.first].quantityAbbreviation +"\n";
		}
	}
}