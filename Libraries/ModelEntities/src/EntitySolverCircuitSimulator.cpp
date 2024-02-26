#include "EntitySolverCircuitSimulator.h"
#include "Types.h"

#include <bsoncxx/builder/basic/array.hpp>

EntitySolverCircuitSimulator::EntitySolverCircuitSimulator(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner) :
	EntitySolver(ID, parent, obs, ms, factory, owner)
{
}

EntitySolverCircuitSimulator::~EntitySolverCircuitSimulator()
{
}