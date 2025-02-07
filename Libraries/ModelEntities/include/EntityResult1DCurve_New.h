#pragma once
#include "EntityBase.h"
#include "IVisualisationCurve.h"

class __declspec(dllexport) EntityResult1DCurve_New : public EntityBase, public IVisualisationCurve
{
public:

	EntityResult1DCurve_New(ot::UID _ID, EntityBase* _parent, EntityObserver* _mdl, ModelState* _ms, ClassFactoryHandler* _factory, const std::string& _owner);

	virtual bool getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) override { return false; };

	virtual bool considerForPropertyFilter(void) override { return true; };
	virtual bool considerChildrenForPropertyFilter(void) override { return false; };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; };

	virtual void addVisualizationNodes(void) override;
	virtual bool updateFromProperties(void) override;

	virtual std::string getClassName(void) { return "EntityResult1DCurve_New"; };

	void createProperties(void);


	// Inherited via IVisualisationCurve
	bool visualiseCurve() override { return true; };
	ot::Plot1DCurveInfoCfg getCurve() override;
	void setCurve(const ot::Plot1DCurveInfoCfg& _curve) override;
};
