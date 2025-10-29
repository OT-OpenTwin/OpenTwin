// @otlicense

#pragma once
#include "PreviewAssembler.h"
class PreviewAssemblerRMD : public PreviewAssembler
{
public:
	PreviewAssemblerRMD(ot::components::ModelComponent* modelComponent, std::string tableFolder) : PreviewAssembler(modelComponent, tableFolder) {};

private:
	virtual void InitiatePreviewTable(std::shared_ptr<EntityResultTableData<std::string>> sourceTable) override;
	virtual void AddFieldsToTable(std::shared_ptr<EntityResultTableData<std::string>> previewTable) override;
	std::map<std::string, std::list<std::string>> CollectUniqueFields();
};
