// @otlicense

#pragma once
#include "EntityContainer.h"
class __declspec(dllexport) EntityBatchImporter : public EntityBase
{
public:
	EntityBatchImporter() : EntityBatchImporter(0, nullptr, nullptr, nullptr, "") {};
	EntityBatchImporter(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, const std::string& _owner);

	void createProperties(void);
	bool getEntityBox(double& _xmin, double& _xmax, double& _ymin, double& _ymax, double& _zmin, double& _zmax) override { return false; };
	entityType getEntityType(void) const override { return entityType::TOPOLOGY; };
	virtual void addVisualizationNodes(void) override;

	static std::string className() { return "EntityBatchImporter"; };
	virtual std::string getClassName(void) const override { return EntityBatchImporter::className(); };

	uint32_t getNumberOfRuns();
	std::string getNameBase();
};
