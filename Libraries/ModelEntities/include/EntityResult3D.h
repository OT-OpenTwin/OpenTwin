#pragma once
#include "EntityVis2D3D.h"
#include "PropertyBundlePlane.h"
#include "PropertyBundleScaling.h"
#include "PropertyBundleVis2D3D.h"

class __declspec(dllexport) EntityResult3D : public EntityVis2D3D
{
public:
	EntityResult3D() : EntityResult3D(0, nullptr, nullptr, nullptr, "") {};
	EntityResult3D(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner);

	static std::string className() { return "EntityResult3D"; }
	virtual std::string getClassName(void) override { return EntityResult3D::className(); }
	
	virtual void createProperties(void) override;
	virtual bool updatePropertyVisibilities(void) override;

protected:
	virtual int getSchemaVersion(void) override { return 1; };

	PropertyBundlePlane propertyBundlePlane;
	PropertyBundleScaling propertyBundleScaling;
	PropertyBundleVis2D3D propertyBundleVis2D3D;
};
