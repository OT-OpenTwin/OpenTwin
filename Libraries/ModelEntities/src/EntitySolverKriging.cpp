
#include "EntitySolverKriging.h"
#include "Types.h"

#include <bsoncxx/builder/basic/array.hpp>

EntitySolverKriging::EntitySolverKriging(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactory *factory, const std::string &owner) :
	EntitySolver(ID, parent, obs, ms, factory, owner)
{
}

EntitySolverKriging::~EntitySolverKriging()
{
}


