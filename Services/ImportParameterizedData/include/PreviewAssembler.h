#pragma once
#include "EntityParameterizedDataTable.h"
#include "EntityParameterizedDataPreviewTable.h"
#include "EntityTableSelectedRanges.h"
#include "EntityResultTableData.h"
#include <OpenTwinFoundation/ModelComponent.h>

#include <memory>
#include <string>
#include <list>
#include <vector>

class PreviewAssembler
{
public:
	PreviewAssembler(ot::components::ModelComponent* modelComponent, std::string tableFolder) : _modelComponent(modelComponent), _tableFolder(tableFolder) {};
	std::shared_ptr<EntityParameterizedDataPreviewTable> AssembleTable(ot::UIDList& existingRanges);

protected:
	int _numberOfFields = 0;
	std::vector<std::shared_ptr<EntityTableSelectedRanges>> _selectedRangeEntities;
	std::vector<std::shared_ptr<EntityParameterizedDataTable>> _tableSources;

	virtual void InitiatePreviewTable(std::shared_ptr<EntityResultTableData<std::string>> sourceTable) = 0;
	virtual void AddFieldsToTable(std::shared_ptr<EntityResultTableData<std::string>> previewTable) = 0;

private:
	ot::components::ModelComponent* _modelComponent = nullptr;
	std::string _tableFolder;
	void CountPreviewFieldsFromSelectedRanges();

	void LoadSelectedRangesAndTableSources(ot::UIDList& tableNames);
	void LoadTableSources(std::list<std::string>& tableNames);

};
