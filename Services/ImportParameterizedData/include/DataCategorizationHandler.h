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
#include <map>
#include <string>
#include "BusinessLogicHandler.h"
#include "EntityParameterizedDataCategorization.h"
#include "EntityParameterizedDataPreviewTable.h"
#include "EntityParameterizedDataTable.h"
#include "OpenTwinCore/CoreTypes.h"
#include "OpenTwinCore/Color.h"
#include "OpenTwinFoundation/TableRange.h"
#include "OpenTwinFoundation/EntityInformation.h"
#include "MetadataAssemblyData.h"


class DataCategorizationHandler : public BusinessLogicHandler
{
public:
	DataCategorizationHandler(std::string baseFolder, std::string parameterFolder, std::string quantityFolder, std::string tableFolder, std::string previewTableName);
	DataCategorizationHandler(const DataCategorizationHandler& other) = delete;
	DataCategorizationHandler& operator=(const DataCategorizationHandler& other) = delete;
	
	void AddSelectionsAsRMD(std::list<ot::UID> selectedEntities);
	void AddSelectionsAsMSMD(std::list<ot::UID> selectedEntities);
	void AddSelectionsAsParameter(std::list<ot::UID> selectedEntities);
	void AddSelectionsAsQuantity(std::list<ot::UID> selectedEntities);
	void StoreSelectionRanges(ot::UID tableEntityID, ot::UID tableEntityVersion, std::vector<ot::TableRange> ranges);
	std::pair<ot::UID, ot::UID> GetPreview(ot::EntityInformation selectedPreviewTable);

	void SetColourOfRanges(std::string tableName);

	void SelectRange(ot::UIDList iDs, ot::UIDList versions);

	ot::Color GetSerializedColour() const
	{ 
		return _backgroundColour;
	};

private:
	const std::string _baseFolder;
	const std::string _parameterFolder;
	const std::string _quantityFolder;
	const std::string _tableFolder;
	const std::string _previewTableName;
	const std::string _msmdFolder = "Measurementseries Metadata";
	std::string _rmdPath;

	const std::string _selectionRangeName = "Selection";

	const ot::Color _rmdColour;
	const ot::Color _msmdColour;
	const ot::Color _quantityColour;
	const ot::Color _parameterColour;

	ot::Color _backgroundColour;

	std::vector<std::shared_ptr<EntityParameterizedDataCategorization>> _activeCollectionEntities;
	std::vector<std::shared_ptr<EntityParameterizedDataCategorization>> _markedForStorringEntities;

	void AddSelectionsWithCategory(std::list<ot::UID>& selectedEntities, EntityParameterizedDataCategorization::DataCategorie category);
	void AddRMDEntries(ot::EntityInformation entityInfos);
	void AddMSMDEntries(std::list<ot::UID>& selectedEntities);
	void AddParamOrQuantityEntries(std::list<ot::UID>& selectedEntities, EntityParameterizedDataCategorization::DataCategorie category);
	void AddNewCategorizationEntity(std::string name, EntityParameterizedDataCategorization::DataCategorie category, bool addToActive);

	void RequestRangesSelection(std::vector<ot::TableRange>& ranges);
	void RequestColouringRanges(std::string colour);
	void RequestColouringRanges(ot::Color colour);

	void FindExistingRanges(std::string containerName, std::list<std::pair<ot::UID, ot::UID>>& existingRanges);
	void FindContainerEntity(std::string containerName, std::pair<ot::UID, ot::UID>& categorizationEntityIdentifier);
	bool CheckIfPreviewIsUpToDate(std::shared_ptr<EntityParameterizedDataPreviewTable> categorizationEntity, std::list<std::pair<ot::UID, ot::UID>>& existingRanges);
	std::pair<ot::UID, ot::UID> CreateNewTable(std::string tableName, EntityParameterizedDataCategorization::DataCategorie category, std::list<std::pair<ot::UID, ot::UID>>& existingRanges);
};
