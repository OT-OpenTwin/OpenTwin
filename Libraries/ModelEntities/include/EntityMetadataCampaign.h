#pragma once
#include "EntityWithDynamicFields.h"
#include <string>

class __declspec(dllexport) EntityMetadataCampaign : public EntityWithDynamicFields
{
public:
	EntityMetadataCampaign(ot::UID ID, EntityBase* parent, EntityObserver* mdl, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);

	std::string getClassName() override { return "EntityMetadataCampaign"; };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	virtual bool getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) override;
	virtual void addVisualizationNodes() override;
};