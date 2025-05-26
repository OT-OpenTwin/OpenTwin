#pragma once
#include "EntityContainer.h"
class __declspec(dllexport) EntityBatchImporter : public EntityBase
{
public:
	EntityBatchImporter(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, ClassFactoryHandler* _factory, const std::string& _owner);
	void createProperties(void);
	bool getEntityBox(double& _xmin, double& _xmax, double& _ymin, double& _ymax, double& _zmin, double& _zmax) override { return false; };
	entityType getEntityType(void) const override { return entityType::TOPOLOGY; };
	virtual void addVisualizationNodes(void) override;
	virtual std::string getClassName(void) override { return "EntityBatchImporter"; };

	uint32_t getNumberOfRuns();
	std::string getNameBase();
};
