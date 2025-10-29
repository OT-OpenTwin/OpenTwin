// @otlicense


#include "EntitySolverKriging.h"
#include "OldTreeIcon.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntitySolverKriging> registrar("EntitySolverKriging");

EntitySolverKriging::EntitySolverKriging(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner) :
	EntitySolver(ID, parent, obs, ms, owner)
{
}

EntitySolverKriging::~EntitySolverKriging()
{
}


