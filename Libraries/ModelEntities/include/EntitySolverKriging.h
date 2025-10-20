#pragma once
#pragma warning(disable : 4251)

#include "EntitySolver.h"
#include "OldTreeIcon.h"

#include <list>

class __declspec(dllexport) EntitySolverKriging : public EntitySolver
{
public:
	EntitySolverKriging() : EntitySolverKriging(0, nullptr, nullptr, nullptr, "") {};
	EntitySolverKriging(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner);
	virtual ~EntitySolverKriging();

	virtual std::string getClassName(void) const override { return "EntitySolverKriging"; } ;

protected:
	virtual int getSchemaVersion(void) override  { return 1; } ;

private:
};

