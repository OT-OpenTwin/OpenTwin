/*****************************************************************//**
 * \file   IndexManager.h
 * \brief	Handler for all types of indices that are needed. Parameter and quantities are abstracted via an indexed name (e.g. P_0, P_1, P_2, ...for parameter)
 *			Additionally, all parameter values are substituted by an index as well. Both types of indices have to be consecutive with already existing ones. 
 *			Thus, this class holds already existing indices and hands out new ones, if required.
 * 
 * \author Wagner
 * \date   June 2023
 *********************************************************************/

#pragma once
#include "EntityMeasurementMetadata.h"
#include "MetadataParameterBundle.h"
#include "MetadataAssemblyRangeData.h"
#include "MetadataQuantity.h"
#include "Documentation.h"
#include "openTwinCore/Variable.h"

#include <set>
#include <map>

class IndexManager
{
public:
	IndexManager(std::list<std::shared_ptr<EntityMeasurementMetadata>> existingMetadataEntities, const std::string& nameField, const std::string& dataTypeField, const std::string& valueField);
	IndexManager(const IndexManager& other) = delete;
	IndexManager(const IndexManager&& other) = delete;
	IndexManager& operator=(const IndexManager& other) = delete;
	IndexManager& operator=(const IndexManager&& other) = delete;

	MetadataParameterBundle CreateMetadataParameter(MetadataAssemblyRangeData& allParameter);
	std::map<std::string, MetadataQuantity*> GetNonExistingQuantityAbbreviationsByName(MetadataAssemblyRangeData& allParameter);
	
	bool DoesMSMDAlreadyExist(const std::string& msmdIndex) { return _takenMetadataNames.find(msmdIndex) != _takenMetadataNames.end(); }

	int32_t GetParameterIndex(const std::string& parameterName, const ot::Variable& value);
	
	int32_t GetQuantityIndex(const std::string& quantityName);

private:
	const std::string _parameterAbbreviationBase = "P_";
	const std::string _quantityAbbreviationBase = "Q_";
	const std::string _nameField;
	const std::string _valueField;
	const std::string _typeField;
	const std::string _msmdNameBase = "Measurementserieses Metadata_";

	std::map<std::string, MetadataParameter> _parameterByName; 
	std::map<std::string, std::map<ot::Variable,int32_t>> _parameterValueIndicesByName;
	


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
	
	void AddMetadataParameterToBundle(const std::map<std::string, std::list<ot::Variable>>& allFields, std::map<std::string, MetadataParameter>& existingMetadata, MetadataParameterBundle& bundle);
	int32_t GetNextQuantityIndex();
	void CreateParameterValueIndices();
	
	void CreateParameterValueIndices(std::map<std::string, MetadataParameter>& parameterByName, std::map<std::string, std::map<ot::Variable, int32_t>>& parameterValueIndices);
	void AddNewQuantities(const std::map<std::string, std::list<ot::Variable>>& felder, std::map<std::string, MetadataQuantity*>& newQuantities);

};