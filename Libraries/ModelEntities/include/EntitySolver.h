#pragma once
#pragma warning(disable : 4251)

#include "EntityContainer.h"
#include "Types.h"

#include <list>

class __declspec(dllexport) EntitySolver : public EntityContainer
{
public:
	EntitySolver(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);
	virtual ~EntitySolver();

	virtual void addVisualizationNodes(void) override;

	virtual std::string getClassName(void) override { return "EntitySolver"; } ;

	virtual entityType getEntityType(void) override { return TOPOLOGY; };

	virtual bool considerForPropertyFilter(void) override { return true; };
	virtual bool considerChildrenForPropertyFilter(void) override { return false; };

	virtual bool updateFromProperties(void) override;

protected:
	virtual int getSchemaVersion(void) override  { return 1; } ;

private:
};



