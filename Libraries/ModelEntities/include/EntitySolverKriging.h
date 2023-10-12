#pragma once
#pragma warning(disable : 4251)

#include "EntitySolver.h"
#include "Types.h"

#include <list>

class __declspec(dllexport) EntitySolverKriging : public EntitySolver
{
public:
	EntitySolverKriging(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);
	virtual ~EntitySolverKriging();

	virtual std::string getClassName(void) override { return "EntitySolverKriging"; } ;

protected:
	virtual int getSchemaVersion(void) override  { return 1; } ;

private:
};

