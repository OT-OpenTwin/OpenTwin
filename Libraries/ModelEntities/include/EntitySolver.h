// @otlicense

#pragma once
#pragma warning(disable : 4251)

#include "EntityContainer.h"
#include "OldTreeIcon.h"

#include <list>

class __declspec(dllexport) EntitySolver : public EntityContainer
{
public:
	EntitySolver() : EntitySolver(0, nullptr, nullptr, nullptr, "") {};
	EntitySolver(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner);
	virtual ~EntitySolver();

	static std::string className() { return "EntitySolver"; }
	virtual std::string getClassName(void) const override { return EntitySolver::className(); }

	virtual void addVisualizationNodes(void) override;

	virtual entityType getEntityType(void) const override { return TOPOLOGY; };

	virtual bool considerForPropertyFilter(void) override { return true; };
	virtual bool considerChildrenForPropertyFilter(void) override { return false; };

	virtual bool updateFromProperties(void) override;

protected:
	virtual int getSchemaVersion(void) override  { return 1; } ;

private:
};



