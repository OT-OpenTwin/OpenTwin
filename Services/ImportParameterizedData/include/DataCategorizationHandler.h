/*****************************************************************//**
 * \file   ParameterizedDataHandler.h
 * \brief  Holds the mechanisms to create the final entities holding the four entities that describe a parameterized data collection.
 *			Research metadata, measurementseries metadata, quantities and parameters.
 *			It is possible to create a new entity of each type, i.e. a new measurement series for instance. If an existed measurement series shall be extended, it needs
 *			to be selected from outside and added to the map in this handler.
 * 
 * \author Wagner
 * \date   February 2023
 *********************************************************************/
#pragma once

#include "BusinessLogicHandler.h"
#include "EntityParameterizedDataCategorization.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/Color.h"
#include "OTServiceFoundation/EntityInformation.h"
#include "MetadataAssemblyData.h"
#include "EntityTableSelectedRanges.h"
#include "OTCore/Variable.h"
#include "OTGui/TableRange.h"
#include "IVisualisationTable.h"

#include <optional>
#include <map>
#include <string>
#include <memory>
#include <set>

class DataCategorizationHandler : public BusinessLogicHandler
{
public:
	DataCategorizationHandler(std::string tableFolder, std::string previewTableName);
	DataCategorizationHandler(const DataCategorizationHandler& other) = delete;
	DataCategorizationHandler& operator=(const DataCategorizationHandler& other) = delete;
	
	void markSelectionForStorage(const std::list<ot::EntityInformation>& _selectedEntities,EntityParameterizedDataCategorization::DataCategorie _category);
	
	void storeSelectionRanges(const std::vector<ot::TableRange>& _ranges);

	inline void ensureEssentials();

private:
	const std::string m_tableFolder;
	const std::string m_previewTableName;
	ot::UID m_scriptFolderUID = -1;
	std::string m_rmdEntityName;
	const std::string _selectionRangeName = "Selection";
	ot::Color m_backgroundColour;

	std::map<std::string, std::list<std::shared_ptr<EntityTableSelectedRanges>>> _allRelevantTableSelectionsByMSMD;
	std::map<std::string, std::list<std::optional<std::list<ot::Variable>>>> _allVariablesByMSMD;
	std::set<std::string> m_bufferedCategorisationNames;
	ot::UID m_bufferedTableID = -1;
	ot::UID m_bufferedTableVersion = -1;
	std::vector<std::shared_ptr<EntityParameterizedDataCategorization>> m_markedForStorringEntities;



	bool isValidSelection(std::list<EntityBase*>& _selectedEntities);
	std::string getTableFromSelection(std::list<EntityBase*>& _selectedEntities);
	void bufferCorrespondingMetadataNames(std::list<EntityBase*>& _selectedEntities, EntityParameterizedDataCategorization::DataCategorie _category);
	void setBackgroundColour(EntityParameterizedDataCategorization::DataCategorie _category);
	void clearBufferedMetadata();
	bool checkForCategorisationEntity(std::list<EntityBase*>& _selectedEntities);
	void addSMDEntries(std::list<EntityBase*>& _selectedEntities);
	void addParamOrQuantityEntries(std::list<EntityBase*>& _selectedEntities, EntityParameterizedDataCategorization::DataCategorie _category);
	void addNewCategorizationEntity(std::string name, EntityParameterizedDataCategorization::DataCategorie category, bool addToActive);
	void requestRangeSelection(const std::string& _tableName);

	std::string determineDataTypeOfSelectionRanges(IVisualisationTable* _table,const std::vector<ot::TableRange>& _selectedRanges);
	std::map<std::string, ot::UID> getAllScripts();
};
