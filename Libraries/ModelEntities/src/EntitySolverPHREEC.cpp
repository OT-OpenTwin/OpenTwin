
#include "EntitySolverPHREEC.h"
#include "OldTreeIcon.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntitySolverPHREEC> registrar("EntitySolverPHREEC");

EntitySolverPHREEC::EntitySolverPHREEC(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner) :
	EntitySolver(ID, parent, obs, ms, owner)
{
}

EntitySolverPHREEC::~EntitySolverPHREEC()
{
}


