#pragma once
#pragma warning(disable : 4251)

#include "EntitySolver.h"
#include "OldTreeIcon.h"

#include <list>

class __declspec(dllexport) EntitySolverPHREEC : public EntitySolver
{
public:
	EntitySolverPHREEC() : EntitySolverPHREEC(0, nullptr, nullptr, nullptr, "") {};
	EntitySolverPHREEC(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner);
	virtual ~EntitySolverPHREEC();

	virtual std::string getClassName(void) const override { return "EntitySolverPHREEC"; } ;

protected:
	virtual int getSchemaVersion(void) override  { return 1; } ;

private:
};

