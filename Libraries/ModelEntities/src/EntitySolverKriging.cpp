
#include "EntitySolverKriging.h"
#include "OldTreeIcon.h"

#include <bsoncxx/builder/basic/array.hpp>

EntitySolverKriging::EntitySolverKriging(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner) :
	EntitySolver(ID, parent, obs, ms, factory, owner)
{
}

EntitySolverKriging::~EntitySolverKriging()
{
}


