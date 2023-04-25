#pragma once

#include "EntityContainer.h"

class __declspec(dllexport) EntitySignalType : public EntityContainer
{
public:
	EntitySignalType(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactory *factory, const std::string &owner);
	virtual ~EntitySignalType();
	virtual void addVisualizationNodes(void) override;

	virtual std::string getClassName(void) override { return "EntitySignalType"; };

	virtual entityType getEntityType(void) override { return TOPOLOGY; };

	virtual bool considerForPropertyFilter(void) override { return true; };
	virtual bool considerChildrenForPropertyFilter(void) override { return false; };

	virtual bool updateFromProperties(void) override;

	void createProperties();

	static const std::string GetPropertyNameExcitationType();
	static const std::string GetValueSinGuasSignal();

	static const std::string GetPropertyNameMinFrequency();
	static const std::string GetPropertyNameMaxFrequency();


protected:
	virtual int getSchemaVersion(void) override { return 1; };

private:

	// Inherited via EntityBase
	virtual bool getEntityBox(double & xmin, double & xmax, double & ymin, double & ymax, double & zmin, double & zmax) override;
};