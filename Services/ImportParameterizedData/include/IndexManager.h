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
	IndexManager(std::list<std::shared_ptr<EntityMeasurementMetadata>> existingMetadataEntities, std::string nameField, std::string dataTypeField, std::string valueField);

	MetadataParameterBundle CreateMetadataParameter(MetadataAssemblyRangeData& allParameter);
	std::map<std::string, MetadataQuantity*> GetNonExistingQuantityAbbreviationsByName(MetadataAssemblyRangeData& allParameter);
	std::string GetNextAvailableMSMDName();
	bool DoesMSMDAlreadyExist(std::string msmdIndex) { return _takenMetadataNames.find(msmdIndex) != _takenMetadataNames.end(); }

	template<class T>
	int32_t GetParameterIndex(std::string parameterName, T value);
	int32_t GetQuantityIndex(const std::string quantityName);

private:
	const std::string _parameterAbbreviationBase = "P_";
	const std::string _quantityAbbreviationBase = "Q_";
	const std::string _nameField;
	const std::string _valueField;
	const std::string _typeField;

	std::map<std::string, MetadataParameter<std::string>> _stringParameterByName; 
	std::map<std::string, MetadataParameter<double>> _doubleParameterByName;
	std::map<std::string, MetadataParameter<int32_t>> _int32ParameterByName;
	std::map<std::string, MetadataParameter<int64_t>> _int64ParameterByName;

	std::map<std::string, std::map<std::string,int32_t>> _stringParameterValueIndicesByName;
	std::map<std::string, std::map<double,int32_t>>_doubleParameterValueIndicesByName;
	std::map<std::string, std::map<int32_t,int32_t>>_int32ParameterValueIndicesByName;
	std::map<std::string, std::map<int64_t,int32_t>> _int64ParameterValueIndicesByName;


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
	void CreateParameterValueIndices();
	template <class T>
	void CreateParameterValueIndices(std::map<std::string, MetadataParameter<T>>& parameterByName, std::map<std::string, std::map<T, int32_t>>& parameterValueIndices);
	template <class T>
	void AddNewQuantities(const std::map<std::string, std::list<T>>& felder, std::string expectedType, std::map<std::string, MetadataQuantity*>& newQuantities, std::string summary);

};

template<class T>
inline int32_t IndexManager::GetParameterIndex(std::string parameterName, T value)
{
	throw std::exception("Not supported Action: trying to receive an index for a not supported parameter type");
}
template<>
inline int32_t IndexManager::GetParameterIndex(std::string parameterName, std::string value)
{
	auto parameterIndices = _stringParameterValueIndicesByName.find(parameterName);
	return parameterIndices->second.find(value)->second;
}
template<>
inline int32_t IndexManager::GetParameterIndex(std::string parameterName, double value)
{
	auto parameterIndices = _doubleParameterValueIndicesByName.find(parameterName);
	return parameterIndices->second.find(value)->second;
}
template<>
inline int32_t IndexManager::GetParameterIndex(std::string parameterName, int32_t value)
{
	auto parameterIndices = _int32ParameterValueIndicesByName.find(parameterName);
	return parameterIndices->second.find(value)->second;
}
template<>
inline int32_t IndexManager::GetParameterIndex(std::string parameterName, int64_t value)
{
	auto parameterIndices = _int64ParameterValueIndicesByName.find(parameterName);
	return parameterIndices->second.find(value)->second;
}

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
			MetadataParameter<T> newParameter;
			MetadataParameter<T>* compareParameter = &existingMetadata[field.first];
			
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

template<class T>
inline void IndexManager::CreateParameterValueIndices(std::map<std::string, MetadataParameter<T>>& parameterByName, std::map<std::string, std::map<T, int32_t>>& parameterValueIndices)
{
	for (const auto& parameter : parameterByName)
	{
		auto valuePointer = parameter.second.uniqueValues.begin();
		for (int i = 1; i < parameter.second.uniqueValues.size(); i++)
		{
			parameterValueIndices[parameter.first][*valuePointer] = i;
			valuePointer++;

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