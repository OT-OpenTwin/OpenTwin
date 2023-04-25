#pragma once
#include "EntityWithDynamicFields.h"
#include <string>

class __declspec(dllexport) EntityResearchMetadata : public EntityWithDynamicFields
{
public:
	EntityResearchMetadata(ot::UID ID, EntityBase* parent, EntityObserver* mdl, ModelState* ms, ClassFactory* factory, const std::string& owner);

	std::string getClassName() override { return "EntityResearchMetadata"; };
	virtual entityType getEntityType(void) override { return TOPOLOGY; };
	virtual bool getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) override;
	virtual void addVisualizationNodes() override;
};