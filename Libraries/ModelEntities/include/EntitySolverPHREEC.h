#pragma once
#pragma warning(disable : 4251)

#include "EntitySolver.h"
#include "Types.h"

#include <list>

class __declspec(dllexport) EntitySolverPHREEC : public EntitySolver
{
public:
	EntitySolverPHREEC(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactory *factory, const std::string &owner);
	virtual ~EntitySolverPHREEC();

	virtual std::string getClassName(void) override { return "EntitySolverPHREEC"; } ;

protected:
	virtual int getSchemaVersion(void) override  { return 1; } ;

private:
};

