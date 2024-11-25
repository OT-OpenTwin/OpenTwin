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
#include "EntityParameterizedDataPreviewTable.h"
#include "EntityParameterizedDataTable.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/Color.h"
#include "OTGui/TableRange.h"
#include "OTServiceFoundation/EntityInformation.h"
#include "MetadataAssemblyData.h"
#include "EntityTableSelectedRanges.h"
#include "OTCore/Variable.h"
#include "OTServiceFoundation/PythonServiceInterface.h"
#include "OTCore/GenericDataStructMatrix.h"

#include <optional>
#include <map>
#include <string>
#include <memory>
#include <set>

class DataCategorizationHandler : public BusinessLogicHandler
{
public:
	DataCategorizationHandler(std::string baseFolder, std::string parameterFolder, std::string quantityFolder, std::string tableFolder, std::string previewTableName);
	DataCategorizationHandler(const DataCategorizationHandler& other) = delete;
	DataCategorizationHandler& operator=(const DataCategorizationHandler& other) = delete;
	
	//! @brief If the selection is valid it returns the table name. Otherwise the string is empty.
	std::string markSelectionForStorage(const std::list<ot::EntityInformation>& _selectedEntities,EntityParameterizedDataCategorization::DataCategorie _category);
	

	
	void storeSelectionRanges(const std::vector<ot::TableRange>& _ranges);
	void CreateNewScriptDescribedMSMD();

	void SelectRange(ot::UIDList iDs, ot::UIDList versions);

	inline void ensureEssentials();

	ot::Color GetSerializedColour() const
	{ 
		return m_backgroundColour;
	};

private:
	const std::string m_baseFolder;
	const std::string m_parameterFolder;
	const std::string m_quantityFolder;
	const std::string m_tableFolder;
	const std::string m_previewTableName;
	const std::string m_smdFolder = "Series Metadata";
	
	ot::UID m_scriptFolderUID = -1;
	std::string m_rmdEntityName;

	const std::string _selectionRangeName = "Selection";

	const ot::Color m_rmdColour;
	const ot::Color m_msmdColour;
	const ot::Color m_quantityColour;
	const ot::Color m_parameterColour;

	ot::Color m_backgroundColour;

	std::map<std::string, std::list<std::shared_ptr<EntityTableSelectedRanges>>> _allRelevantTableSelectionsByMSMD;
	std::map<std::string, std::list<std::optional<std::list<ot::Variable>>>> _allVariablesByMSMD;
	std::set<std::string> m_bufferedCategorisationNames;
	ot::UID m_bufferedTableID = -1;
	ot::UID m_bufferedTableVersion = -1;
	std::vector<std::shared_ptr<EntityParameterizedDataCategorization>> m_markedForStorringEntities;

	ot::PythonServiceInterface* _pythonInterface = nullptr;

	bool isValidSelection(std::list<EntityBase*>& _selectedEntities);
	std::string getTableFromSelection(std::list<EntityBase*>& _selectedEntities);
	void bufferCorrespondingMetadataNames(std::list<EntityBase*>& _selectedEntities, EntityParameterizedDataCategorization::DataCategorie _category);
	void setBackgroundColour(EntityParameterizedDataCategorization::DataCategorie _category);
	void clearBufferedMetadata();
	bool checkForCategorisationEntity(std::list<EntityBase*>& _selectedEntities);
	void addSMDEntries(std::list<EntityBase*>& _selectedEntities);
	void addParamOrQuantityEntries(std::list<EntityBase*>& _selectedEntities, EntityParameterizedDataCategorization::DataCategorie _category);
	void addNewCategorizationEntity(std::string name, EntityParameterizedDataCategorization::DataCategorie category, bool addToActive);

	void requestColouringRanges(bool _clearSelection ,const std::string& _tableName, const ot::Color& _colour, const std::list<ot::TableRange>& ranges);

	void FindExistingRanges(std::string containerName, std::list<std::pair<ot::UID, ot::UID>>& existingRanges);
	void FindContainerEntity(std::string containerName, std::pair<ot::UID, ot::UID>& categorizationEntityIdentifier);
	bool CheckIfPreviewIsUpToDate(std::shared_ptr<EntityParameterizedDataPreviewTable> categorizationEntity, std::list<std::pair<ot::UID, ot::UID>>& existingRanges);

	std::string determineDataTypeOfSelectionRanges(const ot::GenericDataStructMatrix& _tableContent,const std::vector<ot::TableRange>& _selectedRanges);

	std::list<std::shared_ptr<EntityTableSelectedRanges>> FindAllTableSelectionsWithScripts();
	std::map<std::string, std::string> LoadAllPythonScripts(std::list< std::string>& scriptNames);
	
	std::map<std::string, std::pair<ot::UID, ot::UID>> GetAllTables();
	std::map<std::string, ot::UID> getAllScripts();
	
	std::tuple<std::list<std::string>, std::list<std::string>> CreateNewMSMDWithSelections(std::map<std::string, std::list<std::shared_ptr<EntityTableSelectedRanges>>>& allRelevantTableSelectionsByMSMD);
	
};
